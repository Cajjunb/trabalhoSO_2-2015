#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <ctime>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include "dados.h"


void insere_lista( t_processo **lista, t_msg *registro){
	t_processo *novo = (t_processo*)malloc(size_processo);
	t_processo *aux = (t_processo*)*lista;
	std::time_t tempoCorrente;
	struct tm * tempoInfo;
	
	time(&tempoCorrente);
	tempoInfo = localtime(&tempoCorrente); //PEGA O TEMPO ATUAL DE FORMA SEPARADA
	if(aux == NULL || aux->minstamp > novo->minstamp ){
		*lista = novo;
		(*lista)->vezes = registro->vezes;
		(*lista)->deltaHora = registro->hora;
		(*lista)->deltaMin = registro->min;
		(*lista)->minstamp = (registro->hora * 60) + registro->min+ (tempoInfo->tm_hour * 60) + tempoInfo->tm_min;
		strcpy((*lista)->msg,registro->msg);
		if(aux != NULL)
			(*lista)->prox = aux;
	}else{	
		strcpy(novo->msg,registro->msg);
		novo->vezes = registro->vezes;
		novo->deltaHora = registro->hora;
		novo->deltaMin = registro->min;
		novo->minstamp = (registro->hora * 60) + registro->min + (tempoInfo->tm_hour * 60) + tempoInfo->tm_min;
		novo->prox = NULL;
		while(aux->prox->prox != NULL && (aux->prox->minstamp < novo->minstamp )  ){
			aux = aux->prox;
		}
		novo->prox = aux->prox;
		aux->prox = novo;
	}	
	return;
}

void dummy(int signal){}

void imprimeLista(t_processo **cabeca){
    t_processo *aux = *cabeca;
    while(aux != NULL){
		printf("\n\tProcesso\t%d\t deltaHora %u:%u x %u\t minstamp\t%u\n"
								  ,aux->pid
								  ,aux->deltaHora
								  ,aux->deltaMin
								  ,aux->vezes
								  ,aux->minstamp);
		aux = aux->prox;	
	}
	getchar();
	return;
}

/* PROGRAMA QUE VERIFICA A FILA DE MSGS, E TRATA A REQUISICAO!*/
int main(){
	t_processo *cabeca = NULL;
	t_processo *aux;
	t_msg msgrecebida;
	std::time_t tempoCorrente;
	struct tm * tempoInfo;
	int key_msg;
	int tamanhoLista;	
	int ppid;					// Parent Process ID
	
	signal(SIGALRM,dummy);				//ROTINA DE SIGNAL_ALRM
	key_msg = msgget(10,0x1FF);			//CRIAR FILA DE MSG
	while(1){
		while(msgrcv(key_msg,&msgrecebida,size_msg,0,IPC_NOWAIT) > 0){			//LOOP DE RECEBER MENSAGENS E INSERIR NA FILA PROCESSSOS
			insere_lista(&cabeca,&msgrecebida);
		}	
		aux = cabeca;
		tamanhoLista = 0;
		while(aux != NULL){
			time(&tempoCorrente);
// 			tempoInfo = localtime(&tempoCorrente); 					//PEGA O TEMPO ATUAL DE FORMA SEPARADA
			if( aux->minstamp == (unsigned) ((tempoInfo->tm_hour*60) + tempoInfo->tm_min) && aux->vezes > 0  ){
				printf("\n\tHORA DE EXECUTAR UM PROCESSO!\n");
				ppid = fork();
				k
				printf("\n\tppid = %d \n",pid);
				if(ppid == 0){	
					if(execl(aux->msg,aux->msg,EComercial, (char*)0) < 0){
						printf("\nErro no execl! \n");
						exit(1);
					}
				}else{
					aux->vezes--;		
					aux->minstamp = ((tempoInfo->tm_hour*60) + tempoInfo->tm_min) + aux->deltaHora*60 + aux->deltaMin  ; //ATUALIZAR A PROX VEZ
					aux->pid = ppid;
					printf("\n\telse aux->pid = %u \n",aux->pid);
				}
			}
			tamanhoLista++;	
			aux = aux->prox;
			printf("\n\twhile(aux != NULL)FINAL\n");
		}
		printf("\n\tSAI DO LOOP DE EXEC\n");
		imprimeLista(&cabeca);
		int i ;
		aux = cabeca;
		if(aux != NULL){
			for( i = 0; i < tamanhoLista - 1; i++){
				printf("\n\tProcesso\t%d\t deltaHora %u:%u x %u\t minstamp\t%u\tprox\t=%p\n"
								,aux->pid
								,aux->deltaHora
								,aux->deltaMin
								,aux->vezes
								,aux->minstamp
								,aux->prox);
				if(aux->vezes > 0 ){
					kill(aux->pid,SIGSTOP);
					kill(aux->prox->pid,SIGCONT);
					alarm(3);
				}		
				if(aux->prox != NULL)
					aux = aux->prox;
			}
			kill(aux->pid,SIGSTOP);
			kill(cabeca->pid,SIGCONT);		
		}	
	}
	return 0 ;
}
