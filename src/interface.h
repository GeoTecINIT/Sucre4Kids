struct SENSOR
{
    int id;
    int condicion;
    int bloque;
    int puerto;
};
struct SENSORES_POR_BLOQUE
{
    SENSOR sensoresBloque[20];
};

// Cada bloque tiene una secuencia de condiciones. conditions[0] ==> condiciones del bloque 0
struct CONDICION_BLOQUE
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

struct ACTUADORES_POR_BLOQUE
{
    ACTUADOR actuadoresBloque[20];
};