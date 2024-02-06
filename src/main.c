#include <SDL.h>
#include <sys/ucontext.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <stdbool.h>
#include <stdlib.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;
#define w 512
#define h 512
int running=1;
int t = 0;
bool KEYS[322];
int sand[h][w];
void quit(){
         SDL_Quit();
         printf("quiting\n");
         running=0;
}
enum colours {
   NONE=0,
   RED=0xFFFF0000,
   BLUE=0xFF0000FF,
   GREEN=0xFF00FF00,
};
void events(){
   SDL_Event e;
   while (SDL_PollEvent(&e)) {
      if (e.type==SDL_KEYDOWN){
         KEYS[e.key.keysym.sym] = true;
      }else if (e.type == SDL_KEYUP){
         KEYS[e.key.keysym.sym] = false;
      }else if (e.type == SDL_QUIT){
         quit();
      }
   }
   if(KEYS[SDLK_q]) {
      quit();  
   }
}
void tick(){
   for(int yy = h-1; yy >= 0; yy--) {
      for(int xx = 0; xx < w; xx++) {
         if (sand[yy][xx]!=NONE){
            if(sand[yy+1][xx]==NONE){
               sand[yy+1][xx]=sand[yy][xx];
               sand[yy][xx]=NONE;
            }else if(sand[yy+1][xx+1]==NONE){
               sand[yy+1][xx+1]=sand[yy][xx];
               sand[yy][xx]=NONE;
            }else if(sand[yy+1][xx-1]==NONE){
               sand[yy+1][xx-1]=sand[yy][xx];
               sand[yy][xx]=NONE;
            } 

         }
      }
   }
}
void render(){
   if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
   Uint8 * pixels = surface->pixels;

   for(int yy = 0; yy < h; yy++) {
      for(int xx = 0; xx < w; xx++) {
         pixels[(xx + yy * w) * 4 + 0] = sand[yy][xx] & 0xFF;         // Set blue component
         pixels[(xx + yy * w) * 4 + 1] = (sand[yy][xx] >> 8) & 0xFF;  // Set green component
         pixels[(xx + yy * w) * 4 + 2] = (sand[yy][xx] >> 16) & 0xFF; // Set red component
         pixels[(xx + yy * w) * 4 + 3] = (sand[yy][xx] >> 24) & 0xFF; // Set alpha component
      }
   }
   if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
   
   SDL_Texture *screenTexture = SDL_CreateTextureFromSurface(renderer, surface);

   SDL_RenderClear(renderer);
   SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
   SDL_RenderPresent(renderer);
   SDL_DestroyTexture(screenTexture);
}
void mainLoop(){
   events();
   tick();
   render();
   t++;
}
int main(int argc, char* argv[]) {
   SDL_Init(SDL_INIT_VIDEO);
   SDL_CreateWindowAndRenderer(w, h, 0, &window, &renderer);
   surface = SDL_CreateRGBSurface(0, w , h, 32, 0, 0, 0, 0);

   
   for(int yy = 0; yy < h; yy++) {
      for(int xx = 0; xx < w; xx++) {
         int i = rand()%10;
         sand[yy][xx]=NONE;
         switch (i) {
            case 1: 
               sand[yy][xx]=RED;break;
            case 2: 
               sand[yy][xx]=BLUE;break;
            case 3: 
               sand[yy][xx]=GREEN;break;
         } 
            
      }
   }
   for(int i = 0; i < 322; i++) { // init them all to false
      KEYS[i] = false;
   }
#ifdef __EMSCRIPTEN__
   emscripten_set_main_loop(mainLoop, 0, 1);
#else
   while(running) {        
      mainLoop();
      SDL_Delay(16);
   }
#endif 
}
