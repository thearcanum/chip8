#include <c8.h>

void c8_init(c8_t *c8)
{
	int i;
	srand(time(0));
	
	printf("[c8 cpu] Initialisaiton.. \n");
	
	c8->opcode = 0;
	c8->I = 0;
	
	for(i=0;i<0xF;i++) {
		c8->V[i] 	= 0;
		c8->stack[i] = 0;
	}
		
	c8->sp = 0;
	c8->pc = CODE_ADDR;
	
}

void c8_load_program(c8_t *c8, uint16_t *program, uint16_t size)
{
	int i;
	printf("[c8] Chargement du programme..\n");
	
	for(i=0;i<size;i+=2) {
		c8->mem[CODE_ADDR + i] 		= (program[i] >> 8);
		c8->mem[CODE_ADDR + i + 1] 	= program[i] & 0xFF;
	}
}

void c8_run(c8_t *c8)
{
	/* on doit fetch deux octets */
	int i,j;
	uint8_t X,Y,Z;
	uint16_t x,y;
	uint8_t pixel, v, cent, dix, une;
	SDL_Event keypress;
	
	
	c8->opcode = (c8->mem[c8->pc] << 8) | c8->mem[c8->pc + 1];
	
	X = (c8->opcode & 0x0F00) >> 8;
	Y = (c8->opcode & 0x00F0) >> 4;
	Z = (c8->opcode & 0x000F);

	c8->pc += 2;
	
#ifdef DEBUG
	printf("[c8] run : %04x\n", c8->opcode);
	printf("[operand] 0x%02x 0x%02x 0x%02x\n", X,Y,Z);
#endif
	
	switch(c8->opcode & 0xF000) {
	default:
	printf("[c8] Erreur : opcode inconnu (0x%04x)\n", c8->opcode);
	exit(-1);
	break;
	case 0x00:
	switch(c8->opcode & 0xFF) {
		case 0xE0: /* effacer l'écran */
			c8_clrscr(c8);
		break;
		case 0xEE: /* retourner de la sous-routine */
			c8->pc = c8->stack[--c8->sp];
		break;
		default:
		exit(0x42);
		break;
	}
	break;	
	case 0x1000: /* jmp NNN */
		c8->pc = (c8->opcode & 0x0FFF);
		/* printf("jmp $0x%X\n", c8->opcode & 0xFFF); */
	break;
	case 0x2000: /* call NNN */
		/* On doit faire un jump en sauvegardant l'addresse de retour sur la pile */
		/* printf("call $0x%03x\n",	c8->opcode & 0xFFF); */
		
		c8->stack[c8->sp] = c8->pc; /* sauvegarde */
		c8->sp++;

		c8->pc = (c8->opcode & 0x0FFF);
	break;
	
	case 0x3000: 
		if(c8->V[X] == (c8->opcode & 0x00FF)) {
			c8->pc+=2;
		} 
	break;
	
	case 0x4000:
		if(c8->V[X] != (c8->opcode & 0x00FF)) {
			c8->pc += 2;
		} 
	break;
	
	case 0x5000:
		if(c8->V[X] == c8->V[Y])
			c8->pc += 2;
	break;
	
	case 0x6000: /* 6XNN -> mov V[X], NN */
		c8->V[X] = c8->opcode & 0x00FF;
	break;
	
	case 0x7000: 
		c8->V[X] += c8->opcode & 0x00FF;
	break;
	
	case 0x8000: 
		switch(Z) {
		case 0:
		c8->V[X] = c8->V[Y];
		break;
		case 1: 
		c8->V[X] |= c8->V[Y];
		break;
		case 2: 
		c8->V[X] &= c8->V[Y];
		break;
		case 3: 
		c8->V[X] ^= c8->V[Y];
		break;
		case 4: 
		c8->V[X] += c8->V[Y];
		break;
		case 5:
		c8->V[X] -= c8->V[Y];
		break;
		case 6:
		c8->V[X] >>= 1;
		break;
		case 7:
		c8->V[X] = c8->V[Y] - c8->V[X];
		break;
		case 0xE:
		c8->V[X] <<= 1;
		break;
		}
	break;
	
	case 0x9000:
	if(c8->V[X] != c8->V[Y]) 
		c8->pc += 2;
	break;

	case 0xA000: /* ANNN -> mov I, NNN */
	c8->I = c8->opcode & 0xFFF;
	break;
	
	case 0xB000:
		/* jmp v0 + NNN */
		//printf("jmp %03x\n", (c8->opcode & 0xFFF) + c8->V[0]);
		c8->pc = (c8->opcode & 0xFFF) + c8->V[0];
	break;
	
	case 0xC000: /* mov VX, rand ET NN */
	c8->V[X] = rand() & ((Y << 4) | Z);
	break;
	
	case 0xD000: /* dessine sprite 8bit de long N de haut à (VX,VY) */
	c8_gfx(c8);
	break;
	
	case 0xE000:
	switch(c8->opcode & 0xFF) {
		case 0x9E:
		if(c8->keys[c8->V[X]])
			c8->pc += 2;
		break;
		case 0xA1:
		if(!c8->keys[c8->V[X]])
			c8->pc += 2;
		break;
	}
	break;
	
	case 0xF000:
	switch(c8->opcode & 0xFF) {
		case 0x1E:
		c8->I += c8->V[X];
		break;
		case 0x55:
		for(i=0;i<=X;i++)
			c8->mem[c8->I + i] = c8->V[i];
		break;
		case 0x65:
		for(i=0;i<=X;i++)
			c8->V[i] = c8->mem[c8->I + i];
		break;
		case 0x0A:
			c8->pc += 2;
		break;

		case 0x29:
			c8->I = c8->V[X]*5;
		break;
		case 0x33:
			v = c8->V[X];
			for(i=3;i>0;i--) {
				c8->mem[c8->I + i - 1] = v % 10;
				v /= 10;
			}
		break;

		case 0x07:
		c8->V[X] = c8->timer_d;
		break;

		case 0x15:
		c8->timer_d = c8->V[X];
		break;
		case 0x18:
		c8->timer_s = c8->V[X];
		
		break;
	}

	break;
	}
	
	if(c8->timer_d > 0)
		c8->timer_d--;
	if(c8->timer_s > 0)
		c8->timer_s--;
	//if(c8->timer_s == 1) 
		//printf("[Timer Son] BEEP \n");
}

void c8_load_file(c8_t *c8, const char *fname)
{
	FILE * f = fopen(fname, "rb");
	uint16_t *buffer;
	uint32_t size;
	int i;
	
	if(!f) {
		printf("[c8] Impossible d'ouvrir %s ! \n", fname);
		exit(-1);
	}
	
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	printf("[c8] %s : %d octets.\n",fname, size);
	
	buffer = malloc(size * 1);
	
	for(i=0;i<size;i+=2) {
		fread(buffer, 2, 1, f);
		c8->mem[CODE_ADDR + i+1] = (*buffer >> 8);
		c8->mem[CODE_ADDR + i] = (*buffer & 0xFF);
		//printf("%x\n", *buffer);
	}
	
	
	printf("[c8] %s chargé.\n", fname);
}

void c8_keys(c8_t *c8, SDL_Event e)
{
	uint8_t v = (e.type == SDL_KEYDOWN) ? 1 : 0;
	switch(e.key.keysym.sym)
	{
		case SDLK_LEFT:
			c8->keys[1] = v;
			printf("1");
		break;
		case SDLK_2:
			c8->keys[2] = v;
		break;
		case SDLK_3:
			c8->keys[3] = v;
		break;
		case SDLK_4:
			c8->keys[0xC] = v;
		break;
		case SDLK_a:
			c8->keys[4] = v;
		break;
		case SDLK_z:
			c8->keys[5] = v;
		break;
		case SDLK_e:
			c8->keys[6] = v;
		break;
		case SDLK_r:
			c8->keys[0xD] = v;
		break;
		case SDLK_q:
			c8->keys[7] = v;
		break;
		case SDLK_s:
			c8->keys[8] = v;
		break;
		case SDLK_d:
			c8->keys[9] = v;
		break;
		case SDLK_f:
			c8->keys[0xE] = v;
		break;
		case SDLK_w:
			c8->keys[0xA] = v;
		break;
		case SDLK_x:
			c8->keys[0] = v;
		break;
		case SDLK_c:
			c8->keys[0xB] = v;
		break;
		case SDLK_v:
			c8->keys[0xF] = v;
		break;
	}
}

