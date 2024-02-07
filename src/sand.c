#include "renderer.h"
#include "keys.c"
#include "tick.c"
#include <sys/types.h>

typedef struct{
   int colour;
   double dx;
   double dy;
}Cell;

Cell sand[w*h];
int getCellN(int x, int y){ return x+y*w; }

int mcol=0;
void sandRender(Uint8 *pixels){
   for(int yy = 0; yy < h; yy++) {
      for(int xx = 0; xx < w; xx++) {
         pixels[(xx + yy * w) * 4 + 0] =  sand[getCellN(xx,yy)].colour & 0xFF;         // Set blue component
         pixels[(xx + yy * w) * 4 + 1] = (sand[getCellN(xx,yy)].colour >> 8) & 0xFF;  // Set green component
         pixels[(xx + yy * w) * 4 + 2] = (sand[getCellN(xx,yy)].colour >> 16) & 0xFF; // Set red component
         pixels[(xx + yy * w) * 4 + 3] = (sand[getCellN(xx,yy)].colour >> 24) & 0xFF; // Set alpha component
      }
   }
}
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
               if(MKEYS[1]>0){ sand[getCellN(x,y)].colour = hsv_to_int(HsvToRgb(mcol,255,255)); }
               if(MKEYS[3]>0){
                  sand[getCellN(x,y)].colour = 0; // Set the color to RED
               }
            }
         }
      }
   }
}
int t = 0;
void sandTick(){

   t++;
   if(MKEYS[1] || MKEYS[3]){paint();}
   int falldir = t % 2;
   int x;
   if(falldir==0){falldir=-1;}
   for(int y = h-2; y >= 0; y--){
      for(int xx = 0; xx < w ;xx++){
         if (falldir == -1 ){x=(w-xx);}else{ x=xx;};
         if(sand[getCellN(x,y)].colour!=0){
            if(sand[getCellN(x,y+1)].colour==0){
               sand[getCellN(x,y+1)]=sand[getCellN(x,y)];
               sand[getCellN(x,y)].colour=0;
            }else {
               if(sand[getCellN(x,y+1)+falldir].colour==0){
                  sand[getCellN(x,y+1)+falldir]=sand[getCellN(x,y)];
                  sand[getCellN(x,y)].colour=0;
               }else{
                  sand[getCellN(x,y)]=sand[getCellN(x,y)];
               }
            }
         }else {
            sand[getCellN(x,y)].colour=0;

         }
      }
   }
}
void sandInit(){
   addRenderFunction(sandRender);
   addTickFunction(sandTick);
   for(int yy = 0; yy < h; yy++) {
      for(int xx = 0; xx < w; xx++) {
         sand[getCellN(xx,yy)].colour=0;
         if(rand()%30<.01*yy-1){sand[getCellN(xx,yy)].colour = hsv_to_int(HsvToRgb(rand()%255,255,255)); } 
      }
   }
}
