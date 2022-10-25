typedef struct SENSOR
{
    int id;
    int condicion;
    int bloque;
    int puerto;
} Sensor;

typedef struct CONDICION
{
    bool condicionesBloque[19];
} Condicion;

typedef struct ACTUADOR
{
    int id;
    int condicion;
    int bloque;
    int puerto;
    bool evaluate;
} Actuador;

typedef struct BLOQUE
{
    int numSensores = 0;
    SENSOR sensores[20];
    int numActuadores = 0;
    ACTUADOR actuadores[20];
    CONDICION condiciones;
} Bloque;
