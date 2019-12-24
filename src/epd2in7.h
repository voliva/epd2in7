#ifndef EPD2IN7_H
#define EPD2IN7_H

#include "epdif.h"

// Display resolution
#define EPD_WIDTH 176
#define EPD_HEIGHT 264

// EPD2IN7 commands
#define PANEL_SETTING 0x00
#define POWER_SETTING 0x01
#define POWER_OFF 0x02
#define POWER_OFF_SEQUENCE_SETTING 0x03
#define POWER_ON 0x04
#define POWER_ON_MEASURE 0x05
#define BOOSTER_SOFT_START 0x06
#define DEEP_SLEEP 0x07
#define DATA_START_TRANSMISSION_1 0x10
#define DATA_STOP 0x11
#define DISPLAY_REFRESH 0x12
#define DATA_START_TRANSMISSION_2 0x13
#define PARTIAL_DATA_START_TRANSMISSION_1 0x14
#define PARTIAL_DATA_START_TRANSMISSION_2 0x15
#define PARTIAL_DISPLAY_REFRESH 0x16
#define PLL_CONTROL 0x30
#define TEMPERATURE_SENSOR_COMMAND 0x40
#define TEMPERATURE_SENSOR_CALIBRATION 0x41
#define TEMPERATURE_SENSOR_WRITE 0x42
#define TEMPERATURE_SENSOR_READ 0x43
#define VCOM_AND_DATA_INTERVAL_SETTING 0x50
#define LOW_POWER_DETECTION 0x51
#define TCON_SETTING 0x60
#define TCON_RESOLUTION 0x61
#define SOURCE_AND_GATE_START_SETTING 0x62
#define GET_STATUS 0x71
#define AUTO_MEASURE_VCOM 0x80
#define VCOM_VALUE 0x81
#define VCM_DC_SETTING_REGISTER 0x82
#define PROGRAM_MODE 0xA0
#define ACTIVE_PROGRAM 0xA1
#define READ_OTP_DATA 0xA2

// Lut constants
#define LUT_FOR_VCOM 0x20
#define LUT_WHITE_TO_WHITE 0x21
#define LUT_BLACK_TO_WHITE 0x22
#define LUT_WHITE_TO_BLACK 0x23
#define LUT_BLACK_TO_BLACK 0x24

// Colors
#define WHITE 0xC0
#define GRAY1 0x80
#define GRAY2 0x40
#define BLACK 0x00

namespace EPD {
  enum class Speed {
    Normal,
    Quick,
    Quickest
  };

  void init(void);
  void init(Speed speed);

  void initGray(void);
  void initGray(Speed speed);

  void clear(void);

  void display(UBYTE *image);
  void display(UBYTE *image, UWORD x, UWORD y, UWORD w, UWORD h);
  void displayGray(UBYTE *image);
  void displayGray(UBYTE *image, UWORD x, UWORD y, UWORD w, UWORD h);

  void sleep(void);
}

#endif