#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320

typedef struct chip8
{
	 uint8_t 	memory[4096];
	uint16_t 	programCounter;
	uint16_t 	indexRegister;
	uint16_t 	stack[16];
	 uint8_t 	stackPointer;
	 uint8_t 	registers[16];
	 uint8_t 	delayTimer;
	 uint8_t 	soundTimer;
	 uint8_t	gfx[64 * 32];
	 uint8_t 	keypad[16];
} chip8;

const uint8_t fontSet[80] = 
{
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


/*void initChip(chip8 *processor)
{
	memset(processor -> memory, 0, sizeof(processor -> memory));
	memcpy(&processor -> memory[0x050], fontSet, sizeof(fontSet));
	
	processor -> programCounter = 0x200;
	processor -> indexRegister = 0;
	
	memset(processor -> stack, 0, sizeof(processor -> stack));
	processor -> stackPointer = 0;

	memset(processor -> registers, 0, sizeof(processor -> registers));
	
	processor -> delayTimer = 0;
	processor -> soundTimer = 0;

	memset(processor -> gfx, 0, sizeof(processor -> gfx));
	
	memset(processor -> keypad, 0, sizeof(processor -> keypad));
}*/

void initChip(chip8 *processor) {
    memset(processor->memory, 0, sizeof(processor->memory));
    memcpy(&processor->memory[0x050], fontSet, sizeof(fontSet));

    // Debug prints
    for (int i = 0; i < 80; i++) {
        printf("Memory[%d]: 0x%02X\n", 0x050 + i, processor->memory[0x050 + i]);
    }

    processor->programCounter = 0x200;
    processor->indexRegister = 0;

    memset(processor->stack, 0, sizeof(processor->stack));
    processor->stackPointer = 0;

    memset(processor->registers, 0, sizeof(processor->registers));

    processor->delayTimer = 0;
    processor->soundTimer = 0;

    memset(processor->gfx, 0, sizeof(processor->gfx));

    memset(processor->keypad, 0, sizeof(processor->keypad));
}

uint16_t fetchOPCode(chip8 *processor)
{
	uint16_t opCode = 0;

	opCode = processor -> memory[processor -> programCounter];
	opCode <<= 8;	

	processor -> programCounter++;	

	opCode |= processor -> memory[processor -> programCounter];
	processor -> programCounter++;

	printf("Fetched opcode in fetch func: 0x%04X\n", opCode);

	return opCode;
}

void decodeOPCode(chip8 *processor, uint16_t opCode)
{
	uint16_t categoryMask = 0xF000;
	uint8_t alteredRegister = 0;
	uint8_t xCoord = 0;
	uint8_t yCoord = 0;
	uint8_t rowNum = 0;

	printf("Fetched opcode in decode func 0x%04X\n", opCode);

	switch (opCode & categoryMask)
	{
		case 0x0000: //system level
			if (opCode == 0x00E0) 
			{
				printf("apaga tela\n");
				memset(processor -> gfx, 0, sizeof(processor -> gfx));
			}
			break;
		case 0x1000: //jump to address
			printf("pula pra endereco\n");
			processor -> programCounter = opCode & 0x0FFF;
			break;

		case 0x2000: //call subroutine
			break;

		case 0x3000: //skip if equal
			break;

		case 0x4000: //skip if not equal	
			break;

		case 0x5000: //skip if registers equal
			break;

		case 0x6000: //set register
			printf("ajeita registrador\n");
			alteredRegister =  (opCode & 0x0F00) >> 8;
			processor -> registers[alteredRegister] = opCode & 0x00FF; 

			break;

		case 0x7000: //add const to register
			printf("Soma no registrador\n");
			alteredRegister = (opCode & 0x0F00) >> 8;
			processor -> registers[alteredRegister] += opCode & 0x00FF;		

			break;

		case 0x8000: //arithmetic and logic
			break;
		
		case 0x9000: //skip if registers not equal
			break;

		case 0xA000: //set indexRegister
			printf("seta o registrador de indice\n");
			processor -> indexRegister = opCode & 0xFFF;
			break;

		case 0xB000: //jump to address plus register V0
				
			break;
		
		case 0xC000: //random number
			
			break;	

		case 0xD000: //draw sprite
			    printf("Drawing sprite\n");
    uint8_t xCoord = processor->registers[(opCode & 0x0F00) >> 8];  // Vx register
    uint8_t yCoord = processor->registers[(opCode & 0x00F0) >> 4];  // Vy register
    uint8_t rowNum = opCode & 0x000F;

    // Reset VF before drawing
    processor->registers[0xF] = 0;

    for (uint8_t row = 0; row < rowNum; ++row) {
        uint8_t spriteByte = processor->memory[processor->indexRegister + row];

        for (uint8_t bit = 0; bit < 8; ++bit) {
            if (spriteByte & (0x80 >> bit)) {
                uint8_t xPixel = (xCoord + bit) % 64;
                uint8_t yPixel = (yCoord + row) % 32;

                // Handle pixel collision: If the pixel is already on, set it to 0 and set the VF register
                if (processor->gfx[xPixel + (yPixel * 64)] == 1) {
                    processor->gfx[xPixel + (yPixel * 64)] = 0; // Clear the pixel
                    processor->registers[0xF] = 1;  // Set VF to 1 for collision
                } else {
                    processor->gfx[xPixel + (yPixel * 64)] = 1; // Set the pixel
                }
            }
        }
    }

		case 0xE000: //key ops
			break;
		
		case 0xF000: //timer, sound, mem, misc
			break;
	}
}


void emulCycle(chip8 *processor)
{
	uint16_t opCode = fetchOPCode(processor);
	decodeOPCode(processor, opCode);
}

void drawGraphics(SDL_Renderer *renderer, chip8 *processor)
{
    // Definir a cor para limpar a tela (preto)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);  // Limpar a tela

    // Definir a cor para desenhar os pixels (branco)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Desenhar os pixels na tela (a resolução original do CHIP-8 é 64x32)
    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            if (processor->gfx[x + (y * 64)] == 1)  // Se o pixel estiver "ligado"
            {
                SDL_Rect pixel = {x * 10, y * 10, 10, 10};  // Cada pixel será um quadrado de 10x10 pixels
                SDL_RenderFillRect(renderer, &pixel);  // Desenhar o pixel na tela
            }
        }
    }

    // Atualizar a tela para mostrar os gráficos
    SDL_RenderPresent(renderer);
}

/*void loadGame(chip8 *processor, const char *gameRom)
{
	FILE *rom = fopen(gameRom, "rb");
	fread(&processor -> memory[0x200], 1, 4096 - 0x200, rom); 	

	fclose(rom);
}*/
void loadGame(chip8 *processor, const char *gameRom)
{
    FILE *rom = fopen(gameRom, "rb");
    if (rom == NULL) {
        perror("Failed to open ROM file");
        exit(EXIT_FAILURE);
    }

    size_t bytesRead = fread(&processor->memory[0x200], 1, 4096 - 0x200, rom);
    printf("Loaded %zu bytes into memory starting at 0x200.\n", bytesRead);
    
    fclose(rom);
    
    // Debug: Print the contents of the loaded ROM
    for (size_t i = 0x200; i < 0x200 + bytesRead; ++i) {
        printf("new thingy Memory[0x%03lX]: 0x%02X\n", i, processor->memory[i]);
    }
}

int main()
{
	//mingw32-make
	chip8 *processor = malloc(sizeof(chip8));
	initChip(processor);	

	printf("Program Counter initialized to: 0x%04X\n", processor->programCounter);

	loadGame(processor, "IBM.ch8");

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow("8LIV", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

	bool running = true;
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					running = false;
					break;
				default:
					break;			
			}
		}
		emulCycle(processor);
		drawGraphics(renderer, processor);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();	
}