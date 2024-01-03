#include "Keypad.h"
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

float my_vcc_const = 1.1; // must be 1.1

byte MOSFET[10] = {18, 4, 5, 6, 7, 8, 14, 15, 16, 17};  // array of pins
boolean FLAGS[10]; // array of flags of each MOSFET
unsigned long TIMES[10];
byte battery_pin = 6;
boolean RXstate;

byte receiv_data[2];

int fuse_time = 200;
int battery_check = 3700;

byte crypt_key = 123;    // unique key
int check = 111;         // code for connection check
int check_2 = 112;

const byte ROWS = 4;
const byte COLS = 3;
char keys[4][3] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {8, 7, 6, 5};
byte colPins[COLS] = {4, 3, 2};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


RF24 radio(9, 10); // create module on pins 9 and 10

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

void setup() {
  Serial.begin(9600);

  for (int i = 0; i <= 9; i++) {
    pinMode(MOSFET[i], OUTPUT);
  }

  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(0, 15);
  radio.enableAckPayload();
  radio.setPayloadSize(32);

  radio.openReadingPipe(1, address[0]);     //reading pipe 0
  radio.setChannel(0x60);  //set channel

  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_1MBPS);

  radio.powerUp();
  radio.startListening();
}

void loop() {
  byte pipeNo;
  while ( radio.available(&pipeNo)) {
    radio.read( &receiv_data, sizeof(receiv_data) );

    if (receiv_data[0] == crypt_key) {

      if (receiv_data[1] == check) {
        int voltage = analogRead(battery_pin) * readVcc() / 1024;
        if (voltage > battery_check) RXstate = 1; else RXstate = 0;

        radio.writeAckPayload(pipeNo, &RXstate, sizeof(RXstate) );
        Serial.println("Check state sent");
      } else if(receiv_data[1] == check_2) { 
          int voltage = analogRead(battery_pin) * readVcc() / 1024;
          if (voltage > battery_check) RXstate = 1; else RXstate = 0;

          radio.writeAckPayload(pipeNo, &RXstate, sizeof(RXstate) );
          Serial.println("Check state sent");
        
        }  else {
          if(receiv_data[1] == 11) {
            for(int j = 1; j <= 9; j++) {
                  digitalWrite(MOSFET[j], HIGH);
                  delay(150);
                  digitalWrite(MOSFET[j], LOW);  
              }
              digitalWrite(MOSFET[0], HIGH);
              delay(150);
              digitalWrite(MOSFET[0], LOW);
          }
        if (FLAGS[receiv_data[1]] == 0) {
          FLAGS[receiv_data[1]] = 1;
          TIMES[receiv_data[1]] = millis();
          digitalWrite(MOSFET[receiv_data[1]], HIGH);

          Serial.print("Fuse #"); Serial.print(receiv_data[1]); Serial.println(" ON");
        } 
      }
    }
  }

  for (int i = 0; i <= 9; i++) {
    if (millis() - TIMES[i] > fuse_time && FLAGS[i] == 1) {
      FLAGS[i] = 0;
      digitalWrite(MOSFET[i], LOW);
      Serial.print("Fuse #"); Serial.print(i); Serial.println(" OFF");
    }
  }
}

long readVcc() {
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // measuring
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both
  long result = (high << 8) | low;

  result = my_vcc_const * 1023 * 1000 / result;
  return result;
}
