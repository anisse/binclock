#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>

#define LED_SIZE 30
#define LED_OFF 0
#define LED_ON 1

typedef struct binary_time {
	unsigned char h_d;	/*chiffre des dizaines de l'heure */
	unsigned char h_u;	/*chiffre des unités de l'heure */
	unsigned char m_d;
	unsigned char m_u;
	unsigned char s_d;
	unsigned char s_u;	/*chiffre des unités des secondes */
} bin_time_t;

/*I love global vars ;)*/
SDL_Surface *screen = NULL;

bin_time_t *convert(int hour, int min, int sec)
{
	bin_time_t *ret = NULL;

	if (hour < 0 || hour > 23 || min < 0 || min > 59 || sec < 0 || sec > 59)
		return NULL;

	ret = (bin_time_t *) malloc(1 * sizeof(bin_time_t));
	if (ret) {
		ret->h_u = hour % 10;
		ret->h_d = (hour - ret->h_u) / 10;

		ret->m_u = min % 10;
		ret->m_d = (min - ret->m_u) / 10;

		ret->s_u = sec % 10;
		ret->s_d = (sec - ret->s_u) / 10;
	}
	return ret;
}

int PutLed(int x, int y, char type)
{
	static SDL_Surface *ledon = NULL;
	static SDL_Surface *ledoff = NULL;
	SDL_Rect target = { 0, 0, 0, 0 };

	/*First time we run the func */
	if (!ledon || !ledoff) {
		ledon = SDL_LoadBMP("ledon.bmp");
		ledoff = SDL_LoadBMP("ledoff.bmp");
		ledon = SDL_DisplayFormat(ledon);
		ledoff = SDL_DisplayFormat(ledoff);
		/*White as color key */
		SDL_SetColorKey(ledon, 0,
				SDL_MapRGB(ledon->format, 255, 255, 255));
		SDL_SetColorKey(ledoff, 0,
				SDL_MapRGB(ledoff->format, 255, 255, 255));
	}
	/*Failure */
	if (!ledoff || !ledon || !screen)
		return -1;

	target.x = x * LED_SIZE;
	target.y = y * LED_SIZE;

	switch (type) {
	case LED_ON:
		SDL_BlitSurface(ledon, NULL, screen, &target);
		break;
	case LED_OFF:
		SDL_BlitSurface(ledoff, NULL, screen, &target);
		break;
	default:
		/*Unknown type */
		return -1;
	}
	/*succeed */
	return 1;
}

void PutFigure(int figure, int nbbits, int pos)
{
	int test = 1;
	int i;
	for (i = 1; i < nbbits; i++)
		test *= 2;

	while (nbbits) {
		//                      printf("Fig=%d test=%d\n", figure, test);
		if ((figure - test) >= 0) {

			figure -= test;

			PutLed(pos, 4 - nbbits, LED_ON);
		} else
			PutLed(pos, 4 - nbbits, LED_OFF);

		nbbits--;
		test /= 2;
	}
}

void display(bin_time_t * time)
{
	PutFigure((unsigned int)time->h_d, 2, 0);
	PutFigure((unsigned int)time->h_u, 4, 1);

	PutFigure((unsigned int)time->m_d, 3, 2);
	PutFigure((unsigned int)time->m_u, 4, 3);

	PutFigure((unsigned int)time->s_d, 3, 4);
	PutFigure((unsigned int)time->s_u, 4, 5);
}

void InitScreen(void)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
	atexit(SDL_Quit);

	screen =
	    SDL_SetVideoMode(6 * LED_SIZE, 4 * LED_SIZE, 0,
			     SDL_ANYFORMAT | SDL_SWSURFACE /*|SDL_NOFRAME */  |
			     SDL_DOUBLEBUF);
	SDL_SetAlpha(screen, SDL_SRCALPHA | SDL_RLEACCEL, 128);
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 255));
	SDL_EnableUNICODE(1);
	SDL_WM_SetCaption("Binary Clock", NULL);
}

int main()
{
	time_t t;
	struct tm *hour = NULL;
	SDL_Event event;
	bin_time_t *ptime;

	InitScreen();
	if (!screen)
		/*failure */
		exit(EXIT_FAILURE);

	while (1) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				exit(EXIT_SUCCESS);
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE ||
				    event.key.keysym.unicode == 'q' ||
				    event.key.keysym.unicode == 'Q')
					exit(EXIT_SUCCESS);
				if (event.key.keysym.unicode == 'k') {
					screen =
					    SDL_SetVideoMode(6 * LED_SIZE,
							     4 * LED_SIZE, 0,
							     SDL_ANYFORMAT |
							     SDL_SWSURFACE |
							     SDL_NOFRAME |
							     SDL_DOUBLEBUF);
					//SDL_SetAlpha(screen, SDL_SRCALPHA|SDL_RLEACCEL, 128);
					SDL_FillRect(screen, NULL,
						     SDL_MapRGB(screen->format,
								255, 255, 255));
				}
				if (event.key.keysym.unicode == 'j') {
					screen =
					    SDL_SetVideoMode(6 * LED_SIZE,
							     4 * LED_SIZE, 0,
							     SDL_ANYFORMAT |
							     SDL_SWSURFACE |
							     SDL_DOUBLEBUF);
					//SDL_SetAlpha(screen, SDL_SRCALPHA|SDL_RLEACCEL, 128);
					SDL_FillRect(screen, NULL,
						     SDL_MapRGB(screen->format,
								255, 255, 255));
				}
			default:;
			}
		}
		t = time(NULL);

		hour = localtime((time_t *) & t);
		if (!
		    (ptime =
		     convert(hour->tm_hour, hour->tm_min, hour->tm_sec)))
			exit(EXIT_FAILURE);

		display(ptime);
		SDL_Flip(screen);
//  printf("%d%d  %d%d   %d%d\n\n",(unsigned int)ptime->h_d, (unsigned int)ptime->h_u, (unsigned int)ptime->m_d, (unsigned int)ptime->m_u, (unsigned int)ptime->s_d, (unsigned char)ptime->s_u);
		/*   system("cls");
		   printf("%d  %d   %d\n\n",hour->tm_hour ,hour->tm_min, hour->tm_sec );
		   fflush(stdout); */
		free(ptime);
		SDL_Delay(499);
	}

	return 0;
}
