$(shell mkdir -p Build Build/Cache)

Build/Image-Tool: Build/Cache/main.o Build/Cache/libraylib.a
	cc -O2 -o Build/Image-Tool Build/Cache/main.o Build/Cache/libraylib.a -L./Build/Cache -lraylib -lGL -lm -ldl -pthread -g

Build/Cache/libraylib.a: raylib/src
	cmake -S raylib -B raylib/build
	cmake --build raylib/build
	mv -f raylib/build/raylib/libraylib.a Build/Cache/

Build/Cache/main.o: Source/main.c
	cc -O2 -c -o Build/Cache/main.o Source/main.c -I./raylib/include -lraylib -lGL -lm -ldl -pthread -g
