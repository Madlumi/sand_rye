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

void init(){
   keysInit();
   renderInit();   
   sandInit();
}

int running=1;
void quit(){
   SDL_Quit();
   printf("quiting\n");
   running=0;
}

#define timesteps 5
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
      SDL_Delay(8);
   }
#endif 
}
