debug:
	-mkdir out
	gcc src/main.c `sdl2-config --cflags --libs ` -lSDL2_image -lm -o sand_ryedb -pg
debrun: debug
	./sand_ryedb

linux:
	-mkdir out
	gcc src/main.c `sdl2-config --cflags --libs ` -lSDL2_image -lm -o sand_rye
run: linux
	./sand_rye
wasm:
	-mkdir out
	-mkdir web_out
	emcc src/main.c  --shell-file src/web/base.html   -s USE_SDL=2 -s USE_SDL_IMAGE=2 -lSDL --preload-file res -s ALLOW_MEMORY_GROWTH=1 -s SDL2_IMAGE_FORMATS='["png"]' -o web_out/sandi.html
local: wasm
	sudo cp -r web_out/*  /var/www/html/
website: wasm
	##super hardcoded deployment pipeline be like
	sudo cp -r web_out/*html ../Website/pages/
	sudo cp -r web_out/*js ../Website/raw/
	sudo cp -r web_out/*wasm ../Website/raw/
	sudo cp -r web_out/*data ../Website/raw/
	
compilecmd: clean 
	bear -- make
clean:
	-rm -r out/
	-rm -r build/
