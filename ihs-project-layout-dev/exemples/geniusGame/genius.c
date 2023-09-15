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

int ind, sequence[maxSeq];

// Integração com Assembly
extern void addScore(int*);

int waitForInput(int* input){
    // Mudar esse método pra funcionar na placa
    scanf("%d", input);
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
			result = 0x07;
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

int main(int argc, char** argv){
	unsigned int data;
	int fd, retval;
    int input, pontos = 0;
    
    if ((fd = open(argv[1], O_RDWR)) < 0) {
		fprintf(stderr, "Error opening file %s\n", argv[1]);
		return -EBUSY;
	}
    
    srand(time(NULL));
    ind = 0;
    for(int i = 0; i < maxSeq; i++){
        sequence[i] = rand() % 4;
        for(int j = 0; j <= i; j++){
            sleep(1);
            printf("%d", sequence[j]); // Acender a luz
            sleep(1);
            // system("cls");          // Apagar a luz
        }

        for(int j = 0; j <= i; j++){
            waitForInput(&input);
            if(input != sequence[j]){
                printf("Voce perdeu\nPontos: %d\n", pontos);
                return 0;
            }
        }

        addScore(&pontos);

        data = scoreToHex(pontos);
        ioctl(fd, WR_L_DISPLAY);
		retval = write(fd, &data, sizeof(data));
    }
    // printf("Pontos: %d\n", pontos);    // Trocar essa linha
}
