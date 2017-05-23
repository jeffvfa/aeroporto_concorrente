/*
* @author: @jeffvfa github.com/jeffvfa
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>  
#include <semaphore.h> 

#define NUMAVI 10 
#define NUMPORT 5

//pista de pouso 
sem_t semPista; 

//thread avião 
pthread_t avioes[NUMAVI], torre_de_comando; 

/* 
*informação se o avião está voando
*   se voando igual a 1, o avião está voando, 
*   se igual a 0 está pousado;
*/
int se_voando[NUMAVI], portoes[NUMPORT];


//declarações de funções 
void* aviao(void*);
void* torreDeComando();


//implementação de funções

//torre de comando 
void* torreDeComando(){ 
    while(1){ 
        //TODO
    }
    
}

//avião 
void* aviao(void* args){ 
    int id = *((int*) args); 
    
    while(1){
        printf("Avião %d voando\n",id); 
        sleep(3);
        
        printf("Avião %d quer pousar, informando a pista de comando\n.\n.\n.\n",id);
        if(sem_trywait(&semPista)==0){
            //TODO 
            getchar();
        }
        else{
            //TODO 
            printf("a pista não estava liberada então o avião %d vai circular\n",id);
        }
        //TODO
    }
}


int main(){
    //declarações de variáveis
    int i =0, *id;
    
    //inicialização da pista
    sem_init(&semPista,0,0);
    
    //thread torre de comando 
    pthread_create(&(torre_de_comando), NULL, torreDeComando,NULL);
    
    //threads de aviões
    for(i=0;i<NUMAVI;i++){ 
        id = (int *) malloc(sizeof(int));
        *id = i; 
        se_voando[i] = 0;
        pthread_create(&(avioes[i]), NULL, aviao,id);
    }
    
    //portões vazios
    for(i=0;i<NUMPORT;i++){ 
        portoes[i] = 0;
    }
    
    //printf("Olá");
    
    getchar(); 
    return 0;
} 