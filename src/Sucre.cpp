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
bool isValidSensor(int deviceID);
int isNewSensor(int deviceID);
bool isValidActuador(bool evalState, int actuadorID);
int isNewActuador(int deviceID);
void loop();
#line 24 "/Users/marcosgarciagarcia/Documents/Sucre/src/Sucre.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

// *** Variables de Entorno ***
Bloque bloques[2];

bool IF_pasado = false;
bool THEN_pasado = false;
bool ELSE_pasado = false;

int numBloque = -1;
int numCondicionalesBloque = 0;
int numSensoresBloque = 0;
int numActuadoresBloque = 0;

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

  // initializeBLocks(bloques);
}

bool evaluate(SENSOR sensor[], bool condicion[])
{
  if (numSensoresBloque >= 2 && numCondicionalesBloque >= 1)
  {
    if (numCondicionalesBloque < numSensoresBloque)
    {
      bool valorEvaluado = leerSensor(sensor[0].id, sensor[0].condicion, sensor[0].puerto);
      for (int i = 0; i < numCondicionalesBloque - 1; i++)
      {
        struct SENSOR sigSensor = sensor[i];
        int nextValor = leerSensor(sigSensor.id, sigSensor.condicion, sigSensor.puerto);

        if (condicion[i])
          valorEvaluado = valorEvaluado && nextValor;
        else
          valorEvaluado = valorEvaluado || nextValor;
      }
      // Serial.printlnf("Evaluate: %s", valorEvaluado ? "True" : "False");
      return valorEvaluado;
    }
    else
    {
      // Serial.println("Evaluate: numCondicionalesBloque > numSensoresBloque");
    }
  }

  // Serial.println("Evaluate: inficientes sensores");
  return false;
}

// TRUE Si el disposivo no ha sido utilizado en el bloque ACTUAL.
bool isValidSensor(int deviceID)
{
  // Serial.printlnf("deviceID:%d", deviceID);

  BLOQUE bloque = bloques[numBloque];
  for (int i = 0; i < numSensoresBloque; i++)
  {
    // Serial.printlnf("SensorBloqueID:%d", bloque.sensores[i].id);
    if (bloque.sensores[i].id == deviceID)
    {
      Serial.println("Invalid sensor! -> Sensor repetido en el bloque");
      return false;
    }
  };

  // Serial.println("Valid sensor!");
  return true;
}

// -1 si el sensor es nuevo, o el puerto donde esta siendo usado.
int isNewSensor(int deviceID)
{
  for (int i = 0; i <= numBloque; i++)
  {
    BLOQUE bloque = bloques[i];
    for (int j = 0; i < sizeof(bloque.actuadores); i++)
    {
      if (bloque.sensores[j].id == deviceID)
        return bloque.sensores[j].puerto;
    }
  }
  return -1;
}

// True cuando el actuador no se ha usado en ningun Then, evalState=T o, Else evalState=F de NINGUN bloque;
// Actuador valido tanto en el then como en el else, cuando no ha sido usado en ningun bloque para ese state.
// No puedo poner el led verde ON en then de bloque 1 a la vez que led rojo OFF en then de bloque 2.
// Si es posible poner led verde ON en then de bloque 1 y led rojo ON en else de bloque 2.
bool isValidActuador(bool evalState, int actuadorID)
{
  for (int i = 0; i <= numBloque; i++)
  {
    BLOQUE bloque = bloques[i];
    for (int j = 0; i < sizeof(bloque.actuadores); i++)
    {
      if (bloque.actuadores[j].evaluate == evalState && bloque.actuadores[j].id == actuadorID)
        return false;
    }
  }
  return true;
}

// -1 si no ha sido usado en ningun bloque, o el puerto donde se encuantra conectado.
int isNewActuador(int deviceID)
{
  for (int i = 0; i <= numBloque; i++)
  {
    BLOQUE bloque = bloques[i];
    for (int j = 0; i < sizeof(bloque.actuadores); i++)
    {
      if (bloque.actuadores[j].id == deviceID)
        return bloque.actuadores[j].puerto;
    }
  }

  return -1;
}

void loop()
{
  // If tag detected
  if (mfrc522.PICC_IsNewCardPresent())
  {
    Serial.print("Tag Detectada --> ");
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

  // Si se ha pasado nueva tag; se limpia al finalizar su lectura.
  if (tagInfo[0] != -1)
  {
    int deviceID = tagInfo[2];
    Serial.print("Ejecutando tag --> ");

    switch (tagInfo[0])
    {
    // Sensor
    case 0:
      if (IF_pasado && (numSensoresBloque == numCondicionalesBloque) && isValidSensor(deviceID))
      {

        int puerto = isNewSensor(deviceID);
        // Si el puerto es distinto de -1 el sensor ha sido usado previamente. Si es nuevo, obtenemos un puerto disponible.
        if (puerto == -1)
          puerto = asignarPuerto(deviceID);

        if (puerto != -1)
        {

          SENSOR newSensor;
          newSensor.id = deviceID;
          newSensor.condicion = tagInfo[3];
          newSensor.bloque = numBloque;
          newSensor.puerto = puerto;

          bloques[numBloque].sensores[numSensoresBloque] = newSensor;
          numSensoresBloque++;

          displayPrint(esSensor(tagInfo[0]), esAnalogico(tagInfo[1]), newSensor.id, newSensor.condicion, newSensor.puerto);
        }
        else
        {
          Serial.println("Error Puerto");
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

    // Actuador: puede tratarse de un actuador de condicion TRUE o FALSE (para ser usado en el then o el else);
    case 1:
      //  Tag ActuadorTrue: Secuencia actuadores cuando sensores del bloque evaluate a True
      if (THEN_pasado && !ELSE_pasado && isValidActuador(true, deviceID))
      {
        int puerto = isNewActuador(deviceID);
        if (puerto == -1)
          puerto = asignarPuerto(deviceID);

        // Si el puerto es distinto de -1 el actuador ha sido asignado correctamente.
        if (puerto != -1)
        {
          ACTUADOR newActuador;
          newActuador.id = deviceID;
          newActuador.condicion = tagInfo[3];
          newActuador.bloque = numBloque;
          newActuador.puerto = puerto;
          newActuador.evaluate = true;

          bloques[numBloque].actuadores[numActuadoresBloque] = newActuador;
          numActuadoresBloque++;

          displayPrint(esSensor(tagInfo[0]), esAnalogico(tagInfo[1]), newActuador.id, newActuador.condicion, newActuador.puerto);
        }
      }

      //  Tag ActuadorFalse: Secuencia actuadores cuando sensores del bloque evaluate a False
      else
      {
        if (THEN_pasado && ELSE_pasado && isValidActuador(false, deviceID))
        {
          int puerto = isNewActuador(deviceID);
          if (puerto == -1)
            puerto = asignarPuerto(deviceID);

          // Si el puerto es distinto de -1 el actuador ha sido asignado correctamente.
          if (puerto != -1)
          {
            ACTUADOR newActuador;
            newActuador.id = deviceID;
            newActuador.condicion = tagInfo[3];
            newActuador.bloque = numBloque;
            newActuador.puerto = puerto;
            newActuador.evaluate = false;

            bloques[numBloque].actuadores[numActuadoresBloque] = newActuador;
            numActuadoresBloque++;

            displayPrint(esSensor(tagInfo[0]), esAnalogico(tagInfo[1]), newActuador.id, newActuador.condicion, newActuador.puerto);
          }
        }
        else
        {
          Serial.println("Invalid Actuador");
        }
      }

      break;

    // IF: Inicio de un bloque, fin secuencia ActuadoresFalse
    case 2:
      if (numBloque == -1 || numBloque == 0 && numActuadoresBloque > 0)
      {
        numBloque++;
        IF_pasado = true;
        THEN_pasado = false;
        ELSE_pasado = false;

        numCondicionalesBloque = 0;
        numSensoresBloque = 0;
        numActuadoresBloque = 0;

        BLOQUE nuevoBloque;
        bloques[numBloque] = nuevoBloque;
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
        bloques[numBloque].condiciones.condicionesBloque[numCondicionalesBloque] = tagInfo[1];
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

    Serial.println("Fin Tag");
    Serial.printlnf("Num bloques: %d", numBloque);
    Serial.printlnf("Num sensoresBLoque: %d", numSensoresBloque);
    Serial.printlnf("Num condicionesBLoque: %d", numCondicionalesBloque);
    Serial.printlnf("Num CondicionalesBloque: %d", numActuadoresBloque);

    tagInfo[0] = -1;
    Serial.println();
  }

  for (int i = 0; i <= numBloque; i++)
  {
    // Para cada iterazion del loop debemos evaluar los sensores de cada bloque y actuar en consecuencia.

    bool evaluacion = evaluate(bloques[i].sensores, bloques[i].condiciones.condicionesBloque);
    for (int j = 0; j < numActuadoresBloque; j++)
    {
      ACTUADOR actuador = bloques[i].actuadores[j];
      if (evaluacion == actuador.evaluate)
      {
        actuadorHandler(actuador.id, actuador.condicion, actuador.puerto);
      }
    }
  }
}
// Fin loop
