#include <Wire.h>

#define PIN_COUNT 8
    // delay in milliseconds
#define DELAY 100
#define PI_SLICE (360/PIN_COUNT)

int led_pin[PIN_COUNT] = {11, 12, 13, 5, 4, 3, 2, 10};
const unsigned data_rdy_pin = 6;
boolean device_ready = false;
unsigned send_counter(DELAY), cur_led(0);

void setup(){
    Wire.begin();
    Serial.begin(9600);

    Wire.beginTransmission(0x1E);
    Wire.write(0x00);
    Wire.write(0x70);
    Wire.write(0xA0);
    Wire.write(0x00);
    device_ready = !Wire.endTransmission();

    pinMode(data_rdy_pin, INPUT);
}

void loop(){
    if(device_ready && digitalRead(data_rdy_pin)){
        Wire.beginTransmission(0x1E);
            Wire.write(0x03);
        device_ready = !Wire.endTransmission();
        Wire.requestFrom(0x1E, 6);
        const int
            x = ((int)Wire.read() << 8) | Wire.read(),
            z = ((int)Wire.read() << 8) | Wire.read(),
            y = ((int)Wire.read() << 8) | Wire.read()
        ;
        const float pi = 3.14159265359f;
        const float dir(
            (y==0 && x < 0)*180.0f
            + (!(y==0))*(270.0f - (y>0)*180.0f - atan((float)x/y)*180.0f/pi)
        );
        digitalWrite(cur_led, LOW);
        cur_led = map(dir, -PI_SLICE, 360-PI_SLICE, 0, PIN_COUNT-1);
        digitalWrite(led_pin[cur_led], HIGH);
        if(--send_counter == 0){
            send_counter = DELAY;
            Serial.write((byte)(dir/360.0f*256.0f));
        }
    }
}