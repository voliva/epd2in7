#include <functional>
#include <iostream>
#include <nan.h>
#include <node.h>

using namespace std;

#include "epd2in7.h"
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

void init(const FunctionCallbackInfo<Value> &args) {
  if(args[0]->IsUint32()) {
    auto speed = (EPD::Speed)(args[0].As<v8::Uint32>()->Value());
    Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      [speed](){ return EPD::init(speed); },
      new Nan::Callback(args[1].As<v8::Function>()))
    );
  } else {
    Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      [](){ return EPD::init(); }, new Nan::Callback(args[0].As<v8::Function>())));
  }
}

void initGray(const FunctionCallbackInfo<Value> &args) {
  if(args[0]->IsUint32()) {
    auto speed = (EPD::Speed)(args[0].As<v8::Uint32>()->Value());
    Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      [speed](){ return EPD::initGray(speed); },
      new Nan::Callback(args[1].As<v8::Function>()))
    );
  } else {
    Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      [](){ return EPD::initGray(); }, new Nan::Callback(args[0].As<v8::Function>())));
  }
}

void display(const FunctionCallbackInfo<Value> &args) {
  UBYTE *imageData = NULL;

  if (!args[0]->IsNull()) {
    v8::Local<v8::Uint8Array> blackView = args[0].As<v8::Uint8Array>();
    void *blackData = blackView->Buffer()->GetContents().Data();
    imageData = static_cast<UBYTE *>(blackData);
  }

  if(args.Length() == 6) {
    UWORD x = args[1].As<v8::Number>()->Value();
    UWORD y = args[2].As<v8::Number>()->Value();
    UWORD w = args[3].As<v8::Number>()->Value();
    UWORD h = args[4].As<v8::Number>()->Value();

    Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      [imageData, x, y, w, h]() { return EPD::display(imageData, x, y, w, h); },
      new Nan::Callback(args[5].As<v8::Function>()))
    );
  } else {
    Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      [imageData]() { return EPD::display(imageData); },
      new Nan::Callback(args[1].As<v8::Function>()))
    );
  }
}

void displayGray(const FunctionCallbackInfo<Value> &args) {
  UBYTE *imageData = NULL;

  if (!args[0]->IsNull()) {
    v8::Local<v8::Uint8Array> blackView = args[0].As<v8::Uint8Array>();
    void *blackData = blackView->Buffer()->GetContents().Data();
    imageData = static_cast<UBYTE *>(blackData);
  }

  if(args.Length() == 6) {
    UWORD x = args[1].As<v8::Number>()->Value();
    UWORD y = args[2].As<v8::Number>()->Value();
    UWORD w = args[3].As<v8::Number>()->Value();
    UWORD h = args[4].As<v8::Number>()->Value();

    Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      [imageData, x, y, w, h]() { return EPD::displayGray(imageData, x, y, w, h); },
      new Nan::Callback(args[5].As<v8::Function>()))
    );
  } else {
    Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      [imageData]() { return EPD::displayGray(imageData); },
      new Nan::Callback(args[1].As<v8::Function>()))
    );
  }
}

void clear(const FunctionCallbackInfo<Value> &args) {
  Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      bind(EPD::clear), new Nan::Callback(args[0].As<v8::Function>())));
}

void sleep(const FunctionCallbackInfo<Value> &args) {
  Nan::AsyncQueueWorker(new Epd2In7AsyncWorker(
      bind(EPD::sleep), new Nan::Callback(args[0].As<v8::Function>())));
}

void InitAll(Local<Object> exports) {
  NODE_SET_METHOD(exports, "width", width);
  NODE_SET_METHOD(exports, "height", height);

  NODE_SET_METHOD(exports, "init", init);
  NODE_SET_METHOD(exports, "initGray", initGray);

  NODE_SET_METHOD(exports, "clear", clear);

  NODE_SET_METHOD(exports, "display", display);
  NODE_SET_METHOD(exports, "displayGray", displayGray);

  NODE_SET_METHOD(exports, "sleep", sleep);
}

NODE_MODULE(epd2in7, InitAll)
