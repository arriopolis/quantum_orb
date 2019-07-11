#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NeoPixelBus.h>
#include <AccelStepper.h>

#include "config.h"

TaskHandle_t motorTask;

OneWire oneWire(ONE_WIRE_BUS); // OneWire data wire connected to GPIO19
DallasTemperature sensors(&oneWire);

DeviceAddress sensor1 = {0x28, 0xDA, 0xF2, 0xDE, 0xA, 0x0, 0x0, 0x4E};

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(LED_COUNT, LED_PIN);

RgbColor red(LED_COLOR_SATURATION, 0, 0);
RgbColor green(0, LED_COLOR_SATURATION, 0);
RgbColor blue(0, 0, LED_COLOR_SATURATION);
RgbColor white(LED_COLOR_SATURATION);
RgbColor black(0);

AccelStepper stepper1(AccelStepper::DRIVER, STPA_STEP, STPA_DIR);
AccelStepper stepper2(AccelStepper::DRIVER, STPB_STEP, STPB_DIR);
AccelStepper stepper3(AccelStepper::DRIVER, STPC_STEP, STPC_DIR);

void motorCall(void * parameter)
{
    for(;;){
        // Change direction at the limits
        if (stepper1.distanceToGo() == 0)
        {
            stepper1.moveTo(-stepper1.currentPosition());
        }
        stepper1.run();

        if (stepper2.distanceToGo() == 0)
        {
            stepper2.moveTo(-stepper2.currentPosition());
        }
        stepper2.run();

        if (stepper3.distanceToGo() == 0)
        {
            stepper3.moveTo(-stepper3.currentPosition());
        }
        stepper3.run();
        //delay(100);
    }
}


void setup()
{
    Serial.begin(115200);
    sensors.begin();

    strip.Begin();
    strip.Show();

    stepper1.setMaxSpeed(12000.0);
    stepper1.setAcceleration(6000.0);
    stepper1.moveTo(5000);

    stepper2.setMaxSpeed(15000.0);
    stepper2.setAcceleration(1000.0);
    stepper2.moveTo(10000);

    stepper3.setMaxSpeed(8000.0);
    stepper3.setAcceleration(4000.0);
    stepper3.moveTo(8000);

    pinMode(STP_EN, OUTPUT);

    pinMode(STP_MS1, OUTPUT);
    pinMode(STP_MS2, OUTPUT);
    pinMode(STP_MS2, OUTPUT);

    digitalWrite(STP_EN, LOW);

    digitalWrite(STP_MS1, HIGH); //eights microstepping seems ideal = HIGH HIGH LOW
    digitalWrite(STP_MS2, HIGH);
    digitalWrite(STP_MS3, LOW);

    strip.SetPixelColor(0, green);
    strip.SetPixelColor(1, green);
    strip.SetPixelColor(2, green);
    strip.SetPixelColor(3, white);
    strip.Show();

   disableCore0WDT(); //I disable the core becasue i dont have the WDT reset functions working yet
   xTaskCreatePinnedToCore(motorCall, "motorTask", 1000, NULL, 1, &motorTask, 0);
}


void loop()
{
    
    Serial.print("Requesting temperature");
    sensors.requestTemperatures();
    Serial.println("DONE");
    
    Serial.print(" *C Sensor ");
    Serial.println(sensors.getTempC(sensor1));

}
