#include "chip8.h"

// Set the var FontSet
uint8_t fontset[FONTSET_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// Begin! RISE MY CHILD!
void Start(){
	/* Set Program counter to the start Address */
	pc = START_ADDRESS;
	// load font into memory
	unsigned int i;
	for( i = 0; i < FONTSET_START_ADDRESS; i++ ){
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

	table[0x0] = &Table0;
	table[0x1] = &OP_1nnn;
	table[0x2] = &OP_2nnn;
	table[0x3] = &OP_3xkk;
	table[0x4] = &OP_4xkk;
	table[0x5] = &OP_5xy0;
	table[0x6] = &OP_6xkk;
	table[0x7] = &OP_7xkk;
	table[0x8] = &Table8;
	table[0x9] = &OP_9xy0;
	table[0xA] = &OP_Annn;
	table[0xB] = &OP_Bnnn;
	table[0xC] = &OP_Cxkk;
	table[0xD] = &OP_Dxyn;
	table[0xE] = &TableE;
	table[0xF] = &TableF;

	table0[0x0] = &OP_00E0;
	table0[0xE] = &OP_00EE;

	table8[0x0] = &OP_8xy0;
	table8[0x1] = &OP_8xy1;
	table8[0x2] = &OP_8xy2;
	table8[0x3] = &OP_8xy3;
	table8[0x4] = &OP_8xy4;
	table8[0x5] = &OP_8xy5;
	table8[0x6] = &OP_8xy6;
	table8[0x7] = &OP_8xy7;
	table8[0xE] = &OP_8xyE;

	tableE[0x1] = &OP_ExA1;
	tableE[0xE] = &OP_Ex9E;

	tableF[0x07] = &OP_Fx07;
	tableF[0x0A] = &OP_Fx0A;
	tableF[0x15] = &OP_Fx15;
	tableF[0x18] = &OP_Fx18;
	tableF[0x1E] = &OP_Fx1E;
	tableF[0x29] = &OP_Fx29;
	tableF[0x33] = &OP_Fx33;
	tableF[0x55] = &OP_Fx55;
	tableF[0x65] = &OP_Fx65;
}


/* Load into ROM */
void LoadROM(char const* filename){
	FILE *file;
	/* Open the file as stream of binary */
	file = fopen(filename, "rb"); 
	if( file != NULL ){
		long size;
		/* go to the end of file and get its size */
		fseek(file, 0L, SEEK_END);
		size = ftell(file);
		printf("FILE SIZE: %ld\n",size);
		/* go back to the beginning and load the buffer  */
		fseek( file, 0L, SEEK_SET);
		char *buffer[size];
		fread( buffer, size, 1, file );
		/* Load the ROM into the Chip8's Memory, starting at 0x200 */
		long i;
		for( i=0; i < size; i++ ){
			memory[START_ADDRESS + i] = *buffer[i];
		}
	fclose(file);
	} else {
		fprintf( stderr,"ERROR OPENING FILE!");
	}
}

// random num between 0 and 255
uint8_t randByte(){
	srand(time(NULL));
	return rand() % 256;
}

/****************
 * INSTRUCTIONS *
 ****************/

// NOTE : NNN is a hexadecimal mem address, NN hexadecimal byte, N hexdecimal nibble, X and Y are registers 

// Clears the display 
void OP_00E0(){
	memset(video, 0, sizeof(video));
}
// Return
void OP_00EE(){
	--sp;
	pc = stack[sp];
}
//Jumps to location
void OP_1nnn(){
	uint16_t address = opcode & 0x0FFFu;
	pc = address;
}
// Call subroutine at nnn 
void OP_2nnn(){
	uint16_t address = opcode & 0x0FFFu;
	stack[sp] = pc;
	++sp;
	pc = address;
}
// Skip next instruction if VK = kk
void OP_3xkk(){
	uint8_t Vx   = (opcode & 0x0F00u) >> 8u;
	uint8_t byte =  opcode & 0x0FF0u;
	
	if( registers[Vx] == byte )
		pc+=2;
}
// Skip next instruction if VK != kk
void OP_4xkk(){
	uint8_t Vx   = (opcode & 0x0F00u) >> 8u;
	uint8_t byte =  opcode & 0x0FF0u;
	
	if( registers[Vx] != byte )
		pc+=2;
}
// Skip next instruction if Vx = Vy
void OP_5xy0(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if( registers[Vx] == registers[Vy] )
		pc += 2;

}
// set Vx = kk
void OP_6xkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = (opcode & 0x00FFu);

	registers[Vx] = byte;
}
// set Vx = Vx + kk
void OP_7xkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	
	registers[Vx] += byte;
}
// set Vx = Vy
void OP_8xy0(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vy];
}
// Set Vx = Vx or Vy
void OP_8xy1(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] |= registers[Vy];
}
// Set Vx = Vx and Vy
void OP_8xy2(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] &= registers[Vy];
}
// set Vx = Vx XOR Vy 
void OP_8xy3(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] ^= registers[Vy];
}
// set Vx = Vx + Vy, set VF = Carry 
void OP_8xy4(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t sum = registers[Vx] + registers[Vy];
	
	// Set the carry flag on
	if( sum > 255U )
	{
		registers[0xF] = 1;
	}
	else 
	{
		registers[0xF] = 0;
	}

	registers[Vx] = sum & 0xFFu;
}
// set Vx = Vx - Vy, set VF = Not borrow 
void OP_8xy5(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if( registers[Vx] > registers[Vy] )
	{
		registers[0xF] = 1;
	}
	else 
	{
		registers[0xF] = 0;
	}

	registers[Vx] -= registers[Vy];
}
// Set Vx = Vx SHR 1 (Least Significaint Bit)
void OP_8xy6(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// save LSB in VF
	registers[0xF] = registers[Vx] & 0x1u;

	registers[Vx] >>= 1;
}
// set Vx = Vy - Vx, VF = NOT borrorw
void OP_8xy7(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if(registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	} 
	else 
	{
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vy] - registers[Vx];
}
// set VX = Vx SHL 1 (Most significaint bit of Vx)
void OP_8xyE(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	//save MSB in VF
	registers[0xF] = ( registers[Vx] & 0x80u ) >> 7u;

	registers[Vx] <<= 1;
}
// Skip next instruction if Vx != Vy 
void OP_9xy0(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if( registers[Vx] != registers[Vy] )
		pc += 2;
}
// set I = nnn
void OP_Annn(){
	uint16_t address = opcode & 0x0FFFu;

	Index = address;
}
// Jump to location nnn + V0
void OP_Bnnn(){
	uint16_t address = opcode & 0x0FFFu;

	pc = registers[0] + address;
}
// Set Vx = random byte AND kk
void OP_Cxkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = randByte() & byte;
}
//DRW, Display sprite starting at mem location I at (Vx,Vy) set VF = collision.
void OP_Dxyn(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = ( opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Make sure we are not beyond screen boundaries
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;
 
	registers[0xF] = 0;
	unsigned int row;
	for(row = 0; row < height; ++row){
		uint8_t spriteByte = memory[ Index + row ];
		unsigned int col;
		for( col = 0; col < 8; ++col ){
			uint8_t spritePixel = spriteByte & ( 0x80u >> col );
			uint32_t* screenPixel = &video[ (yPos + row) * VIDEO_WIDTH + (xPos + col) ];

			// sprite pixel is on
			if( spritePixel ){
				if( *screenPixel == 0xFFFFFFF )
					registers[0xF] = 1;

				*screenPixel ^= 0xFFFFFFF;
			}
		}
	}
}
// skip next instruction is key pressed 
void OP_Ex9E(){
	uint8_t Vx = ( opcode & 0x0F00u ) >> 8u;
	uint8_t key = registers[Vx];

	if( keypad[key] )
		pc += 2;
}
// skip next instruction if key with value VX not pressed 
void OP_ExA1(){
	uint8_t Vx = ( opcode & 0x0F00u ) >> 8u;
	uint8_t key = registers[Vx];

	if( !keypad[key] )
		pc += 2;
}
// set Vx delay time value
void OP_Fx07(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[Vx] = delayTimer;
}
// wait for keypress store the value of the value of key in Vx
void OP_Fx0A(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if(keypad[0])
		registers[Vx] = 0;
	else if (keypad[1])
		registers[Vx] = 1;
	else if ( keypad[2] ) 
		registers[Vx] = 2;
	else if ( keypad[3] )
		registers[Vx] = 3;
	else if ( keypad[4] ) 
		registers[Vx] = 4;
	else if ( keypad[5] ) 
		registers[Vx] = 5;
	else if ( keypad[6] ) 
		registers[Vx] = 6;
	else if ( keypad[7] ) 
		registers[Vx] = 7;
	else if ( keypad[8] ) 
		registers[Vx] = 8;
	else if ( keypad[9] ) 
		registers[Vx] = 9;
	else if ( keypad[10] ) 
		registers[Vx] = 10;
	else if ( keypad[11] ) 
		registers[Vx] = 11;
	else if ( keypad[12] ) 
		registers[Vx] = 12;
	else if ( keypad[13] ) 
		registers[Vx] = 13;
	else if ( keypad[14] ) 
		registers[Vx] = 14;
	else if ( keypad[15] ) 
		registers[Vx] = 15;
	else 
		pc -= 2;
}
// set delay timer = Vx;
void OP_Fx15(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	delayTimer = registers[Vx];
}
//set sound timer = Vx.
void OP_Fx18(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	soundTimer = registers[Vx];
}
// set I = I + Vx.
void OP_Fx1E(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	Index += registers[Vx];
}
// set I = location of sprite for digit Vx
void OP_Fx29(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = registers[Vx];

	Index = FONTSET_START_ADDRESS + (5 * digit);
}
// store BCD representation of Vx in memory locations I, I+1, I+2
void OP_Fx33(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];

	// ones place 
	memory[Index + 2] = value % 10;
	value /= 10;
	// Tens place 
	memory[Index + 1] = value % 10;
	value /= 10;
	// hundards place
	memory[Index] = value % 10;
}
// store registers V0 through Vx in memory starting at location 1
void OP_Fx55(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t i;
	for(i = 0; i <= Vx; ++i )
		memory[Index + i] = registers[i];

}
// Read Registers V0 through Vx from memory starting at Location I
void OP_Fx65(){
	uint8_t Vx = (opcode & 0x000Fu) >> 8u;
	
	uint8_t i;
	for( i = 0; i < Vx; ++i )
		registers[i] = memory[ Index + i ];
}


// Setting up our Function Pointer Table
voidFunc table [ 0xF + 1 ]  = { &OP_NULL };

voidFunc table0[ 0xE + 1 ]  = { &OP_NULL };

voidFunc table8[ 0xE + 1 ]  = { &OP_NULL };

voidFunc tableE[ 0xE + 1 ]  = { &OP_NULL };

voidFunc tableF[ 0x65 + 1 ] = { &OP_NULL };

void Table0(){
	table0[ opcode & 0x000Fu ]();
}

void Table8(){
	table8[ opcode & 0x000Fu ]();
}

void TableE(){
	tableE[opcode & 0x000Fu]();
}

void TableF(){
	printf("I am in F!op=%d    ", opcode & 0x00FFu);
	//tableF[ opcode & 0x00FFu ]();
}

void OP_NULL() { }

// Fetch, decode, excute 
void Cycle(){
	// Fetch 
	opcode = ( memory[pc] << 8u ) | memory[ pc + 1 ];
	if( opcode != 0 )
		printf("%d\n",(opcode & 0xF000u) >> 12u);

	// increase the program counter
	pc += 2;

	// Decode and excute
	table[ (opcode & 0xF000u) >> 12u ]();

	// decrement delay timer if it has been set  
	if( delayTimer > 0 )
		--delayTimer;
	// Decrement the sound timer if it has been set
	if( soundTimer > 0 )
		--soundTimer;
}

