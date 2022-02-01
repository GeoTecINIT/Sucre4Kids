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
#include <actuadores.h>
#include <sensores.h>

// Unconnected mode ON
#line 24 "/Users/marcosgarciagarcia/Documents/Sucre/src/Sucre.ino"
void setup();
void loop();
#line 24 "/Users/marcosgarciagarcia/Documents/Sucre/src/Sucre.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

bool sensoresArray[2] = {false, false};
char *puertosSensores[2] = {"-1", "-1"};

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
    // Leemos la tag y guardamos la informacion codificada en tagInfo.
    getTagID(tagInfo);

    // Ejemplo de informacion codificada.
    Serial.println(esSensor(tagInfo[0]) ? "Es Sensor" : "Es Actuador");
    Serial.println(esAnalogico(tagInfo[1]) ? "Es Analogico" : "Es Digital");

    // En caso de sensor:
    if (esSensor(tagInfo[0]))
    {
      // Si no t enemos 2 sensores.
      if (numSensores(sensoresArray) < 2)
      {
        // Añadimos el sensor a un puerto disponible.
        char *puerto = asignarPuerto(tagInfo[1]);

        // Si el puerto no es un string vacio se ha sido asignado correctamente.
        if (strcmp(puerto, "") != 0)
        {
          // Actualizamos la cantidad de sensores en el sistema
          incrementarSensor(sensoresArray);
          Serial.printlnf("\nSensores conectados: %d", numSensores(sensoresArray));

          // Guardamos el puerto asignado del sensor.
          // Indice del sensor.
          int num = numSensores(sensoresArray) - 1;
          puertosSensores[num] = puerto;
        }
      }
      // Si ya tenemos 2 sensores.
      else
      {
        Serial.print("Ya hay dos sensores");
      }
    }
    // Actuador
    else
    {
      // Si teneos al menos un sensor;
      if (numSensores(sensoresArray) >= 1)
      {
        // Asignamos un puerto al actuador
        char *puerto = asignarPuerto(tagInfo[1]);

        // Si el puerto no es un string vacio se ha sido asignado correctamente.
        if (strcmp(puerto, "") != 0)
        {
          int num = numSensores(sensoresArray);
          // Leemos el valor del sensor 1;
          // bool valor1 = leerSensor();
          // Si hay 2 sensores, leemos el valor del sensor 2;
          // bool valor2 = leerSensor();

          // bool valor = valor1 && valor2;
          // Ejecutamos el actuador con todo ya calculado;
          // activarActuador(tagInfo[2], tagInfo[3], numSensores(sensoresArray), puertosSensores);
        }
      }
      else
      {
        Serial.println("Error: necesario al menos un sensor");
      }
    }

    display.display();
    Serial.println("***************");

  } // Fin loop
}
