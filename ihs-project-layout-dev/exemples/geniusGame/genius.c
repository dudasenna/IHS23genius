// Compilar primeiro o assembly, depois o C
//      nasm -f elf addScore.asm
//      gcc -o genius ./genius.c addScore.o

#include <stdio.h>
#include <time.h>    // pra usar o srand
#include <windows.h> // sleep no Windows
#include <unistd.h>  // sleep no Linux

#define maxSeq 5     // Quantidade de níveis  

int ind, sequence[maxSeq];

// Integração com Assembly
// extern void addScore(int*);
void addScore(int* pontos){
    *pontos += 1;
}

int waitForInput(int* input){
    // Mudar esse método pra funcionar na placa
    scanf("%d", input);
}

int main(){
    int input, pontos = 0;
    srand(time(NULL));
    ind = 0;
    for(int i = 0; i < maxSeq; i++){
        sequence[i] = rand() % 4;
        for(int j = 0; j <= i; j++){
            sleep(1);
            printf("%d", sequence[j]); // Acender a luz
            sleep(1);
            system("cls");             // Apagar a luz
        }

        for(int j = 0; j <= i; j++){
            waitForInput(&input);
            system("cls");             // Remover essa linha
            if(input != sequence[j]){
                printf("Voce perdeu\nPontos: %d\n", pontos);
                return 0;
            }
        }

        addScore(&pontos);
    }
    printf("Pontos: %d\n", pontos);    // Trocar essa linha
}