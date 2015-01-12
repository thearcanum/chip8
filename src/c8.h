#ifndef __CHIP_8__
#define __CHIP_8__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <time.h>

#define MEM_SIZE	4096
#define REGS_COUNT	16
#define CODE_ADDR	0x200

/* Graphiques en noir et blanc, 64*32 pixels */

#define PIXEL_SIZE	8
#define SCR_X		64
#define	SCR_Y		32

typedef struct
{
	uint8_t map[SCR_X][SCR_Y];
	SDL_Surface *scr;
} graphics_t;



typedef struct
{
	uint16_t opcode;
	uint8_t mem[MEM_SIZE];
	uint8_t V[REGS_COUNT]; /* 15 registres V0..14 et un V15 utilisé pour les flags */
	uint8_t timer_d; /* timer général */
	uint8_t timer_s;  /* son */
	
	uint16_t pc, sp; /* program counter / stack pointer */
	uint16_t stack[16]; /* pile de sauvegarde d'adresses */
	uint16_t I; /* Index */
	
	graphics_t g;
	uint8_t keys[16];
} c8_t;

void c8_run(c8_t *);
void c8_init(c8_t *);
void c8_load_program(c8_t *, uint16_t *program, uint16_t size);
void c8_load_file(c8_t *, const char *);
void c8_ginit(c8_t *);
void c8_gfx(c8_t *);
void c8_draw(c8_t *);
void c8_clrscr(c8_t *);
void c8_keys(c8_t *, SDL_Event);
uint8_t KEY_MAP(uint32_t);

void _clear();

#endif
