#ifndef TICK
#define TICK

typedef void (*tickFunc)(void);

#define MAX_TICK_FUNC 10

tickFunc tickFuncs[MAX_TICK_FUNC];

void addTickFunction(tickFunc func) {
    for (int i = 0; i < MAX_TICK_FUNC; ++i) {
        if (!tickFuncs[i]) { tickFuncs[i] = func; break; }
    }
}


void tick(){
   for (int i = 0; i < MAX_TICK_FUNC; ++i) {
      if (tickFuncs[i]) { tickFuncs[i](); }
   }
}

#endif
