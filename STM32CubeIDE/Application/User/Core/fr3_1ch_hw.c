/*
 * fr3_1ch_hw.c
 *
 *  Created on: Jan 28, 2026
 *      Author: zapman
 */
#include <stdio.h>
#include <main.h>
#include <spi.h>
#include <fr3_1ch_hw.h>

#define SPI_DEV_BIT_START  0
#define SPI_DEV_BIT_LEN    2

#define SPI_LEN_BIT_START  (SPI_DEV_BIT_START + SPI_DEV_BIT_LEN)
#define SPI_LEN_BIT_LEN    2

#define SPI_FLAG_RX_BIT    31

#define CMD_MASK           0xFF

#define CMD_SPI            0x53
#define CMD_FLAG_WAIT      (1 << 31)

#define SPI_DEV_MASK       (((1 << SPI_DEV_BIT_LEN) - 1) << SPI_DEV_BIT_START)
#define SPI_LEN_MASK       (((1 << SPI_LEN_BIT_LEN) - 1) << SPI_LEN_BIT_START)

#define GET_SPI_DEV(x)     (((x) & SPI_DEV_MASK) >> SPI_DEV_BIT_START)
#define GET_SPI_LEN(x)     (((x) & SPI_LEN_MASK) >> SPI_LEN_BIT_START)


typedef struct {
  ULONG cmd_flags;
  ULONG payload[3];
} hw_req_t;

#define NUM_HW_REQ   32

ULONG hw_req_data[NUM_HW_REQ * (sizeof(hw_req_t) + sizeof(void *))];


static ULONG hw_request_queue_data[NUM_HW_REQ];

static TX_BLOCK_POOL hw_request_pool;


static TX_EVENT_FLAGS_GROUP hw_req_flags;
static TX_THREAD hw_thread;
static TX_QUEUE hw_queue;

typedef struct  {
  ULONG cmd_flags;
  ULONG dev_flags_size;
  ULONG data;
  ULONG l;
} spi_req_t;

int hw_req_get_id(const hw_req_t *req) {
  return ((ULONG *)req - hw_req_data) / (sizeof(hw_req_t) + sizeof(void *));
}

hw_req_t *hw_req_allocate(void)
{
  hw_req_t *retval;

  tx_block_allocate(&hw_request_pool, (void **)&retval, TX_WAIT_FOREVER);

  tx_event_flags_set(&hw_req_flags, ~(1 << hw_req_get_id(retval)), TX_AND);

  return retval;
}

void hw_req_wait(hw_req_t *req)
{
  ULONG flags;

  tx_event_flags_get(&hw_req_flags, (1 << hw_req_get_id(req)), TX_AND_CLEAR, &flags, TX_WAIT_FOREVER);
}

TX_SEMAPHORE spi_semaphore;

int spi_transmit(int device, int lenb, uint32_t w)
{
  spi_req_t *req = (spi_req_t *)hw_req_allocate();

  req->cmd_flags = CMD_SPI;
  req->dev_flags_size = (device << SPI_DEV_BIT_START) & SPI_DEV_MASK;
  req->dev_flags_size |= (lenb << SPI_LEN_BIT_START) & SPI_LEN_MASK;

  req->data = w;

  tx_queue_send(&hw_queue, &req, TX_WAIT_FOREVER);

  return 0;
}

int spi_transfer(int device, int lenb, uint32_t *v)
{
  spi_req_t *req = (spi_req_t *)hw_req_allocate();

  req->cmd_flags = CMD_SPI | CMD_FLAG_WAIT;
  req->dev_flags_size = (device << SPI_DEV_BIT_START) & SPI_DEV_MASK;
  req->dev_flags_size |= (lenb << SPI_LEN_BIT_START) & SPI_LEN_MASK;

  req->data = *v;

  tx_queue_send(&hw_queue, &req, TX_WAIT_FOREVER);

  hw_req_wait((hw_req_t *)req);

  return 0;
}



void spi_callback(SPI_HandleTypeDef *hspi)
{
  tx_semaphore_put(&spi_semaphore);
}

static void handle_spi_cmd(spi_req_t *req)
{
  uint32_t i, len;
  uint8_t tx_buf[4], rx_buf[4];
  uint8_t *pbyte;

  len = GET_SPI_LEN(req->dev_flags_size);

  printf("SPI COMMAND %ld %ld %08lx\n",
      GET_SPI_DEV(req->dev_flags_size),
      len,
      req->data);

  pbyte = &tx_buf[len - 1];

  for (i = 0; i < len; i++) {
    *pbyte-- = req->data & 0xFF;
    req->data >>= 8;
  }

  if (req->cmd_flags & CMD_FLAG_WAIT) {
    int result;

    result = HAL_SPI_TransmitReceive_IT(&hspi4, tx_buf, rx_buf, len);

    if (result == HAL_OK) {
      tx_semaphore_get(&spi_semaphore, TX_WAIT_FOREVER);

      pbyte = rx_buf;
      req->data = 0;

      for (i = 0; i < len; i++) {
        req->data = (req->data << 8) | *pbyte++;
      }
    } else {
      printf("SPI failed to transfer\n");
      req->data = 0xFFFFFFFF;
    }


    tx_event_flags_set(&hw_req_flags, ~(1 << hw_req_get_id((hw_req_t *)req)), TX_AND);
  } else {
    HAL_SPI_Transmit_IT(&hspi4, tx_buf, len);
  }
}


static VOID hw_thread_entry(ULONG _a)
{
  while(1) {
    int cmd;
    hw_req_t *req;

    tx_queue_receive(&hw_queue, (void **)&req, TX_WAIT_FOREVER);

    // DO SHIT
    cmd = req->cmd_flags & CMD_MASK;

    switch(cmd) {
    case CMD_SPI:
      handle_spi_cmd((spi_req_t *)req);
      break;
    default:
      printf("Unkown command: %2.2x\n", cmd);
      break;
    }

    if (!(req->cmd_flags & CMD_FLAG_WAIT)) {
      tx_block_release((void *)req);
    }

  }
}

char hw_stack[1024];

void fr3_1ch_hw_init(void)
{
  int result;

  HAL_SPI_RegisterCallback(&hspi4, HAL_SPI_TX_RX_COMPLETE_CB_ID, spi_callback);

  result = tx_semaphore_create(&spi_semaphore, "SPI Semaphore", 0);

  if (result != TX_SUCCESS) {
    Error_Handler();
  }

  result = tx_event_flags_create(&hw_req_flags, "HW Req Events");

  if (result != TX_SUCCESS) {
    Error_Handler();
  }

  result = tx_block_pool_create(&hw_request_pool,
      "HW Request Pool",
      sizeof(hw_req_t),
      (void *)hw_req_data,
      sizeof(hw_req_data));

  if (result != TX_SUCCESS) {
    Error_Handler();
  }

  result = tx_queue_create(&hw_queue,
      "HW Request QUeue",
      1,
      hw_request_queue_data,
      sizeof(hw_request_queue_data));

  if (result != TX_SUCCESS) {
    Error_Handler();
  }

  result = tx_thread_create(&hw_thread, "hw_thread", hw_thread_entry, 1, hw_stack, sizeof(hw_stack), 20, 20, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (result != TX_SUCCESS) {
    Error_Handler();
  }
}
