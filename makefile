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
	-mkdir build
	emcc -c src/main.c -o build/app.o -s USE_SDL=2
	emcc build/app.o -o build/index.html -s USE_SDL=2

web: wasm
	sudo cp build/index.html build/index.js build/index.wasm /var/www/html/
compilecmd: clean 
	bear -- make
clean:
	-rm -r out/
	-rm -r build/
