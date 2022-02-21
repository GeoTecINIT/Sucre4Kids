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

// RGB LED
#define NUM_LEDS 5

// Memory sector chosen to r/w fron NFC Tag
byte trailerBlock = 7;
byte blockAddr = 4;

Adafruit_SSD1306 display(-1);
char buf[64];

String dispositivos[2] = {"-", "-"};

// struct
typedef struct
{
   uint8_t pin;
   uint8_t pin2;

} disp;

// typedef struct port Port;
disp Disp[8];

// NFC Variables
#define RST_PIN D8 // constante para referenciar pin de reset
#define SS_PIN A3  // constante para referenciar pin de slave select
// #define WIDTH 80

unsigned char data[] = {"0#1#7#1#0#1024"}; //{"1#1#1#1#0#0"};  //  //  {"0#1#4#1#0#1"}; //  // //
char delim[] = "#";

int puertoDigital = 3;
int puertoAnalogico = 0;

bool haveSensor = false;

boolean start = false;
boolean Sdigi = false;
boolean Adigi = false;

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

// Recibe el primer valor del tagInfo, true si es sensor (0) o false si actuador (1)
bool esSensor(int id)
{
   return id == 0;
}

// Recibe el segundo valor del tagInfo, true si es analogico (0) o false si digital (1)
bool esAnalogico(int id)
{
   return id == 0;
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
            dispositivos[0] = "RGB-Verde-Rojo";
            break;
         case 1:
            dispositivos[0] = "RGB-Amarillo-Morado";
            break;
         case 2:
            dispositivos[0] = "RGB-Azul-Naranja";
            break;
         case 3:
            dispositivos[0] = "RGB-Blink";
            break;
         case 4:
            dispositivos[0] = "RGB-RainBow";
            break;

         case 5:
            dispositivos[0] = "RGB-On-Off";
            break;
         }

      case 1:
         condicion == 0 ? dispositivos[0] = "Zumbador On-Off" : dispositivos[0] = "Zumbador Blink";
         break;

      default:
         Serial.println("ActuadorNoValidoError");
         break;
      }
   }

   snprintf(buf, sizeof(buf), dispositivos[1] + (isAnalogico ? " A" : " D") + (String)puerto);
   display.println(buf);

   snprintf(buf, sizeof(buf), dispositivos[0]);
   display.println(buf);

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
{ //}, int solution[], int j){
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

   if (!solution[0])
   {
      haveSensor = true;
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
   {
      Serial.printf("Es analogico\n");
   }
   else
   {
      Serial.printf("Es digital\n");
   }
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
   // writeDataToBLock(blockAddr);

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

int asignarPuerto(int id)
{
   int option;
   // Serial.printf("Digital %d, Analogico %d\n", puertoDigital, puertoAnalogico);

   if (esAnalogico(id))
   {
      // Serial.print(" \t y es analógico\n");
      option = puertoAnalogico;
   }
   else
   {
      // Serial.print(" \t y es digital\n");
      option = puertoDigital;
   }

   switch (option)
   {
   case 0:
      Serial.print("Conectar al puerto A0\n");

      puertoAnalogico++;
      // Disp[0].pin = A0;
      // Disp[0].pin2 = A1;

      // statements executed if the expression equals the   p_A0
      // value of this constant_expression                  p_A1
      return 0;
      break;

   case 1:
      Serial.print("Conectar al puerto A2\n");
      puertoAnalogico++;
      // Disp[0].pin = A2;
      // Disp[0].pin2 = A3;
      // statements executed if the expression equals the p_A2
      // value of this constant_expression                 p_A3
      return 2;
      break;

   case 2:
      Serial.print("Conectar al puerto A4\n");

      puertoAnalogico = -1;
      // Disp[0].pin = A4;
      // Disp[0].pin2 = A5;
      // statements executed if the expression equals the p_A4
      // value of this constant_expression                  p_A5
      return 4;
      break;

   case 3:
      Serial.print("Conectar al puerto D2\n");

      puertoDigital++;
      // Disp[0].pin = D2;
      //  Disp[0].pin2 = D3;
      //  statements executed if the expression equals the  p_D2
      //  value of this constant_expression                  p_D3
      return 2;
      break;

   case 4:
      Serial.print("Conectar al puerto D4\n");

      puertoDigital++;
      // Disp[0].pin = D4;
      // Disp[0].pin2 = D5;
      // statements executed if the expression equals the p_D4
      // value of this constant_expression               p_D5
      return 4;
      break;
   case 5:
      Serial.print("Conectar al puerto D6\n");
      puertoDigital = -1;
      // Disp[0].pin = D4;
      // Disp[0].pin2 = D5;
      // statements executed if the expression equals the p_D4
      // value of this constant_expression               p_D5
      return 6;
      break;
   default:
      Serial.print("Error: No hay mas puertos disponibles \n");
      return -1;
      // statements executed if expression does not equal
      // any case constant_expression
   }
}

// El numero de sensores representar la cantidad de elementos a true del vector.
int numSensores(bool array[])
{
   int contador = 0;
   int j = 0;

   for (j; j < 2; j++)
   {
      if (array[j])
         contador++;
   }

   return contador;
}

// Al tratarse de un vector booleano, incrementar la cantidad supone poner a true el siguiente false.
void incrementarSensor(bool array[])
{
   int i = 0;
   while (array[i])
      i++;

   array[i] = true;
}
