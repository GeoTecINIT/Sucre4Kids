// This #include statement was automatically added by the Particle IDE.
#include <Grove_ChainableLED.h>

bool init = true;

void ledApagar()
{
    ledObject.setColorRGB(0, 0, 0, 0);
}
void ledRojo()
{
    // Serial.println("LedRojo");
    ledObject.setColorRGB(0, 255, 0, 0);
}
void ledVerde()
{
    ledObject.setColorRGB(0, 0, 255, 10);
}
void ledAzul()
{
    ledObject.setColorRGB(0, 0, 0, 255);
}
void ledAmarillo()
{
    ledObject.setColorRGB(0, 255, 233, 0);
}
void ledMorado()
{
    ledObject.setColorRGB(0, 108, 70, 117);
}
void ledNaranja()
{
    ledObject.setColorRGB(0, 255, 128, 0);
}
void ledBlanco()
{
    ledObject.setColorRGB(0, 255, 255, 255);
}
void ledArcoiris()
{
    for (double i = 0; i < 1; i += 0.01)
    {
        ledObject.setColorHSB(0, i, 0.75, 0.75);
    }

}


//-------------------------------------------------------------------------------
//--------------------------------- M O D O   0 ---------------------------------
//-------------------------------------------------------------------------------

#define Zumbador_PIN D6
#define led_PIN1 4
#define led_PIN2 5
#define NUM_LEDS 5

void 
pitidoON0()
{
   //Serial.println("Zumbador ON");
   digitalWrite(Zumbador_PIN, 1);
   digitalWrite(Zumbador_PIN, HIGH);
   digitalWrite(D7, HIGH);
   digitalWrite(D7, 1);
}

void pitidoOFF0()
{
   //Serial.println("Zumbador OFF");
   digitalWrite(Zumbador_PIN, 0);
   digitalWrite(Zumbador_PIN, LOW);
   digitalWrite(D7, LOW);
   digitalWrite(D7, 0);
}

void pitidoBlink0()
{
   pitidoON0();
   delay(100);
   pitidoOFF0();
   delay(100);
}

void ledAzulNaranja(boolean estado)
{
   if (estado)
   {
      ledAzul();
   }
   else
   {
      ledNaranja();
   }
}
void ledMoradoAmarillo(boolean estado)
{
   if (estado)
   {
      ledAmarillo();
   }
   else
   {
      ledMorado();
   }
}
void ledVerdeRojo(boolean estado)
{
   if (estado)
   {
      ledVerde();
   }
   else
   {
      ledRojo();
   }
}
void ledBlink0(boolean estado)
{
   if (estado)
   {
      ledBlanco();
      delay(400);
      ledApagar();
      delay(200);
   }
   else
   {
      ledApagar();
   }
}

void ledOnOff(boolean estado)
{
   if (estado)
   {
      ledBlanco();
   }
   else
   {
      ledApagar();
   }
}

void ledArcoiris0(boolean estado)
{
   if (estado)
   {
      ledArcoiris();
   }
   else
   {
      ledApagar();
   }
}

void blinkAndSleep(boolean estado)
{
   if (estado)
   {
      pitidoBlink0();
   }
   else
   {
      pitidoOFF0();
   }
}
void zumbador(boolean estado)
{
   if (estado)
   {
      pitidoON0();
   }
   else
   {
      pitidoOFF0();
   }
}

void activarActuador(int actuador, int opcion, boolean valor)
{
   switch (actuador)
   {
      pitidoOFF0();
   // Actuador led
   case 0:
      switch (opcion) {
         case 0:
            ledVerdeRojo(valor);
            break;

         case 1:
            ledMoradoAmarillo(valor);
            break;

         case 2:
            ledAzulNaranja(valor);
            break;

         case 3:
            ledBlink0(valor);
            break;

         case 4:
            ledArcoiris0(valor);
            break;

         case 5:
            ledOnOff(valor);
            break;

         default:
            Serial.println("Valor no valido");
            break;
      }
      break;

   //Zumbador
   case 1:
      ledApagar();
      switch (opcion) {
         case 0:
            zumbador(valor);
            break;
         case 1:
            blinkAndSleep(valor);
            break;
         default:
            Serial.println("Valor no valido");
            break;
      }
      break;
      
   default:
      Serial.println("Valor no valido");
      break;
   }
}

//-------------------------------------------------------------------------------
//--------------------------------- M O D O   1 ---------------------------------
//-------------------------------------------------------------------------------

void ledBlink()
{
    ledBlanco();
    delay(400);
    ledApagar();
    delay(200);
}

// Acciones del zumbador MODO 1
void pitidoON1(int puerto)
{
    digitalWrite(puerto, 1);
    digitalWrite(puerto, HIGH);
    digitalWrite(puerto + 1, HIGH);
    digitalWrite(puerto + 1, 1);
}

void pitidoOFF1(int puerto)
{
    digitalWrite(puerto, 0);
    digitalWrite(puerto, LOW);
    digitalWrite(puerto + 1, LOW);
    digitalWrite(puerto + 1, 0);
}

void pitidoBlink1(int puerto)
{
    pitidoON1(puerto);
    delay(100);
    pitidoOFF1(puerto);
    delay(100);
}

// Recive el actuador que es, el tipo de actuador, el puerto al que esta conectado, el valor de los sensores para actuar en consecuencia y los puertos de dichos sensores.
void activarLED(int opcion, int puerto)
{
    // Serial.println("Actiaver LED");
    if (init)
    {
        ledObject = ChainableLED(puerto, puerto + 1, 5);
        ledObject.init();
        init = false;
    }

    switch (opcion)
    {
    case 0:
        ledVerde();
        break;

    case 1:
        ledRojo();
        break;

    case 2:
        ledAmarillo();
        break;

    case 3:
        ledMorado();
        break;

    case 4:
        ledAzul();
        break;

    case 5:
        ledNaranja();
        break;
    case 6:
        ledBlink();
        break;
    case 7:
        ledArcoiris();
        break;

    default:
        Serial.println("LedErrorOption");
        break;
    }
}

void activarZumbador(int opcion, int puerto)
{
    pinMode(puerto, OUTPUT);

    switch (opcion)
    {
    case 0:
        pitidoON1(puerto);
        break;
    case 1:
        pitidoBlink1(puerto);
        break;
    default:
        Serial.println("Valor no valido");
        break;
    }
}

void apagarActuador(int id, int puerto)
{
    id == 0 ? ledApagar() : digitalWrite(puerto, 0);
}

void actuadorHandler(int id, int opcion, int puerto)
{
    if (id < 2)
        id == 0 ? activarLED(opcion, puerto) : activarZumbador(opcion, puerto);
}