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

// Unconnected mode ON, evita conexion wifi.
void setup();
void loop();
#line 21 "/Users/marcosgarciagarcia/Documents/Sucre/src/Sucre.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

// *** Variables de Entorno ***
bool IF_pasado = false;
bool THEN_pasado = false;
bool ELSE_pasado = false;

int numBloque = 0;
int numCondicionalesBloque = 0;
int numSensoresBloque = 0;
int numActuadoresBloque = 0;

struct SENSOR
{
  int id;
  int condicion;
  int bloque;
  int puerto;
};
struct SENSOR sensoresBloque[20];

struct ACTUADOR
{
  int id;
  int condicion;
  int bloque;
  int puerto;
  String type;
};
struct ACTUADOR actuadoresBloque[20];

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
          struct SENSOR newSensor;
          newSensor.id = tagInfo[2];
          newSensor.condicion = tagInfo[3];
          newSensor.bloque = numBloque;
          newSensor.puerto = puerto;

          sensoresBloque[numSensoresBloque] = newSensor;
          numSensoresBloque++;

          displayPrint(esSensor(tagInfo[0]), esAnalogico(tagInfo[1]), newSensor.id, newSensor.condicion, newSensor.puerto);
        }
      }
      else
      {
        if (!IF_pasado)
          Serial.println("Se esperaba IF tag");

        if (numSensoresBloque != numCondicionalesBloque)
          Serial.println("Se esperaba condicional");
      }

      break;

    // Actuador: puede tratarse de un actuador de condicion TRUE o FALSE;
    case 1:
      //  Tag ActuadorTrue: Secuencia actuadores cuando sensores del bloque evualuen a True
      if (THEN_pasado && !ELSE_pasado)
      {
        // Si el puerto es distinto de -1 el actuador ha sido asignado correctamente.
        int puerto = asignarPuerto(tagInfo[1]);
        if (puerto != -1)
        {
          struct ACTUADOR newActuador;
          newActuador.id = tagInfo[2];
          newActuador.condicion = tagInfo[3];
          newActuador.bloque = numBloque;
          newActuador.puerto = puerto;

          actuadoresBloque[numActuadoresBloque] = newActuador;
          numActuadoresBloque++;

          // Mostramos Actuador en pantalla:
          displayPrint(esSensor(tagInfo[0]), esAnalogico(tagInfo[1]), newActuador.id, newActuador.condicion, newActuador.puerto);
        }
        break;
      }

      // Tag ActuadorFalse: Secuencia actuadores cuando sensores del bloque evualuen a False
      if (ELSE_pasado)
      {
      }
      break;

    // IF: Inicio de un bloque, fin secuencia ActuadoresFalse
    case 2:
      break;

    // AND/OR: Inicio de un bloque, fin secuencia ActuadoresFalse
    case 3:
      break;

    // THEN: Inicio de un bloque, fin secuencia ActuadoresFalse
    case 4:
      break;

    // ELSE: Inicio de un bloque, fin secuencia ActuadoresFalse
    case 5:
      break;

    default:
      Serial.println("Error");
      break;
    }

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
          // Actualizamos la cantidad de sensores en el sistema
          incrementarSensor(sensoresArray);
          // Serial.printlnf("\nSensores conectados: %d", numSensores(sensoresArray));

          // Indice del sensor.
          int indice = numSensores(sensoresArray) - 1;

          // Guardamos el ID
          idSensor[indice] = tagInfo[2];
          // Serial.printf("ID: %d ", idSensor[indice]);

          // Guardamos la condicion
          condicionSensor[indice] = tagInfo[3];
          // Serial.printf("Condicion: %d ", condicionSensor[indice]);

          // Guardamos el puerto asignado del sensor.
          puertosSensores[indice] = puerto;
          // Serial.printlnf("Puerto: %d", puertosSensores[indice]);

          // Mostramos Sensor en pantalla:
          displayPrint(esSensor(tagInfo[0]), esAnalogico(tagInfo[1]), idSensor[indice], condicionSensor[indice], puerto);
        }
      }
      else
      {
        // Si ya tenemos 2 sensores.
        Serial.println("Se espera actuador");
      }

      // Si la Tag es Actuador
    }
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

        // Mostramos Actuador en pantalla:
        displayPrint(esSensor(tagInfo[0]), esAnalogico(tagInfo[1]), tagInfo[2], tagInfo[3], puertoActuador);
      }
      else
      {
        Serial.println("Error: necesario al menos un sensor");
      }
    }

    tagInfo[0] = -1;
  }

  // Para iteacion del loop tenemos que leer los sensores y ejecutar actuadores, en caso de ser posible.
  int num = numSensores(sensoresArray);

  if (num > 0)
  {
    //  Leemos el valor del sensor 1;
    bool valueSensor1 = leerSensor(idSensor[0], condicionSensor[0], puertosSensores[0]);
    // Serial.printlnf("Sensor 1: %s", valueSensor1 ? "True" : "False");

    // Operacion nos permite juntar el output de dos sensores ya sea con AND u OR.
    int operacion = valueSensor1;

    // Por defecto operacion tipo AND a falta de tarjeta.
    // Por lo que solo si sensor1 TRUE y hay dos sensores, leemos el valor del sensor 2;
    if (num == 2 && valueSensor1)
    {
      bool valueSensor2 = leerSensor(idSensor[1], condicionSensor[1], puertosSensores[1]);
      // Serial.printlnf("Sensor 2: %s", valueSensor2 ? "True" : "False");

      // La asignacion de operacion podra verse modificada con una carta condicional (AND / OR)
      valueSensor2 ? operacion = true : operacion = false;
    }

    // Si tenemos un actuador.
    if (puertoActuador != -1)
    {
      // Ejecutamos el actuador (LED o Zumbador) con toda la informacion necesaria;
      tagInfo[2] == 0 ? activarLED(tagInfo[3], puertoActuador, operacion) : activarZumbador(tagInfo[3], puertoActuador, operacion);
    }
  }

  display.display();
}
// Fin loop
