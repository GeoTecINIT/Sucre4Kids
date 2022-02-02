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
void setup();
void loop();
#line 24 "/Users/marcosgarciagarcia/Documents/Sucre/src/Sucre.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

// El primer indice corresponde al primer sensor, y el segundo indice al posible segundo sensor.
// True if the sensor is conected.
bool sensoresArray[2] = {false, false};
// TagInfo[1] value.
int idSensor[2] = {-1, -1};
// TagInfo[2] value.
int condicionSensor[2] = {-1, -1};
// Puerto asignado al sensor.
int puertosSensores[2] = {-1, -1};

int puertoActuador = -1;

int tagInfo[6] = {-1, -1, -1, -1, -1, -1};

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

    // Leemos la tag y guardamos la informacion codificada en tagInfo.
    getTagID(tagInfo);
  }

  // Si se ha pasado nueva tag:
  if (tagInfo[0] != -1)
  {
    // Ejemplo de informacion codificada.
    Serial.println(esSensor(tagInfo[0]) ? "Es Sensor" : "Es Actuador");
    Serial.println(esAnalogico(tagInfo[1]) ? "Es Analogico" : "Es Digital");

    // Si la tag es Sensor:
    if (esSensor(tagInfo[0]))
    {
      //  Si no tenemos 2 sensores.
      if (numSensores(sensoresArray) < 2)
      {
        // Añadimos el sensor a un puerto disponible.
        int puerto = asignarPuerto(tagInfo[1]);

        // Si el puerto es distinto de -1 el sensor ha sido asignado correctamente.
        if (puerto != -1)
        {
          // Serial.println("2");
          //  Actualizamos la cantidad de sensores en el sistema
          incrementarSensor(sensoresArray);
          Serial.printlnf("\nSensores conectados: %d", numSensores(sensoresArray));

          // Indice del sensor.
          int indice = numSensores(sensoresArray) - 1;

          // Guardamos el ID
          idSensor[indice] = tagInfo[2];
          Serial.printf("ID: %d ", idSensor[indice]);

          // Guardamos la condicion
          condicionSensor[indice] = tagInfo[3];
          Serial.printf("Condicion: %d ", condicionSensor[indice]);

          // Guardamos el puerto asignado del sensor.
          puertosSensores[indice] = puerto;
          Serial.printlnf("Puerto: %d", puertosSensores[indice]);
        }
      }
      else
      {
        // Si ya tenemos 2 sensores.
        Serial.println("Se espera actuador");
      }
    }

    // Si la Tag es Actuador
    else
    {
      //  Si teneos al menos un sensor;
      int num = numSensores(sensoresArray);
      if (num >= 1)
      {
        // Asignamos un puerto al actuador
        int puerto = asignarPuerto(tagInfo[1]);
        // Si el puerto es distinto de -1 el actuador ha sido asignado correctamente.
        if (puerto != -1)
        {
          puertoActuador = puerto;
        }
      }
      else
      {
        Serial.println("Error: necesario al menos un sensor");
      }
    }
    tagInfo[0] = -1;
  }

  int num = numSensores(sensoresArray);
  if (num > 0)
  {
    // Serial.printlnf("Sensores : %d", num);

    //  Leemos el valor del sensor 1;
    bool valueSensor1 = leerSensor(idSensor[0], condicionSensor[0], puertosSensores[0]);
    // Serial.printlnf("Sensor 1: %s", valueSensor1 ? "True" : "False");

    bool operacion = valueSensor1;
    // Si hay 2 sensores, leemos el valor del sensor 2;
    if (num == 2)
    {
      bool valueSensor2 = leerSensor(idSensor[1], condicionSensor[1], puertosSensores[1]);
      operacion = operacion && valueSensor2;
    }

    if (puertoActuador != -1)
    {
      //  Ejecutamos el actuador (LED o Zumbador) con todo ya calculado;
      tagInfo[2] == 0 ? activarLED(tagInfo[3], puertoActuador, valueSensor1) : activarZumbador(tagInfo[3], puertoActuador, valueSensor1);
    }
    // Serial.println();
  }

  display.display();

  // Fin loop
}
