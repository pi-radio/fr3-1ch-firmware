#include <usbxx/ux_api.h>
#include <usbxx/stm32/dcd.hpp>
#include <usbxx/stm32/endpoint.hpp>

#include <usbxx/endian.hpp>
#include <usbxx/device.hpp>
#include <usbxx/event_log.hpp>

using namespace USBXX;

static constexpr uint32_t winterruptmask =
    USB_CNTR_CTRM  | USB_CNTR_WKUPM |
    USB_CNTR_SUSPM | USB_CNTR_ERRM |
    USB_CNTR_SOFM | USB_CNTR_ESOFM |
    USB_CNTR_RESETM | USB_CNTR_L1REQM;

void STM32::DCD::enable_interrupts()
{
  // Clear pending
  pcd->ISTR = 0U;
  pcd->CNTR = winterruptmask;
}

void STM32::DCD::disable_interrupts()
{
  pcd->CNTR &= ~winterruptmask;
}


void STM32::DCD::endpoint_IRQ()
{
  uint16_t wIstr;
  uint16_t wEPVal;
  uint8_t epindex;

  /* stay in loop while pending interrupts */
  while ((pcd->ISTR & USB_ISTR_CTR) != 0U)
  {
    wIstr = (uint16_t)pcd->ISTR;

    epindex = (uint8_t)(wIstr & USB_ISTR_IDN);

    wEPVal = (uint16_t)PCD_GET_ENDPOINT(pcd, epindex);

    event_log.push_event(UsbEvent::ENDPOINT_IRQ, epindex);

    if (wEPVal & USB_EP_VTRX)
    {
      auto ep = endpoints[epindex];

      ep->on_interrupt();
    }

    if (wEPVal & USB_EP_VTTX)
    {
      auto ep = endpoints[0x80 | epindex];

      ep->on_interrupt();
    }
  }
    /* Decode and service non control endpoints interrupt */
    /* process related endpoint register */


  return;
}


void STM32::DCD::handle_IRQ()
{
  uint32_t wIstr = pcd->ISTR;

  if ((wIstr & USB_ISTR_CTR) == USB_ISTR_CTR)
  {
    endpoint_IRQ();

    return;
  }

  if ((wIstr & USB_ISTR_RESET) == USB_ISTR_RESET)
  {
    pcd->ISTR &= ~USB_ISTR_RESET;

    reset();

    set_device_address(0);

    return;
  }

  if ((wIstr & USB_ISTR_PMAOVR) == USB_ISTR_PMAOVR)
  {
    pcd->ISTR &= ~USB_ISTR_PMAOVR;

    return;
  }

  if ((wIstr & USB_ISTR_ERR) == USB_ISTR_ERR)
  {
    pcd->ISTR &= ~USB_ISTR_ERR;

    return;
  }

  if ((wIstr & USB_ISTR_WKUP) == USB_ISTR_WKUP)
  {
    pcd->CNTR &= ~(USB_CNTR_SUSPRDY);
    pcd->CNTR &= ~(USB_CNTR_SUSPEN);

    if (hpcd.LPM_State == LPM_L1)
    {
      hpcd.LPM_State = LPM_L0;

      device->on_lpm_event(LPMEvent::L0);
    }

    resume();

    pcd->ISTR &= ~USB_ISTR_WKUP;

    //event_log.push_event(UsbEvent::END_IRQ);

    return;
  }

  if ((wIstr & USB_ISTR_SUSP) == USB_ISTR_SUSP)
  {
    /* Force low-power mode in the macrocell */
    pcd->CNTR |= USB_CNTR_SUSPEN;

    pcd->ISTR &= ~USB_ISTR_SUSP;

    pcd->CNTR |= USB_CNTR_SUSPRDY;

    suspend();

    //event_log.push_event(UsbEvent::END_IRQ);

    return;
  }

  /* Handle LPM Interrupt */
  if ((wIstr & USB_ISTR_L1REQ) == USB_ISTR_L1REQ)
  {
    pcd->ISTR &= ~USB_ISTR_L1REQ;

    if (hpcd.LPM_State == LPM_L0)
    {
      /* Force suspend and low-power mode before going to L1 state*/
      pcd->CNTR |= USB_CNTR_SUSPRDY;
      pcd->CNTR |= USB_CNTR_SUSPEN;

      hpcd.LPM_State = LPM_L1;
      hpcd.BESL = ((uint32_t)pcd->LPMCSR & USB_LPMCSR_BESL) >> 2;
      device->on_lpm_event(LPMEvent::L1);
    }
    else
    {
      suspend();
    }

    //event_log.push_event(UsbEvent::END_IRQ);

    return;
  }

  if ((wIstr & USB_ISTR_SOF) == USB_ISTR_SOF)
  {
    pcd->ISTR &= ~USB_ISTR_SOF;

    event_log.set_frame(get_frame_number());

    on_sof();

    //event_log.push_event(UsbEvent::END_IRQ);

    return;
  }

  if ((wIstr & USB_ISTR_ESOF) == USB_ISTR_ESOF)
  {
    pcd->ISTR &= ~USB_ISTR_ESOF;

    event_log.push_event(UsbEvent::ESOF, (pcd->FNR >> 11) & 3);

    return;
  }
}

uint32_t STM32::DCD::get_frame_number()
{
 return pcd->FNR & 0x7FF;
}

static const char *what = nullptr;

extern "C" void USB_DRD_FS_IRQHandler(void)
{
  try {
    STM32::gDCD->handle_IRQ();

    return;
  } catch (const USBXX::runtime_error &e) {
    what = e.what();
    __asm volatile ("BKPT     %0" : : "i"(0));
  } catch (const std::exception &e) {
    what = e.what();
    __asm volatile ("BKPT     %0" : : "i"(0));
  }
}
