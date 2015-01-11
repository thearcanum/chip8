#include <c8.h>

uint8_t c8_fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, 
  0x20, 0x60, 0x20, 0x20, 0x70, 
  0xF0, 0x10, 0xF0, 0x80, 0xF0, 
  0xF0, 0x10, 0xF0, 0x10, 0xF0, 
  0x90, 0x90, 0xF0, 0x10, 0x10, 
  0xF0, 0x80, 0xF0, 0x10, 0xF0, 
  0xF0, 0x80, 0xF0, 0x90, 0xF0,
  0xF0, 0x10, 0x20, 0x40, 0x40, 
  0xF0, 0x90, 0xF0, 0x90, 0xF0, 
  0xF0, 0x90, 0xF0, 0x10, 0xF0, 
  0xF0, 0x90, 0xF0, 0x90, 0x90, 
  0xE0, 0x90, 0xE0, 0x90, 0xE0, 
  0xF0, 0x80, 0x80, 0x80, 0xF0, 
  0xE0, 0x90, 0x90, 0x90, 0xE0, 
  0xF0, 0x80, 0xF0, 0x80, 0xF0, 
  0xF0, 0x80, 0xF0, 0x80, 0x80  
};


void c8_ginit(c8_t *c8)
{
	int i, j;
	
	
	for(i=0;i<SCR_X;i++) {
		for(j=0;j<SCR_Y;j++) {
			c8->g.map[i][j] = 0;
		}
	}

	for(i=0;i<80;i++)
		c8->mem[i] = c8_fontset[i];
	
	SDL_Init(SDL_INIT_VIDEO);
	c8->g.scr = SDL_SetVideoMode(SCR_X*PIXEL_SIZE,SCR_Y*PIXEL_SIZE, 32, SDL_HWSURFACE);
	SDL_WM_SetCaption("arCanum8 émulateur", NULL);

}

void c8_clrscr(c8_t *c8)
{	
	int i,j;

	for(i=0;i<SCR_X;i++)
		for(j=0;j<SCR_Y;j++)
			c8->g.map[i][j] = 0x0;
}

void c8_gfx(c8_t *c8) 
{ 
	int i, j;
	uint16_t x = c8->V[(c8->opcode & 0x0F00) >> 8];
 	uint16_t y = c8->V[(c8->opcode & 0x00F0) >> 4];
 	uint16_t h = c8->opcode & 0x000F;
  	uint16_t pixel;
 
  	c8->V[0xF] = 0;
  	for (i = 0; i < h; i++)
  	{
    	pixel = c8->mem[c8->I + i];
    	for(j = 0; j < 8; j++)
    	{
		  	if((pixel & (0x80 >> j)) != 0)
		  	{
		    	if(c8->g.map[x + j][y + i] == 1)
		      		c8->V[0xF] = 1;                                 
		    	c8->g.map[x + j][y + i] ^= 1;
		  	}
    	}
  	}
}


void c8_draw(c8_t *c8)
{
	int i,j;
	uint8_t c = 0;
	SDL_Rect rect_buffer;
	
	rect_buffer.w = PIXEL_SIZE;
	rect_buffer.h = PIXEL_SIZE;

	//SDL_FillRect(c8->g.scr, 0, SDL_MapRGB(c8->g.scr->format,0, 0, 0));

	for(i=0;i<SCR_X;i++) {
		for(j=0;j<SCR_Y;j++) {
			rect_buffer.x = i*PIXEL_SIZE;
			rect_buffer.y = j*PIXEL_SIZE;
			c = c8->g.map[i][j] ? 0xFF : 0;
			SDL_FillRect(c8->g.scr, &rect_buffer, SDL_MapRGB(c8->g.scr->format, c,c,c));
		}
	}
}
