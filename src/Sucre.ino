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

// Unconnected mode ON, evita conexion wifi.
SYSTEM_MODE(SEMI_AUTOMATIC);

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

// True cuando el actuador no se ha usado en ningun Then (evalState=T) ni Else (evalState=F) de NINGUN bloque;
// Actuador valido tanto en el then como en el else, cuando no ha sido usado en ningun bloque para ese state.
// No puedo poner el led verde ON en then de bloque 1 a la vez que led rojo OFF en then de bloque 2.
// Si es posible poner led verde ON en then de bloque 1 y led rojo ON en else de bloque 2.
bool isValidActuador(bool evalState, int actuadorID)
{
  for (int i = 0; i <= numBloque; i++)
  {
    BLOQUE bloque = bloques[i];
    for (int j = 0; j < bloques[i].numActuadores; j++)
    {
      ACTUADOR actuador = bloques[i].actuadores[j];
      if (actuador.evaluate == evalState && actuador.id == actuadorID)
        return false;
    }
  }
  return true;
}

// -1 si no ha sido usado en ningun bloque, o el puerto donde se encuantra conectado.
int isNewActuador(int deviceID)
{
  for (int j = 0; j <= numBloque; j++)
  {
    for (int i = 0; i < bloques[j].numActuadores; i++)
    {
      ACTUADOR actuador = bloques[numBloque].actuadores[i];

      if (actuador.id == deviceID)
      {
        return actuador.puerto;
      }
    }
  }

  return -1;
}

// True si el actuador es usado tanto para THEN como ELSE; Necesario para no apagarlo y encenderlo constantemente.
bool isActuadorDual(int deviceID)
{
  int contador = 0;
  for (int i = 0; i < numActuadoresBloque; i++)
  {
    if (bloques[numBloque].actuadores[i].id == deviceID)
      contador++;

    if (contador > 1)
      return true;
  }

  return false;
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
      // return;
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

    switch (tagInfo[0]) {

    // Sensor
    case 0:

      Serial.println("Sensor detectado");

      if ( MODE == 0 ) {
        IF_pasado = true;
        numBloque = 0;
        numSensoresBloque = 0;
      }

      if ( IF_pasado && (numSensoresBloque == numCondicionalesBloque) && isValidSensor(deviceID) ) {

        int puerto = isNewSensor(deviceID);
        // Si el puerto es distinto de -1 el sensor ha sido usado previamente. Si es nuevo, obtenemos un puerto disponible.
        if (puerto == -1)
          puerto = asignarPuerto(deviceID, tagInfo[1]);

        if (puerto != -1) {

          SENSOR newSensor;
          newSensor.id = deviceID;
          newSensor.condicion = tagInfo[3];
          newSensor.bloque = numBloque;
          newSensor.puerto = puerto;

          bloques[numBloque].sensores[numSensoresBloque] = newSensor;
          numSensoresBloque++;

          displayPrint(esSensor(tagInfo[0]), esAnalogico(tagInfo[1]), newSensor.id, newSensor.condicion, newSensor.puerto);
        
        } else {

          Serial.println("Error Puerto");
        
        }

      } else {

        if ((!IF_pasado) && (MODE == 1)) {

          Serial.println("Se esperaba IF tag");
          break;
          
        }

        if (numSensoresBloque != numCondicionalesBloque) {

            Serial.println("Despues de un sensor se espera una concion: AND u OR");
        
        }
      
      }
      
      break;

    // Actuador: puede tratarse de un actuador de condicion TRUE o FALSE (para ser usado en el then o el else);
    case 1:

      Serial.println("Actuador detectado");

      if ( MODE == 0 ) {
        THEN_pasado = true;
        numBloque = 0;
        numActuadoresBloque = 0;
      }

      //  Tag ActuadorTrue: Secuencia actuadores cuando sensores del bloque evaluate a True
      if ( THEN_pasado && !ELSE_pasado && isValidActuador(true, deviceID) ) {

        int puerto = isNewActuador(deviceID);

        if (puerto == -1)
          puerto = asignarPuerto(deviceID, tagInfo[1]);

        // Si el puerto es distinto de -1 el actuador ha sido asignado correctamente.
        if (puerto != -1) {

          ACTUADOR newActuador;
          newActuador.id = deviceID;
          newActuador.condicion = tagInfo[3];
          newActuador.bloque = numBloque;
          newActuador.puerto = puerto;
          newActuador.evaluate = true;

          bloques[numBloque].actuadores[numActuadoresBloque] = newActuador;
          numActuadoresBloque++;
          bloques[numBloque].numActuadores++;

          // displayPrint(esSensor(tagInfo[0]), esAnalogico(tagInfo[1]), newActuador.id, newActuador.condicion, newActuador.puerto);
        }

      //  Tag ActuadorFalse: Secuencia actuadores cuando sensores del bloque evaluate a False
      } else if ( THEN_pasado && ELSE_pasado && isValidActuador(false, deviceID) ) {

        int puerto = isNewActuador(deviceID);

        if (puerto == -1)
          puerto = asignarPuerto(deviceID, tagInfo[1]);

        // Si el puerto es distinto de -1 el actuador ha sido asignado correctamente.
        if (puerto != -1) {

          ACTUADOR newActuador;
          newActuador.id = deviceID;
          newActuador.condicion = tagInfo[3];
          newActuador.bloque = numBloque;
          newActuador.puerto = puerto;
          newActuador.evaluate = false;

          bloques[numBloque].actuadores[numActuadoresBloque] = newActuador;
          bloques[numBloque].numActuadores++;
          numActuadoresBloque++;

          // displayPrint(esSensor(tagInfo[0]), esAnalogico(tagInfo[1]), newActuador.id, newActuador.condicion, newActuador.puerto);
        }

      } else {

        if (!IF_pasado) {
          Serial.println("Se esperaba IF tag");
          break;
        }

        Serial.println("Invalid Actuador");
        
      }

      break;

    // IF: Inicio de un bloque, fin secuencia ActuadoresFalse
    case 2:

      Serial.println("IF detectado");

      // Si está el modo KIDS activo no se permite IF
      if (MODE == 0) {

        Serial.println("Tag inválida para el modo KIDS");

      } else {

        if (numBloque == -1 || numBloque == 0 && numActuadoresBloque > 0) {

          numBloque++;
          IF_pasado = true;
          THEN_pasado = false;
          ELSE_pasado = false;

          numCondicionalesBloque = 0;
          numSensoresBloque = 0;
          numActuadoresBloque = 0;

          BLOQUE nuevoBloque;
          bloques[numBloque] = nuevoBloque;
        
        } else {

          Serial.println("Numero de bloques > 2 || Se necesita al menos un actuador para terminar el bloque");
        
        }

      }
      break;

    // AND/OR: Condicion entre sensores
    case 3:

      Serial.println("AND/OR detectado");

      if (MODE == 0) {

        Serial.println("Tag inválida para el modo KIDS");

      } else {

        if (numCondicionalesBloque < numSensoresBloque && numActuadoresBloque == 0) {

          // Tag condicional => 3#0 | 3#1 == OR | AND
          bloques[numBloque].condiciones.condicionesBloque[numCondicionalesBloque] = tagInfo[1];
          numCondicionalesBloque++;
        } else {

          if (!IF_pasado) {

            Serial.println("Se esperaba IF tag");
            break;

          }
          Serial.println("Una concicion solo puede ir seguida de un sensor.");

        }
      }
      break;

    // THEN: Fin secuencia sensores
    case 4:

      Serial.println("THEN detectado");

      if (MODE == 0) {

        Serial.println("Tag inválida para el modo KIDS");

      } else {

        if (numSensoresBloque > 0 && numSensoresBloque > numCondicionalesBloque) {

          THEN_pasado = true;
        
        } else {

          if (!IF_pasado) {

            Serial.println("Se esperaba IF tag");
            break;

          }
          Serial.println("Numero de sensores infucientes");
        }
      }
      break;

    // ELSE: Fin secuencia actuadores TRUE
    case 5:

      Serial.println("ELSE detectado");

      if (MODE == 0) {

        Serial.println("Tag inválida para el modo KIDS");

      } else {

        if (numActuadoresBloque > 0) {

          ELSE_pasado = true;
        
        } else {

          if (!IF_pasado) {

            Serial.println("Se esperaba IF tag");
            break;
          
          }

        }
      }
      break;

    default:

      Serial.println("ID Tag incorrecta");
      break;

    }

    Serial.println("Fin Tag");
    Serial.println("# Bloque | # Sensores | # Condiciones | # Actuadores");
    Serial.printlnf("    %d \t|\t %d \t|\t %d \t|\t %d \t|\t %d", numBloque, numSensoresBloque, numCondicionalesBloque, numActuadoresBloque, bloques[numBloque].numActuadores);
    // Serial.printlnf("Num bloques: %d", numBloque);
    // Serial.printlnf("Num sensoresBLoque: %d", numSensoresBloque);
    // Serial.printlnf("Num condicionesBLoque: %d", numCondicionalesBloque);
    // Serial.printlnf("Num ActuadoresBloque: %d", numActuadoresBloque);

    tagInfo[0] = -1;
    Serial.println();
  }

  display.display();

  for (int i = 0; i <= numBloque; i++)
  {
    // Para cada iterazion del loop debemos evaluar los sensores de cada bloque y actuar en consecuencia.
    if (THEN_pasado)
    {
      bool evaluacion = makeEvaluate(bloques[i].sensores, bloques[i].condiciones.condicionesBloque);
      display.setCursor(0, 0);
      display.clearDisplay();
      display.print(evaluacion ? "True" : "False");
      for (int j = 0; j < numActuadoresBloque; j++)
      {
        ACTUADOR actuador = bloques[i].actuadores[j];
        // Serial.printlnf("Actuandor: %d , %s", actuador.id, actuador.evaluate ? "True" : "False");
        if (evaluacion == actuador.evaluate)
        {
          // Serial.println("ActivarActuador");
          actuadorHandler(actuador.id, actuador.condicion, actuador.puerto);
        }
        else
        {
          if (!isActuadorDual(actuador.id))
          {
            // Serial.println("ApagarActuador");
            apagarActuador(actuador.id, actuador.puerto);
          }
          else
          {
            // Serial.printlnf("%d:%d -> Actuador se usa dos veces", actuador.id, actuador.condicion);
          }
        }
      }
    }
  }
}
// Fin loop