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
		(*lista)->deltaHora = registro->deltaHora;
		(*lista)->deltaMin = registro->deltaMin;
		(*lista)->minstamp = (registro->deltaHora * 60) + registro->deltaMin;
		strcpy((*lista)->msg,registro->msg);
		if(aux != NULL)
			(*lista)->prox = aux;
	}else{	
		strcpy(novo->msg,registro->msg);
		novo->vezes = registro->vezes;
		novo->deltaHora = registro->deltaHora;
		novo->deltaMin = registro->deltaMin;
		novo->minstamp = (registro->deltaHora * 60) + registro->deltaMin;
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
	int pid;
	
	signal(SIGALRM,dummy);			//ROTINA DE SIGNAL_ALRM
	key_msg = msgget(10,0x1FF);		//CRIAR FILA DE MSG
	while(1){
		while(msgrcv(key_msg,&msgrecebida,size_msg,0,IPC_NOWAIT) > 0){
			insere_lista(&cabeca,&msgrecebida);
		}	
		aux = cabeca;
		tamanhoLista = 0;
		while(aux != NULL){
			time(&tempoCorrente);
			tempoInfo = localtime(&tempoCorrente); //PEGA O TEMPO ATUAL DE FORMA SEPARADA
			if(( aux->deltaHora == (unsigned)tempoInfo->tm_hour && aux->deltaMin == (unsigned)tempoInfo->tm_deltaMin)  ){
				pid = fork();
				if(pid == 0){	
				
					if(execl(aux->msg,aux->msg,EComercial, (char*)0) < 0){
						printf("\nErro no execl! \n");
						exit(1);
					}
				}
			}
			else{
				aux->pid = pid;
			}	
			tamanhoLista++;	
			aux = aux->prox;
		
		}	
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
				/*if(aux->vezes > 0 ){
					kill(aux->pid,SIGSTOP);
					kill(aux->prox->pid,SIGCONT);
					alarm(3);
				}*/		
				if(aux->prox != NULL)
					aux = aux->prox;
			}
			//kill(aux->pid,SIGSTOP);
			//kill(cabeca->pid,SIGCONT);		
		}	
	}
	return 0 ;
}
