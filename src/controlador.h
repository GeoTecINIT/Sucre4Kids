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
bool IF_pasado = false, THEN_pasado = false, ELSE_pasado = false;

int numBloque = -1;
int numCondicionalesBloque = 0, numSensoresBloque = 0, numActuadoresBloque = 0;

unsigned char data[] = {"6#1#0"};

String tarjetas_modoBasico[12] = { "0#2#0","0#3#0","0#4#1","0#1#1#0","0#1#1#1","0#0#1#0","0#0#1#1","0#0#1#2","0#0#1#3","0#0#1#4","0#0#1#5" };

String tarjetas_modoAvanzado[28] = { "1#0#1#7#0","1#0#1#7#1","1#0#0#2#0","1#0#0#2#1","1#0#0#3#0","1#0#0#3#1","1#0#1#4#0","1#0#1#4#1","1#0#1#5#0","1#0#1#5#1","1#0#1#6#0","1#0#1#6#1","1#0#1#6#2",
                        "1#1#1#1#0","1#1#1#1#1","1#1#1#0#0","1#1#1#0#1","1#1#1#0#2","1#1#1#0#3","1#1#1#0#4","1#1#1#0#5","1#1#1#0#6","1#1#1#0#7",
                        "1#2","1#3#0","1#3#1","1#4","1#5" };

String tarjetas_modoMusica[35] = {"2#0#0#0","2#0#0#1","2#0#0#2","2#0#1#0","2#0#1#1","2#0#1#2","2#0#2#0","2#0#2#1","2#0#2#2","2#0#3#0","2#0#3#1","2#0#3#2",
                        "2#0#4#0","2#0#4#1","2#0#4#2","2#0#5#0","2#0#5#1","2#0#5#2","2#0#6#0","2#0#6#1","2#0#6#2","2#0#7#0","2#0#7#1","2#0#7#2",
                        "2#1#0","2#2#0","2#2#1","2#2#2","2#2#3","2#2#4","2#2#5","2#2#6","2#2#7","2#2#8"};

String tarjetas_comunes[8] = {"6#0#0","6#0#1","6#0#2","6#1#0","6#2#0","6#2#1","6#2#2"};

String tarjetas[80] = {"6#2#1","6#2#2"};
int tarjeta = 0;
char delim[] = "#";

int puertoDigital = 3, puertoDigital_bloque = 0;
int puertoAnalogico = 0, puertoAnalogico_bloque = 0;

unsigned long startTime;
unsigned long currentTime;
unsigned long limit = 4000;
bool bitmap = false;
bool iniciando = true;

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;


// ------- *** Variables MODO MUSICA *** --------------

// NOTAS 4ta octava (Hz)
int DO = 262, RE = 294, MI = 330, FA = 349, SOL = 392, LA = 440, SI = 494, DO_ = 523;

int notas[200], duraciones[200];

int posicion = 0;

bool bucle = false;
int tam_bucle = 0;

String secuencia[30];
//--------------------------------  BITMAPS  -------------------------------------

void showBitmap(int id1, int id2, String msg) {
   display.clearDisplay();
   display.setCursor(0,0);
   bitmap = true;
   startTime = millis();

   switch (id1)
   {
   // MODO (0): BASICO (0), AVANZADO (1), MUSICA (2)
   case 0:

      switch (id2)
      {
      case 0:
         // display.drawBitmap(0,0, modoBasicoBitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), "MODO BASICO");
         break;
      
      case 1:
         // display.drawBitmap(0,0, modoAvanzadoBitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), "MODO AVANZADO");
         break;

      case 2:
         // display.drawBitmap(0,0, modoMusicaBitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), "MODO MUSICA");
         break;

      default:
         snprintf(buf, sizeof(buf), "Bitmap not available.");
         Serial.println("Bitmap not available.");
         break;
      }
      
      break;
   
   // CONEX. PUERTO (1): FIGURAS
   case 1:

      switch (id2)
      {
      case 0:
         // display.drawBitmap(0,0, cuadrado_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), msg+" CONECTAR A HEXAGONO");
         break;
      
      case 1:
         // display.drawBitmap(0,0, cuadrado_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), msg+" CONECTAR A CUADRADO");
         break;

      case 2:
         // display.drawBitmap(0,0, cuadrado_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), msg+" CONECTAR A TRIANGULO");
         break;

      case 3:
         // display.drawBitmap(0,0, cuadrado_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), msg+" CONECTAR A ROMBO");
         break;

      case 4:
         // display.drawBitmap(0,0, cuadrado_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), msg+" CONECTAR A SEMICIRCULO");
         break;

      case 5:
         // display.drawBitmap(0,0, cuadrado_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), "CONECTAR A CIRCULO");
         break;

      case 6:
         // display.drawBitmap(0,0, cuadrado_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), "YA CONECTADO");
         break;

      default:
         snprintf(buf, sizeof(buf), "Bitmap not available.");
         Serial.println("Bitmap not available.");
         break;
      }

      break;

   // TARJETA INVALIDA (2): MODO (0), BORRADO (1), ORDEN (2), PUERTO no disp. (3), PLAY no disp (4),...
   case 2:

      switch (id2)
      {
      case 0:
         // display.drawBitmap(0,0, modoInvalido_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), "Tarjeta invalida para este modo");
         break;
      
      case 1:
         // display.drawBitmap(0,0, borradoInvalido_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), "Borrado invalido para este modo");
         break;

      case 2:
         // display.drawBitmap(0,0, ordenInvalido_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), "Orden de tarjetas invalido");
         break;

      case 3:
         // display.drawBitmap(0,0, puertoInvalido_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), "Puerto no disponible");
         break;

      case 4:
         // display.drawBitmap(0,0, playInvalido_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), "Play no disponible"+msg);
         break;

      case 5:
         // display.drawBitmap(0,0, sensorInvalido_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), "Sensor no disponible (en uso)");
         break;

      case 6:
         // display.drawBitmap(0,0, actuadorInvalido_bitmap, bitmap_width, bitmap_height, 1);
         snprintf(buf, sizeof(buf), "Actuador no disponible (en uso)");
         break;

      default:
         snprintf(buf, sizeof(buf), "Bitmap not available.");
         Serial.println("Bitmap not available.");
         break;
      }

      break;

   // TARJETA VÁLIDA (3)
   case 3:

      switch (id2)
      {
      case 0:
         snprintf(buf, sizeof(buf), msg);
         break;
      
      default:
         snprintf(buf, sizeof(buf), "Bitmap not available.");
         Serial.println("Bitmap not available.");
         break;
      }
      break;

   default:
      snprintf(buf, sizeof(buf), "Bitmap not available.");
      Serial.println("Bitmap not available.");
      break;
   }
   Serial.println(buf);
   display.print(buf);
   display.display();

}

void showPort(int id, int puerto ) {

   if (tagInfo[2]==0) { // A0, A2, A4

      switch (puerto)
      {
      case 0:
         showBitmap(1,3,"");
         break;
      
      case 2:
         showBitmap(1,4,"");
         break;

      case 4:
         showBitmap(1,5,"");
         break;

      default:
         showBitmap(2,3,""); //PUERTO NO DISPO.
         break;
      }
      
   } else { // D2, D4, D6

      switch (puerto)
      {
      case 2:
         showBitmap(1,0,"");
         break;
      
      case 4:
         showBitmap(1,1,"");
         break;

      case 6:
         showBitmap(1,2,"");
         break;

      default:
         showBitmap(2,3,""); //PUERTO NO DISPO.
         break;
      }

   }
}


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
   case 2: {
      posicion = 0;
      bucle = false;
      tam_bucle = 0;
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
   // Modo avanzado (bloque)
   case 1:

      if (numBloque==0) {
         borradoALL(1);
         showBitmap(3,0,"Borrado de bloque realizado");
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
         showBitmap(3,0,"Borrado de bloque realizado");

      } else {
         Serial.println("Nada que borrar");
      }
      break;
   
   // Modo musica (loop)
   case 2:
      if (bucle) {
         posicion = posicion - (tam_bucle+1);
         showBitmap(3,0,"Borrado de loop realizado");

      } else if ( notas[posicion-1] == -2 ) {
         posicion = posicion - (duraciones[posicion-1]+2);
         showBitmap(3,0,"Borrado de loop realizado");
      
      } else {
         Serial.println("Nada que borrar.");
         showBitmap(3,0,"Nada que borrar.");

      }
      bucle = false;
      tam_bucle = 0;
      break;

   default:
      break;
   }
}

void borradoPOP(){
   posicion--;

   if (notas[posicion] == -2) { // Ultima es N iter
      bucle = true;
      tam_bucle = duraciones[posicion];

   } else if (notas[posicion] == -1) { // Ultima es LOOP
      bucle = false;

   } else {
      if (bucle) {
         tam_bucle--;
      }
   }
}

//--------------------------------  REPRODUCCION  -------------------------------------

int decodificarNOTA(int nota) {
   switch (nota)
   {
   case 0:
      return DO;
   
   case 1:
      return RE;

   case 2:
      return MI;

   case 3:
      return FA;

   case 4:
      return SOL;

   case 5:
      return LA;

   case 6:
      return SI;

   case 7:
      return DO_;

   default:
      Serial.println("Nota inválida");
      return -1;
   }
}

int decodificarTIPO(int tipo) {
   switch (tipo)
   {
   case 0:
      return 250;
      
   case 1:
      return 500;

   case 2:
      return 1000;

   default:
      Serial.println("Tipo inválido");
      return -1;
   }
}

String decodificarNOTA_msg(int nota) {
   switch (nota)
   {
   case 0:
      return "DO";
   
   case 1:
      return "RE";

   case 2:
      return "MI";

   case 3:
      return "FA";

   case 4:
      return "SOL";

   case 5:
      return "LA";

   case 6:
      return "SI";

   case 7:
      return "do";

   default:
      return "Nota inválida";
   }
}

String decodificarTIPO_msg(int tipo) {
   switch (tipo)
   {
   case 0:
      return "_C";
      
   case 1:
      return "_N";

   case 2:
      return "_B";

   default:
      return "Tipo inválido";
   }
}

void reproducirNOTA(int nota, int tipo) {
   int frecuencia = decodificarNOTA(nota);
   int duracion = decodificarTIPO(tipo);

   tone(Zumbador_PIN, frecuencia);
   delay(duracion);
   noTone(Zumbador_PIN);
   delay(30);
}

void reproducir() {
   if (!bucle) {

      int n;
      int iteracion;
      int inicio;

      int i = 0;
      while ( i < posicion) {

         Serial.printlnf("%d - %d", notas[i], duraciones[i]);

         if ( notas[i] == -1 ) {
            n = duraciones[i];
            iteracion = 0;            
            inicio=i;

         } else if ( notas[i] == -2 ) {
            iteracion++;
            if (iteracion < n) {
               i = inicio;
            }
            
         } else {
            reproducirNOTA(notas[i],duraciones[i]);
         }
         
         i++;

      }

   } else {
      Serial.println("Finaliza el bucle antes de reproducir");
      showBitmap(2,4,"");
   }
   
}

// --------------------------------------------------------------------------------------

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
      showBitmap(3,0,"Iniciando modo BASICO...");

   } else if (modo == 1) {

      Serial.println("Modo AVANZADO detectado");
      MODE = 1;
      showBitmap(3,0,"Iniciando modo AVANZADO...");

   } else if (modo == 2) {
      Serial.println("Modo MUSICA detectado");
      MODE = 2;
      showBitmap(3,0,"Iniciando modo MUSICA...");

   }
   EEPROM.put(0, MODE);
}

/**
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
}*/

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
   int condigo_length = tarjetas[tarjeta].length()+1;
   char codigo[condigo_length];
   tarjetas[tarjeta].toCharArray(codigo, condigo_length);

   status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(blockAddr, (byte *)codigo, 16);
   if (status != MFRC522::STATUS_OK)
   {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
   } else {
      Serial.println(String(tarjeta));
      tarjeta++;
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
      return 0;

   case 1:
      Serial.print("Conectar al puerto A2\n");
      puertoAnalogico++;
      puertoAnalogico_bloque++;
      return 2;

   case 2:
      Serial.print("Conectar al puerto A4\n");
      puertoAnalogico = -1;
      puertoAnalogico_bloque++;
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