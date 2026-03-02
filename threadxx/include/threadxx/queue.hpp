#pragma once

#include <string>

#include <tx_api.h>

#include <threadxx/ring_buffer.hpp>

#include <threadxx/intr.hpp>

namespace TXX
{
  
  template <class T, int depth>
  class queue
  {
  };

  class QueueEmpty
  {
  };

  template <uint32_t msg_size, uint32_t queue_size>
  class Queue
  {
    const std::string name;
    TX_QUEUE queue;
    uint32_t buffer[msg_size * queue_size];
    
  public:
    Queue(const std::string &_name) : name(_name) {}
    
    UINT create() {
      return tx_queue_create(&queue, (char *)name.c_str(), msg_size, &buffer, queue_size);
    }

    void send(const uint32_t (&msg)[msg_size]) {
      tx_queue_send(&queue, &msg, TX_WAIT_FOREVER);
    }
    
    void recv(uint32_t (&msg)[msg_size]) {
      tx_queue_receive(&queue, &msg, TX_WAIT_FOREVER);
    }
    
    void recv_wait(uint32_t (&msg)[msg_size], uint32_t wait) {
      auto result = tx_queue_receive(&queue, &msg, wait);

      if (result == TX_QUEUE_EMPTY) {
        throw QueueEmpty();
      }
    }
  };

  template <uint32_t queue_size>
  class Queue<1, queue_size>
  {
    const std::string name;
    TX_QUEUE queue;
    uint32_t buffer[queue_size];
    
  public:
    Queue(const std::string &_name) : name(_name) {}

    UINT create() {
      return tx_queue_create(&queue, (char *)name.c_str(), 1, &buffer, queue_size);
    }

    void send(const uint32_t &msg) {
      tx_queue_send(&queue, (void *)&msg, TX_WAIT_FOREVER);
    }
    
    void recv(uint32_t &msg) {
      tx_queue_receive(&queue, &msg, TX_WAIT_FOREVER);
    }

    uint32_t recv() {
      uint32_t retval;
      recv(retval);
      return retval;
    }
    
    uint32_t recv_wait(uint32_t wait) {
      uint32_t msg;
      auto result = tx_queue_receive(&queue, &msg, wait);

      if (result == TX_QUEUE_EMPTY) {
        throw QueueEmpty();
      }

      return msg;
    }
  };

}
