/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "c:/Users/diego/Documents/VisualStudio/Sucre/src/Sucre.ino"
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

// Unconnected mode ON, evita conexion wifi.
void setup();
void loop();
#line 19 "c:/Users/diego/Documents/VisualStudio/Sucre/src/Sucre.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

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

  MODE = EEPROM.read(0);

  // EEPROM empty
  if ( MODE == 0xFF ) {
    
    Serial.println("EEPROM empty");
    MODE = 0;
    EEPROM.put(0, MODE);

  }

  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  if ( MODE == 0 ) {

    // MODO BÁSICO iniciado
    pinMode(Zumbador_PIN, OUTPUT);
    ledObject.init();
    init = true;

    Bloque bloque;
    numBloque++;
    bloques[numBloque] = bloque;

    showBitmap(0,0,"");

  } else if ( MODE == 1 ) {

    // MODO AVANZADO iniciado
    showBitmap(0,1,"");

  } else {

    // MODO MUSICA iniciado
    pinMode(Zumbador_PIN, OUTPUT);
    showBitmap(0,2,"");
  }
  
}

// Declaramos función reset en dirección 0.
void(* resetFunc) (void) = 0;


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
    // Leemos la tag y guardamos la informacion codificada en tagInfo.
    getTagID(tagInfo);

    play = false;
    if ( MODE == 0 ) {
      pitidoOFF0();
    } else {
      pitidoOFF1(2);
      pitidoOFF1(4);
      pitidoOFF1(6);
    }
  
  }

  if (!play && init) {
    ledApagar();
  }

  if (bitmap) {
    currentTime = millis();
    // Si han pasado 4 segundos quitamos el bitmap/mensaje
    if ( currentTime-startTime >= limit ) {
      bitmap=false;
      display.clearDisplay();
      display.display();
    }
  }

  // ------------------------------ Modo BASICO --------------------------------------
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
            
            cambioModo(tagInfo[2]);
            resetFunc();
            break;
          
          // Ejecucion
          case 1:
            play = true;
            showBitmap(3,0,"Ejecutando...");
            break;

          // Borrado (ALL)
          case 2:
            if ( tagInfo[2] == 1 ) {
              borradoALL(0);
              showBitmap(3,0,"Borrado completo realizado");

            } else {
              showBitmap(2,1,"");
              Serial.println("Borrado no permitido para este modo");
            }
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

          blinkAndSleep(true);
          
          if (sensor.puerto == 0) {
            showBitmap(1,3,""); //A0
          } else {
            showBitmap(1,0,""); //D2
          }
          //displayPrint0(id); // Update screen info
          
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

            blinkAndSleep(true);

            if (actuador.puerto == 0) {
              showBitmap(1,4,""); //A2
            } else {
              showBitmap(1,1,""); //D4
            }
            //displayPrint0(id);
          }

        }
        else
        {
            ledApagar();
        }

        break;

      default:
        if ( tagInfo[0] != -1 ) {
          showBitmap(2,0,"");
          Serial.println("Tarjeta inválida para este modo");
        }
        break;
    }

    if (tagInfo[0]!=-1) { 
      tagInfo[0]=-1;
    }

    if (numActuadoresBloque > 0 && play == true)
    {
      valor = leerSensor(bloques[0].sensores[0].id, 1, bloques[0].sensores[0].puerto);
      activarActuador(bloques[0].actuadores[0].id, estado, valor);
    }


  // ------------------------------- Modo AVANZADO --------------------------------------
  } else if ( MODE == 1 ) {
    
    // Tipo de tarjeta
    switch (tagInfo[0])
    {

      // Tarjeta COMUN
      case 6: {

        switch (tagInfo[1])
        {
          //Cambio de modo
          case 0:
            cambioModo(tagInfo[2]);
            resetFunc();
            break;
          

          // Ejecucion secuencia
          case 1:
            play = true;
            showBitmap(3,0,"Ejecutando...");
            break;


          // Borrado (ALL Y BLOQUE)
          case 2:
            if ( tagInfo[2] == 1 ) {
              borradoALL(1);
              showBitmap(3,0,"Borrado completo realizado");

            } else if (tagInfo[2] == 2) {
              borradoBLOQUE(1);

            } else {
              Serial.println("Borrado no permitido para este modo");
              showBitmap(2,1,"");
            }
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

                //displayPrint(esSensor(tagInfo[1]), esAnalogico(tagInfo[2]), newSensor.id, newSensor.condicion, newSensor.puerto);
                
                showPort(tagInfo[2], puerto);

              } else {

                Serial.println("Puerto no disponible");
                showBitmap(2,3,"");

              }

            } else {

              if ((!IF_pasado)) {

                Serial.println("Se esperaba IF tag");
                showBitmap(2,2,"");
                
              } else if (numSensoresBloque != numCondicionalesBloque) {

                Serial.println("Despues de un sensor se espera una concion: AND u OR");
                showBitmap(2,2,"");
              
              } else {
                
                Serial.println("Sensor invalido");
                showBitmap(2,5,"");
                // Sensor no disponible

              }
            
            }
            
            break;
          }

          // Actuador - then/else
          case 1: {
            Serial.println("Actuador detectado");
            int deviceState = tagInfo[4];

            //  Actuador then ( condicion = True )
            if ( THEN_pasado && !ELSE_pasado && isValidActuador(deviceState, deviceID) && numActuadoresBloque==0 ) {

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

                //displayPrint(esSensor(tagInfo[1]), esAnalogico(tagInfo[2]), newActuador.id, newActuador.condicion, newActuador.puerto);

                showPort(tagInfo[2], puerto);

                // Si es LED se inicia para mantenerlo apagado hasta el PLAY
                if (deviceID == 0)
                {
                  ledObject = ChainableLED(puerto, puerto+1, 5);
                  ledObject.init();
                  init = true;
                }
                
              } else {
                Serial.println("Puerto no disponible");
                showBitmap(2,3,"");
              }

            //  Actuador else ( condicion = False )
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

                //displayPrint(esSensor(tagInfo[1]), esAnalogico(tagInfo[2]), newActuador.id, newActuador.condicion, newActuador.puerto);

                showPort(tagInfo[2], puerto);

                // Si es LED se inicia para mantenerlo apagado hasta el PLAY
                if (deviceID == 0)
                {
                  ledObject = ChainableLED(puerto, puerto+1, 5);
                  ledObject.init();
                  init = true;
                }

              } else {
                Serial.println("Puerto no disponible");
                showBitmap(2,3,"");
              }

            } else {

              if (!IF_pasado) {
                Serial.println("Se esperaba IF tag");
                showBitmap(2,2,"");

              } else if (!THEN_pasado) {
                Serial.println("Se esperaba THEN tag");
                showBitmap(2,2,"");
              
              } else {

                if (!isValidActuador(deviceState, deviceID)) {
                  Serial.println("Invalid Actuador");
                  showBitmap(2,6,"");

                } else if(!ELSE_pasado) {
                  Serial.println("Se esperaba ELSE tag");
                  showBitmap(2,2,"");

                }
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

              puertoAnalogico_bloque = 0;
              puertoDigital_bloque = 0;

              Bloque nuevoBloque;
              bloques[numBloque] = nuevoBloque;

              showBitmap(3,0,"IF");
            
            } else {

              Serial.println("Numero de bloques > 2 || Se necesita al menos un actuador para terminar el bloque");
              showBitmap(2,2,"");
            
            }

            break;
          }

          // AND/OR: Condicion entre sensores
          case 3: {
            Serial.println("AND/OR detectado");

            if ( IF_pasado && (numCondicionalesBloque < numSensoresBloque) && numActuadoresBloque == 0) {

              // Tag condicional => 3#0 | 3#1 == OR | AND
              bloques[numBloque].condiciones.condicionesBloque[numCondicionalesBloque] = tagInfo[2];
              numCondicionalesBloque++;

              tagInfo[2]==0 ? showBitmap(3,0,"OR") : showBitmap(3,0,"AND");

              
            } else {

              if (!IF_pasado) {

                Serial.println("Se esperaba IF tag");
                showBitmap(2,2,"");

              } else if ( numSensoresBloque >= numCondicionalesBloque ) {

                Serial.println("Una concicion solo puede ir seguida de un sensor.");
                showBitmap(2,2,"");

              }

            }
            break;
          }

          // THEN: Fin secuencia sensores
          case 4: {
            Serial.println("THEN detectado");

            if ( IF_pasado && (numSensoresBloque > 0) && (numSensoresBloque > numCondicionalesBloque)) {

              THEN_pasado = true;
              showBitmap(3,0,"THEN");
            
            } else {

              if (!IF_pasado) {

                Serial.println("Se esperaba IF tag");
                showBitmap(2,2,"");

              } else {

                Serial.println("Numero de sensores infucientes");
                showBitmap(2,2,"");

              }
              
            }           
            break;
          }

          // ELSE: Fin secuencia actuadores TRUE
          case 5: {
            Serial.println("ELSE detectado");

            if ( IF_pasado && (numActuadoresBloque > 0)) {

              ELSE_pasado = true;
              showBitmap(3,0,"ELSE");
            
            } else {

              if (!IF_pasado) {

                Serial.println("Se esperaba IF tag");
                showBitmap(2,2,"");
              
              } else {
                
                Serial.println("Introduce un Actuador para la evaluación a TRUE");
                showBitmap(2,2,"");

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
        Serial.println();

        break;
      }

      default: {
        if ( tagInfo[0] != -1 ) {
          Serial.println("Tarjeta inválida para este modo");
          showBitmap(2,0,"");
        }
        break;
      }

    }

    tagInfo[0] = -1;
    display.display();

    if (play) {
      // Bloque 1
      if ( (numBloque==0 && THEN_pasado) || numBloque==1 ) {

        bool evaluacion = makeEvaluate(bloques[0]);
        ejecutarEvaluacion(evaluacion, 0);
      }

      // Bloque 2
      if ( numBloque==1 && THEN_pasado ) {
        bool evaluacion = makeEvaluate(bloques[1]);
        ejecutarEvaluacion(evaluacion, 1);
      }

    }
  
  // ------------------------------- Modo MUSICA --------------------------------------
  } else {

    switch (tagInfo[0])
    {

    // Tarjeta COMUN
    case 6:
      switch (tagInfo[1])
      {
        // Cambio de MODO
        case 0:
          
          cambioModo(tagInfo[2]);
          resetFunc();
          break;
        
        // Ejecucion
        case 1:
          play = true;
          showBitmap(3,0,"Ejecutando...");   
          break;

        // Borrado
        case 2:
          if ( tagInfo[2] == 0 ) {
            borradoPOP();
            showBitmap(3,0,"Borrada última lectura");

          } else if ( tagInfo[2] == 1 ){
            borradoALL(2);
            showBitmap(3,0,"Borrado completo realizado");

          } else {
            borradoBLOQUE(2);

          }
          break;

        default:
          break;
      }

      break;
    
    // Tarjeta MODO MUSICA
    case 2:

      switch (tagInfo[1])
      {

      // NOTA
      case 0:
        showBitmap(3,0,decodificarNOTA_msg(tagInfo[2])+decodificarTIPO_msg(tagInfo[3]));
        Serial.printlnf(decodificarNOTA_msg(tagInfo[2])+decodificarTIPO_msg(tagInfo[3]));

        reproducirNOTA(tagInfo[2], tagInfo[3]);
        notas[posicion] = tagInfo[2];
        duraciones[posicion] = tagInfo[3];
        
        posicion++;

        if (bucle) {
          tam_bucle++;
        }
        break;
      
      // LOOP
      case 1:
        if (!bucle) {
          Serial.println("Loop");
          showBitmap(3,0,"Loop");
          notas[posicion] = -1;
          duraciones[posicion] = 0;
          
          posicion++;
          bucle = true;

        } else {
          Serial.println("Para comenzar un loop hay que finalizar el anterior.");
          showBitmap(2,2,"");
        }
        break;

      // N iteraciones
      case 2:
        if (bucle && tam_bucle>0) {
          Serial.println("END Loop");
          showBitmap(3,0,String(tagInfo[2]+2)+" iteraciones");

          notas[posicion] = -2;
          duraciones[posicion] = tam_bucle; // Almacenamos el tamaño del bucle en la posicion donde acaba
          duraciones[posicion - (tam_bucle+1)] = tagInfo[2]+2; // Almacenamos el num de iteraciones en la posicion de inicio del bucle
          
          posicion++;
          bucle = false;
          tam_bucle = 0;

        } else {
          Serial.println("Loop sin comenzar o sin contenido");
          showBitmap(2,2,"");
        }
        break;

              
      default:
        break;
      }
      break;

    default:
      if ( tagInfo[0] != -1 ) {
        Serial.println("Tarjeta inválida para este modo");
        showBitmap(2,0,"");
      }
      break;
    }

    tagInfo[0] = -1;

    // Reproducir cancion
    if (play) {
      reproducir();
      play = false;
    }
  }

}

  