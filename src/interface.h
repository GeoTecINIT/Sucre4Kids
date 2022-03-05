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
    bool actuadorTrue;
};

struct BLOQUE
{
    SENSOR sensores[20];
    ACTUADOR actuadores[20];
    CONDICION condiciones;
};

typedef struct BLOQUE Bloque;

Bloque bloques[2];
