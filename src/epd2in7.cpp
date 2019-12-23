#include <functional>
#include <iostream>
#include <nan.h>
#include <node.h>

using namespace std;

#include "epdif.h"
#include "lut.h"

using v8::Array;
using v8::Boolean;
using v8::Context;
using v8::Exception;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

// Async worker
class Epd2In7AsyncWorker : public Nan::AsyncWorker {
public:
  function<void()> method;

  Epd2In7AsyncWorker(function<void()> method, Nan::Callback *callback)
      : Nan::AsyncWorker(callback) {
    this->method = method;
  }

  void Execute() { this->method(); }

  void HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {Nan::Null(), // no error occured
                                   Nan::Null()};
    callback->Call(2, argv);
  }

  void HandleErrorCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {
        Nan::New(this->ErrorMessage()).ToLocalChecked(), // return error message
        Nan::Null()};
    callback->Call(2, argv);
  }
};

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
  while (EpdIf::DigitalRead(BUSY_PIN) == 0) { // 0: busy, 1: idle
    EpdIf::DelayMs(100);
  }
}

void Reset(void) {
  EpdIf::DigitalWrite(RST_PIN, HIGH);
  EpdIf::DelayMs(200);
  EpdIf::DigitalWrite(RST_PIN, LOW); // module reset
  EpdIf::DelayMs(10);
  EpdIf::DigitalWrite(RST_PIN, HIGH);
  EpdIf::DelayMs(200);
}

void RefreshDisplay(void) {
  SendCommand(DISPLAY_REFRESH);
  EpdIf::DelayMs(100);
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

  EpdIf::DelayMs(100);
  WaitUntilIdle();
}

#define LUT_FOR_VCOM 0x20
#define LUT_WHITE_TO_WHITE 0x21
#define LUT_BLACK_TO_WHITE 0x22
#define LUT_WHITE_TO_BLACK 0x23
#define LUT_BLACK_TO_BLACK 0x24
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

void width(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Number> num = Number::New(isolate, EPD_WIDTH);
  args.GetReturnValue().Set(num);
}

void height(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Number> num = Number::New(isolate, EPD_HEIGHT);
  args.GetReturnValue().Set(num);
}

void global_init(bool vdhr) {
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

void init_sync(void) {
  if (EpdIf::IfInit() != 0) {
    // TODO : throw error
  } else {
    global_init(true);

    SendCommand(PANEL_SETTING);
    SendData(0xAF); // KW-BF   KWR-AF    BWROTP 0f

    SendCommand(PLL_CONTROL);
    SendData(0x3A); // 3A 100HZ   29 150Hz 39 200HZ    31 171HZ

    SendCommand(VCM_DC_SETTING_REGISTER);
    SendData(0x12);

    SetLut();

    // TODO: return 0;
  }
}

void init(const FunctionCallbackInfo<Value> &args) {
  Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      bind(init_sync), new Nan::Callback(args[0].As<v8::Function>())));
}

void init_grey_sync(void) {
  if (EpdIf::IfInit() != 0) {
    // TODO : throw error
  } else {
    global_init(false);

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

    SetGrayLut();
  }
}

void init_gray(const FunctionCallbackInfo<Value> &args) {
  Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      bind(init_grey_sync), new Nan::Callback(args[0].As<v8::Function>())));
}

// #include <fstream>
// std::ofstream logFile("logfile.txt");

void displayPartial(UBYTE *image, UWORD x, UWORD y, UWORD w, UWORD h) {
  UWORD Width, Height;
  Width = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
  Height = EPD_HEIGHT;

  x = x & 0xf8;
  y = y & 0xf8;
  w = w & 0xf8;

  SendCommand(PARTIAL_DATA_START_TRANSMISSION_2);
  SendData(x >> 8);
  SendData(x & 0xf8); // x should be the multiple of 8, the last 3 bit will
                      // always be ignored
  SendData(y >> 8);
  SendData(y & 0xff);
  SendData(w >> 8);
  SendData(w & 0xf8); // w (width) should be the multiple of 8, the last 3 bit
                      // will always be ignored
  SendData(h >> 8);
  SendData(h & 0xff);
  EpdIf::DelayMs(2);

  for (UWORD j = y; j < h; j++) {
    for (UWORD i = x; i < w; i++) {
      UBYTE buffer = 0x00;
      // for (UWORD k = 0; k < 8; k++) {
      //   buffer = (buffer << 1) | ((image[8 * (i + j * Width) + k] & 0x80) >> 7);
      // }
      SendData(buffer);
    }
  }

  PartialRefresh(x, y, w, h);
}

void displayPartialFrame(const FunctionCallbackInfo<Value> &args) {
  UBYTE *imageData = NULL;

  if (!args[0]->IsNull()) {
    v8::Local<v8::Uint8Array> blackView = args[0].As<v8::Uint8Array>();
    void *blackData = blackView->Buffer()->GetContents().Data();
    imageData = static_cast<UBYTE *>(blackData);
  }

  UWORD x = args[1].As<v8::Number>()->Value();
  UWORD y = args[2].As<v8::Number>()->Value();
  UWORD w = args[3].As<v8::Number>()->Value();
  UWORD h = args[4].As<v8::Number>()->Value();

  Nan::AsyncQueueWorker(
      new Epd2In7AsyncWorker(bind(displayPartial, imageData, x, y, w, h),
                             new Nan::Callback(args[5].As<v8::Function>())));
}

void display(UBYTE *image) {
  UWORD Width, Height;
  Width = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
  Height = EPD_HEIGHT;

  SendCommand(DATA_START_TRANSMISSION_1);
  for (UWORD j = 0; j < Height; j++) {
    for (UWORD i = 0; i < Width; i++) {
      SendData(0XFF);
    }
  }
  SendCommand(DATA_START_TRANSMISSION_2);
  for (UWORD j = 0; j < Height; j++) {
    for (UWORD i = 0; i < Width; i++) {
      // SendData(image[i + j * Width]);
      UBYTE buffer = 0xff;
      for (UWORD k = 0; k < 8; k++) {
        buffer = (buffer << 1) | ((image[8 * (i + j * Width) + k] & 0x80) >> 7);
      }
      SendData(buffer);
    }
  }
  RefreshDisplay();
  WaitUntilIdle();
}

void displayFrame(const FunctionCallbackInfo<Value> &args) {
  UBYTE *imageData = NULL;

  if (!args[0]->IsNull()) {
    v8::Local<v8::Uint8Array> blackView = args[0].As<v8::Uint8Array>();
    void *blackData = blackView->Buffer()->GetContents().Data();
    imageData = static_cast<UBYTE *>(blackData);
  }

  Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      bind(display, imageData), new Nan::Callback(args[1].As<v8::Function>())));
}

#define WHITE 0xC0
#define GRAY1 0x80
#define GRAY2 0x40
#define BLACK 0x00
void displayGray(UBYTE *image) {
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

  RefreshDisplay();
  WaitUntilIdle();
}

void displayGrayFrame(const FunctionCallbackInfo<Value> &args) {
  UBYTE *imageData = NULL;

  if (!args[0]->IsNull()) {
    v8::Local<v8::Uint8Array> grayView = args[0].As<v8::Uint8Array>();
    void *grayData = grayView->Buffer()->GetContents().Data();
    imageData = static_cast<UBYTE *>(grayData);
  }

  Nan::AsyncQueueWorker(
      new Epd2In7AsyncWorker(bind(displayGray, imageData),
                             new Nan::Callback(args[1].As<v8::Function>())));
}

void clear_sync(void) {
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
  WaitUntilIdle();
}

void clear(const FunctionCallbackInfo<Value> &args) {
  Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      bind(clear_sync), new Nan::Callback(args[0].As<v8::Function>())));
}

void sleep_sync(void) {
  SendCommand(POWER_OFF);
  WaitUntilIdle();
  SendCommand(DEEP_SLEEP);
  SendData(0XA5);
}

void sleep(const FunctionCallbackInfo<Value> &args) {
  Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      bind(sleep_sync), new Nan::Callback(args[0].As<v8::Function>())));
}

void InitAll(Local<Object> exports) {
  NODE_SET_METHOD(exports, "init", init);
  NODE_SET_METHOD(exports, "init_gray", init_gray);
  NODE_SET_METHOD(exports, "clear", clear);
  NODE_SET_METHOD(exports, "sleep", sleep);
  NODE_SET_METHOD(exports, "width", width);
  NODE_SET_METHOD(exports, "height", height);
  NODE_SET_METHOD(exports, "displayFrame", displayFrame);
  NODE_SET_METHOD(exports, "displayGrayFrame", displayGrayFrame);
  NODE_SET_METHOD(exports, "displayPartialFrame", displayPartialFrame);
}

NODE_MODULE(epd2in7, InitAll)
