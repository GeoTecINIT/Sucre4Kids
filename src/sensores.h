boolean noDistancia(int puerto)
{
    return true;
}
boolean siDistancia(int puerto)
{
    return true;
}

boolean siLuz(int puerto)
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

boolean noLuz(int puerto)
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

// boolean leerSensorSonido(int puerto)
// {
//     // Serial.printlnf("analogico: %d", analogRead(A4));
//     if (analogRead(puerto) >= 700)
//         return true;
//     return false;
// }

boolean siRuido(int puerto)
{
    if (analogRead(puerto) >= 700)
        return true;
    return false;
}

boolean noRuido(int puerto)
{
    if (analogRead(puerto) <= 700)
        return true;
    return false;
}

// boolean leerBoton(int puerto)
// {
//     pinMode(puerto, INPUT);
//     Serial.println("Leer boton");
//     if (digitalRead(puerto) == HIGH)
//         return true;
//     return false;
// }

boolean siBoton(int puerto)
{
    pinMode(puerto, INPUT);
    if (digitalRead(puerto) == HIGH)
        return true;
    return false;
}

boolean noBoton(int puerto)
{
    pinMode(puerto, INPUT);
    if (digitalRead(puerto) == LOW)
        return true;
    return false;
}

// boolean leerAngulo(int puerto)
// {
//     float voltage;
//     int sensor_value = analogRead(puerto);
//     voltage = (float)sensor_value * 5 / 1023;
//     float degrees = (voltage * 300) / 5;

//     if (degrees >= 680)
//     {
//         // Serial.println(degrees);
//         return true;
//     }
//     return false;
// }

boolean siRotativo(int puerto)
{
    float voltage;
    int sensor_value = analogRead(puerto);
    voltage = (float)sensor_value * 5 / 1023;
    float degrees = (voltage * 300) / 5;

    if (degrees >= 680)
        return true;
    return false;
}

boolean noRotativo(int puerto)
{
    float voltage;
    int sensor_value = analogRead(puerto);
    voltage = (float)sensor_value * 5 / 1023;
    float degrees = (voltage * 300) / 5;

    if (degrees <= 680)
        return true;
    return false;
}

boolean tempFrio(int puerto)
{
    return true;
}
boolean tempTemplado(int puerto)
{
    return true;
}
boolean tempCalor(int puerto)
{
    return true;
}

boolean leerSensor(int id, int condicion, int puerto)
{
    switch (id)
    {
    case 1:
        return (condicion == 0 ? noDistancia(puerto) : siDistancia(puerto));

    case 2:
        return (condicion == 0 ? noLuz(puerto) : siLuz(puerto));
        break;

    case 3:
        return (condicion == 0 ? noRuido(puerto) : siRuido(puerto));
        break;

    case 4:
        return (condicion == 0 ? noBoton(puerto) : siBoton(puerto));
        break;

    case 5:
        return (condicion == 0 ? noRotativo(puerto) : siRotativo(puerto));
        break;

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

    default:
        Serial.println("InvalidSensorError");
        return false;
        break;
    }
}