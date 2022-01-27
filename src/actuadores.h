// // actuador TagInfo, sensoresArray
// void activarActuador(int actuador, int opcion, int numSensores, char *puertosSensores[])
// {
//     Serial.printlnf("Actuador: %d; Tipo: %d; Actua sobre %d sensores en: %s & %s", actuador, opcion, numSensores, puertosSensores[0], puertosSensores[1]);

//     switch (actuador)
//     {
//     // Actuador led
//     case 0:
//         switch (opcion)
//         {
//         case 0:
//             ledVerdeRojo(valor);
//             break;

//         case 1:
//             ledMoradoAmarillo(valor);
//             break;

//         case 2:
//             ledAzulNaranja(valor);
//             break;

//         case 3:
//             ledBlink(valor);
//             break;

//         case 4:
//             ledArcoiris(valor);
//             break;

//         case 5:
//             ledOnOff(valor);
//             break;

//         default:
//             Serial.println("Valor no valido");
//             break;
//         }
//         break;

//     // Zumbador
//     case 1:
//         switch (opcion)
//         {
//         case 0:
//             zumbador(valor);
//             break;
//         case 1:
//             blinkAndSleep(valor);
//             break;
//         default:
//             Serial.println("Valor no valido");
//             break;
//         }
//         break;

//     default:
//         Serial.println("Valor no valido");
//         break;
//     }
// }

// void ledApagar()
// {
//     leds.setColorRGB(0, 0, 0, 0);
// }
// void ledRojo()
// {
//     leds.setColorRGB(0, 255, 0, 0);
// }
// void ledVerde()
// {
//     leds.setColorRGB(0, 0, 255, 10);
// }
// void ledAzul()
// {
//     leds.setColorRGB(0, 0, 0, 255);
// }
// void ledAmarillo()
// {
//     leds.setColorRGB(0, 255, 233, 0);
// }
// void ledMorado()
// {
//     leds.setColorRGB(0, 108, 70, 117);
// }
// void ledNaranja()
// {
//     leds.setColorRGB(0, 255, 128, 0);
// }
// void ledBlanco()
// {
//     leds.setColorRGB(0, 255, 255, 255);
// }

// // Acciones del zumbador
// void pitidoON()
// {
//     // Serial.println("Zumbador ON");
//     digitalWrite(Zumbador_PIN, 1);
//     digitalWrite(Zumbador_PIN, HIGH);
//     digitalWrite(D3, HIGH);
//     digitalWrite(D3, 1);
// }
// void pitidoOFF()
// {
//     // Serial.println("Zumbador OFF");
//     digitalWrite(Zumbador_PIN, 0);
//     digitalWrite(Zumbador_PIN, LOW);
//     digitalWrite(D7, LOW);
//     digitalWrite(D7, 0);
// }

// void pitidoBlink()
// {
//     pitidoON();
//     delay(100);
//     pitidoOFF();
//     delay(100);
// }

// void ledAzulNaranja(boolean estado)
// {
//     if (estado)
//     {
//         ledAzul();
//     }
//     else
//     {
//         ledNaranja();
//     }
// }
// void ledMoradoAmarillo(boolean estado)
// {
//     if (estado)
//     {
//         ledMorado();
//     }
//     else
//     {
//         ledAmarillo();
//     }
// }
// void ledVerdeRojo(boolean estado)
// {
//     if (estado)
//     {
//         ledRojo();
//     }
//     else
//     {
//         ledVerde();
//     }
// }
// void ledBlink(boolean estado)
// {
//     if (estado)
//     {
//         ledBlanco();
//         delay(400);
//         ledApagar();
//         delay(200);
//     }
//     else
//     {
//         ledApagar();
//     }
// }

// void ledOnOff(boolean estado)
// {
//     if (estado)
//     {
//         ledBlanco();
//     }
//     else
//     {
//         ledApagar();
//     }
// }

// void ledArcoiris(boolean estado)
// {
//     if (estado)
//     {
//         ledRojo();
//         delay(200);
//         ledNaranja();
//         delay(200);
//         ledAmarillo();
//         delay(200);
//         ledVerde();
//         delay(200);
//         ledMorado();
//         delay(200);
//         ledAzul();
//         delay(200);
//     }
//     else
//     {
//         ledApagar();
//     }
// }
// void blinkAndSleep(boolean estado)
// {
//     if (estado)
//     {
//         pitidoBlink();
//     }
//     else
//     {
//         pitidoOFF();
//     }
// }
// void zumbador(boolean estado)
// {
//     if (estado)
//     {
//         pitidoON();
//     }
//     else
//     {
//         pitidoOFF();
//     }
// }
