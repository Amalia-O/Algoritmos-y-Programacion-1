#ifndef CONFIG_H
#define CONFIG_H

#define CANTIDAD_TESELAS 4096
#define ANCHO_TESELA 8
#define ALTO_TESELA 8

#define ARCHIVO_ROM_R   "roms/6841.rom"
#define ARCHIVO_ROM_G   "roms/6842.rom"
#define ARCHIVO_ROM_B   "roms/6843.rom"

#define VENTANA_ANCHO 800
#define VENTANA_ALTO 600
#define CENTRO_VERTICAL 162

#define JUEGO_FPS 30

#define MASK_0 	0x00
#define MASK_R 	0x04		//0000 0100
#define MASK_G 	0x02		//0000 0010
#define MASK_B 	0x01		//0000 0001
#define SHIFT_R 8
#define SHIFT_G 4
#define MASK_COMPONENTE 0x0f	//0000 1111
#define RUTA "6840.rom"
#define E 2.7182818284
#endif
