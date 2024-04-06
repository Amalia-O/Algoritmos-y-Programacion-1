#ifndef RUTA_H
#define RUTA_H

#include <stdbool.h>
#include <stddef.h>

#define MAXIMO_X 4199


struct figura_en_ruta {
    int figura;     
    size_t paleta;
    int y;
    bool reflejar;
};

struct figura{
    size_t inicio;
    size_t ancho;
    size_t alto;
};

size_t get_datos_figura(size_t indice_figura, bool *reflejar,size_t * paleta);

imagen_t *generar_figura(uint16_t rom[], size_t indice_figura);

struct ruta {
    float radio_curva;
    size_t indice_figura;
};

extern const struct figura_en_ruta figuras_en_ruta[];
extern const struct ruta ruta[4200 + 70];

float ruta_curvatura (size_t posicion);

void obtener_u_c(size_t inicio, float u[]);

void obtener_u_l(float posicion_moto, float u[]);

#endif
