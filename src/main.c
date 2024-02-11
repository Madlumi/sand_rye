#include <SDL.h>
#include <stdio.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <stdlib.h>

#include "sand.c"
#include "renderer.h"
#include "keys.c"
#include "tick.c"

int running=1;
void init(){
   keysInit();
   renderInit(512,512);   
   sandInit();

   loadImg("res/test.png");
}
void quit(){
   SDL_Quit();
   sandFree();
   running=0;
}

#define timesteps 3
void mainLoop(){
   for(int i = 0; i < timesteps; i++){ tick(); }
   render();
   if(QUIT){quit();}
}

int main(int argc, char* argv[]) {
   init();
#ifdef __EMSCRIPTEN__
   emscripten_set_main_loop(mainLoop, 0, 1);
#else
   while(running) {        
      mainLoop();
      SDL_Delay(16);
   }
#endif 
}
