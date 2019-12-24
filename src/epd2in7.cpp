#include <functional>
#include <iostream>

using namespace std;

#include "epdif.h"
#include "lut.h"
#include "epd2in7.h"

void SendCommand(UBYTE command) {
  EpdIf::DigitalWrite(DC_PIN, LOW);
  EpdIf::DigitalWrite(CS_PIN, LOW);
  EpdIf::SpiTransfer(command);
  EpdIf::DigitalWrite(CS_PIN, HIGH);
}

void SendData(UBYTE data) {
  EpdIf::DigitalWrite(DC_PIN, HIGH);
  EpdIf::DigitalWrite(CS_PIN, LOW);
  EpdIf::SpiTransfer(data);
  EpdIf::DigitalWrite(CS_PIN, HIGH);
}

void WaitUntilIdle(void) {
  do {
    EpdIf::DelayMs(10);
  } while (EpdIf::DigitalRead(BUSY_PIN) == 0); // 0: busy, 1: idle
}

void Reset(void) {
  EpdIf::DigitalWrite(RST_PIN, HIGH);
  EpdIf::DelayMs(10);
  EpdIf::DigitalWrite(RST_PIN, LOW); // module reset
  EpdIf::DelayMs(10);
  EpdIf::DigitalWrite(RST_PIN, HIGH);
  EpdIf::DelayMs(10);
}

void RefreshDisplay(void) {
  SendCommand(DISPLAY_REFRESH);
  WaitUntilIdle();
}

void PartialRefresh(UWORD x, UWORD y, UWORD w, UWORD l) {
  SendCommand(PARTIAL_DISPLAY_REFRESH);
  SendData(x >> 8);
  SendData(x & 0xf8); // x should be the multiple of 8, the last 3 bit will
                      // always be ignored
  SendData(y >> 8);
  SendData(y & 0xff);
  SendData(w >> 8);
  SendData(w & 0xf8); // w (width) should be the multiple of 8, the last 3 bit
                      // will always be ignored
  SendData(l >> 8);
  SendData(l & 0xff);

  WaitUntilIdle();
}

void SetLut(void) {
  unsigned int count;

  SendCommand(LUT_FOR_VCOM); // vcom
  for (count = 0; count < 44; count++) {
    SendData(lut_vcom0[count]);
  }

  SendCommand(LUT_WHITE_TO_WHITE); // ww --
  for (count = 0; count < 42; count++) {
    SendData(lut_ww[count]);
  }

  SendCommand(LUT_BLACK_TO_WHITE); // bw r
  for (count = 0; count < 42; count++) {
    SendData(lut_bw[count]);
  }

  SendCommand(LUT_WHITE_TO_BLACK); // wb w
  for (count = 0; count < 42; count++) {
    SendData(lut_wb[count]);
  }

  SendCommand(LUT_BLACK_TO_BLACK); // bb b
  for (count = 0; count < 42; count++) {
    SendData(lut_bb[count]);
  }
}

void SetQuickLut(void) {
  unsigned int count;

  SendCommand(LUT_FOR_VCOM); // vcom
  for (count = 0; count < 44; count++) {
    SendData(quick_lut_vcom0[count]);
  }

  SendCommand(LUT_WHITE_TO_WHITE); // ww --
  for (count = 0; count < 42; count++) {
    SendData(quick_lut_ww[count]);
  }

  SendCommand(LUT_BLACK_TO_WHITE); // bw r
  for (count = 0; count < 42; count++) {
    SendData(quick_lut_bw[count]);
  }

  SendCommand(LUT_WHITE_TO_BLACK); // wb w
  for (count = 0; count < 42; count++) {
    SendData(quick_lut_wb[count]);
  }

  SendCommand(LUT_BLACK_TO_BLACK); // bb b
  for (count = 0; count < 42; count++) {
    SendData(quick_lut_bb[count]);
  }
}

void SetQuickestLut(void) {
  unsigned int count;

  SendCommand(LUT_FOR_VCOM); // vcom
  for (count = 0; count < 44; count++) {
    SendData(quickest_lut_vcom0[count]);
  }

  SendCommand(LUT_WHITE_TO_WHITE); // ww --
  for (count = 0; count < 42; count++) {
    SendData(quickest_lut_ww[count]);
  }

  SendCommand(LUT_BLACK_TO_WHITE); // bw r
  for (count = 0; count < 42; count++) {
    SendData(quickest_lut_bw[count]);
  }

  SendCommand(LUT_WHITE_TO_BLACK); // wb w
  for (count = 0; count < 42; count++) {
    SendData(quickest_lut_wb[count]);
  }

  SendCommand(LUT_BLACK_TO_BLACK); // bb b
  for (count = 0; count < 42; count++) {
    SendData(quickest_lut_bb[count]);
  }
}

void SetGrayLut(void) {
  unsigned int count;

  SendCommand(LUT_FOR_VCOM); // vcom
  for (count = 0; count < 44; count++) {
    SendData(gray_lut_vcom[count]);
  }

  SendCommand(LUT_WHITE_TO_WHITE); // ww --
  for (count = 0; count < 42; count++) {
    SendData(gray_lut_ww[count]);
  }

  SendCommand(LUT_BLACK_TO_WHITE); // bw r
  for (count = 0; count < 42; count++) {
    SendData(gray_lut_bw[count]);
  }

  SendCommand(LUT_WHITE_TO_BLACK); // wb w
  for (count = 0; count < 42; count++) {
    SendData(gray_lut_wb[count]);
  }

  SendCommand(LUT_BLACK_TO_BLACK); // bb b
  for (count = 0; count < 42; count++) {
    SendData(gray_lut_bb[count]);
  }
}

void SetQuickGrayLut(void) {
  unsigned int count;

  SendCommand(LUT_FOR_VCOM); // vcom
  for (count = 0; count < 44; count++) {
    SendData(quick_gray_lut_vcom[count]);
  }

  SendCommand(LUT_WHITE_TO_WHITE); // ww --
  for (count = 0; count < 42; count++) {
    SendData(quick_gray_lut_ww[count]);
  }

  SendCommand(LUT_BLACK_TO_WHITE); // bw r
  for (count = 0; count < 42; count++) {
    SendData(quick_gray_lut_bw[count]);
  }

  SendCommand(LUT_WHITE_TO_BLACK); // wb w
  for (count = 0; count < 42; count++) {
    SendData(quick_gray_lut_wb[count]);
  }

  SendCommand(LUT_BLACK_TO_BLACK); // bb b
  for (count = 0; count < 42; count++) {
    SendData(quick_gray_lut_bb[count]);
  }
}

void SetQuickestGrayLut(void) {
  unsigned int count;

  SendCommand(LUT_FOR_VCOM); // vcom
  for (count = 0; count < 44; count++) {
    SendData(quickest_gray_lut_vcom[count]);
  }

  SendCommand(LUT_WHITE_TO_WHITE); // ww --
  for (count = 0; count < 42; count++) {
    SendData(quickest_gray_lut_ww[count]);
  }

  SendCommand(LUT_BLACK_TO_WHITE); // bw r
  for (count = 0; count < 42; count++) {
    SendData(quickest_gray_lut_bw[count]);
  }

  SendCommand(LUT_WHITE_TO_BLACK); // wb w
  for (count = 0; count < 42; count++) {
    SendData(quickest_gray_lut_wb[count]);
  }

  SendCommand(LUT_BLACK_TO_BLACK); // bb b
  for (count = 0; count < 42; count++) {
    SendData(quickest_gray_lut_bb[count]);
  }
}

void Init(bool vdhr) {
  Reset();

  SendCommand(POWER_SETTING); // POWER SETTING
  SendData(0x03);             // VDS_EN, VDG_EN
  SendData(0x00);             // VCOM_HV, VGHL_LV[1], VGHL_LV[0]
  SendData(0x2b);             // VDH
  SendData(0x2b);             // VDL
  if(vdhr) {
    SendData(0x09);             // VDHR
  }

  // Power optimization
  SendCommand(0xF8);
  SendData(0x60);
  SendData(0xA5);

  // Power optimization
  SendCommand(0xF8);
  SendData(0x89);
  SendData(0xA5);

  // Power optimization
  SendCommand(0xF8);
  SendData(0x90);
  SendData(0x00);

  // Power optimization
  SendCommand(0xF8);
  SendData(0x93);
  SendData(0x2A);

  // Power optimization
  SendCommand(0xF8);
  SendData(0xA0);
  SendData(0xA5);

  // Power optimization
  SendCommand(0xF8);
  SendData(0xA1);
  SendData(0x00);

  // Power optimization
  SendCommand(0xF8);
  SendData(0x73);
  SendData(0x41);

  SendCommand(PARTIAL_DISPLAY_REFRESH);
  SendData(0x00);

  SendCommand(BOOSTER_SOFT_START); // boost soft start
  SendData(0x07);
  SendData(0x07);
  SendData(0x17);

  SendCommand(POWER_ON);
  WaitUntilIdle();
}

void TransmitMonochromeImage(UBYTE *image) {
  UWORD Width, Height;
  Width = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
  Height = EPD_HEIGHT;

  /**
  * TRANSMISSION_2 is the back buffer - We draw the image there
  * and the system uses what's in TRANSMISSION_1 to know whether the pixel changes from black to white or whatever
  * and picks the corresponding LUT table.
  * Originally this was sent when display Black-and-white without partial, but I think it's useless here.
  * The driver already knows what the back buffer looks like.
  * This is useful only for clearing or applying a gray scale
  */
  // SendCommand(DATA_START_TRANSMISSION_1);
  // for (UWORD j = 0; j < Height; j++) {
  //   for (UWORD i = 0; i < Width; i++) {
  //     SendData(0x00);
  //   }
  // }

  SendCommand(DATA_START_TRANSMISSION_2);
  for (UWORD j = 0; j < Height; j++) {
    for (UWORD i = 0; i < Width; i++) {
      UBYTE buffer = 0xff;
      for (UWORD k = 0; k < 8; k++) {
        buffer = (buffer << 1) | ((image[8 * (i + j * Width) + k] & 0x80) >> 7);
      }
      SendData(buffer);
    }
  }
}

void TransmitGrayscaleImage(UBYTE *image) {
  UWORD Width, Height;
  Width = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
  Height = EPD_HEIGHT;

  SendCommand(DATA_START_TRANSMISSION_1);
  for (UWORD j = 0; j < Height; j++) {
    for (UWORD i = 0; i < Width; i++) {
      UBYTE buffer = 0xff;
      for (UWORD k = 0; k < 8; k++) {
        UBYTE color = (image[8 * (i + j * Width) + k]) & 0xC0;
        if (color == WHITE || color == GRAY1) {
          buffer = (buffer << 1) | 1;
        } else {
          buffer = buffer << 1;
        }
      }
      SendData(buffer);
    }
  }

  SendCommand(DATA_START_TRANSMISSION_2);
  for (UWORD j = 0; j < Height; j++) {
    for (UWORD i = 0; i < Width; i++) {
      UBYTE buffer = 0xff;
      for (UWORD k = 0; k < 8; k++) {
        UBYTE color = (image[8 * (i + j * Width) + k]) & 0xC0;
        // It seems that to achieve a gray color we set first the value to one
        // color and then the opposite.
        if (color == WHITE || color == GRAY2) {
          buffer = (buffer << 1) | 1;
        } else {
          buffer = buffer << 1;
        }
      }
      SendData(buffer);
    }
  }
}

namespace EPD {
  void init(Speed speed) {
    if (EpdIf::IfInit() != 0) {
      // TODO : throw error
    } else {
      Init(true);

      SendCommand(PANEL_SETTING);
      SendData(0xAF); // KW-BF   KWR-AF    BWROTP 0f

      SendCommand(PLL_CONTROL);
      SendData(0x3A); // 3A 100HZ   29 150Hz 39 200HZ    31 171HZ

      SendCommand(VCM_DC_SETTING_REGISTER);
      SendData(0x12);

      switch(speed) {
        case Speed::Normal:
          SetLut();
          break;
        case Speed::Quick:
          SetQuickLut();
          break;
        case Speed::Quickest:
          SetQuickestLut();
          break;
      }
    }
  }
  void init(void) {
    init(Speed::Normal);
  }

  void initGray(Speed speed) {
    if (EpdIf::IfInit() != 0) {
      // TODO : throw error
    } else {
      Init(false);

      SendCommand(PANEL_SETTING); // panel setting
      SendData(0xbf);             //  #KW-BF   KWR-AF BWROTP 0f

      SendCommand(PLL_CONTROL); // PLL setting
      SendData(0x90);           //       #100hz

      SendCommand(TCON_RESOLUTION); // resolution setting
      SendData(0x00);
      SendData(0xb0);
      SendData(0x01);
      SendData(0x08);

      SendCommand(VCM_DC_SETTING_REGISTER); // vcom_DC setting
      SendData(0x12);

      SendCommand(
          VCOM_AND_DATA_INTERVAL_SETTING); // VCOM AND DATA INTERVAL SETTING
      SendData(0x97);

      switch(speed) {
        case Speed::Normal:
          SetGrayLut();
          break;
        case Speed::Quick:
          SetQuickGrayLut();
          break;
        case Speed::Quickest:
          SetQuickestGrayLut();
          break;
      }
    }
  }
  void initGray(void) {
    initGray(Speed::Normal);
  }

  void clear(void) {
    UWORD Width, Height;
    Width = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

    SendCommand(DATA_START_TRANSMISSION_1);
    for (UWORD j = 0; j < Height; j++) {
      for (UWORD i = 0; i < Width; i++) {
        SendData(0xFF);
      }
    }

    SendCommand(DATA_START_TRANSMISSION_2);
    for (UWORD j = 0; j < Height; j++) {
      for (UWORD i = 0; i < Width; i++) {
        SendData(0xFF);
      }
    }

    RefreshDisplay();
  }

  void display(UBYTE *image) {
    TransmitMonochromeImage(image);
    RefreshDisplay();
  }
  void display(UBYTE *image, UWORD x, UWORD y, UWORD w, UWORD h) {
    TransmitMonochromeImage(image);
    PartialRefresh(x, y, w, h);
  }

  void displayGray(UBYTE *image) {
    TransmitGrayscaleImage(image);
    RefreshDisplay();
  }
  void displayGray(UBYTE *image, UWORD x, UWORD y, UWORD w, UWORD h) {
    TransmitGrayscaleImage(image);
    PartialRefresh(x, y, w, h);
  }

  void sleep(void) {
    SendCommand(POWER_OFF);
    WaitUntilIdle();
    SendCommand(DEEP_SLEEP);
    SendData(0XA5);
  }
}
