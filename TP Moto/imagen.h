#ifndef IMAGEN_H
#define IMAGEN_H

#include<stdint.h>
#include<assert.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdbool.h>


typedef uint16_t pixel_t;

typedef struct{
    pixel_t **pixeles;
    size_t ancho, alto;
}imagen_t;

size_t imagen_get_ancho(const imagen_t *im);

size_t imagen_get_alto(const imagen_t *im);

pixel_t imagen_get_pixel(const imagen_t *im, size_t x, size_t y);

bool imagen_set_pixel(const imagen_t *im, size_t x, size_t y, pixel_t p);

pixel_t pixel3_crear(bool r, bool g, bool b);

void pixel3_a_rgb(pixel_t pixel3, uint8_t *r, uint8_t *g, uint8_t *b);

pixel_t pixel12_crear(uint8_t r, uint8_t g, uint8_t b);

void pixel12_a_rgb(pixel_t pixel12, uint8_t *r, uint8_t *g, uint8_t *b);

pixel_t pixel16_crear(uint8_t a, uint8_t b, uint8_t c,uint8_t d);

void pixel16_a_rgb(pixel_t pixel16, uint8_t *a, uint8_t *b, uint8_t *c, uint8_t *d);

void imagen_destruir(imagen_t *im);

imagen_t *imagen_generar(size_t ancho, size_t alto, pixel_t valor);

void imagen_pegar(imagen_t *destino, const imagen_t *origen, int x, int y);

imagen_t *imagen_escalar(const imagen_t *origen, size_t ancho_destino, size_t alto_destino);

void imagen_pegar_con_paleta( imagen_t *destino, const imagen_t *origen, int x, int y, const pixel_t paleta[]);

void imagen_escribir_ppm(const imagen_t *im, FILE *fo, void(*pixel_a_rgb)(pixel_t, uint8_t *, uint8_t *, uint8_t *, uint8_t *));

bool imagen_guardar_ppm(const imagen_t *im, const char *fn, void(*pixel_a_rgb)(pixel_t, uint8_t *, uint8_t *, uint8_t *, uint8_t *));

imagen_t *espejar_imagen(imagen_t *im);

void imagen_pegar_espejo(imagen_t *destino, const imagen_t *origen, int x, int y);

void imagen_a_textura(imagen_t *im, uint16_t *canvas);

void leer_ruta(imagen_t *im,  const char *fn);

bool leer_rom(uint16_t rom[]);

bool leer_teselas(imagen_t *teselas[]);

void imagen_pegar_texto(imagen_t *texto, imagen_t *teselas[], size_t* s, size_t x, size_t y, size_t fila, const pixel_t *paleta);

void pegar_texto(imagen_t *imagen, imagen_t *teselas[],char *s, size_t x, size_t y, const pixel_t *paleta);

void imagen_pegar360(imagen_t *destino, const imagen_t *origen, int x, int y);

#endif
