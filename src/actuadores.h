// This #include statement was automatically added by the Particle IDE.
#include <Grove_ChainableLED.h>

bool init = true;
ChainableLED ledObject = ChainableLED(0, 0, 5);

void ledApagar()
{
    ledObject.setColorRGB(0, 0, 0, 0);
}
void ledRojo()
{
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

void ledBlink()
{
    ledBlanco();
    delay(400);
    ledApagar();
    delay(200);
}

void ledArcoiris()
{

    ledRojo();
    delay(200);
    ledNaranja();
    delay(200);
    ledAmarillo();
    delay(200);
    ledVerde();
    delay(200);
    ledMorado();
    delay(200);
    ledAzul();
    delay(200);
}

// Acciones del zumbador
void pitidoON(int puerto)
{
    digitalWrite(puerto, 1);
    digitalWrite(puerto, HIGH);
    digitalWrite(puerto + 1, HIGH);
    digitalWrite(puerto + 1, 1);
}

void pitidoOFF(int puerto)
{
    digitalWrite(puerto, 0);
    digitalWrite(puerto, LOW);
    digitalWrite(puerto + 1, LOW);
    digitalWrite(puerto + 1, 0);
}

void pitidoBlink(int puerto)
{
    pitidoON(puerto);
    delay(100);
    pitidoOFF(puerto);
    delay(100);
}

void blinkAndSleep(boolean estado, int puerto)
{
    if (estado)
        pitidoBlink(puerto);
    else
        pitidoOFF(puerto);
}

void zumbador(boolean estado, int puerto)
{
    if (estado)
        pitidoON(puerto);
    else
        pitidoOFF(puerto);
}

// Recive el actuador que es, el tipo de actuador, el puerto al que esta conectado, el valor de los sensores para actuar en consecuencia y los puertos de dichos sensores.
void activarLED(int opcion, int puerto)
{

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
        pitidoON(puerto);
        break;
    case 1:
        pitidoBlink(puerto);
        break;
    default:
        Serial.println("Valor no valido");
        break;
    }
}

void apagarActuador(int puerto)
{
    digitalWrite(puerto, 0);
    digitalWrite(puerto, LOW);
}

void actuadorHandler(int id, int opcion, int puerto)
{
    if (id < 2)
        id == 0 ? activarLED(opcion, puerto) : activarZumbador(opcion, puerto);
    else
        apagarActuador(puerto);
}