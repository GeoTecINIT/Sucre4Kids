// #define sensorLuz_PIN A2
// #define boton_PIN D2
// #define sensorSonido_PIN A4
// #define rotoryAngle_PIN A0

boolean leerSensorLuz(int puerto)
{
    if (analogRead(puerto) >= 2500)
        return true;
    return false;
}

boolean leerBoton(int puerto)
{
    pinMode(puerto, INPUT);
    // Serial.printlnf("Valor boton %d , %d", analogRead(boton_PIN), digitalRead(boton_PIN));
    if (digitalRead(puerto) == HIGH)
        return true;
    return false;
}

boolean leerAngulo(int puerto)
{
    float voltage;
    int sensor_value = analogRead(puerto);
    voltage = (float)sensor_value * 5 / 1023;
    float degrees = (voltage * 300) / 5;

    if (degrees >= 680)
    {
        // Serial.println(degrees);
        return true;
    }

    return false;
}

boolean leerSensorSonido(int puerto)
{
    // Serial.printlnf("analogico: %d", analogRead(A4));
    if (analogRead(puerto) >= 700)
        return true;
    return false;
}

boolean leerSensor(int id, int condicion, int puerto)
{
    switch (id)
    {
    case 2:
        return leerSensorLuz(puerto);
        break;

    case 3:
        return leerSensorSonido(puerto);
        break;

    case 4:
        pinMode(puerto, INPUT);
        return leerBoton(puerto);
        break;

    case 5:
        return leerAngulo(puerto);
        break;

    default:

        // TODO No entra en el lector MIRAR IMPORTANTE
        Serial.println("InvalidSensorError");
        return false;
        break;
    }
}