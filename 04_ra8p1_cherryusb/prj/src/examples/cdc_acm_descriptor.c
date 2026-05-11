#include "cdc_acm_descriptor.h"
#include "usbd_cdc_acm.h"
#include "usb_config.h"

#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x83

#define USBD_VID           0x0483
#define USBD_PID           0x5720
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

static const uint8_t s_device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01)
};

static const uint8_t s_config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02)
};

static const char *s_string_descriptors[] = {
    (const char[]){ 0x09, 0x04 },
    "CherryUSB",
    "CherryUSB CDC DEMO",
    "2022123456",
};

#ifdef CONFIG_USB_HS
static const uint8_t s_device_qualifier_descriptor[] = {
    0x0A, 0x06, 0x00, 0x02, 0xEF, 0x02, 0x01, CDC_MAX_MPS, 0x01, 0x00
};

static const uint8_t *device_qualifier_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return s_device_qualifier_descriptor;
}
#endif

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return s_device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return s_config_descriptor;
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    (void)speed;
    if (index >= (sizeof(s_string_descriptors) / sizeof(char *))) {
        return NULL;
    }
    return s_string_descriptors[index];
}

struct usb_descriptor cdc_acm_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
#ifdef CONFIG_USB_HS
    .device_quality_descriptor_callback = device_qualifier_descriptor_callback,
#else
    .device_quality_descriptor_callback = NULL,
#endif
};
