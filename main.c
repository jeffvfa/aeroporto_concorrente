/*
* @author: @jeffvfa github.com/jeffvfa
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define DEBUG printf("DEBUG");

#define NUMAVI 20
#define NUMPORT 5
#define NUMSOLO 2

#define TEMPO 2

//pista de pouso
sem_t semPista;

//aguarda pouso
sem_t aguarda_pouso;

//equipes de solo
sem_t equipes;

//emergencia 
sem_t semEmergencia;

//locks
pthread_mutex_t lockPortoes = PTHREAD_MUTEX_INITIALIZER, lockProblema = PTHREAD_MUTEX_INITIALIZER;

//thread avião
pthread_t avioes[NUMAVI], torre_de_comando;

/*
*informação se o avião está voando
*   se voando igual a 1, o avião está voando,
*   se igual a 0 está pousado;
*/
int se_voando[NUMAVI], portoes[NUMPORT], portaoIndicado, problemaGlobal = 0;

//condição de emergência
pthread_cond_t emergencia = PTHREAD_COND_INITIALIZER;

//declarações de funções
void* aviao(void*);
void* torreDeComando();
int portaoLivre();

//implementação de funções

//torre de comando
void* torreDeComando(){
    int portaoL, i,j;
    while(1){
        //verifica se há portão de embarque livre
        portaoL = portaoLivre();

        //se há
        if(portaoL != -1){
          pthread_mutex_lock(&lockPortoes);

          printf("\tTORRE DE COMANDO: o portão %d está livre, liberando a pista\n",portaoL);
          portaoIndicado = portaoL;
          //libera a pista
          sem_post(&semPista);

          printf("\tTORRE DE COMANDO: Pista liberada! Avião deve se dirigir ao portão %d\n\taguardando pouso para liberação da pista\n",portaoL);
          pthread_mutex_unlock(&lockPortoes);
        }
        
        pthread_mutex_lock(&lockProblema); 
        if(problemaGlobal != 0){
            sem_post(&semEmergencia);
        }
        else 
            pthread_cond_broadcast(&emergencia);
        pthread_mutex_unlock(&lockProblema); 
        
        sem_wait(&aguarda_pouso);
        
        
	    //verifica quais portões estão ocupados, e quem os ocupa
	    for(i=0;i<NUMPORT;i++){
            if(portoes[i] != -1)
                printf("\tTORRE DE COMANDO: o portão %d está ocupado pelo avião %d\n",i,portoes[i]);
	    }

	}

}

//avião
void* aviao(void* args){
    int id = *((int*) args), j, portaoOcupado, problema;

    while(1){
        //voa
        printf("Avião %d voando\n",id);
        sleep(TEMPO*(rand()%9)); 
        problema = (rand()%9)+1;
        
        /*pthread_mutex_lock(&lockProblema);
        if(!problemaGlobal){ 
            if(problema <= 2){ 
                problemaGlobal = id;
                printf("Avião %d está com problemas e precisa de pousar com urgência!!!! Informando a torrre de controle\n.\n.\n.\n",id); 
                sem_post(&aguarda_pouso);
                sem_wait(&semEmergencia);
            }
            else
                printf("Avião %d quer pousar, informando a torrre de controle\n.\n.\n.\n",id);
        }
        else{
            pthread_cond_wait(&emergencia,&lockProblema);
        }
        pthread_mutex_unlock(&lockProblema);
        */
        if(sem_trywait(&semPista)==0){
          
          pthread_mutex_lock(&lockPortoes);

            if(portoes[portaoIndicado] == -1){
            printf("Avião %d está pousando, irá fazer o desembarque no portão %d\n",id,portaoIndicado);
            sleep(TEMPO*2);
            portoes[portaoIndicado] = id;
            portaoOcupado = portaoIndicado;
          }
	        pthread_mutex_unlock(&lockPortoes);
          printf("Avião %d está no portão %d\n",id,portaoIndicado);  
          
            pthread_mutex_lock(&lockProblema);
            if(problemaGlobal == id) 
                problemaGlobal = 0;
            pthread_mutex_unlock(&lockProblema);
          
        DEBUG
          sem_post(&aguarda_pouso);

	        printf("Passageiros do avião %d estão desembarcando no portão %d\n",id,portaoIndicado);
	        sleep(TEMPO*5);
	        printf("Desembarque completo!Avião %d aguardando equipes de solo para prearar o voo\n",id);

	        sem_wait(&equipes);
	        sleep(TEMPO*2);
	        printf("Equipes já prepararam o avião %d! Aguardando liberação de pista para decolar\n",id);
	        sem_post(&equipes);

	        sem_wait(&semPista);
	        pthread_mutex_lock(&lockPortoes);
	            portoes[portaoOcupado] = -1;
            pthread_mutex_unlock(&lockPortoes);
	        
	        printf("Pista liberada avião %d decolou\n",id);
	        sem_post(&aguarda_pouso);

	        
        }
        //se não consegue fica circulando
        else{
            printf("A pista não estava liberada então o avião %d vai circular\n",id);
            sleep(TEMPO*5);
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
        if(portoes[i] == -1)
            return i;
    }

    return -1;
}

int main(){
    //semente do rand
    srand(time(NULL));
    
    //declarações de variáveis
    int i =0, *id;

    //portões vazios
    for(i=0;i<NUMPORT;i++){
        portoes[i] = -1;
    }
    
    //inicialização da pista
    sem_init(&semEmergencia,0,0);

    //inicialização da pista
    sem_init(&semPista,0,0);

    //ativar torre
    sem_init(&aguarda_pouso,0,0);

    //inicialização das equipes de solo
    sem_init(&equipes,0,NUMSOLO);

    //thread torre de comando
    pthread_create(&(torre_de_comando), NULL, torreDeComando,NULL);

    //threads de aviões
    for(i=0;i<NUMAVI;i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        se_voando[i] = 0;
        pthread_create(&(avioes[i]), NULL, aviao,id);
    }

    pthread_join(avioes[1],NULL);

    getchar();
    return 0;
}
