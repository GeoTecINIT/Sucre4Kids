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

void ledAzulNaranja(boolean estado)
{
    if (estado)
        ledAzul();
    else
        ledNaranja();
}

void ledMoradoAmarillo(boolean estado)
{

    if (estado)
        ledMorado();
    else
        ledAmarillo();
}

void ledVerdeRojo(boolean estado)
{
    if (estado)
        ledRojo();
    else
        ledVerde();
}

void ledBlink(boolean estado)
{
    if (estado)
    {
        ledBlanco();
        delay(400);
        ledApagar();
        delay(200);
    }
    else
        ledApagar();
}

void ledOnOff(boolean estado)
{
    if (estado)
        ledBlanco();
    else
        ledApagar();
}

void ledArcoiris(boolean estado)
{

    if (estado)
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
    else
        ledApagar();
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
void activarLED(int opcion, int puerto, bool valor)
{
    ledObject = ChainableLED(puerto, puerto + 1, 5);

    if (init)
    {
        ledObject.init();
        init = false;
    }

    switch (opcion)
    {
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
        ledBlink(valor);
        break;

    case 4:
        ledArcoiris(valor);
        break;

    case 5:
        ledOnOff(valor);
        break;

    default:
        Serial.println("LedErrorOption");
        break;
    }
}

void activarZumbador(int opcion, int puerto, bool valor)
{
    pinMode(puerto, OUTPUT);

    switch (opcion)
    {
    case 0:
        zumbador(valor, puerto);
        break;
    case 1:
        blinkAndSleep(valor, puerto);
        break;
    default:
        Serial.println("Valor no valido");
        break;
    }
}