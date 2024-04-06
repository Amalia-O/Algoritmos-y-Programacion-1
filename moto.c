#include"imagen.h"
#include"config.h"
#include "paleta.h"
#include "fondo.h"
#include "ruta.h"

#include<stdint.h>
#include<assert.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdbool.h>
#include<math.h>

size_t get_ancho_moto (size_t giro, imagen_t **vector_moto){

	return imagen_get_ancho(vector_moto[giro]);
}

size_t get_alto_moto (size_t giro, imagen_t **vector_moto){

	return imagen_get_alto(vector_moto[giro]);
}

void vector_destruir( imagen_t **vector ){

	size_t i;
	for(i = 0 ; vector[i] != NULL ; i++){
		imagen_destruir(vector[i]);
	}
	imagen_destruir(vector[i]);
	free(vector);
}

imagen_t *leer_imagen_rom(uint16_t rom[],size_t ancho, size_t alto,size_t inicio){

	pixel_t aux;
	uint8_t pixel1,pixel2,pixel3,pixel4;
	size_t contador = inicio;

	imagen_t *imagen = imagen_generar(ancho,alto,0);

	for(size_t i = 0; i < alto ; i++){
		for(size_t j = 0; j < ancho +2; j+=4){

			aux = rom[contador++];
	
			pixel16_a_rgb(aux, &pixel1,&pixel2,&pixel3,&pixel4);

			if(pixel1 != 0x000f) imagen_set_pixel(imagen,j,i, pixel1);
			if(pixel2 != 0x000f) imagen_set_pixel(imagen,j+1,i, pixel2);
			if(pixel3 != 0x000f) imagen_set_pixel(imagen,j+2,i, pixel3);

			if(pixel4 == 0x000f){
				aux = rom[contador++];
				while(aux == 0x0f0f ){
					aux = rom[contador++];
				}
				contador--;
				break;
			
			}else{
				imagen_set_pixel(imagen,j+3,i,pixel4);
			}		
		}
	}

	return imagen;
}

imagen_t **crear_moto(uint16_t rom[]){

	imagen_t **vector_moto = malloc(sizeof(imagen_t *) * 5); 
		if (vector_moto == NULL) return NULL;

	vector_moto[0] = leer_imagen_rom(rom, 36, 72, 532);
	vector_moto[1] = leer_imagen_rom(rom, 36, 70, 5670);
	vector_moto[2] = leer_imagen_rom(rom, 46, 63, 11284);
	vector_moto[3] = leer_imagen_rom(rom, 60, 54, 17215);
	vector_moto[4] = NULL;
	
	return vector_moto;
	
}
	
imagen_t **crear_figuras(uint16_t rom[]){
	size_t tamanio = 0;
	imagen_t *aux = generar_figura(rom, tamanio);
	while( aux != NULL){
		imagen_destruir(aux);
		tamanio++;	
		aux = generar_figura(rom, tamanio);
	}	

	imagen_t **vector_figuras = malloc(sizeof(imagen_t *)* (1+tamanio) );
	if (vector_figuras == NULL) return NULL;

	// {ARBOL, CARTEL, ROCA, BELL, FORUM, DELFIN, SEMAFORO, VIGA, BANNER, GOAL, NULL}

	for(size_t i = 0 ; i < tamanio ; i++ ){
	
		vector_figuras[i] = generar_figura(rom, i);
	}

	vector_figuras[tamanio] = NULL;
	
	return vector_figuras;
}
	
imagen_t **reflejar_vector(imagen_t **vector){

	size_t aux = 0;

	for(size_t i = 0 ; vector[i] != NULL ; i++){
		aux++;	}
	
	imagen_t **vector_reflejo = malloc(sizeof(imagen_t *) * (aux+1));
	
	if (vector_reflejo == NULL) return NULL;
	
	for(size_t i = 0 ; i < aux ; i++){
	
		vector_reflejo[i] = espejar_imagen(vector[i]);	
	}
	
	vector_reflejo[aux] = NULL;
	
	return vector_reflejo;

}

imagen_t *escalar_figura(imagen_t *figura, float posicion, float *u_d, float *v_d){

	*v_d= pow(E, (0-0.11) * posicion);
	*v_d = 96 - 96* (*v_d);
	*u_d = -112 * (96- (*v_d))/96;
	
	size_t ancho = imagen_get_ancho(figura);
	size_t alto = imagen_get_alto(figura);
			
	size_t h = (alto)*(float)(96-(*v_d))/96 + (float)(5*((*v_d)))/96;
	size_t a = ((float)h/(float)alto)*ancho;
	a = a<3? 3 : a;

	imagen_t* figura_escalada = imagen_escalar(figura, a ,h);
			
	return figura_escalada;

}

void acelerar_moto( float y_moto, float *vi, float *vf){

	float resultado = -0.224358*(1/(float)JUEGO_FPS);     	
	float exp = pow(E, resultado);
	*vf = 279 - ((279 - (*vi))*exp);
	*vi = *vf; 
				
       	if(y_moto > 215 && *vf > 92){
           	*vf-=3;
           	*vi= *vf;
        }
           		
	if(y_moto <-215 && *vf > 92){
           	*vf -=3;
           	*vi = *vf;
        }
}

void desacelerar_moto(float *vi, float *vf){

	*vf = *vi - (90 * (1.0*JUEGO_FPS/(3600)));
	*vi = *vf;
}

void frenar_moto( float *vi, float *vf){

	*vf = *vi - (300 * (1.0*JUEGO_FPS/3600));
	*vi = *vf;

}

bool giro_derecha_moto( float *y_moto, size_t *giro_moto){

	if (*giro_moto == 1) 
		(*y_moto) += 3;
	if (*giro_moto == 2) 
		(*y_moto) += 9;
	if (*giro_moto == 3)
		 (*y_moto) += 15;
	if (*y_moto > 435)
		(*y_moto) = 435;
	if (*giro_moto < 3)
		(*giro_moto)++;
		
	return false;
}

bool giro_izquierda_moto( float *y_moto, size_t *giro_moto){

	if ((*giro_moto) == 1) (*y_moto) -= 3;
	if ((*giro_moto) == 2) (*y_moto) -= 9;
	if ((*giro_moto) == 3) (*y_moto) -= 15;

	if ((*y_moto) < -435)
		(*y_moto) = -435;
				
	if ((*giro_moto) < 3)
		(*giro_moto)++;
		
	return true;
}

size_t moto_obtener_paleta( bool choque, bool frenar, size_t textura_moto){

	if(choque)	return 2;
	
	else if(frenar){
		return (textura_moto < 3 ? 3 : 2); 		
	}
	
	return(textura_moto < 1 ? 1 : 0);		
}







	
