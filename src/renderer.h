#ifndef RENDERER
#define RENDERER 
#include "SDL_rect.h"
#include "keys.c"
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <sys/ucontext.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <stdbool.h>
#include <stdlib.h>
#include "Colours.c"
#include "mutil.h"

#define IN(x,l,h) ((l)<=(x)&&(x)<=(h))
#include<unistd.h>


int w=512;
int h=125;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;

typedef void (*RenderFunction)(Uint32*);

#define MAX_RENDER_FUNCTIONS 10
RenderFunction renderFunctions[MAX_RENDER_FUNCTIONS];

void addRenderFunction(RenderFunction func) {
    // Find the first available slot in the array and add the function pointer
    for (int i = 0; i < MAX_RENDER_FUNCTIONS; ++i) {
        if (!renderFunctions[i]) {
            renderFunctions[i] = func;
            break;
        }
    }
}
SDL_Surface* imgSurfScaled;
void renderInit(CON I W, CON I H){
   w=W;h=H;
   SDL_Init(SDL_INIT_VIDEO);
   SDL_CreateWindowAndRenderer(w, h, 0, &window, &renderer);
   

   surface = SDL_CreateRGBSurface(0, w , h, 32, 0, 0, 0, 0);
}
V renderFree(){
}
void render(){

   
   if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
   Uint32 * pixels = (Uint32 *)surface->pixels;
   //Uint32 * surfacePixels = (Uint32 *)surface->pixels;


   
   for (int i = 0; i < MAX_RENDER_FUNCTIONS; ++i) {
      if (renderFunctions[i]) {
         renderFunctions[i](pixels);
      }
   }
//   for(int yy = 0; yy < h; yy++) {
//      for(int xx = 0; xx < w; xx++) {
//         pixels[(xx + yy * w) * 4 + 0] =  sand[getCellN(xx,yy)].colour & 0xFF;         // Set blue component
//         pixels[(xx + yy * w) * 4 + 1] = (sand[getCellN(xx,yy)].colour >> 8) & 0xFF;  // Set green component
//         pixels[(xx + yy * w) * 4 + 2] = (sand[getCellN(xx,yy)].colour >> 16) & 0xFF; // Set red component
//         pixels[(xx + yy * w) * 4 + 3] = (sand[getCellN(xx,yy)].colour >> 24) & 0xFF; // Set alpha component
//      }
//   }
   if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
   
   SDL_Texture *screenTexture = SDL_CreateTextureFromSurface(renderer, surface);

   SDL_RenderClear(renderer);
   SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
   SDL_RenderPresent(renderer);
   SDL_DestroyTexture(screenTexture);
}

#endif

