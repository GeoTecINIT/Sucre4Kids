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

#include <sensores.h>
#include <actuadores.h>
#include <interface.h>
#include <splash.h>

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

// Memory sector chosen to r/w fron NFC Tag
byte trailerBlock = 7;
byte blockAddr = 4;

Adafruit_SSD1306 display(-1);
char buf[64];

String dispositivos[2] = {"-", "-"};

//struct
typedef struct
{
   uint8_t pin;
   uint8_t pin2;

} disp;

//typedef struct port Port;
disp Disp[8];

// NFC Variables
#define RST_PIN D8 // constante para referenciar pin de reset
#define SS_PIN A3  // constante para referenciar pin de slave select
// #define WIDTH 80

// *** Variables de Entorno ***
bool IF_pasado = false;
bool THEN_pasado = false;
bool ELSE_pasado = false;

int numBloque = -1;
int numCondicionalesBloque = 0;
int numSensoresBloque = 0;
int numActuadoresBloque = 0;

// *** /Variables de Entorno ***

unsigned char data[] = {"6#2#2"}; //{"1#1#1#1#0#0"};
char delim[] = "#";

int puertoDigital = 3;
int puertoDigital_bloque = 0;
int puertoAnalogico = 0;
int puertoAnalogico_bloque = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

//--------------------------------  BORRADO  -------------------------------------

// Borrado ALL
void borradoALL(int modo)
{
   switch (modo)
   {
   case 0: {
      Bloque bloque;
      bloques[0] = bloque;
      numSensoresBloque = 0;
      numActuadoresBloque = 0;
      break;
   }
   case 1: {
      IF_pasado = false;
      THEN_pasado = false;
      ELSE_pasado = false;

      numBloque = -1;

      puertoDigital = 3;
      puertoAnalogico = 0;
      break;
   }
   default:
      break;
   }
}

// Borrado del bloque/loop actual
void borradoBLOQUE(int modo)
{
   switch (modo)
   {
   case 1:

      if (numBloque==0) {
         borradoALL(1);
      
      // Si borramos el segundo solo debemos resetear los puertos asignados en este
      } else if (numBloque==1) {
         IF_pasado = false;
         numBloque = 0;
         
         THEN_pasado = true;           //Para permitir la ejecucion del primer bloque
         numActuadoresBloque = 1;      //Para permitir iniciar el segundo bloque

         // Caso en el que se hayan completado los puertos en el segundo bloque
         if (puertoAnalogico == -1){   
            puertoAnalogico=3;
         }
         if (puertoDigital == -1){
            puertoDigital=6;
         }
         puertoAnalogico -= puertoAnalogico_bloque;
         puertoDigital -= puertoDigital_bloque;

      } else {
         Serial.println("Nada que borrar");
      }
      break;
   
   case 2:
      break;

   default:
      break;
   }
}

// Recibe el primer valor del tagInfo, true si es sensor (0) o false si actuador (1)
bool esSensor(int id)
{
   return id == 0;
}

// Recibe el segundo valor del tagInfo, true si es analogico (0) o false si digital (1)
bool esAnalogico(int type)
{
   return type == 0;
}

// TRUE Si el disposivo no ha sido utilizado en el bloque ACTUAL.
bool isValidSensor(int deviceID)
{
  for (int i = 0; i < bloques[numBloque].numSensores; i++)
  {
    if (bloques[numBloque].sensores[i].id == deviceID)
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
        // Mismo actuador con el mismo estado
        if (bloques[i].actuadores[j].condicion == deviceState && bloques[i].actuadores[j].id == actuadorID)
          return false;
      }
    }
    
    // Comprobar que no se ha empleado en el bloque anterior, si existe
    if ( numBloque == 1)
    {
      for (int j = 0; j < bloques[0].numActuadores; j++)
      {
        // Mismo actuador
        if (bloques[0].actuadores[j].id == actuadorID)
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
      if (bloques[j].actuadores[i].id == deviceID)
      {
        return bloques[j].actuadores[i].puerto;
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

// Evaluate recorre el vector de sensores, leyendo su valor y concatenandolo con el valor del siguiente
// en funcion de la condición que los une (AND u OR). Comenienzando por el primer sensor.
bool makeEvaluate(Bloque bloque)
{
   bool valorEvaluado = leerSensor(bloque.sensores[0].id, bloque.sensores[0].condicion, bloque.sensores[0].puerto);

   for (int i = 1; i < bloque.numSensores; i++)
   {
      Sensor sigSensor = bloque.sensores[i];
      bool nextValor = leerSensor(sigSensor.id, sigSensor.condicion, sigSensor.puerto);

      if (bloque.condiciones.condicionesBloque[i - 1])
         valorEvaluado = (valorEvaluado && nextValor);
      else
         valorEvaluado = (valorEvaluado || nextValor);
   }

   return valorEvaluado;
}

void ejecutarEvaluacion(bool evaluacion, int bloque) {

  for (int j = 0; j < bloques[bloque].numActuadores; j++)
  {
    Actuador actuador = bloques[bloque].actuadores[j];
    
    if (evaluacion == actuador.evaluate)
    {
      actuadorHandler(actuador.id, actuador.condicion, actuador.puerto);
    }
    
    else
    {
      if (!isActuadorDual(actuador.id, bloque))
      {
        apagarActuador(actuador.id, actuador.puerto);
      }
    }
    
  }

}


void cambioModo(int modo)
{
   if (modo == 0 ) {

      Serial.println("Modo BASICO detectado");
      MODE = 0;

   } else if (modo == 1) {

      Serial.println("Modo AVANZADO detectado");
      MODE = 1;

   }
   EEPROM.put(0, MODE);
}


void displayPrint0(int n)
{
   display.clearDisplay();

   switch (n)
   {
   case 0:
      dispositivos[0] = "Actuador RGB";
      break;

   case 1:
      dispositivos[0] = "Actuador Zumbador";
      break;

   case 2:
      dispositivos[1] = "Sensor de Luz";
      break;

   case 3:
      dispositivos[1] = "Sensor de Ruido";
      break;

   case 4:
      dispositivos[1] = "Sensor Boton";
      break;

   case 5:
      dispositivos[1] = "Sensor Rotativo";
      break;

   default:
      Serial.println("No es valido");
      break;
   }

   snprintf(buf, sizeof(buf), dispositivos[1]);
   display.println(buf);

   snprintf(buf, sizeof(buf), dispositivos[0]);
   display.println(buf);

   display.setCursor(0, 0);
}

void displayPrint(bool isSensor, bool isAnalogico, int id, int condicion, int puerto)
{
   display.clearDisplay();

   if (isSensor)
   {
      switch (id)
      {
      // Distancia
      case 1:
         condicion == 0 ? dispositivos[1] = "Sensor Poca Distancia" : dispositivos[1] = "Sensor Mucha Distancia";
         break;

      // Luz
      case 2:
         Serial.printlnf("OLED: Sensor Luz -> %d", condicion);
         // dispositivos[1] = "Sensor de Luz";
         condicion == 0 ? dispositivos[1] = "Sensor No Luz" : dispositivos[1] = "Sensor Si Luz";
         break;

      // Ruido
      case 3:
         // dispositivos[1] = "Sensor de Ruido";
         condicion == 0 ? dispositivos[1] = "Sensor No Ruido" : dispositivos[1] = "Sensor Si Ruido";
         break;

      // Boton
      case 4:
         condicion == 0 ? dispositivos[1] = "Sensor No Boton" : dispositivos[1] = "Sensor Si Boton";
         break;

      // Rotativo
      case 5:
         condicion == 0 ? dispositivos[1] = "Sensor No Rotativo" : dispositivos[1] = "Sensor Si Rotativo";
         break;

      // Temperatura
      case 6:
         switch (condicion)
         {
         case 0:
            dispositivos[1] = "Sensor Temp.Frio";
            break;
         case 1:
            dispositivos[1] = "Sensor Temp.Templado";
            break;
         case 2:
            dispositivos[1] = "Sensor Temp.Calor";
            break;
         }
         break;

      // Distancia
      case 7:
         condicion == 0 ? dispositivos[1] = "Sensor Distancia Cerca" : dispositivos[1] = "Sensor Distancia Lejos";

      default:
         Serial.println("SensorNoValidoError");
         break;
      }

      snprintf(buf, sizeof(buf), dispositivos[1] + (isAnalogico ? " A" : " D") + (String)puerto);
      display.println(buf);
   }
   else
   {
      // Actuadores.
      switch (id)
      {
      case 0:
         switch (condicion)
         {
         case 0:
            dispositivos[0] = "RGB Verde";
            break;
         case 1:
            dispositivos[0] = "RGB Rojo";
            break;
         case 2:
            dispositivos[0] = "RGB Amarillo";
            break;
         case 3:
            dispositivos[0] = "RGB Morado";
            break;
         case 4:
            dispositivos[0] = "RGB Azul";
            break;
         case 5:
            dispositivos[0] = "RGB Naranja";
            break;
         case 6:
            dispositivos[0] = "RGB Blink";
            break;
         case 7:
            dispositivos[0] = "RGB Rainbow";
            break;
         }
         break;

      case 1:
         condicion == 0 ? dispositivos[0] = "Zumbador On-Off" : dispositivos[0] = "Zumbador Blink";
         break;

      default:
         Serial.println("ActuadorNoValidoError");
         break;
      }
      
      snprintf(buf, sizeof(buf), dispositivos[0] + (isAnalogico ? " A" : " D") + (String)puerto);
      display.println(buf);

   }

   display.setCursor(0, 0);
}

void printArray(byte *buffer, byte bufferSize)
{
   for (byte i = 0; i < bufferSize; i++)
   {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
   }
}

int atoi(char *str)
{
   int num = 0;
   int i = 0;
   bool isNegetive = false;
   if (str[i] == '-')
   {
      isNegetive = true;
      i++;
   }

   while (str[i] && (str[i] >= '0' && str[i] <= '9'))
   {
      num = num * 10 + (str[i] - '0');
      i++;
   }

   if (isNegetive)
      num = -1 * num;
   return num;
}

void split(char cadena[], char delim[], int solution[])
{
   char *ptr = strtok(cadena, delim);
   int j = 0;
   while (ptr != NULL)
   {
      solution[j] = atoi(ptr);
      j++;
      ptr = strtok(NULL, delim);
   }
}

void print(int datos[])
{
   if (!datos[0])
   {
      Serial.printf("Es el sensor: %d\n", datos[1]);
      Serial.printf("Condición: %d\n", datos[2]);

      Serial.printf("Rango de %d a %d\n", datos[3], datos[4]);
   }
   else
   {
      Serial.printf("Es un actuador\n");
   }

   if (!datos[5])
      Serial.printf("Es analogico\n");

   else
      Serial.printf("Es digital\n");
}

void writeDataToBLock(byte blockAddr)
{
   status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(blockAddr, (byte *)data, 16);
   if (status != MFRC522::STATUS_OK)
   {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
   }
}

bool readDataFromBlock(byte blockAddr, byte buffer[], byte size)
{
   // Serial.print(F("Leer datos del sector ")); Serial.print(blockAddr);
   status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, buffer, &size);
   if (status != MFRC522::STATUS_OK)
   {
      Serial.print(F("MIFARE_Read() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return false;
   }
   return true;
}

void getTagID(int infoTag[])
{

   status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
   if (status != MFRC522::STATUS_OK)
   {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
   }

   // Write data to tag:
   //writeDataToBLock(blockAddr);

   // Read data from the block's Tag.
   byte buffer[18];
   byte size = sizeof(buffer);
   if (!readDataFromBlock(blockAddr, buffer, size))
   {
      return;
   }
   

   // Conversión y almacenamiento
   // Convert from hex to String:
   char tagInfo[16];
   int i = 0;
   for (byte j = 0; j < sizeof(buffer); j++)
   {
      tagInfo[i] = (char)buffer[j];
      i++;
      // Serial.write(buffer[j]);
      // tagInfo += Serial.write(buffer[j]);
   }
   Serial.printlnf("TagInfo: %s", tagInfo);
   split(tagInfo, delim, infoTag);

   // Evitamos seguir leyendo la misma tag.
   mfrc522.PICC_HaltA();
   // Stop encryption on PCD
   mfrc522.PCD_StopCrypto1();
}


void showBitmap(int mode) {
   display.clearDisplay();

   switch (mode)
   {
   case 0:
      display.drawBitmap(0,0, conexionBitmap, conexion_width, conexion_height, 1);
      break;
   
   default:
      break;
   }
   
   display.display();
   delay(1000);
}


int asignarPuerto(int type)
{
   int option;
   
   if (esAnalogico(type))
   {
      option = puertoAnalogico;
   }
   else
      option = puertoDigital;

   switch (option)
   {
   case 0:
      Serial.print("Conectar al puerto A0\n");
      puertoAnalogico++;
      puertoAnalogico_bloque++;
      Serial.printlnf("%d,%d",puertoAnalogico,puertoAnalogico_bloque);
      return 0;

   case 1:
      Serial.print("Conectar al puerto A2\n");
      puertoAnalogico++;
      puertoAnalogico_bloque++;
      Serial.printlnf("%d,%d",puertoAnalogico,puertoAnalogico_bloque);
      return 2;

   case 2:
      Serial.print("Conectar al puerto A4\n");
      puertoAnalogico = -1;
      puertoAnalogico_bloque++;
      Serial.printlnf("%d,%d",puertoAnalogico,puertoAnalogico_bloque);
      return 4;

   case 3:
      Serial.print("Conectar al puerto D2\n");
      puertoDigital++;
      puertoDigital_bloque++;
      return 2;

   case 4:
      Serial.print("Conectar al puerto D4\n");
      puertoDigital++;
      puertoDigital_bloque++;
      return 4;

   case 5:
      Serial.print("Conectar al puerto D6\n");
      puertoDigital = -1;
      puertoDigital_bloque++;
      return 6;

   default:
      Serial.print("Error: No hay mas puertos disponibles \n");
      return -1;
   }
}