#ifndef CHIP8_H
#define CHIP8_H

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<time.h>
#include<string.h>

// Rom start address
#define START_ADDRESS 0x200

// font that will be used in the chip8, load it at address 0x50
#define FONTSET_START_ADDRESS 0x50
#define FONTSET_SIZE 80
// our video defs
#define VIDEO_WIDTH  64
#define VIDEO_HEIGHT 32
// our specs
#define STACK_LEVELS  16
#define REGISTER_COUNT 16
#define MEMORY_SIZE 4096
#define KEY_COUNT 16

uint8_t registers[REGISTER_COUNT];
uint8_t memory[MEMORY_SIZE];
uint16_t Index;
uint16_t pc;
uint16_t stack[STACK_LEVELS];
uint8_t sp;
uint8_t delayTimer;
uint8_t soundTimer;
uint8_t keypad[16];
uint32_t video[ VIDEO_WIDTH * VIDEO_HEIGHT ];
uint16_t opcode;


// the things we want to run first!
void Start();

// Load ROM into Memory
void LoadROM(char const* filename);

// random byte gen between 0 and 255
uint8_t randByte();

// font set
extern uint8_t fontset[FONTSET_SIZE];

/*** Instructions ***/
// Clear the display
void OP_00E0();
// Return
void OP_00EE();
// Jump to location nnn
void OP_1nnn();
// call subroutine at nnn
void OP_2nnn();
// Skip next instruction vk = kk
void OP_3xkk();
// Skip next instruction vk != kk
void OP_4xkk();
// Skip next instruction if Vx = Vy
void OP_5xy0();
// set vk = kk
void OP_6xkk();
// set Vx = Vx + kk
void OP_7xkk();
// set Vx = Vy
void OP_8xy0();
// Set Vx = Vx or Vy
void OP_8xy1();
// Set Vx = Vx and Vy
void OP_8xy2();
// set Vx = Vx XOR Vy 
void OP_8xy3();
// set Vx = Vx + Vy, set VF = Carry 
void OP_8xy4();
// set Vx = Vx - Vy, set VF = Not borrow 
void OP_8xy5();
// Set Vx = Vx SHR 1 (Least Significaint Bit)
void OP_8xy6();
// set Vx = Vy - Vx, VF = NOT borrorw
void OP_8xy7();
// set VX = Vx SHL 1 (Most significaint bit of Vx)
void OP_8xyE();
// Skip next instruction if Vx != Vy 
void OP_9xy0();
// skip next instruction is key pressed 
void OP_Ex9E();
// set I = nnn
void OP_Annn();
// Jump to location nnn + V0
void OP_Bnnn();
// Set Vx = random byte AND kk
void OP_Cxkk();
//DRW, Display sprite starting at mem location I at (Vx,Vy) set VF = collision.
void OP_Dxyn();
// skip next instruction if key with value VX not pressed 
void OP_ExA1();
// set Vx delay time value
void OP_Fx07();
// wait for keypress store the value of the value of key in Vx
void OP_Fx0A();
// set delay timer = Vx;
void OP_Fx15();
//set sound timer = Vx.
void OP_Fx18();
// set I = I + Vx.
void OP_Fx1E();
// set I = location of sprite for digit Vx
void OP_Fx29();
// store BCD representation of Vx in memory locations I, I+1, I+2
void OP_Fx33();
// store registers V0 through Vx in memory starting at location 1
void OP_Fx55();
// Read Registers V0 through Vx from memory starting at Location I
void OP_Fx65();

// our function pointers table

typedef void (*voidFunc)(void);

extern voidFunc table[ 0xF + 1 ];

extern voidFunc table0[ 0xE + 1 ];

extern voidFunc table8[ 0xE + 1 ];

extern voidFunc tableE[ 0xE + 1 ];

extern voidFunc tableF[ 0x65 + 1 ];

void Table0();
void Table8();
void TableE();
void TableF();
void OP_NULL();

// Fetch, decode, excute 
void Cycle();

#endif
