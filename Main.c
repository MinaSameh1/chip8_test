#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<time.h>
#include "chip8.h" 
#include "platform.h"

int main(int argc, char* argv[]){
	if( argc != 4 ) {
		fprintf(stderr, "Usage: %s <Scale> <Delay> <ROM>\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	int videoScale;
	int cycleDelay;
	sscanf( argv[1], "%d", &videoScale);
	sscanf( argv[2], "%d", &cycleDelay);
	char* romFilename = argv[3];

	Start();
	Chip8_start("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);
	LoadROM(romFilename);

	int videoPitch = sizeof( video[0] ) * VIDEO_WIDTH;

	auto start = clock();
	int quit = 0;

	while( !quit ) {
		quit = ProcessInput(keypad);

		auto currentTime = clock();

		auto dt = currentTime - start;
		if( dt > cycleDelay ) {
			start = currentTime;
			Cycle();
			Update((void const*)video, videoPitch);
		}

	}
	Exit();
	return 0;
}
