/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "/Users/marcosgarciagarcia/Documents/Sucre/src/Sucre.ino"


// This #include statement was automatically added by the Particle IDE.
#include <Grove_ChainableLED.h>

// This #include statement was automatically added by the Particle IDE.
//#include <Arduino.h>

// This #include statement was automatically added by the Particle IDE.
#include <MFRC522.h>

// OLED Screen Library
#include <Adafruit_SSD1306_RK.h>

#include "Particle.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Internal assets
#include <controlador.h>
// #include <actuadores.h>
// #include <sensores.h>

// Unconnected mode ON
void setup();
void loop();
#line 26 "/Users/marcosgarciagarcia/Documents/Sucre/src/Sucre.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

void setup()
{
  // Particle setup
  Serial.begin(9600);
  SPI.begin();
  // Unconnected mode ON
  Particle.disconnect();
  WiFi.off();

  // Oled setup
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // NFC Reader setup
  mfrc522.PCD_Init();
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }
}

void loop()
{
  // If tag detected
  if (mfrc522.PICC_IsNewCardPresent())
  {
    // Error in comunications, i.e Tag broken:
    if (!mfrc522.PICC_ReadCardSerial())
    {
      // Not sure if restart loop or finish program.
      return;
    }

    display.clearDisplay();
    display.setTextSize(1.5);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);

    int infoTag[6];
    getTagID(infoTag);

    Serial.println(infoTag[0] ? "Es Actuador" : "Es Sensor");
    Serial.println(infoTag[1] ? "Es Digital" : "Es Analogico");

    asignarPuerto(infoTag[1]);

    // for (int i = 0; i < sizeof(tagInfo); i++)
    // {
    //   Serial.println(tagInfo[i]);
    // }

    // if (isSensor(tagID)){
    //   if(isFisrtSensor(tagID)){

    //   //Second sensor
    //   }else{
    //     // only if logic and other sensor present
    //     if(Logic() && FisrtSensor()){
    //       //store the second sensor in sensorArray
    //     } else {
    //       // ERROR
    //     }
    //   }
    // }else{
    //   if(isLogic(tagID)){
    //     // only if one sensor present
    //     if(FisrtSensor()){

    //     } else {
    //       // ERROR
    //     }
    //   // is actuator
    //   }else{
    //     if(AtLeastOneSensor())

    //   }
    // }
  }

  // Analizamos la información de la tag leida.
  // Si la tag corresponde a un sensor:
  // if (infoVec[0] >= 2)
  // {
  //   sensor = infoVec[0];
  //   blinkAndSleep(true);  // Zumbador: confirmación sonara al pasar un tag
  //   displayPrint(sensor); // Actualizamos la información de la pantalla con el nuevo sensor.
  //   infoVec[0] = -1;
  // }

  // // Si la tag corresponde a un actuador
  // if (sensor != -1)
  // {
  //   valor = leerSensor(sensor);
  //   if (infoVec[0] == 0 || infoVec[0] == 1)
  //   {
  //     actuador = infoVec[0];
  //     blinkAndSleep(true);    // Zumbador: confirmación sonara al pasar un tag
  //     displayPrint(actuador); // Actualizamos la información de la pantalla con el nuevo sensor.
  //     infoVec[0] = -1;
  //   }
  // }
  // else
  // {
  //   ledApagar();
  // }

  // if (actuador != -1)
  // {
  //   activarActuador(actuador, infoVec[1], valor);
  // }

  // Mostramos la información que hayamos actualizado de la pantalla.
  display.display();

} // Fin loop
