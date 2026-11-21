// Tools -> USB Mode: Hardware CDC and JTAG
// Tools -> USB CDC On Boot: Enabled
// Tools -> JTAG Adapter: Integrated USB JTAG
#include <Arduino.h>
#include "soc/usb_serial_jtag_reg.h"
#include "soc/gpio_sig_map.h"

#define PIN_TCK 4
#define PIN_TMS 5
#define PIN_TDI 6
#define PIN_TDO 7

static bool was_conn = false;

static void route(){
  pinMode(PIN_TCK, OUTPUT); pinMode(PIN_TMS, OUTPUT);
  pinMode(PIN_TDI, OUTPUT); pinMode(PIN_TDO, INPUT);
  WRITE_PERI_REG(USB_SERIAL_JTAG_CONF0_REG,
    READ_PERI_REG(USB_SERIAL_JTAG_CONF0_REG) | USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN);
  esp_rom_gpio_connect_out_signal(PIN_TCK, USB_JTAG_TCK_IDX, false, false);
  esp_rom_gpio_connect_out_signal(PIN_TMS, USB_JTAG_TMS_IDX, false, false);
  esp_rom_gpio_connect_out_signal(PIN_TDI, USB_JTAG_TDI_IDX, false, false);
  esp_rom_gpio_connect_in_signal (PIN_TDO, USB_JTAG_TDO_BRIDGE_IDX, false);
}

static void unroute(){
  WRITE_PERI_REG(USB_SERIAL_JTAG_CONF0_REG,
    READ_PERI_REG(USB_SERIAL_JTAG_CONF0_REG) & ~USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN);
  pinMode(PIN_TCK, INPUT); pinMode(PIN_TMS, INPUT);
  pinMode(PIN_TDI, INPUT); pinMode(PIN_TDO, INPUT);
}

void setup(){ Serial.begin(); }

void loop(){
  bool c = usb_serial_jtag_is_connected();
  if (!was_conn && c) route();
  if (was_conn && !c) unroute();
  was_conn = c;
}
