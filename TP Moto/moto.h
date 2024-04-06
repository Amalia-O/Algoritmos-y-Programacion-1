#ifndef MOTO_H
#define MOTO_H

#include<stdint.h>
#include<assert.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdbool.h>

size_t get_ancho_moto (size_t giro, imagen_t **vector_moto);

size_t get_alto_moto (size_t giro, imagen_t **vector_moto);

void vector_destruir( imagen_t **vector );

imagen_t *leer_imagen_rom(uint16_t rom[],size_t ancho, size_t alto,size_t inicio);

imagen_t **crear_moto(uint16_t rom[]);

imagen_t **crear_figuras(uint16_t rom[]);

imagen_t **reflejar_vector(imagen_t **vector);

imagen_t *escalar_figura(imagen_t *figura, float posicion, float *u_d, float *v_d);

void acelerar_moto( float y_moto, float *vi, float *vf);

void desacelerar_moto(float *vi, float *vf);

void frenar_moto( float *vi, float *vf);

bool giro_derecha_moto(float *y_moto, size_t *giro_moto);

bool giro_izquierda_moto( float *y_moto, size_t *giro_moto);

size_t moto_obtener_paleta( bool choque, bool frenar, size_t textura_moto);

#endif
