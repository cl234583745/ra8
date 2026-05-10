#include "usbd_core.h"
#include "usbd_cdc_acm.h"
#include "cdc_acm_descriptor.h"

#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x02

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[16384];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[16384];

static volatile bool g_ep_tx_busy;
static volatile bool g_dtr;

static struct usbd_interface intf0;
static struct usbd_interface intf1;

struct usbd_endpoint cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = NULL,
};

struct usbd_endpoint cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = NULL,
};

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
        case USBD_EVENT_CONFIGURED:
            g_ep_tx_busy = false;
            usbd_ep_start_read(busid, CDC_OUT_EP, read_buffer, sizeof(read_buffer));
            break;
        default:
            break;
    }
}

void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    usbd_ep_start_read(busid, CDC_OUT_EP, read_buffer, sizeof(read_buffer));
}

void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    if (g_dtr) {
        usbd_ep_start_write(busid, CDC_IN_EP, write_buffer, sizeof(write_buffer));
    } else {
        g_ep_tx_busy = false;
    }
}

void usbd_cdc_acm_set_dtr(uint8_t busid, uint8_t intf, bool dtr)
{
    g_dtr = dtr;
    if (dtr) {
        g_ep_tx_busy = false;
        usbd_ep_start_write(busid, CDC_IN_EP, write_buffer, sizeof(write_buffer));
    }
}

void cdc_acm_init(uint8_t busid, uintptr_t reg_base)
{
    for (uint32_t i = 0; i < sizeof(write_buffer); i++) {
        write_buffer[i] = (uint8_t)(i & 0xFF);
    }

    cdc_out_ep.ep_cb = usbd_cdc_acm_bulk_out;
    cdc_in_ep.ep_cb = usbd_cdc_acm_bulk_in;

    usbd_desc_register(busid, &cdc_acm_descriptor);

    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf0));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf1));
    usbd_add_endpoint(busid, &cdc_out_ep);
    usbd_add_endpoint(busid, &cdc_in_ep);
    usbd_initialize(busid, reg_base, usbd_event_handler);
}
