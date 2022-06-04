struct SENSOR
{
    int id;
    int condicion;
    int bloque;
    int puerto;
};

struct CONDICION
{
    bool condicionesBloque[19];
};

struct ACTUADOR
{
    int id;
    int condicion;
    int bloque;
    int puerto;
    bool evaluate;
};

struct BLOQUE
{
    int numSensores = 0;
    SENSOR sensores[20];
    int numActuadores = 0;
    ACTUADOR actuadores[20];
    CONDICION condiciones;
};

typedef struct BLOQUE Bloque;
