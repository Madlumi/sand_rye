#include "SDL_rect.h"
#include <SDL.h>
#include <stdio.h>
#include <sys/ucontext.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <stdbool.h>
#include <stdlib.h>
#include "Colours.c"

#define IN(x,l,h) ((l)<=(x)&&(x)<=(h))
#include<unistd.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;
#define w 512
#define h 512
int running=1;
int t = 0;
#define keyn 512
#define mkeyn 24
bool KEYS[keyn];
SDL_Point mpos = {0,0};
int MKEYS[mkeyn];
int sand[2][h][w];
int timesteps=5;
int st=0;
int stN=1;
void step(){
   int t =st;
   st = stN;
   stN= t;
}
void render();

void quit(){ SDL_Quit(); printf("quiting\n"); running=0; }

void events(){
   SDL_Event e;
   SDL_GetMouseState(&mpos.x, &mpos.y);
   for(int i = 0; i < mkeyn; i++){if(MKEYS[i]>1){MKEYS[i]--;}}
   while(SDL_PollEvent(&e)) {
      if (e.type==SDL_KEYDOWN){if(!IN(e.key.keysym.sym,0,keyn-1)){printf("key: %d\n",e.key.keysym.sym);return;}KEYS[e.key.keysym.sym] = 1;}
      else if (e.type == SDL_KEYUP){if(!IN(e.key.keysym.sym,0,keyn-1)){return;}KEYS[e.key.keysym.sym] = 0;}
      else if (e.type == SDL_MOUSEBUTTONDOWN){if(!IN(e.button.button,0,mkeyn-1)){printf("key: %d\n",e.button.button);return;}MKEYS[e.button.button]=2;}
      else if (e.type == SDL_MOUSEBUTTONUP){if(!IN(e.button.button,0,mkeyn-1)){printf("key: %d\n",e.button.button);return;}MKEYS[e.button.button]=0;}
      else if (e.type == SDL_QUIT){quit();}
   }
   if(KEYS[SDLK_q]) {
      quit();  
   }
}
   int linedir=-1;
int mcol=0;
void paint(){
   mcol=(mcol+2) % 255;
   int radius = 20;

   int centerX = mpos.x;
   int centerY = mpos.y;

   for (int y = centerY - radius; y <= centerY + radius; y++) {
      for (int x = centerX - radius; x <= centerX + radius; x++) {
         // Calculate the distance from the center of the disk (mpos) to the current point (x, y)
         int distanceSquared = (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY);

         // Check if the current point is within the radius of the disk
         if (distanceSquared <= radius * radius) {
            // Make sure the current point is within the bounds of the sand array
            if (x >= 0 && x < w&& y >= 0 && y < h) {
               if(MKEYS[1]>0){ sand[st][y][x] = hsv_to_int(HsvToRgb(mcol,255,255)); }
               if(MKEYS[3]>0){
                  sand[st][y][x] = 0; // Set the color to RED
               }
            }
         }
      }
   }
}
void sandFall(){
   int falldir = t % 2;
   int x;
   if(falldir==0){falldir=-1;}
   for(int y = h-2; y >= 0; y--){
      for(int xx = 0; xx < w ;xx++){
         if (falldir == -1 ){x=(w-xx);}else{ x=xx;};
         if(sand[st][y][x]!=0){
            if(sand[st][y+1][x]==0){
               sand[st][y+1][x]=sand[st][y][x];
               sand[st][y][x]=0;
            }else {
               if(sand[st][y+1][x+falldir]==0){
                  sand[st][y+1][x+falldir]=sand[st][y][x];
                  sand[st][y][x]=0;
               }else{
                  sand[st][y][x]=sand[st][y][x];
               }
            }
         }else {
            sand[st][y][x]=0;

         }
      }
   }
}
void tick(){
   t+=1;
   if(MKEYS[1] || MKEYS[3]){paint();}
   sandFall();
}
void render(){
   if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
   Uint8 * pixels = surface->pixels;

   for(int yy = 0; yy < h; yy++) {
      for(int xx = 0; xx < w; xx++) {
         pixels[(xx + yy * w) * 4 + 0] = sand[st][yy][xx] & 0xFF;         // Set blue component
         pixels[(xx + yy * w) * 4 + 1] = (sand[st][yy][xx] >> 8) & 0xFF;  // Set green component
         pixels[(xx + yy * w) * 4 + 2] = (sand[st][yy][xx] >> 16) & 0xFF; // Set red component
         pixels[(xx + yy * w) * 4 + 3] = (sand[st][yy][xx] >> 24) & 0xFF; // Set alpha component
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
   for(int i = 0; i < timesteps; i++){
      events();
      tick();
   }
   render();
}
int main(int argc, char* argv[]) {
   SDL_Init(SDL_INIT_VIDEO);
   SDL_CreateWindowAndRenderer(w, h, 0, &window, &renderer);
   surface = SDL_CreateRGBSurface(0, w , h, 32, 0, 0, 0, 0);

   
   for(int yy = 0; yy < h; yy++) {
      for(int xx = 0; xx < w; xx++) {
         int i = rand()%30;
         sand[st][yy][xx]=0;
         sand[stN][yy][xx]=0;
         if(i<.01*yy-1){sand[st][yy][xx] = hsv_to_int(HsvToRgb(rand()%255,255,255)); } 
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
      SDL_Delay(8);
   }
#endif 
}
