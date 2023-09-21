// Compilar primeiro o assembly, depois o C
//      nasm -f elf64 ./addScore.asm
//      gcc -o genius ./genius.c ./addScore.o

#include <stdio.h>
#include <time.h>    	// pra usar o srand
#include <unistd.h>  	// sleep no Linux
#include <stdint.h>		// uints types
#include <sys/types.h>	// size_t ,ssize_t, off_t...
#include <unistd.h>		// close() read() write()
#include <fcntl.h>		// open()
#include <sys/ioctl.h>	// ioctl()
#include <errno.h>		// error codes

#include "../../include/ioctl_cmds.h"


#define maxSeq 10    	// Quantidade de níveis  

int ind, sequence[maxSeq], dirty, fd;

// Integração com Assembly
extern void addScore(int*);

int hexToIndex(int* input, unsigned int value){
	int result = -1;
	
	switch(value){
		case 0x7:
			result = 0;
			break;
		case 0xB:
			result = 1;
			break;
		case 0xD:
			result = 2;
			break;
		case 0xE:
			result = 3;
			break;
	}
	
	*input = result;
}

unsigned int digitToHex(int digit){
	unsigned int result = 0;
	switch(digit){
		case 0:
			result = 0x40;
			break;
		case 1:
			result = 0x79;
			break;
		case 2:
			result = 0x24;
			break;
		case 3:
			result = 0x30;
			break;
		case 4:
			result = 0x19;
			break;
		case 5:
			result = 0x12;
			break;
		case 6:
			result = 0x02;
			break;
		case 7:
			result = 0x78;
			break;
		case 8:
			result = 0x00;
			break;
		case 9:
			result = 0x10;
			break;
		default:
			result = 0x40;
	}
	return result;
			
}

unsigned int scoreToHex(int pontos){
	unsigned int result = 0x40400000;
	int aux = pontos % 10;
	result += digitToHex(aux);
	aux = (int)(pontos / 10);
	result += (digitToHex(aux) * 256);
	return result;
}

void endGame(){
	unsigned int data;
	int retval;
	ioctl(fd, WR_RED_LEDS);
	
	for(int i = 0; i < 5; i++){
		data = 0x0007FFFF;
		retval = write(fd, &data, sizeof(data));
		sleep(1);
		data = 0x00000000;
		retval = write(fd, &data, sizeof(data));
		sleep(1);
	}
}

int main(int argc, char** argv){
	unsigned int data = 0x40404040, lightData = 0b00000000, redLight, btnInput = 0x00, lastBtn = 0x00;
	int retval, dirty = 0, count = 0;
    int input, pontos = 0;
   
    if ((fd = open(argv[1], O_RDWR)) < 0) {
		fprintf(stderr, "Error opening file %s\n", argv[1]);
		return -EBUSY;
	}
	
    ioctl(fd, WR_R_DISPLAY);
	retval = write(fd, &data, sizeof(data));
	
    ioctl(fd, WR_L_DISPLAY);
	retval = write(fd, &data, sizeof(data));
	
	redLight = 0x00000000;
	ioctl(fd, WR_RED_LEDS);
	retval = write(fd, &redLight, sizeof(redLight));
    
    lightData = 0b00000000;
    ioctl(fd, WR_GREEN_LEDS);
    retval = write(fd, &lightData, sizeof(lightData));

    srand(time(NULL));
    ind = 0;
    for(int i = 0; i < maxSeq; i++){
        sequence[i] = rand() % 4;
        for(int j = 0; j <= i; j++){
            sleep(1);
        	ioctl(fd, WR_GREEN_LEDS);
        	// printf("%d ", sequence[j]);
        	lightData = 0b00000000 | (0b11000000 >> (2 * sequence[j]));
        	retval = write(fd, &lightData, sizeof(lightData));
            sleep(1);
            lightData = 0b00000000;
            retval = write(fd, &lightData, sizeof(lightData));
        }
        // printf("\n");

        for(int j = 0; j <= i; j++){
			while(dirty){
				
				btnInput = 0x00;
				ioctl(fd, RD_PBUTTONS);
				read(fd, &btnInput, 1);
				dirty = btnInput != 0xF;
			}
			
        	do {
        		btnInput = 0x00;
				ioctl(fd, RD_PBUTTONS);
				read(fd, &btnInput, 1);
				if(btnInput == lastBtn){
				 	++count;
				}
				else{
					lastBtn = btnInput;
					count = 0;
				}
			} while(btnInput == 0xF || count < 20);
      
        	hexToIndex(&input, btnInput);
            if(input != sequence[j]){
            	printf("%X %d\n", btnInput, j);
                printf("Voce perdeu\nPontos: %d\n", pontos);
            	endGame();
                return 0;
            }
            dirty = 1;
            lastBtn = 0x00;
			count = 0;
        }

        addScore(&pontos);

        data = scoreToHex(pontos);
        ioctl(fd, WR_R_DISPLAY);
		retval = write(fd, &data, sizeof(data));
    }
	endGame();
	close(fd);
    // printf("Pontos: %d\n", pontos);    // Trocar essa linha
}
