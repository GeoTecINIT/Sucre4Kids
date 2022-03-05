/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "/Users/marcosgarciagarcia/Documents/Sucre/src/Sucre.ino"
// This #include statement was automatically added by the Particle IDE.
#include <Grove_ChainableLED.h>

// This #include statement was automatically added by the Particle IDE.
#include <MFRC522.h>

// OLED Screen Library
#include <Adafruit_SSD1306_RK.h>

#include "Particle.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Internal assets.
#include <controlador.h>
#include <actuadores.h>
#include <sensores.h>

// struct
#include <interface.h>

// Unconnected mode ON, evita conexion wifi.
void setup();
void loop();
#line 24 "/Users/marcosgarciagarcia/Documents/Sucre/src/Sucre.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

// *** Variables de Entorno ***
bool IF_pasado = false;
bool THEN_pasado = false;
bool ELSE_pasado = false;

int numBloque = -1;
int numCondicionalesBloque = 0;
int numSensoresBloque = 0;
int numActuadoresBloque = 0;

SENSORES_POR_BLOQUE sensoresPorBloque[2];

CONDICION_BLOQUE condicionesPorBloque[2];

ACTUADORES_POR_BLOQUE actuadoresPorBloque[2];

// El primer indice corresponde al primer sensor, y el segundo indice al posible segundo sensor.
// True if the sensor is conected.
bool sensoresArray[2] = {false, false};
// El id se obtiene del TagInfo[1] value.
int idSensor[2] = {-1, -1};

// La condicion se obtiene del TagInfo[2] value.
int condicionSensor[2] = {-1, -1};

// Mapea los puerto asignados al sensor.
int puertosSensores[2] = {-1, -1};

// Mapea el puerto asignado al actuador.
int puertoActuador = -1;

// Informacion de la tarjeta leida.
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
    switch (tagInfo[0])
    {
    // Sensor
    case 0:
      if (IF_pasado && (numSensoresBloque == numCondicionalesBloque))
      {
        // Si el puerto es distinto de -1 el sensor ha sido asignado correctamente.
        int puerto = asignarPuerto(tagInfo[1]);
        if (puerto != -1)
        {
          SENSOR newSensor;
          newSensor.id = tagInfo[2];
          newSensor.condicion = tagInfo[3];
          newSensor.bloque = numBloque;
          newSensor.puerto = puerto;

          sensoresPorBloque[numBloque].sensoresBloque[numSensoresBloque] = newSensor;
          numSensoresBloque++;

          displayPrint(esSensor(tagInfo[0]), esAnalogico(tagInfo[1]), newSensor.id, newSensor.condicion, newSensor.puerto);
        }
      }
      else
      {
        if (!IF_pasado)
          Serial.println("Se esperaba IF tag");

        if (numSensoresBloque != numCondicionalesBloque)
          Serial.println("Despues de un sensor se espera una concion: AND u OR");
      }

      break;

    // Actuador: puede tratarse de un actuador de condicion TRUE o FALSE;
    case 1:
      //  Tag ActuadorTrue: Secuencia actuadores cuando sensores del bloque evualuen a True
      if (THEN_pasado)
      {
        // Si el puerto es distinto de -1 el actuador ha sido asignado correctamente.
        int puerto = asignarPuerto(tagInfo[1]);
        if (puerto != -1)
        {
          ACTUADOR newActuador;
          newActuador.id = tagInfo[2];
          newActuador.condicion = tagInfo[3];
          newActuador.bloque = numBloque;
          newActuador.puerto = puerto;

          // Tag ActuadorFalse: Secuencia actuadores cuando sensores del bloque evualuen a False
          if (!ELSE_pasado)
            newActuador.actuadorTrue = true;
          else
            newActuador.actuadorTrue = false;

          actuadoresPorBloque[numBloque].actuadoresBloque[numActuadoresBloque] = newActuador;
          numActuadoresBloque++;

          // Mostramos Actuador en pantalla:
          displayPrint(esSensor(tagInfo[0]), esAnalogico(tagInfo[1]), newActuador.id, newActuador.condicion, newActuador.puerto);
        }
      }
      break;

    // IF: Inicio de un bloque, fin secuencia ActuadoresFalse
    case 2:
      if (numBloque == 0 || numBloque == 1 && numActuadoresBloque > 0)
      {
        numBloque++;

        IF_pasado = true;
        THEN_pasado = false;
        ELSE_pasado = false;

        numCondicionalesBloque = 0;
        numSensoresBloque = 0;
        numActuadoresBloque = 0;
      }
      else
      {
        Serial.println("Numero de bloques > 2 || Se necesita al menos un actuador para terminar el bloque");
      }
      break;

    // AND/OR: Condicion entre sensores
    case 3:
      if (numCondicionalesBloque < numSensoresBloque && numActuadoresBloque == 0)
      {
        // Tag condicional => 3#0 | 3#1 == OR | AND
        condicionesPorBloque[0].condicionesBloque[numCondicionalesBloque] = tagInfo[1];
        numCondicionalesBloque++;
      }
      else
      {
        Serial.println("Una concicion solo puede ir seguida de un sensor.");
      }
      break;

    // THEN: Fin secuencia sensores
    case 4:
      if (numSensoresBloque > 0 && numSensoresBloque > numCondicionalesBloque)
      {
        THEN_pasado = true;
      }
      else
      {
        Serial.println("Numero de sensores infucientes");
      }
      break;

    // ELSE: Fin secuencia actuadores TRUE
    case 5:
      if (numActuadoresBloque > 0)
      {
        ELSE_pasado = true;
      }
      else
      {
      }
      break;

    default:
      Serial.println("Error");
      break;
    }

    tagInfo[0] = -1;
  }

  // Para cada iterazion del loop debemos evaluar los sensores de cada bloque y actuar en consecuencia.
  for (int i = 0; i < numBloque; i++)
  {
    SENSOR *sensoresBloque = sensoresPorBloque[i].sensoresBloque;

    bool evaluacion = evaluate(sensoresPorBloque[i].sensoresBloque, condicionesPorBloque[i].condicionesBloque);
  }
}
// Fin loop
