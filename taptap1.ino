/*
   Copyright (c) 2015 Intel Corporation.  All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <CurieBLE.h>

BLEPeripheral blePeripheral; // BLE Peripheral Device (the board you're programming)
BLEService tapService("7c3f5818-3255-4307-b138-158e09ec8130"); // BLE tap Service

/// BLE Switch Characteristic
BLEUnsignedCharCharacteristic switchCharacteristic("f71d47a6-fb4e-4c87-9be9-1b2bea79a2db", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  pinMode(10,OUTPUT);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  
  // set advertised local name and service UUID:
  blePeripheral.setLocalName("TapTap1");
  blePeripheral.setAdvertisedServiceUuid(tapService.uuid());
  
  // add service and characteristic:
  blePeripheral.addAttribute(tapService);
  blePeripheral.addAttribute(switchCharacteristic);
  
  // set the initial value for the characeristic:
  switchCharacteristic.setValue(0);
  
  // begin advertising BLE service:
  blePeripheral.begin();
  Serial.println("BLE Peripheral");
}

boolean pressed = false;
int i=1;

void loop() {
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {  
      int sw = digitalRead(4);
      int back = digitalRead(5);
      
      if (sw==HIGH){
        if(pressed==false){
          i=0;
          switchCharacteristic.setValue(i);
          Serial.print("up");
          i=1;
          pressed=true;
        }      
      }
      else if(sw==LOW){
        if (pressed==true){
          switchCharacteristic.setValue(i);
          Serial.print("down");
        }
        pressed=false;
      }

      if (back==LOW){    
          switchCharacteristic.setValue(2);
          // if the remote device wrote to the characteristic,
          // 값을 사용하여 진동모터 제어
          if (switchCharacteristic.written()) {
            if (switchCharacteristic.value() == 3) {
              digitalWrite(10,100);
              delay(500);
              digitalWrite(10,LOW); // sets the digital pin 10 off
              switchCharacteristic.setValue(0);
            }
          }
      }
    }
    
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}


