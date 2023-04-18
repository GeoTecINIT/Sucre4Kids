// Temp & Humidiy
#include <Adafruit_DHT_Particle.h>

// Distance
#include "Grove-Ultrasonic-Ranger.h"

//---------------------------------------------------------------------------------------------------
//------------------------------------- M O D O   0 -------------------------------------------------
//---------------------------------------------------------------------------------------------------

#define sensorLuz_PIN A0
#define boton_PIN D2
#define sensorSonido_PIN A0
#define rotoryAngle_PIN A0

boolean leerSensorLuz()
{
   if (analogRead(sensorLuz_PIN) >= 2500)
      return true;
   return false;
}

boolean leerBoton()
{
   if (digitalRead(boton_PIN) == HIGH)
      return true;
   return false;
}

boolean leerAngulo()
{
   float voltage;
   int sensor_value = analogRead(rotoryAngle_PIN);
   voltage = (float)sensor_value * 5 / 1023;
   float degrees = (voltage * 300) / 5;

   if (degrees >= 680)
   {
      //Serial.println(degrees);
      return true;
   }

   return false;
}

boolean leerSensorSonido()
{
   // Serial.printlnf("analogico: %d", analogRead(A4));
   if (analogRead(sensorSonido_PIN) >= 700)
      return true;
   return false;
}

boolean leerSensor0(int sensor)
{
   switch (sensor)
   {
   case 2:
      return leerSensorLuz();
      break;

   case 3:
      return leerSensorSonido();
      break;

   case 4:
      return leerBoton();
      break;

   case 5:
      return leerAngulo();
      break;

   default:
      Serial.println("No es valido");
      return false;
      break;
   }
}



//---------------------------------------------------------------------------------------------------
//------------------------------------- M O D O   1 -------------------------------------------------
//---------------------------------------------------------------------------------------------------

bool noDistancia(int puerto)
{
    Ultrasonic ultrasonic(puerto);
    long range = ultrasonic.MeasureInCentimeters();
    // Serial.printlnf("Distancia: %d", range);
    if (range >= 0 && range < 10)
        return true;
    else
        return false;
}

bool siDistancia(int puerto)
{
    Ultrasonic ultrasonic(puerto);
    long range = ultrasonic.MeasureInCentimeters();
    // Serial.printlnf("Distancia: %d", range);
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

bool siRuido(int puerto)
{
    int sonido = analogRead(puerto);
    if (sonido >= 700)
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
    int sensor_value;
    switch (puerto)
    {
    case 0:
        sensor_value = analogRead(A0);
        break;
    case 2:
        sensor_value = analogRead(A2);
        break;
    case 4:
        sensor_value = analogRead(A4);
        break;
    }

    voltage = (float)sensor_value * 3.3 / 1023;
    float degrees = (voltage * 300) / 5;
    if (degrees <= 360)
        return true;
    return false;
}

bool noRotativo(int puerto)
{
    float voltage;
    int sensor_value;
    switch (puerto)
    {
    case 0:
        sensor_value = analogRead(A0);
        break;
    case 2:
        sensor_value = analogRead(A2);
        break;
    case 4:
        sensor_value = analogRead(A4);
        break;
    }

    voltage = (float)sensor_value * 3.3 / 1023;
    float degrees = (voltage * 300) / 5;

    if (degrees >= 360)
        return true;
    return false;
}

bool BotonDualA(int puerto)
{
    pinMode(puerto, INPUT);
    pinMode(puerto+1, INPUT);
    if (digitalRead(puerto) == LOW && digitalRead(puerto+1) == HIGH)
        return true;
    return false;
}

bool BotonDualB(int puerto)
{
    pinMode(puerto+1, INPUT);
    pinMode(puerto, INPUT);
    if (digitalRead(puerto+1) == LOW && digitalRead(puerto) == HIGH)
        return true;
    return false;
}

bool BotonDual2(int puerto)
{
    pinMode(puerto, INPUT);
    pinMode(puerto+1, INPUT);
    if (digitalRead(puerto) == LOW && digitalRead(puerto+1) == LOW)
        return true;
    return false;
}

bool noBotonDual(int puerto)
{
    pinMode(puerto, INPUT);
    pinMode(puerto+1, INPUT);
    if (digitalRead(puerto) == HIGH && digitalRead(puerto+1) == HIGH)
        return true;
    return false;
}

bool siAgua(int puerto)
{
    pinMode(puerto, INPUT);
    if (digitalRead(puerto) == LOW)
        return true;
    return false;
}

bool noAgua(int puerto)
{
    pinMode(puerto, INPUT);
    if (digitalRead(puerto) == HIGH)
        return true;
    return false;
}


bool Cafe(int puerto)
{
    int sensor_value;
    switch (puerto)
    {
    case 0:
        sensor_value = analogRead(A0);
        break;
    case 2:
        sensor_value = analogRead(A2);
        break;
    case 4:
        sensor_value = analogRead(A4);
        break;
    }
    Serial.println(sensor_value);
    if (sensor_value < 2000)
        return true;
    return false;
}

bool AguaClara(int puerto)
{
    int sensor_value;
    switch (puerto)
    {
    case 0:
        sensor_value = analogRead(A0);
        break;
    case 2:
        sensor_value = analogRead(A2);
        break;
    case 4:
        sensor_value = analogRead(A4);
        break;
    }
    if (sensor_value > 2500)
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
    while (isnan(t) || (t == 0.0))
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

//---------------------------------------------------------------------------------------------------
//------------------------------------- M O D O   3 -------------------------------------------------
//---------------------------------------------------------------------------------------------------

int DistanciaExp(int puerto)
{
    Ultrasonic ultrasonic(puerto);
    int range = ultrasonic.MeasureInCentimeters();
    // Serial.printlnf("Distancia: %d", range);
    return range;
}

int AnguloExp()
{
   float voltage;
   int sensor_value = analogRead(rotoryAngle_PIN);
   voltage = (float)sensor_value * 5 / 1023;
   float degrees = (voltage * 300) / 5;

   return (int) degrees;
}

int LuzExp()
{
   return analogRead(sensorLuz_PIN);
}

int SensorSonidoExp()
{
    //return analogRead(sensorSonido_PIN);
    const int sampleWindow = 50;                              // Sample window width in mS (50 mS = 20Hz)
    unsigned int sample;
    unsigned long startMillis= millis();                   // Start of sample window
    double peakToPeak = 0;                                  // peak-to-peak level
    
    unsigned int signalMax = 0;                            //minimum value
    unsigned int signalMin = 1024;                         //maximum value
    
                                                            // collect data for 50 mS
    while (millis() - startMillis < sampleWindow)
    {
        sample = analogRead(sensorSonido_PIN);                    //get reading from microphone
        if (sample < 1024)                                  // toss out spurious readings
        {
            if (sample > signalMax)
            {
                signalMax = sample;                           // save just the max levels
            }
            else if (sample < signalMin)
            {
                signalMin = sample;                           // save just the min levels
            }
        }
    }
        
    peakToPeak = signalMax - signalMin;                    // max - min = peak-peak amplitude
    int db = map(peakToPeak,20.0,900.0,49.5,90.0); 
    return db;
    }

int SensorTempExp(int puerto)
{
    DHT dht(puerto, DHT11);
    dht.begin();
    float t = dht.getTempCelcius();

    // The fast read may cause an invalid value like 0.0000 or NuLL. Repeat until valid value.
    while (isnan(t) | t == 0.0)
    {
        t = dht.getTempCelcius();
    }
    return t;
}

int TurbiaExp(int puerto)
{
    int sensor_value;
    switch (puerto)
    {
    case 0: 
        sensor_value = analogRead(A0);
        break;
    case 2:
        sensor_value = analogRead(A2);
        break;
    case 4:
        sensor_value = analogRead(A4);
        break;
    }
    return sensor_value;
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
        return (condicion == 0 ? noRotativo(puerto) : siRotativo(puerto));

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
        return (condicion == 0 ? noDistancia(puerto) : siDistancia(puerto));

    case 8:
        return (condicion == 0 ? noAgua(puerto) : siAgua(puerto));

    case 9:
        switch (condicion)
        {
        case 0:
            return noBotonDual(puerto);
        case 1:
            return BotonDualA(puerto);
        case 2:
            return BotonDualB(puerto);
        case 3:
            return BotonDual2(puerto);
        }
    case 10:
        return BotonDualB(puerto);
    case 11:
        return BotonDual2(puerto);
    case 12:
        switch (condicion)
        {
        case 0:
            return Cafe(puerto);
        case 1:
            return AguaClara(puerto);           
        }
        

    default:
        Serial.println("InvalidSensorError");
        return false;
    }
}

int leerSensorExp(int id, int puerto){
    switch (id)
    {
    case 2:
        return LuzExp();
    case 3:
        return SensorSonidoExp();
    case 5:
        return AnguloExp();
    case 6:
        return SensorTempExp(puerto);
    case 7:
        return DistanciaExp(puerto);
    case 12:
        return TurbiaExp(puerto);

    
    default:
        Serial.println("InvalidSensorError");
        return -1;
    }
}

