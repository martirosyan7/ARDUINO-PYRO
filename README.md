# Arduino Wireless Keypad and Receiver

This Arduino project consists of two parts: a wireless keypad (transmitter) and a receiver. The wireless communication is achieved using NRF24L01 radio modules.

## Keypad (Transmitter)

The keypad part of the project is responsible for reading input from a 4x3 matrix keypad and sending the pressed key data wirelessly to the receiver.

### Main Features

- Uses a 4x3 matrix keypad for input.
- Implements NRF24L01 radio communication for wireless data transfer.
- Utilizes LEDs to indicate the status of the transmission process.

### Setup

1. Connect the NRF24L01 radio module to pins 9 and 10 on the Arduino.
2. Connect the keypad to the specified pins and configure the rows and columns.
3. Set a unique address for the transmitter (keypad) in the `address` array.
4. Adjust parameters such as the cryptographic key, check codes, and LED pins as needed.

### Usage

- Press digits 0-9 on the keypad to send corresponding data to the receiver.
- Use the '*' key to trigger specific actions based on the flag state.
- '#' key resets the flag and turns on the green LED.

## Receiver

The receiver part of the project receives wireless data from the keypad and performs actions based on the received information.

### Main Features

- Monitors multiple NRF24L01 channels for incoming data.
- Controls MOSFETs based on the received data to toggle corresponding devices.
- Implements a voltage check on the battery level for protection.

### Setup

1. Connect MOSFETs to specified pins for device control.
2. Connect a battery to be monitored for voltage.
3. Set a unique address for the receiver in the `address` array.

### Usage

- The receiver listens for data from the keypad.
- Depending on the received data, it may toggle MOSFETs to control devices.
- It checks the battery voltage and responds to specific check codes from the keypad.

## Requirements

- Arduino boards for both transmitter and receiver.
- NRF24L01 radio modules for wireless communication.
- 4x3 matrix keypad for the transmitter.
- MOSFETs and devices for the receiver.
