#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <emscripten.h>

#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600

const unsigned int size = 64;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

SDL_Point velocity = {0, 0};
SDL_Rect sprite = {0, 0, size, size};
SDL_Rect background = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
SDL_Texture *texture[10] = {NULL};

bool init() {
     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }

    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
    if (window == NULL | renderer == NULL) {
        return false;
    } 

    return true;
}

/*
void test() {
	
	for (int x = 0; x < surface->w; x++) //Scanline loop backwards
    {
		for (int y = 0; x < surface->h; y++) //Column loop forwards
			{
			putpixel(surface,x,y, SDL_MapRGB(surface->format, 132, 132, 132);
			}
		}

	
	}
*/
//Uint32 SDL_MapRGB(SDL_PixelFormat *fmt, Uint8 r, Uint8 g, Uint8 b);


Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}




void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}


void load_textures() {
    SDL_Surface *player_surface = IMG_Load("assets/player.png");
    SDL_Surface *background_surface = IMG_Load("assets/forest.png");
    if (!player_surface) {
        printf("%s\n", IMG_GetError());
    }
    SDL_SetColorKey(player_surface, SDL_TRUE, SDL_MapRGB(player_surface->format, 255, 255, 255));
    SDL_SetColorKey(background_surface, SDL_TRUE, SDL_MapRGB(background_surface->format, 255, 255, 255));
 
    texture[0] = SDL_CreateTextureFromSurface(renderer, background_surface);
    texture[1] = SDL_CreateTextureFromSurface(renderer, player_surface);
    SDL_FreeSurface(player_surface);
    SDL_FreeSurface(background_surface);

}






void process_event(SDL_Event *event) {
    SDL_Keycode key = event->key.keysym.sym;
    
    if (event->key.type == SDL_KEYDOWN) {
        if (key == SDLK_LEFT || key == SDLK_RIGHT) {
            velocity.x = key == SDLK_LEFT ? -1 : 1;
        }
        if (key == SDLK_UP || key == SDLK_DOWN) {
            velocity.y = key == SDLK_UP ? -1 : 1;
        }
    }
    if (event->key.type == SDL_KEYUP) {
        velocity.x = 0;
        velocity.y = 0;
    }
}

void process_input() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        process_event(&event);
    }
}

void update() {
    sprite.x += velocity.x;
    sprite.y += velocity.y;
}

void draw() {
    SDL_RenderCopy(renderer, texture[0], NULL, &background);
    SDL_RenderCopy(renderer, texture[1], NULL, &sprite);
    
}

void main_loop() {
    process_input();

    SDL_SetRenderDrawColor(renderer, 30, 10, 0, 255);
    SDL_RenderClear(renderer);
    
    update();
    draw();    

    SDL_RenderPresent(renderer);
}

void destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    init();
    load_textures();
    
    background.x = 0;
	background.y = 0;
	
    sprite.x = (WINDOW_WIDTH - size) / 2;
    sprite.y = (WINDOW_HEIGHT - size) / 2;

    emscripten_set_main_loop(main_loop, -1, 1);

    destroy();
    return EXIT_SUCCESS;
}
