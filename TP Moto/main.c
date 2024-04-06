#include <SDL2/SDL.h>
#include <stdbool.h>

#include "imagen.h"
#include "paleta.h"
#include "fondo.h"
#include "config.h"
#include "ruta.h"
#include "moto.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

imagen_t *generar_mosaico(imagen_t *teselas[], const pixel_t paleta[][8], size_t filas, size_t columnas, const uint16_t mosaico_teselas[filas][columnas], const uint8_t mosaico_paletas[filas][columnas]){

	imagen_t *imagen = imagen_generar(columnas * 8, filas * 8, 0); 
	if (imagen == NULL)
		return NULL;

	for (size_t i = 0; i < filas; i++)
	{
		for (size_t j = 0; j < columnas; j++)
		{
			imagen_pegar_con_paleta(imagen, teselas[mosaico_teselas[i][j]], j * 8, i * 8, paleta[mosaico_paletas[i][j]]);
		}
	}

	return imagen;
}


int main()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;

	SDL_CreateWindowAndRenderer(VENTANA_ANCHO, VENTANA_ALTO, 0, &window, &renderer);
	SDL_SetWindowTitle(window, "Hang-On");

	SDL_Texture *texture = SDL_CreateTexture(renderer,
											 SDL_PIXELFORMAT_RGB444, SDL_TEXTUREACCESS_STATIC, VENTANA_ANCHO, VENTANA_ALTO);
	uint16_t canvas[VENTANA_ALTO * VENTANA_ANCHO];

	int dormir = 0;

	// BEGIN código del alumno

	// begin variables
	
	float x_moto = 4100;
	float y_moto = 0;
	size_t giro_moto = 0, paleta_moto = 0;
	bool acelerar = false, frenar = false, izquierda = false, derecha = false, giro_izquierda = false, choque = false, primera_aceleracion = false;
				   
	float vi = 0;				// velocidad v_i en km/h
	float vf = 0;				// velocidad v_(i+1) en km/h
	float delta_t = 1 / (float)30; 		// tiempo en segundos

 	float tiempo_nivel = 75;		//tiempo del nivel
   	float puntaje = 0; 
    	size_t tiempo_choque=5;
   	float top_puntaje = 0;
 
    	int frames = 0;
	float radio_acumulado = 0;		//para el desplazamiento de la ruta
   	size_t contador_inicio = 0;		//se utiliza al iniciar el juego
   	size_t paleta_inicial = 0;
	
	//end variables
	
	// leer rom

	uint16_t rom[229376];
	leer_rom(rom);

	// end leer rom
	
	// begin figuras

	imagen_t **vector_moto = crear_moto(rom);
	if(vector_moto == NULL)	return 1;
	
	imagen_t **vector_moto_reflejo = reflejar_vector(vector_moto);
	if(vector_moto_reflejo == NULL)	return 1;

	imagen_t **vector_figuras = crear_figuras(rom);
	if(vector_figuras == NULL)	return 1;
	
	imagen_t **vector_figuras_reflejo = reflejar_vector(vector_figuras);
	if(vector_figuras_reflejo == NULL)	return 1;
	
	// {ARBOL, CARTEL, ROCA, BELL, FORUM, DELFIN, SEMAFORO, VIGA, BANNER, GOAL, NULL}

	// end leer figuras

	// begin pasto
	
	imagen_t *pasto = imagen_generar(1, 96, pixel12_crear(0, 13, 9));
	
	pixel_t colores_pasto[10] = {0x089, 0x099, 0x099, 0x0a9, 0x0a9, 0x0a9, 0x0b9, 0x0b9, 0x0c9, 0x0c9};
	for (size_t i = 0; i < 10; i++)
		imagen_set_pixel(pasto, 0, i, colores_pasto[i]);

	imagen_t *pasto_estirado = imagen_escalar(pasto, VENTANA_ANCHO, 128);
	imagen_destruir(pasto);
	
	// end pasto

	// begin fondo

	imagen_t *teselas[CANTIDAD_TESELAS];

	for (size_t i = 0; i < CANTIDAD_TESELAS; i++)
		teselas[i] = imagen_generar(ANCHO_TESELA, ALTO_TESELA, 0);

	if (!leer_teselas(teselas)){
		fprintf(stderr, "No se pudieron leer las teselas\n");

		for (size_t i = 0; i < CANTIDAD_TESELAS; i++)
			imagen_destruir(teselas[i]);

		return 1;
	}
	imagen_t *fondo1 = generar_mosaico(teselas, paleta_3, 2, 216, fondo1_mosaico, fondo1_paleta);
	imagen_t *fondo1grande = imagen_escalar(fondo1, 1728, 16);
	imagen_destruir(fondo1);
	imagen_t *fondo2 = generar_mosaico(teselas, paleta_3, 8, 152, fondo2_mosaico, fondo2_paleta);
	imagen_t *fondo2grande = imagen_escalar(fondo2, 1216, 64);
	imagen_destruir(fondo2);

	// end fondo
	
	// begin ruta
	
	imagen_t *ruta_im = imagen_generar(512, 100, 0);
	leer_ruta(ruta_im, RUTA);
	imagen_t *ruta_im2 = espejar_imagen(ruta_im);

	imagen_t **filas_ruta = malloc(sizeof(imagen_t *) * 128);
	for (size_t i = 0; i < 100; i++){
		filas_ruta[i] = imagen_generar(1024, 1, 0);
		for (size_t j = 0; j < 512; j++){
			imagen_set_pixel(filas_ruta[i], j, 0, imagen_get_pixel(ruta_im,j,i));
			imagen_set_pixel(filas_ruta[i], j + 504, 0, imagen_get_pixel(ruta_im2,j,i));
		}
	}
	imagen_destruir(ruta_im);
	imagen_destruir(ruta_im2);
	
	// end ruta
	

	size_t paleta = 1;
	unsigned int ticks = SDL_GetTicks();
	while (1) {
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				break;
			// BEGIN código del alumno
			if (event.type == SDL_KEYDOWN) {
				// Se apretó una tecla
				switch (event.key.keysym.sym) {
				case SDLK_UP:
					acelerar = true;
					break;
				case SDLK_DOWN:
					frenar = true;
					break;
				case SDLK_RIGHT:
					derecha = true;
					break;
				case SDLK_LEFT:
					izquierda = true;
					break;
				}
			}
			else if (event.type == SDL_KEYUP)
			{
				// Se soltó una tecla
				switch (event.key.keysym.sym)
				{
				case SDLK_UP:
					acelerar = false;
					break;
				case SDLK_DOWN:
					frenar = false;
					break;
				case SDLK_RIGHT:
					derecha = false;
					break;
				case SDLK_LEFT:
					izquierda = false;
					break;
				}
			}
			// END código del alumno
			continue;
		}

		// BEGIN código del alumno

		imagen_t *cuadro = imagen_generar(320, 224, 0x00f);
		
		size_t ancho_cuadro = imagen_get_ancho(cuadro);
		size_t alto_cuadro = imagen_get_alto(cuadro);
		
		//begin fisica
		
		float u_l[96];
		float u_c[96];
		float u_r[96];

		obtener_u_c((int)truncf(x_moto), u_c);
		obtener_u_l(y_moto, u_l);
		
		for (size_t i = 0; i < 96; i++)
			u_r[i] = u_l[i] + u_c[i];
			
		//end física

		// begin controles
		
		if(tiempo_nivel !=0 && contador_inicio > 2 && !choque){
			tiempo_choque = 5;
			
			if(acelerar){ 

				acelerar_moto(y_moto, &vi, &vf);
				primera_aceleracion = true;	
			}
    	
			if(frenar && vi > 81){
			
				frenar_moto(&vi, &vf);
			}
			
			if(!frenar && !acelerar && primera_aceleracion){
		
				if(vi < 80){
					 
					acelerar_moto(y_moto, &vi, &vf);
				}
				
				if (vi > 81){
				
					desacelerar_moto(&vi, &vf);
				}	
			} 
		

			if (derecha){
		
				giro_izquierda = giro_derecha_moto( &y_moto, &giro_moto);
			}	
			

			if (izquierda){ 
		
				giro_izquierda = giro_izquierda_moto( &y_moto, &giro_moto);
			}

			if(!izquierda && !derecha && giro_moto > 0) giro_moto--;
		}
		
		if (choque){
			giro_moto = 0;
			
			if(y_moto+144 > 144)	y_moto-=3;
			if(y_moto+144 < 144)	y_moto+=3;
			
			if(frames == 0)	tiempo_choque--;
			if(tiempo_choque ==0){
				choque = false;				
				}
			vf = 0;
			vi = vf;
		}
		
		//end controles

		// begin ruta
		
		imagen_pegar(cuadro, pasto_estirado, 0, 128);
		
		float u_ruta;
		
		if(choque || tiempo_nivel == 0) paleta = paleta_inicial;
		
		for (size_t i = 96; i > 0; i--){
		
			if(!choque && tiempo_nivel)	paleta_inicial = paleta;
			
			u_ruta =  y_moto * (96.0-i) / 5000.0 + u_r[96-i] - 346 ;
			
			float v_ruta = log( (float)i/96.0 )/(-0.11) ;	
		
			imagen_pegar_con_paleta(cuadro, filas_ruta[i],u_ruta, 127+i, color_ruta[paleta]);
			
			paleta = (size_t)(x_moto + v_ruta)%4;
		
		}

		// end ruta

		// begin fondo

    		imagen_pegar360(cuadro, fondo2grande, ancho_cuadro - radio_acumulado*0.75, 64);
    		imagen_pegar360(cuadro, fondo1grande, ancho_cuadro - radio_acumulado, 112);

		// end fondo
		
		//begin figuras
	
		//arco de inicio

		float u1_d, v1_d;
		size_t a, h;
		
		if( 7 - x_moto > 0){	
				
			//semaforos
			
			imagen_t* figura_escalada1 = escalar_figura(vector_figuras[6], 7 - x_moto, &u1_d, &v1_d);
			
			size_t figura1_ancho = imagen_get_ancho(figura_escalada1);
			size_t figura1_alto = imagen_get_alto(figura_escalada1);
		
			float ul = -y_moto *(96 - v1_d)/96;
			
			if( 224 -v1_d - figura1_alto > 0){
				imagen_pegar_con_paleta(cuadro,figura_escalada1, ul+ CENTRO_VERTICAL +u1_d - 2*figura1_ancho , alto_cuadro -v1_d - figura1_alto,paleta_4[41+contador_inicio]);
				imagen_pegar_con_paleta(cuadro,figura_escalada1, ul - u1_d+ figura1_ancho + CENTRO_VERTICAL , alto_cuadro -v1_d - figura1_alto,paleta_4[41+contador_inicio]);
				
			}
			
			imagen_destruir(figura_escalada1);
			
			//vigas	
			
			imagen_t* figura_escalada2 = escalar_figura(vector_figuras[7], 7 - x_moto, &u1_d, &v1_d);
			size_t figura2_ancho = imagen_get_ancho(figura_escalada2);
			size_t figura2_alto = imagen_get_alto(figura_escalada2);
			
			if(v1_d>0){
				imagen_pegar_con_paleta(cuadro,figura_escalada2, ul+CENTRO_VERTICAL +u1_d - 2*figura1_ancho , alto_cuadro-v1_d-figura1_alto-figura2_alto,paleta_4[45]);
				imagen_pegar_con_paleta(cuadro,figura_escalada2,ul-u1_d+(figura1_ancho + CENTRO_VERTICAL)+figura1_ancho -figura2_ancho , alto_cuadro -v1_d-figura1_alto-figura2_alto,paleta_4[45]);
			}
			
			imagen_destruir(figura_escalada2);
			
			//banner
			imagen_t* figura_escalada3 = escalar_figura(vector_figuras[8], 7 - x_moto, &u1_d, &v1_d);
			
			imagen_pegar_con_paleta(cuadro,figura_escalada3,ul+CENTRO_VERTICAL +u1_d,alto_cuadro-v1_d-figura1_alto-figura2_alto,paleta_4[45]);
			
			imagen_destruir(figura_escalada3);
		}
	
		//arco final
	
		if(4200- x_moto < 30){
	
			//semaforos
			
			imagen_t *figura_escalada1 = escalar_figura(vector_figuras[6], MAXIMO_X-x_moto, &u1_d, &v1_d);
			
			float ul = -y_moto *(96 - v1_d)/96;
			
			size_t figura1_ancho = imagen_get_ancho(figura_escalada1);
			size_t figura1_alto = imagen_get_alto(figura_escalada1);
			
			imagen_pegar_con_paleta(cuadro,figura_escalada1, ul+CENTRO_VERTICAL +u1_d - 2*figura1_ancho , VENTANA_ALTO -v1_d - figura1_alto,paleta_4[44]);
			imagen_pegar_con_paleta(cuadro,figura_escalada1, ul-u1_d+figura1_ancho + CENTRO_VERTICAL, VENTANA_ALTO -v1_d - figura1_alto,paleta_4[44]);
			
			
			imagen_destruir(figura_escalada1);
			
			//vigas		
			
			imagen_t *figura_escalada2 = escalar_figura(vector_figuras[7], MAXIMO_X-x_moto, &u1_d, &v1_d);

			size_t figura2_ancho = imagen_get_ancho(figura_escalada2);
			size_t figura2_alto = imagen_get_alto(figura_escalada2);			
			
			imagen_pegar_con_paleta(cuadro,figura_escalada2, ul+CENTRO_VERTICAL+u1_d-2*figura1_ancho , VENTANA_ALTO-v1_d-figura1_alto-figura2_alto , paleta_4[45]);
			imagen_pegar_con_paleta(cuadro,figura_escalada2, ul-u1_d+figura1_ancho + CENTRO_VERTICAL+figura1_ancho -figura2_ancho ,VENTANA_ALTO-v1_d-figura1_alto-figura2_alto, paleta_4[45]);
			
			imagen_destruir(figura_escalada2);
			
			//banner

			imagen_t *figura_escalada3 = escalar_figura(vector_figuras[8], MAXIMO_X-x_moto, &u1_d, &v1_d);
			
			imagen_pegar_con_paleta(cuadro,figura_escalada3, ul + CENTRO_VERTICAL + u1_d ,VENTANA_ALTO-v1_d-figura1_alto-figura2_alto,paleta_4[47]);
			
			imagen_destruir(figura_escalada3);
		}
		
		//begin figuras
		for(size_t d = 60; d > 0; d--){
			
			int v_d = 96 - 96*exp(-0.11*d); 
			
			struct ruta aux = ruta[(int)trunc(x_moto) + d]; 
			
			int y_figura =  figuras_en_ruta[aux.indice_figura].y;

			float u_figura = y_figura * ((96 - v_d) / 96.0) + y_figura * v_d / 5000.0 + u_r[v_d];

			bool reflejar;
			size_t paleta;
			size_t indice = get_datos_figura(x_moto+d, &reflejar, &paleta);		
			
			if (indice < 10){ 				
				
				h = (imagen_get_alto(vector_figuras[indice]))*(float)(96-v_d)/96 + (float)(5*v_d)/96;
				a = ((float)h/(float)imagen_get_alto(vector_figuras[indice]))*imagen_get_ancho(vector_figuras[indice]);
				a = a<3? 3 : a;
				
				   
				struct figura_en_ruta figura = figuras_en_ruta[aux.indice_figura];
				int desplazamiento = -a/2; 
				
				if(reflejar){
					imagen_t* espejada = imagen_escalar(vector_figuras_reflejo[indice], a ,h);
					imagen_pegar_con_paleta(cuadro,espejada, u_figura+CENTRO_VERTICAL+desplazamiento,alto_cuadro-v_d-h,paleta_4[figura.paleta]);
					imagen_destruir(espejada);
				}
				else{
					imagen_t* figura_escalada = imagen_escalar(vector_figuras[indice], a ,h);
					imagen_pegar_con_paleta(cuadro , figura_escalada , u_figura+CENTRO_VERTICAL+desplazamiento , alto_cuadro-v_d-h , paleta_4[figura.paleta]);
					imagen_destruir(figura_escalada);
					}

				// y_figura (int) + a (size_t) /2
				// y_figura int + algo size_t ->size_t
				int moto_derecha = (int) truncf((y_moto + get_ancho_moto(giro_moto, vector_moto)/2));
				int moto_izquierda = (int)  truncf(y_moto - get_ancho_moto(giro_moto, vector_moto)/2);
				
				int figura_izquierda = (int) truncf(y_figura - a/2.0);
				int figura_derecha = (int) truncf(y_figura + a/2.0);
				
				if ((moto_derecha >  figura_izquierda) 
					&& (moto_izquierda < figura_derecha) 
					&& v_d<10) choque = true;
			}
		} 

		// end figuras
		
		//begin texturas moto
		
		if(tiempo_nivel !=0 && contador_inicio >2 && primera_aceleracion)
			paleta_moto = moto_obtener_paleta(choque, frenar, paleta_moto);

		if (giro_izquierda)
			imagen_pegar_con_paleta(cuadro, vector_moto_reflejo[giro_moto], 144 + ( 18 - get_ancho_moto(giro_moto, vector_moto)/2 ), 224 - get_alto_moto(giro_moto, vector_moto), paleta_4[paleta_moto]);
		else
			imagen_pegar_con_paleta(cuadro, vector_moto[giro_moto], 144 + ( 18 - get_ancho_moto(giro_moto, vector_moto)/2 ), 224 - get_alto_moto(giro_moto, vector_moto), paleta_4[paleta_moto]);
			
		//end texturas moto
		
		//begin sumas
		
		if(tiempo_nivel !=0 && x_moto < 4200){
			frames++;
			if(frames == 30){
				tiempo_nivel--;
				frames = 0;
				if(contador_inicio<3){
				 	tiempo_nivel++;
				 	contador_inicio++;
				 	}
					
			}
			if(vf!=0 && !choque ){
				radio_acumulado += ruta_curvatura((int)truncf(x_moto));
		
				float delta_x =(vf*1000/3600)*delta_t ;
				y_moto -= 2.5*delta_x*ruta_curvatura(x_moto);
				x_moto += delta_x;
			
				
				if(112 + 234 + y_moto + u_ruta  + imagen_get_ancho(vector_moto[giro_moto])/2 < 215 && y_moto + 112 +234 + u_ruta -imagen_get_ancho(vector_moto[giro_moto]) > -215){
					if( vf<117){ puntaje+= (delta_x*125);	}
					else { puntaje += (delta_x*(3.13*vf - 240));	}
				}
			}
		}
		else if(x_moto > 4200){
			tiempo_nivel = 0;
			imagen_pegar_con_paleta(cuadro,vector_figuras[9], (CENTRO_VERTICAL-imagen_get_ancho(vector_figuras[9])),(alto_cuadro-imagen_get_alto(vector_figuras[9]))/2,paleta_4[28]);
			vf = 0;	
			if (top_puntaje	< puntaje)	top_puntaje = puntaje;		
		}
			
		else if(x_moto < 4200 && tiempo_nivel == 0){ pegar_texto(cuadro, teselas, "GAME OVER",	40, 80,paleta_3[5]); }	

		//end sumas
		
		//begin textos
		
		char velocidad[100];
		int s = vf;
		sprintf(velocidad, "SPEED %d KM", s);	
	
		s = puntaje;
		char puntaje_display[100];
		sprintf(puntaje_display, "%d", s);	

		s = tiempo_nivel;
		char tiempo_display[100];
		sprintf(tiempo_display, "%d", s);

		s =  top_puntaje; 
		char top_display[100];
		sprintf(top_display, "%10.5d", s);
		
		
		pegar_texto(cuadro, teselas, "STAGE 1", 24, 40, paleta_3[5]);
		
		imagen_pegar_texto(cuadro, teselas, top, 16, 8, 5, paleta_3[5]);
		pegar_texto(cuadro, teselas, top_display,24, 24,paleta_3[6]);
		
		imagen_pegar_texto(cuadro, teselas, tiempo, 136, 8, 6, paleta_3[5]);
		pegar_texto(cuadro, teselas, tiempo_display,	136, 40,paleta_3[5]);
		
    		imagen_pegar_texto(cuadro,teselas,score,	184, 8,7,paleta_3[5]);
		pegar_texto(cuadro, teselas, puntaje_display, 240,24, paleta_3[7]);
		
		pegar_texto(cuadro, teselas, velocidad,	184, 40,paleta_3[5]);
		
		// end textos

		// begin cuadro escalado
		imagen_t *cuadro_escalado = imagen_escalar(cuadro, VENTANA_ANCHO, VENTANA_ALTO);
		imagen_a_textura(cuadro_escalado, canvas);

		imagen_destruir(cuadro_escalado);
		imagen_destruir(cuadro);
		// end cuadro escalado

		// END código del alumno

		SDL_UpdateTexture(texture, NULL, canvas, VENTANA_ANCHO * sizeof(uint16_t));
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		ticks = SDL_GetTicks() - ticks;
		if(dormir){
			SDL_Delay(dormir);
			dormir = 0;
		}
		else if (ticks < 1000 / JUEGO_FPS)
			SDL_Delay(1000 / JUEGO_FPS - ticks);
		else
			printf("Perdiendo cuadros\n");
		ticks = SDL_GetTicks();
	}

	// BEGIN código del alumno

	imagen_destruir(fondo2grande);
	imagen_destruir(fondo1grande);

	for (size_t i = 0; i < 100; i++){
	
		imagen_destruir(filas_ruta[i]);
	}
	free(filas_ruta);

	imagen_destruir(pasto_estirado);

	vector_destruir(vector_moto);
	vector_destruir(vector_moto_reflejo);
	
	vector_destruir(vector_figuras);
	vector_destruir(vector_figuras_reflejo);	

	for (size_t i = 0; i < CANTIDAD_TESELAS; i++) imagen_destruir(teselas[i]);
	
	//  END código del alumno

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
} 
