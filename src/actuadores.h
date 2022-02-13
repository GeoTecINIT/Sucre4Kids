
// This #include statement was automatically added by the Particle IDE.
#include <Grove_ChainableLED.h>

void ledApagar(ChainableLED leds)
{
    leds.setColorRGB(0, 0, 0, 0);
}
void ledRojo(ChainableLED leds)
{
    leds.setColorRGB(0, 255, 0, 0);
}
void ledVerde(ChainableLED leds)
{
    leds.setColorRGB(0, 0, 255, 10);
}
void ledAzul(ChainableLED leds)
{
    leds.setColorRGB(0, 0, 0, 255);
}
void ledAmarillo(ChainableLED leds)
{
    leds.setColorRGB(0, 255, 233, 0);
}
void ledMorado(ChainableLED leds)
{
    leds.setColorRGB(0, 108, 70, 117);
}
void ledNaranja(ChainableLED leds)
{
    leds.setColorRGB(0, 255, 128, 0);
}
void ledBlanco(ChainableLED leds)
{
    Serial.println("Entra ledBlanco");
    leds.setColorRGB(0, 255, 255, 255);
    Serial.println("Termina ledBlanco");
}

void ledAzulNaranja(boolean estado, ChainableLED leds)
{
    if (estado)
    {
        ledAzul(leds);
    }
    else
    {
        ledNaranja(leds);
    }
}

void ledMoradoAmarillo(boolean estado, ChainableLED leds)
{
    if (estado)
    {
        ledMorado(leds);
    }
    else
    {
        ledAmarillo(leds);
    }
}

void ledVerdeRojo(boolean estado, ChainableLED leds)
{
    if (estado)
    {
        ledRojo(leds);
    }
    else
    {
        ledVerde(leds);
    }
}

void ledBlink(boolean estado, ChainableLED leds)
{
    if (estado)
    {
        ledBlanco(leds);
        delay(400);
        ledApagar(leds);
        delay(200);
    }
    else
    {
        ledApagar(leds);
    }
}

void ledOnOff(boolean estado, ChainableLED leds)
{
    if (estado)
    {
        ledBlanco(leds);
    }
    else
    {
        ledApagar(leds);
    }
}

void ledArcoiris(boolean estado, ChainableLED leds)
{
    if (estado)
    {
        ledRojo(leds);
        delay(200);
        ledNaranja(leds);
        delay(200);
        ledAmarillo(leds);
        delay(200);
        ledVerde(leds);
        delay(200);
        ledMorado(leds);
        delay(200);
        ledAzul(leds);
        delay(200);
    }
    else
    {
        ledApagar(leds);
    }
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
    // Serial.println("Zumbador OFF");
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
    {
        pitidoBlink(puerto);
    }
    else
    {
        pitidoOFF(puerto);
    }
}

void zumbador(boolean estado, int puerto)
{
    if (estado)
    {
        pitidoON(puerto);
    }
    else
    {
        pitidoOFF(puerto);
    }
}

// Recive el actuador que es, el tipo de actuador, el puerto al que esta conectado, el valor de los sensores para actuar en consecuencia y los puertos de dichos sensores.
void activarLED(int opcion, int puerto, bool valor)
{

    int led_PIN1 = puerto;
    int led_PIN2 = puerto + 1;
    ChainableLED leds(led_PIN1, led_PIN2, 5);
    leds.init();
    Serial.printlnf("Activar RGB en puerto: %d & %d", puerto, puerto + 1);

    switch (opcion)
    {
    case 0:
        ledVerdeRojo(valor, leds);
        break;

    case 1:
        ledMoradoAmarillo(valor, leds);
        break;

    case 2:
        ledAzulNaranja(valor, leds);
        break;

    case 3:
        ledBlink(valor, leds);
        break;

    case 4:
        ledArcoiris(valor, leds);
        break;

    case 5:
        Serial.printlnf("Entra LED ONOF");
        ledOnOff(valor, leds);
        break;

    default:
        Serial.println("Valor no valido");
        break;
    }

    Serial.println("Sale swith");
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