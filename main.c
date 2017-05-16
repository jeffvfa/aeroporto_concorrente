/*
* @author: @jeffvfa github.com/jeffvfa
*/

#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>  
#include <semaphore.h> 

#define NUMAVI 100 

//pista de pouso 
sem_t pista; 

//thread avião 
pthread_t avioes[NUMAVI], torre_de_comando; 

/* 
*informação se o avião está voando
*   se voando igual a 1, o avião está voando, 
*   se igual a 0 está pousado;
*/
int se_voando[NUMAVI];


//declarações de funções 
void* aviao();
void* torreDeComando();


//implementação de funções
//avião 
void* aviao(void* args){ 
    int id = *((int*) args); 
    
    while(1){
        printf("\t %d \n",id);
        //TODO
    }
}


int main(){
    //declarações de variáveis
    int i =0, *id;
    
    //inicialização da pista
    sem_init(&pista,0,1);
    
    //thread torre de comando 
    pthread_create(&(torre_de_comando), NULL, torreDeComando,NULL);
    
    //threads de aviões
    for(i=0;i<NUMAVI;i++){ 
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&(avioes[i]), NULL, aviao,id);
    }
    
    printf("Olá");
    
    getchar(); 
    return 0;
} 