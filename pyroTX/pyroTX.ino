#include "Keypad.h"
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9, 10);
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

byte redLED = 14;
byte greenLED = 15;

byte crypt_key = 123;
int check = 111;
int check_2 = 112;
byte transm_data[2];
boolean check_answer;

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
byte flag;

void setup() {
  Serial.begin(9600);

  transm_data[0] = crypt_key;

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  digitalWrite(greenLED, HIGH);

  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(0, 15);
  radio.enableAckPayload();
  radio.setPayloadSize(32);

  radio.openWritingPipe(address[0]);
  radio.setChannel(0x60);

  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_1MBPS);

  radio.powerUp();
  radio.stopListening();
}

void loop() {
  byte gotByte;
  char key = keypad.waitForKey();
  int keyInt = key - '0';

  if (keyInt >= 0 && keyInt <= 9 && flag == 1) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    transm_data[1] = keyInt;
    radio.write(&transm_data, sizeof(transm_data));
    delay(70);
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);

  }else if (keyInt == 5 && flag == 2) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    transm_data[1] = 11;
    radio.write(&transm_data, sizeof(transm_data));
    delay(70);
    flag = 0;
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);

  } else if (key == '*') {
    flag = 0;

    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, LOW);
    delay(80);

    transm_data[1] = check;
    if ( radio.write(&transm_data, sizeof(transm_data)) ) { 
      if (!radio.available()) {
        Serial.println("No answer");
        digitalWrite(greenLED, HIGH);
      } else {
        while (radio.available() ) {
          radio.read( &check_answer, 1);
          if (check_answer == 1) {
            flag = 1;
            digitalWrite(redLED, HIGH);
            Serial.println("Status OK");
          } else {
            Serial.println("Status BAD");
            for (int i = 0; i < 9; i++) {
              digitalWrite(redLED, HIGH);
              delay(200);
              digitalWrite(redLED, LOW);
              delay(200);
            }
            digitalWrite(greenLED, HIGH);
          }
        }
      }
      
    } else {
      Serial.println("Sending failed");
      digitalWrite(greenLED, HIGH);
    }

  }else if (key == '0') {
    flag = 0;

    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, LOW);
    delay(80);

    transm_data[1] = check_2;
    if ( radio.write(&transm_data, sizeof(transm_data)) ) {
      if (!radio.available()) {
        Serial.println("No answer");
        digitalWrite(greenLED, HIGH);
      } else {
        while (radio.available() ) {
          radio.read( &check_answer, 1);
          if (check_answer == 1) {
            flag = 2;
            for(int i = 0; i <= 9; i++) {
              digitalWrite(redLED, LOW);
              delay(50);
              digitalWrite(redLED, HIGH);
              delay(50);
            }
            Serial.println("Status OK");
          } else {
            Serial.println("Status BAD");
            for (int i = 0; i < 9; i++) {
              digitalWrite(redLED, HIGH);
              delay(200);
              digitalWrite(redLED, LOW);
              delay(200);
            }
            digitalWrite(greenLED, HIGH);
          }
        }
      }
      
    } else {
      Serial.println("Sending failed");
      digitalWrite(greenLED, HIGH);
    }

  } else if (key == '#') {
    flag = 0;
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
  }
}
