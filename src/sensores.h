// Temp & Humidiy
#include <Adafruit_DHT_Particle.h>

// Distance
#include "Grove-Ultrasonic-Ranger.h"

bool noDistancia(int puerto)
{
    Ultrasonic ultrasonic(puerto);
    long range = ultrasonic.MeasureInCentimeters();
    Serial.printlnf("Distancia: %d", range);
    if (range >= 0 && range < 10)
        return true;
    else
        return false;
}
bool siDistancia(int puerto)
{
    Ultrasonic ultrasonic(puerto);
    long range = ultrasonic.MeasureInCentimeters();
    Serial.printlnf("Distancia: %d", range);
    if (range > 10)
        return true;
    else
        return false;
}

bool siLuz(int puerto)
{
    int threshold = 1000;
    switch (puerto)
    {
    case 0:
        if (analogRead(A0) >= threshold)
            return true;
        return false;

    case 2:
        if (analogRead(A2) >= threshold)
            return true;
        return false;

    case 4:
        if (analogRead(A4) >= threshold)
            return true;
        return false;

    default:
        return false;
    }
}

bool noLuz(int puerto)
{
    int threshold = 1000;
    switch (puerto)
    {
    case 0:
        if (analogRead(A0) <= threshold)
            return true;
        return false;

    case 2:
        if (analogRead(A2) <= threshold)
            return true;
        return false;

    case 4:
        if (analogRead(A4) <= threshold)
            return true;
        return false;

    default:
        return false;
    }
}

// bool leerSensorSonido(int puerto)
// {
//     // Serial.printlnf("analogico: %d", analogRead(A4));
//     if (analogRead(puerto) >= 700)
//         return true;
//     return false;
// }

bool siRuido(int puerto)
{
    if (analogRead(puerto) >= 700)
        return true;
    return false;
}

bool noRuido(int puerto)
{
    if (analogRead(puerto) <= 700)
        return true;
    return false;
}

bool siBoton(int puerto)
{
    pinMode(puerto, INPUT);
    if (digitalRead(puerto) == HIGH)
        return true;
    return false;
}

bool noBoton(int puerto)
{
    pinMode(puerto, INPUT);
    if (digitalRead(puerto) == LOW)
        return true;
    return false;
}

bool siRotativo(int puerto)
{
    float voltage;
    int sensor_value = analogRead(puerto);
    voltage = (float)sensor_value * 5 / 1023;
    float degrees = (voltage * 300) / 5;

    if (degrees >= 680)
        return true;
    return false;
}

bool noRotativo(int puerto)
{
    float voltage;
    int sensor_value = analogRead(puerto);
    voltage = (float)sensor_value * 5 / 1023;
    float degrees = (voltage * 300) / 5;

    if (degrees <= 680)
        return true;
    return false;
}

bool tempFrio(int puerto)
{
    DHT dht(puerto, DHT11);
    dht.begin();
    float t = dht.getTempCelcius();

    // The fast read may cause an invalid value like 0.0000 or NuLL. Repeat until valid value.
    while (isnan(t) | t == 0.0)
    {
        t = dht.getTempCelcius();
    }

    Serial.printlnf("Temp: %f", t);
    if (t > 0.0 && t < 8)
        return true;
    else
        return false;
}
bool tempTemplado(int puerto)
{
    DHT dht(puerto, DHT11);
    dht.begin();
    float t = dht.getTempCelcius();

    // The fast read may cause an invalid value like 0.0000 or NuLL. Repeat until valid value.
    while (isnan(t) | t == 0.0)
    {
        t = dht.getTempCelcius();
    }

    Serial.printlnf("Temp: %f", t);
    if (t > 7 && t < 26)
        return true;
    else
        return false;
}
bool tempCalor(int puerto)
{
    DHT dht(puerto, DHT11);
    dht.begin();
    float t = dht.getTempCelcius();

    // The fast read may cause an invalid value like 0.0000 or NuLL. Repeat until valid value.
    while (isnan(t) | t == 0.0)
    {
        t = dht.getTempCelcius();
    }

    Serial.printlnf("Temp: %f", t);
    if (t > 25 && t < 50)
        return true;
    else
        return false;
}

bool leerSensor(int id, int condicion, int puerto)
{
    switch (id)
    {
    case 1:
        return (condicion == 0 ? noDistancia(puerto) : siDistancia(puerto));

    case 2:
        return (condicion == 0 ? noLuz(puerto) : siLuz(puerto));

    case 3:
        return (condicion == 0 ? noRuido(puerto) : siRuido(puerto));

    case 4:
        return (condicion == 0 ? noBoton(puerto) : siBoton(puerto));

    case 5:
        return (condicion == 0) ? noRotativo(puerto) : siRotativo(puerto);

    case 6:
        switch (condicion)
        {
        case 0:
            return tempFrio(puerto);
        case 1:
            return tempTemplado(puerto);
        case 2:
            return tempCalor(puerto);
        }
    case 7:
        return (condicion == 0) ? noDistancia(puerto) : siDistancia(puerto);

    default:
        Serial.println("InvalidSensorError");
        return false;
    }
}