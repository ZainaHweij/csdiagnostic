#include "Arduino.h"
#include "FSR.h"
#include <EloquentTinyML.h>
#include <eloquent_tinyml/tensorflow.h>

#include "model.h"

#define NB_INPUTS 7
#define NB_OUTPUTS 1
#define Tensor_arena 2 * 1024  //may need to tweak this value: it's a trial and error process

Eloquent::TinyML::TfLite<NB_INPUTS, NB_OUTPUTS, Tensor_arena> my_model;

#define FSR_1_PIN_1 A10
#define FSR_2_PIN_1 A0
#define FSR_3_PIN_1 A3
#define FSR_4_PIN_1 A13
#define FSR_5_PIN_1 A11
#define FSR_6_PIN_1 A7

String blehm10Str = "";
HardwareSerial &blehm10(Serial1);
FSR fsr_1(FSR_1_PIN_1);
FSR fsr_2(FSR_2_PIN_1);
FSR fsr_3(FSR_3_PIN_1);
FSR fsr_4(FSR_4_PIN_1);
FSR fsr_5(FSR_5_PIN_1);
FSR fsr_6(FSR_6_PIN_1);

const int timeout = 10000;
char menuOption = 0;
long time0;

void setup() {
  Serial.begin(9600);
  my_model.begin(model);

  while (!Serial)
    ;
  Serial.println(F("Start"));

  blehm10.begin(9600);
  blehm10.println(F("BLE On"));
  menuOption = menu();
}

void loop() {
  if (menuOption == '1') {
    if (blehm10.available()) {
      blehm10Str = blehm10.readStringUntil('\n');
      if (!blehm10Str.isEmpty()) {
        Serial.print(F("BT Raw Data: "));
        Serial.println(blehm10Str);
      }
    }
    blehm10.println(F("SENSOR DATA HERE"));
  } else if (menuOption == '2') {
    float fsr_1Force = fsr_1.getForce();
    Serial.print(F("Force: "));
    Serial.print(fsr_1Force);
    Serial.println(F(" [g]"));
  } else if (menuOption == '3') {
    float input[NB_INPUTS] = { fsr_1.getForce(), fsr_2.getForce(), fsr_3.getForce(), fsr_4.getForce(), fsr_5.getForce(), fsr_6.getForce() };
    float output = my_model.predict(input);
    Serial.println(output);
  } else if (menuOption == '4') {
    if (blehm10.available()) {
      blehm10Str = blehm10.readStringUntil('\n');
      if (!blehm10Str.isEmpty()) {
        Serial.print(F("BT Raw Data: "));
        Serial.println(blehm10Str);
      }
    }

    float input[NB_INPUTS] = { fsr_1.getForce(), fsr_2.getForce(), fsr_3.getForce(), fsr_4.getForce(), fsr_5.getForce(), fsr_6.getForce() };
    float output = my_model.predict(input);
    blehm10.println(output);
  }

  if (millis() - time0 > timeout || Serial.availiable()) {
    menuOption = menu();
  }
}

char menu() {
  Serial.println(F("\nWhich component would you like to test?"));
  Serial.println(F("(1) HM-10 BLE Bluetooth 4.0"));
  Serial.println(F("(2) Force Sensitive Resistor 0.5'' #1"));
  Serial.println(F("(3) Realtime machine learning"));
  Serial.println(F("(4) Realtime machine learning + Bluetooth"));

  while (!Serial.available())
    ;

  while (Serial.available()) {
    char c = Serial.read();
    if (isAlphaNumeric(c)) {
      if (c == '1')
        Serial.println(F("Now Testing HM-10 BLE Bluetooth 4.0"));
      else if (c == '2')
        Serial.println(F("Now Testing Force Sensitive Resistor 0.5'' #1"));
      else if (c == '3')
        Serial.println(F("Now Testing Machine Learning"));
      else if (c == '4')
        Serial.println(F("Now Testing Machine Learning + Bluetooth"));
      else {
        Serial.println(F("Illegal input!"));
        return 0;
      }

      time0 = millis();
      return c;
    }
  }
}