CFLAGS  += $(shell cat compile_flags.txt) -O3
LDFLAGS += -Wl,-rpath,@executable_path/../Frameworks

APP = GameOfLife.app
BIN = $(APP)/Contents/MacOS/GameOfLife
FW  = $(APP)/Contents/Frameworks

gol: main.c
	gcc -o gol main.c $(CFLAGS) $(LDFLAGS)

app: gol
	mkdir -p $(APP)/Contents/{MacOS,Frameworks}
	cp gol $(BIN)
	cp /opt/homebrew/opt/raylib/lib/libraylib.550.dylib $(FW)
	codesign --force --deep --sign - $(APP)
