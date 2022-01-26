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
SYSTEM_MODE(SEMI_AUTOMATIC);

bool sensoresArray[2] = {false, false};
void setup()
{
  // Unconnected mode ON
  Particle.disconnect();
  WiFi.off();

  // Particle setup
  Serial.begin(9600);
  SPI.begin();

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

    int tagInfo[6];
    getTagID(tagInfo);

    Serial.println(esSensor(tagInfo[0]) ? "Es Sensor" : "Es Actuador");
    Serial.println(esAnalogico(tagInfo[1]) ? "Es Analogico" : "Es Digital");
    if (esSensor(tagInfo[0]))
    {

      if (numSensores(sensoresArray) < 2)
      {

        char *puerto = asignarPuerto(tagInfo[1]);
        Serial.printlnf("Puerto: %s", puerto);

        if (strcmp(puerto, "") != 0)
        {
          incrementarSensor(sensoresArray);
        }
        Serial.printlnf("\nSensores conectados: %d", numSensores(sensoresArray));
      }
      else
      {
        // Ya hay dos sensores
        Serial.print("Ya hay dos sensores");
      }
    }
    else
    {
      if (numSensores(sensoresArray) >= 1)
      {
        // Actuador
      }
      else
      {
        Serial.println("Error: necesario al menos un sensor");
      }
    }

    display.display();

  } // Fin loop
}
