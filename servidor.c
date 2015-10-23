#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include "dados.h"


void insere_lista( t_processo **lista, t_msg *registro){
	t_processo *novo = (t_processo*)malloc(size_processo);
	t_processo *aux = (t_processo*)*lista;
	if(aux == NULL || aux->minstamp > novo->minstamp ){
		*lista = novo;
		(*lista)->vezes = registro->vezes;
		(*lista)->hora = registro->hora;
		(*lista)->min = registro->min;
		(*lista)->minstamp = (registro->hora * 60) + registro->min;
		strcpy((*lista)->msg,registro->msg);
		if(aux != NULL)
			(*lista)->prox = aux;
	}else{	
		strcpy(novo->msg,registro->msg);
		novo->vezes = registro->vezes;
		novo->hora = registro->hora;
		novo->min = registro->min;
		novo->minstamp = (registro->hora * 60) + registro->min;
		while(aux->prox->prox != NULL && (aux->prox->minstamp < novo->minstamp )  ){
			printf("\n\t\aux->minstamp %u",aux->minstamp);
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
		printf("\n\tProcesso\t%d\t hora %u:%u x %u\t minstamp\t%u\n"
								  ,aux->pid
								  ,aux->hora
								  ,aux->min
								  ,aux->vezes
								  ,aux->minstamp);
		aux = aux->prox;		
	}
	return;
}


int main(){
	t_processo *cabeca = NULL;
	t_processo *aux;
	t_msg msgrecebida;
	int key_msg;
	int tamanhoLista = 0;	
	int pid;
	
	signal(SIGALRM,dummy);
	key_msg = msgget(10,0x1FF);	
	while(1){
		while(msgrcv(key_msg,&msgrecebida,size_msg,0,IPC_NOWAIT) > 0){
			insere_lista(&cabeca,&msgrecebida);
		}	
		aux = cabeca;
		while(aux != NULL){
			pid = fork();
			if(pid == 0 ){
				if(execl(aux->msg,aux->msg,EComercial, (char*)0) < 0){
					printf("\nErro no execl! \n");
					exit(1);
				}
			}else{
				aux->pid = pid;		
			}		
			tamanhoLista++;	
			aux = aux->prox;
		
		}	
		int i ;
		aux = cabeca;
		if(aux != NULL){
			for( i = 0; i < tamanhoLista - 1; i++){
	/*		printf("\n\tProcesso\t%d\t hora %u:%u x %u\t minstamp\t%u\tprox\t=%p\n"
								,aux->pid
								,aux->hora
								,aux->min
								,aux->vezes
								,aux->minstamp
								,aux->prox);
		*/		if(aux->vezes > 0){
					kill(aux->pid,SIGSTOP);
					kill(aux->prox->pid,SIGCONT);
					alarm(3);		
				}	
				aux = aux->prox;	
			}
			kill(aux->pid,SIGSTOP);
			kill(cabeca->pid,SIGCONT);		
		}	
	}
	return 0 ;
}
