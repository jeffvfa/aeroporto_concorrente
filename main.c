/*
* @author: @jeffvfa github.com/jeffvfa
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>  
#include <semaphore.h> 

#define NUMAVI 50 
#define NUMPORT 5

//pista de pouso 
sem_t semPista;  

//lock dos portões 
pthread_mutex_t lockPortoes = PTHREAD_MUTEX_INITIALIZER;

//thread avião 
pthread_t avioes[NUMAVI], torre_de_comando; 

/* 
*informação se o avião está voando
*   se voando igual a 1, o avião está voando, 
*   se igual a 0 está pousado;
*/
int se_voando[NUMAVI], portoes[NUMPORT], portaoIndicado;


//declarações de funções 
void* aviao(void*);
void* torreDeComando(); 
int portaoLivre(); 


//implementação de funções

//torre de comando 
void* torreDeComando(){ 
    int portaoL;
    while(1){
        //verifica se há portão de embarque livre
        pthread_mutex_lock(&lockPortoes); 
        portaoL = portaoLivre(); 
        if(portaoL != -1){ 
            printf("\tTORRE DE COMANDO: o portão %d está livre, liberando a pista\n",portaoL); 
            portaoIndicado = portaoL;
            sem_post(&semPista);
        }
	    pthread_mutex_unlock(&lockPortoes); 
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
            printf("\n.\n.\n.\n.\nAvião %d pousou!!\n",id); 
            pthread_mutex_lock(&lockPortoes); 
                if(portoes[portaoIndicado] == 0){ 
                portoes[portaoIndicado] = id;
                printf("Avião %d está no portão %d\n",id,portaoIndicado); 
                }
	        pthread_mutex_unlock(&lockPortoes); 
	        
	        sleep(5);
	        
        }
        else{
            //TODO 
            printf("A pista não estava liberada então o avião %d vai circular\n",id); 
            sleep(5);
        }
        //TODO
    }
}

/*
* se há um portão livre retorna o número do portão
* se não há retorna -1
*/
int portaoLivre(){  
    int i;
    for(i=0;i<NUMPORT;i++){ 
        if(portoes[i] == 0) 
            return i;
    } 
    
    return -1;
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