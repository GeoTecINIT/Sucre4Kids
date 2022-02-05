
boolean leerSensorLuz(int puerto)
{
    Serial.printlnf("Leer Luz");

    int threshold = 2500;
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

boolean leerBoton(int puerto)
{
    pinMode(puerto, INPUT);
    Serial.println("Leer boton");
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