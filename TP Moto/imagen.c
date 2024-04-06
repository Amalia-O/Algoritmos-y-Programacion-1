#include"imagen.h"
#include"config.h"
#include "paleta.h"
#include "fondo.h"

#define MASK_LINE 0x000f
#define MASK_A 0xf000
#define MASK_C 0x00f0
#define MASK_D 0x000f
#define MASK_B2 0x0f00


#include<stdint.h>
#include<assert.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdbool.h>
#include<math.h>


void imagen_destruir(imagen_t *imagen){
	
	if (!imagen) return;
	//misma implementacion que el ejercicio 3
	for(size_t j = 0; j < imagen->alto ; j++){
	
		free(imagen->pixeles[j]);
	}
	
	free(imagen->pixeles);
	free(imagen);
}

static imagen_t *_imagen_crear(size_t ancho, size_t alto){

	//misma implementacion que el ej3

	imagen_t *aux = malloc(sizeof(imagen_t));		//valido la memoria pedida
	if(aux == NULL){
		return NULL;
	}

	aux->ancho = ancho;
	aux->alto = alto;	

	aux->pixeles = malloc(alto * sizeof(pixel_t*));		//valido la memoriaa pedida 512
	if (aux->pixeles == NULL){
		free(aux);
		return NULL;
	}
	
	for(size_t i = 0; i < alto; i++){
		aux->pixeles[i] = malloc(ancho * sizeof(pixel_t));
		if (aux->pixeles[i] == NULL){			//valido la memoria pedida
			aux->alto = i;
			imagen_destruir(aux);
			return NULL;	
		}
	}


return aux ;
}


imagen_t *imagen_generar(size_t ancho, size_t alto, pixel_t valor){

	imagen_t *ptr = _imagen_crear (ancho , alto);

	if(ptr == NULL){
		return NULL;
	}

	for(size_t i = 0; i < alto ; i++){

		for(size_t j = 0 ; j < ancho ; j++){
		//considero que mi matriz **pixeles se puede recorrer de la forma pixeles[fila][columna] (explicado en clase)
			ptr->pixeles[i][j] = valor;
		}
	}


return ptr;
}

void imagen_pegar(imagen_t *destino, const imagen_t *origen, int x, int y){

	//misma implementación que el ejercicio obligatorio 2.
		
	for (int i = 0 ; i < origen->alto ; i++){
	
		for(int j = 0 ; j < origen->ancho ; j++){
			
			if(origen->pixeles[i][j]>0 && (x+j) < destino->ancho && (y+i) < destino->alto && origen->pixeles[i][j] != 255){
			
				destino->pixeles[y+i][x+j] = origen->pixeles[i][j];
			} 
		}
	}
}

imagen_t *imagen_escalar(const imagen_t *origen, size_t ancho_destino, size_t alto_destino){

	imagen_t *destino = imagen_generar(ancho_destino, alto_destino, 0);

	if(destino==NULL){
		return NULL;
}

	//misma implementación que el ejercicio obligatorio 2.

	float escala_alto = 1.0*origen->alto/alto_destino;
	float escala_ancho = 1.0*origen->ancho/ancho_destino;

	for (size_t i = 0 ; i < alto_destino ; i++){
	
		for(size_t j = 0; j < ancho_destino ; j++){
		
			int m = (i*escala_alto);
			int n = (j*escala_ancho);		
			destino->pixeles[i][j] = origen->pixeles[m][n];
		
	}
}

return destino;
}

size_t imagen_get_ancho(const imagen_t *im){
	size_t ancho = im->ancho;
	return ancho;
}

size_t imagen_get_alto(const imagen_t *im){
	size_t alto = im->alto;
	return alto;
}

pixel_t imagen_get_pixel(const imagen_t *im, size_t x, size_t y){
	return im->pixeles[y][x];
}

bool imagen_set_pixel(const imagen_t *im, size_t x, size_t y, pixel_t p){

	if(x>=im->ancho || y>=im->alto)
		return false;
	
	im->pixeles[y][x] = p;
	return true;
}

pixel_t pixel3_crear(bool r, bool g, bool b){
//devuelve un pixel_t [r g b]
	
	pixel_t pixel = 0x00;	
	
	if(r)
		pixel|= MASK_R;	
	else
		pixel &= ~MASK_R;
	
	if(g)
		pixel|= MASK_G;	
	else
		pixel &= ~MASK_G;	
		
	if(b)
		pixel|= MASK_B;	
	else
		pixel &= ~MASK_B;
	
return pixel;
}

void pixel3_a_rgb(pixel_t pixel3, uint8_t *r, uint8_t *g, uint8_t *b){

	if((pixel3 & MASK_R)!=0){
		*r=255;
	}
	else{
		*r=0;
	}
	
	if((pixel3 & MASK_G)!=0){
		*g=255;
	}
	else{
		*g=0;
	}
	
	if((pixel3 & MASK_B)!=0){
		*b=255;
	}
	else{
		*b=0;
	}	

}


pixel_t pixel12_crear(uint8_t r, uint8_t g, uint8_t b){		//si r=1000 	g=0100 		b=0001
								
	pixel_t pixel = 0;
	return (pixel | (r<<SHIFT_R) | (g<<SHIFT_G) | b);	//pixel = xxxx 1000 0100 0001
}


void pixel12_a_rgb(pixel_t pixel12, uint8_t *r, uint8_t *g, uint8_t *b){	

	*r = (pixel12 >> 8) << 4;					
	
	*g = ((pixel12 >> 4)& MASK_COMPONENTE)<<4;					

	*b = (pixel12 & MASK_COMPONENTE)<<4;			
	
	
}
pixel_t pixel16_crear(uint8_t a, uint8_t b, uint8_t c,uint8_t d){
	
	pixel_t pixel = 0x0000;

	pixel |= d;
	pixel |= (c << 4);
	pixel |= (b << 8);
	pixel |= (a << 12);

	return pixel;

}

void pixel16_a_rgb(pixel_t pixel16, uint8_t *a, uint8_t *b, uint8_t *c, uint8_t *d){

	*a = (pixel16 & MASK_A) >> 12;	

	*b = (pixel16 & MASK_B2) >> 8;

	*c = (pixel16 & MASK_C) >>4 ; 

	*d = (pixel16 & MASK_D);
}



void imagen_pegar_con_paleta( imagen_t *destino, const imagen_t *origen, int x, int y, const pixel_t paleta[]){
		
	for(int f = y >= 0 ? 0 : -y; f < origen->alto && f + y < destino->alto; f++){
    	for(int c = x >= 0 ? 0 : -x; c < origen->ancho && c + x < destino->ancho; c++){
	
			if(origen->pixeles[f][c]>0){		
					destino->pixeles[y+f][x+c] = paleta[origen->pixeles[f][c]];
					}
			} 
		}
}


void imagen_escribir_ppm(const imagen_t *im, FILE *fo, void(*pixel_a_rgb)(pixel_t, uint8_t *, uint8_t *, uint8_t *, uint8_t *)){

	uint8_t a,b,c,d;
	size_t alto = imagen_get_alto(im);
	size_t ancho = imagen_get_ancho(im);
	fprintf(fo, "P3\n%ld\t%ld\n255\n", ancho, alto);
	
	for(size_t i=0; i<alto; i++){
	
		for(size_t j=0; j<ancho ; j++){
			pixel_a_rgb(im->pixeles[i][j], &a, &b, &c,&d);
			fprintf(fo, "%d\n%d\n%d\n%d",a,b,c,d);
		}
	}
	
	
}

bool imagen_guardar_ppm(const imagen_t *im, const char *fn, void(*pixel_a_rgb)(pixel_t, uint8_t *, uint8_t *, uint8_t *, uint8_t *)){
	
	
	FILE *archivo = fopen(fn, "wb");
	if(archivo == NULL)
		return false;
	imagen_escribir_ppm(im, archivo, pixel_a_rgb);
	
	fclose(archivo); 
	return true;
}


imagen_t *espejar_imagen(imagen_t *im){

	pixel_t aux;
	
	imagen_t *espejo = imagen_generar(im->ancho, im->alto, 0);
	if (espejo == NULL)	
		return NULL;

	
	for(size_t i=0 ; i < im->alto ; i++ ){
		
		for(size_t j = 0 ; j < im->ancho ; j++){
			
			aux = imagen_get_pixel(im, im->ancho-j-1, i);
			imagen_set_pixel(espejo, j, i, aux);		
		}
	}

	return espejo;
}

void imagen_pegar_espejo(imagen_t *destino, const imagen_t *origen, int x, int y){
		
	for (int i = 0 ; i < origen->alto ; i++){
	
		for(int j = 0 ; j < origen->ancho ; j++){
	
			if(origen->pixeles[i][j]>0 && (x-j) < destino->ancho && (y+i) < destino->alto && (origen->pixeles[i][j] != 0xfff)){
			
				destino->pixeles[y+i][x-j] = origen->pixeles[i][(origen->ancho)-j];
			} 
		}
	}
}  



void imagen_a_textura(imagen_t *im, uint16_t *canvas){
	for( size_t i = 0; i< im->alto ; i++){
		for (size_t j = 0 ; j < im->ancho ;j++){
		
			canvas[(i*im->ancho) + j] = imagen_get_pixel(im, j, i);
		}
	}        	
}

void leer_ruta(imagen_t *im,  const char *fn){
	
	FILE *f = fopen(fn, "rb");
	
	imagen_t *im2 = imagen_generar(512, 128, 0);
	if (im2 == NULL) return;
    pixel_t pixel, aux ,componente;
    size_t i, j;
   
   	for(size_t k = 0; k < 4 ; k++){ 
    	
    		for( i = 0; i < 128 ; i++){
    	
    			for( j = 0 ; j < 512 ; j++){
    			
       				fread(&pixel, sizeof(uint8_t) , 1 , f); 
       				
       				for(size_t l = 0; l < 8; l++, j++){
       					
       					aux = imagen_get_pixel(im2, j, i);
       					componente = (pixel & (MASK_B <<(7-l)))>>(7-l);
       					componente |= (aux<<1);
       					im2->pixeles[i][j] = componente;
    					}	
    					j--;
   	 		}
   	 	}
	}
    for( i = 16; i < 116 ; i++){
    
    	for( j = 0 ; im2->pixeles[i][j] == 15 ; j++){

    		im2->pixeles[i][j] = 0;
    		}
    	for(size_t k = 0; k< 512; k++){
    	
    		im->pixeles[i-16][k] = im2->pixeles[i][k];
    	}
    		
   	}	
	imagen_destruir(im2);
	fclose(f);
}


bool leer_rom(uint16_t rom[]){

	uint8_t ROM_1, ROM_2;
	
	uint16_t aux;
	
	size_t i,j;

	for(j = 0; j<7 ; j++){
		FILE *rom1 = fopen(*(roms + (2*j)),"rb");
			if(rom1 == NULL)	return false;
		FILE *rom2 = fopen(*(roms+1+(2*j)),"rb");
			if(rom2 == NULL)	return false;

		for(i = 0; i< 32768; i++){

			fread(&ROM_1 , sizeof(uint8_t),1,rom1);
			fread(&ROM_2 , sizeof(uint8_t),1,rom2);

			aux = ROM_1 | (ROM_2 << 8);
			rom[(i + (j*32768))] = aux;
			//printf("rom[%zd] = %x\n", i + (j*32768) , rom[i + (j*32768)]);

		}
		fclose(rom1);
		fclose(rom2);
	}
	
	return true;
}

bool leer_teselas(imagen_t *teselas[]){

	FILE *ROM_R =fopen( ARCHIVO_ROM_R, "rb");
	if(ROM_R == NULL)
		return false;

	FILE *ROM_G =fopen( ARCHIVO_ROM_G, "rb");
	if(ROM_G == NULL){
		fclose(ROM_R);
		return false;
		}
		
	FILE *ROM_B =fopen( ARCHIVO_ROM_B, "rb");
	if(ROM_B == NULL){
		fclose(ROM_R);
		fclose(ROM_G);
		return false;
		}

	uint8_t aux_r, aux_g, aux_b;
	pixel_t aux;
	bool componente_r, componente_g, componente_b;
	
	for(size_t i=0; i<CANTIDAD_TESELAS; i++){
		
		for(size_t y=0; y<ALTO_TESELA; y++){
			fread(&aux_r,1,1, ROM_R);
			fread(&aux_g,1,1, ROM_G);
			fread(&aux_b,1,1, ROM_B);
			
			for(size_t x=0; x<ANCHO_TESELA; x++){
				componente_r = (aux_r&(0x1<<(7-x)))>>(7-x);
				componente_g = (aux_g&(0x1<<(7-x)))>>(7-x);
				componente_b = (aux_b&(0x1<<(7-x)))>>(7-x);
				
				aux = pixel3_crear(componente_r, componente_g, componente_b);
				
				imagen_set_pixel(teselas[i], x, y, aux);	
			}
		}	
	}	
	fclose(ROM_R);
	fclose(ROM_G);
	fclose(ROM_B);
			
	return true;
}

void imagen_pegar_texto(imagen_t *texto, imagen_t *teselas[CANTIDAD_TESELAS], size_t* s, size_t x, size_t y, size_t fila, const pixel_t *paleta){
	for(size_t i = 0,j=0,k=0; k<fila * 3; i++,k++){
		if(i% fila == 0)j++,i=0;
			imagen_pegar_con_paleta(texto,teselas[(size_t)s[k]],x +i*8,y+j*8,paleta);
		
	}
}

void pegar_texto(imagen_t *imagen, imagen_t *teselas[CANTIDAD_TESELAS],char *s, size_t x, size_t y, const pixel_t *paleta){
	for(size_t i = 0; s[i]; i++){
		imagen_pegar_con_paleta(imagen,teselas[(size_t)s[i]], x + i * 8, y , paleta);
		}
}

void imagen_pegar360(imagen_t *destino, const imagen_t *origen, int x, int y){
		
	for(int i = y >= 0 ? 0 : -y; i < origen->alto && i + y < destino->alto; i++){
    	for(int j = x >= 0 ? 0 : -x; j < origen->ancho && j + x < destino->ancho; j++){
			
			if(origen->pixeles[i][j]>0 && (x+j) < destino->ancho && (y+i) < destino->alto && origen->pixeles[i][j] != 255){
		
				destino->pixeles[y+i][x+j] = origen->pixeles[i][j];
			} 
			
			else if(origen->pixeles[i][j]>0 && ( x +j -320 -origen->ancho )>0 && ( +x +j -320 - origen->ancho )<320 ){
				destino->pixeles[y+i][+x +j -320 -1 -origen->ancho] = origen->pixeles[i][j];
				//printf("destino->ancho - x - j = %ld\n",+x +j -origen->ancho );
			} 
		}
	}
}










