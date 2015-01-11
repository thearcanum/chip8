#include <c8.h>

int main()
{
	c8_t c;
	SDL_Event e;
	int r = 1, i;
	char game[48] = {0};
	
	
	printf("[c8] ArCanum8 : émulateur chip8\n");
	printf("Jeu : ");
	scanf("%s", game);
	c8_init(&c);
	c8_load_file(&c, game);
	c8_ginit(&c);
	
	while(r) {
		if(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT)
				r = 0;
				
			if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
				c8_keys(&c, e);	
			}
		}

		c8_run(&c);
		c8_draw(&c);
		
		
		SDL_Flip(c.g.scr);
		SDL_Delay(6);
	}
	
	SDL_Quit();
	return 0;
}
