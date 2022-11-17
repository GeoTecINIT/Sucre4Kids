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

// MODO de uso
uint8_t MODE;

// Ejecucion ACTIVA/INActiva
boolean play = false;

// Informacion de la tarjeta leida.
int tagInfo[6] = {-1, -1, -1, -1, -1, -1};
int id;     // [1]
int tipo;   // [2]
int estado; // [3]
boolean valor = false;

// Variable intermedia para almacenar el puerto
int puerto;

Bloque bloques[2];

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

  MODE = EEPROM.read(0);

  // EEPROM empty
  if ( MODE == 0xFF ) {
    
    Serial.println("EEPROM empty");
    MODE = 0;
    EEPROM.put(0, MODE);

  }

  if ( MODE == 0 ) {

    Serial.println("MODO BÁSICO iniciado");
    pinMode(Zumbador_PIN, OUTPUT);

    Bloque bloque;
    numBloque++;
    bloques[numBloque] = bloque;

  } else if ( MODE == 1 ) {

    Serial.println("MODO AVANZADO iniciado");

  }
  
}

// Declaramos función reset en dirección 0.
void(* resetFunc) (void) = 0;

// TRUE Si el disposivo no ha sido utilizado en el bloque ACTUAL.
bool isValidSensor(int deviceID)
{
  Bloque bloque = bloques[numBloque];
  for (int i = 0; i < bloque.numSensores; i++)
  {
    if (bloque.sensores[i].id == deviceID)
    {
      Serial.println("Invalid sensor! -> Sensor repetido en el bloque");
      return false;
    }
  };

  return true;
}

// -1 si el sensor es nuevo, o el puerto donde esta siendo usado.
int isNewSensor(int deviceID)
{
  for (int i = 0; i <= numBloque; i++)
  {
    for (int j = 0; j < bloques[i].numSensores; j++)
    {
      if (bloques[i].sensores[j].id == deviceID)
        return bloques[i].sensores[j].puerto;
    }
  }
  return -1;
}


bool isValidActuador(int deviceState, int actuadorID)
{

  for (int i = 0; i <= numBloque; i++)
  {
    // Comprobar que no se ha empleado el mismo estado en el bloque actual
    if ( i == numBloque ) 
    {
      for (int j = 0; j < bloques[i].numActuadores; j++)
      {
        Actuador actuador = bloques[i].actuadores[j];
        // Mismo actuador con el mismo estado
        if (actuador.condicion == deviceState && actuador.id == actuadorID)
          return false;
      }
    }
    
    // Comprobar que no se ha empleado en el bloque anterior, si existe
    if ( numBloque == 1)
    {
      for (int j = 0; j < bloques[0].numActuadores; j++)
      {
        Actuador actuador = bloques[0].actuadores[j];
        // Mismo actuador
        if (actuador.id == actuadorID)
          return false;
      }
      
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
      Actuador actuador = bloques[j].actuadores[i];

      if (actuador.id == deviceID)
      {
        return actuador.puerto;
      }
    }
  }

  return -1;
}

// True si el actuador es usado tanto para THEN como ELSE; Necesario para no apagarlo y encenderlo constantemente.
bool isActuadorDual(int deviceID, int bloque)
{
  int contador = 0;
  for (int i = 0; i < bloques[bloque].numActuadores; i++)
  {
    if (bloques[bloque].actuadores[i].id == deviceID)
      contador++;

    if (contador > 1)
      return true;
  }

  return false;
}


void ejecutarEvaluacion(bool evaluacion, int bloque) {

  for (int j = 0; j < bloques[bloque].numActuadores; j++)
  {
    Actuador actuador = bloques[bloque].actuadores[j];
    // Serial.printlnf("Actuandor: %d , %s", actuador.id, actuador.evaluate ? "True" : "False");
    if (evaluacion == actuador.evaluate)
    {
      // Serial.println("ActivarActuador");
      actuadorHandler(actuador.id, actuador.condicion, actuador.puerto);
    }
    
    else
    {
      if (!isActuadorDual(actuador.id, bloque))
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

    play = false;
    ledApagar();
    if ( MODE == 0 ) {
      pitidoOFF0();
    } else {
      pitidoOFF1(2);
      pitidoOFF1(4);
      pitidoOFF1(6);
    }
  }

  // Modo BASICO
  if ( MODE == 0 ) 
  {
    
    // Tipo de tarjeta
    switch (tagInfo[0])
    {

      // Tarjeta COMUN
      case 6:
        blinkAndSleep(true);
        switch (tagInfo[1])
        {
          // Cambio de MODO
          case 0:

            if (tagInfo[2] == 0 ) {

              Serial.println("Modo BASICO detectado");
              MODE = 0;

            } else if (tagInfo[2] == 1) {

              Serial.println("Modo AVANZADO detectado");
              MODE = 1;

            }

            EEPROM.put(0, MODE);
            ledApagar();
            pitidoOFF0();
            resetFunc();

            break;
          
          // Ejecucion
          case 1:
            play = true;
            tagInfo[0] = -1;
            break;

          default:
            break;
        }

        break;
      
      // Tarjeta MODO BASICO
      case 0:

        id = tagInfo[1];
        tipo = tagInfo[2];
        estado = tagInfo[3];

        // Si la tag corresponde a un sensor:
        if (id >= 2) {
          Serial.println("Sensor detectado");

          Sensor sensor;
          sensor.id = id;

          // Sensores: A0 y D2
          tipo == 0 ? sensor.puerto = 0 : sensor.puerto = 2;
          
          bloques[0].sensores[0] = sensor;
          bloques[0].numSensores++;
          numSensoresBloque++;

          blinkAndSleep(true);  // Zumbador: confirmación sonara al pasar un tag
          displayPrint0(id); // Actualizamos la información de la pantalla con el nuevo sensor.
          tagInfo[1] = -1;
          
        }

        // Si la tag corresponde a un actuador y se ha leido un sensor ( != -1 )
        if ( numSensoresBloque > 0) {

          if (tagInfo[1] == 0 || tagInfo[1] == 1) {
            Serial.println("Actuador detectado");

            Actuador actuador;
            actuador.id = id;

            // Actuadores: A2 y D4
            tipo == 0 ? actuador.puerto = 2 : actuador.puerto = 4;

            bloques[0].actuadores[0] = actuador;
            bloques[0].numActuadores++;
            numActuadoresBloque++;

            blinkAndSleep(true);    // Zumbador: confirmación sonara al pasar un tag
            displayPrint0(id); // Actualizamos la información de la pantalla con el nuevo sensor.
            tagInfo[0] = -1;
          }

        }
        else
        {
            ledApagar();
        }

        break;

      default:
        if ( tagInfo[0] != -1 ) {
          Serial.println("Tarjeta inválida para este modo");
          tagInfo[0] = -1;
        }
        break;
    }

    if (numActuadoresBloque > 0 && play == true)
    {
      valor = leerSensor(bloques[0].sensores[0].id, 1, bloques[0].sensores[0].puerto);
      activarActuador(bloques[0].actuadores[0], estado, valor);
    }

    // Mostramos la información que hayamos actualizado de la pantalla.
    display.display();
  
  // Modo AVANZADO
  } else {
    
    // Tipo de tarjeta
    switch (tagInfo[0])
    {

      // Tarjeta COMUN
      case 6: {

        switch (tagInfo[1])
        {
          //Cambio de modo
          case 0:
            
            if (tagInfo[2] == 0 ) {

              Serial.println("Modo BASICO detectado");
              MODE = 0;

            } else if (tagInfo[2] == 1) {

              Serial.println("Modo AVANZADO detectado");
              MODE = 1;

            }

            EEPROM.put(0, MODE);
            ledApagar();
            pitidoOFF1(2);
            pitidoOFF1(4);
            pitidoOFF1(6);
            resetFunc();

            break;
          
          default:
            break;
        }

        break;
      }
      // Tarjeta MODO AVANZADO
      case 1: {

        int deviceID = tagInfo[3];
        Serial.print("Ejecutando tag --> ");

        switch (tagInfo[1])
        {

          // Sensor
          case 0: {
            Serial.println("Sensor detectado");

            if ( IF_pasado && (numSensoresBloque == numCondicionalesBloque) && isValidSensor(deviceID) ) {

              int puerto = isNewSensor(deviceID);
              // Si el puerto es distinto de -1 el sensor ha sido usado previamente. Si es nuevo, obtenemos un puerto disponible.
              if (puerto == -1)
                puerto = asignarPuerto(tagInfo[2]);

              if (puerto != -1) {

                Sensor newSensor;
                newSensor.id = deviceID;
                newSensor.condicion = tagInfo[4];
                newSensor.bloque = numBloque;
                newSensor.puerto = puerto;

                bloques[numBloque].sensores[numSensoresBloque] = newSensor;
                bloques[numBloque].numSensores++;
                numSensoresBloque++;

                displayPrint(esSensor(tagInfo[1]), esAnalogico(tagInfo[2]), newSensor.id, newSensor.condicion, newSensor.puerto);
              
              } else {

                Serial.println("Error Puerto");
              
              }

            } else {

              if ((!IF_pasado)) {

                Serial.println("Se esperaba IF tag");
                break;
                
              }

              if (numSensoresBloque != numCondicionalesBloque) {

                  Serial.println("Despues de un sensor se espera una concion: AND u OR");
              
              }
            
            }
            
            break;
          }

          // Actuador: puede tratarse de un actuador de condicion TRUE o FALSE (para ser usado en el then o el else);
          case 1: {
            Serial.println("Actuador detectado");
            int deviceState = tagInfo[4];

            //  Tag ActuadorTrue: Secuencia actuadores cuando sensores del bloque evaluate a True
            if ( THEN_pasado && !ELSE_pasado && isValidActuador(deviceState, deviceID) ) {

              int puerto = isNewActuador(deviceID);

              if (puerto == -1)
                puerto = asignarPuerto(tagInfo[2]);

              // Si el puerto es distinto de -1 el actuador ha sido asignado correctamente.
              if (puerto != -1) {

                Actuador newActuador;
                newActuador.id = deviceID;
                newActuador.condicion = deviceState;
                newActuador.bloque = numBloque;
                newActuador.puerto = puerto;
                newActuador.evaluate = true;

                bloques[numBloque].actuadores[numActuadoresBloque] = newActuador;
                numActuadoresBloque++;
                bloques[numBloque].numActuadores++;

                displayPrint(esSensor(tagInfo[1]), esAnalogico(tagInfo[2]), newActuador.id, newActuador.condicion, newActuador.puerto);
              }

            //  Tag ActuadorFalse: Secuencia actuadores cuando sensores del bloque evaluate a False
            } else if ( THEN_pasado && ELSE_pasado && isValidActuador(deviceState, deviceID) ) {

              int puerto = isNewActuador(deviceID);

              if (puerto == -1)
                puerto = asignarPuerto(tagInfo[2]);

              // Si el puerto es distinto de -1 el actuador ha sido asignado correctamente.
              if (puerto != -1) {

                Actuador newActuador;
                newActuador.id = deviceID;
                newActuador.condicion = tagInfo[4];
                newActuador.bloque = numBloque;
                newActuador.puerto = puerto;
                newActuador.evaluate = false;

                bloques[numBloque].actuadores[numActuadoresBloque] = newActuador;
                bloques[numBloque].numActuadores++;
                numActuadoresBloque++;

                displayPrint(esSensor(tagInfo[1]), esAnalogico(tagInfo[2]), newActuador.id, newActuador.condicion, newActuador.puerto);
              }

            } else {

              if (!IF_pasado) {
                Serial.println("Se esperaba IF tag");

              } else if (!THEN_pasado) {
                Serial.println("Se esperaba THEN tag");
              
              } else {
                Serial.println("Invalid Actuador");
              }

            }

            break;
          }

          // IF: Inicio de un bloque, fin secuencia ActuadoresFalse
          case 2: {
            Serial.println("IF detectado");

            if (numBloque == -1 || ( numBloque == 0 && numActuadoresBloque > 0 ) ) {

              numBloque++;
              IF_pasado = true;
              THEN_pasado = false;
              ELSE_pasado = false;

              numCondicionalesBloque = 0;
              numSensoresBloque = 0;
              numActuadoresBloque = 0;

              Bloque nuevoBloque;
              bloques[numBloque] = nuevoBloque;
            
            } else {

              Serial.println("Numero de bloques > 2 || Se necesita al menos un actuador para terminar el bloque");
            
            }

            break;
          }

          // AND/OR: Condicion entre sensores
          case 3: {
            Serial.println("AND/OR detectado");

            if ( (numCondicionalesBloque < numSensoresBloque) && numActuadoresBloque == 0) {

              // Tag condicional => 3#0 | 3#1 == OR | AND
              bloques[numBloque].condiciones.condicionesBloque[numCondicionalesBloque] = tagInfo[2];
              numCondicionalesBloque++;
            } else {

              if (!IF_pasado) {

                Serial.println("Se esperaba IF tag");
                break;

              }
              Serial.println("Una concicion solo puede ir seguida de un sensor.");

            }
            
            break;
          }

          // THEN: Fin secuencia sensores
          case 4: {
            Serial.println("THEN detectado");

            if ((numSensoresBloque > 0) && (numSensoresBloque > numCondicionalesBloque)) {

              THEN_pasado = true;
            
            } else {

              if (!IF_pasado) {

                Serial.println("Se esperaba IF tag");
                break;

              }
              Serial.println("Numero de sensores infucientes");
            }
            
            break;
          }

          // ELSE: Fin secuencia actuadores TRUE
          case 5: {
            Serial.println("ELSE detectado");

            if (numActuadoresBloque > 0) {

              ELSE_pasado = true;
            
            } else {

              if (!IF_pasado) {

                Serial.println("Se esperaba IF tag");
                break;
              
              }

            }
            
            break;
          }

          default:
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

        break;
      }

      default: {
        if ( tagInfo[0] != -1 ) {
          Serial.println("Tarjeta inválida para este modo");
          tagInfo[0] = -1;
        }
        break;
      }

    }

    display.display();

    // Evaluación primer bloque
    if ( (numBloque==0 && THEN_pasado) || numBloque==1 ) {
      Bloque bloque = bloques[0];
      bool evaluacion = makeEvaluate(bloque);
      
      ejecutarEvaluacion(evaluacion, 0);
    }

    // Evaluación segundo bloque
    if ( numBloque==1 && THEN_pasado ) {
      Bloque bloque = bloques[1];
      bool evaluacion = makeEvaluate(bloques[1]);
      
      ejecutarEvaluacion(evaluacion, 1);
    }

  }

}

  