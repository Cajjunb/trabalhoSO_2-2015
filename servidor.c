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
	if(*lista == NULL){
		*lista = novo;
		(*lista)->vezes = registro->vezes;
		(*lista)->hora = registro->hora;
		(*lista)->min = registro->min;
		(*lista)->minstamp = (registro->hora * 60) + registro->min;
		strcpy((*lista)->msg,registro->msg);	
	}else{	

		t_processo *aux = (t_processo*)*lista;
		strcpy(novo->msg,registro->msg);
		novo->vezes = registro->vezes;
		novo->hora = registro->hora;
		novo->min = registro->min;
		novo->minstamp = (registro->hora * 60) + registro->min;
		while(aux->prox != NULL && (aux->minstamp < novo->minstamp )  ){
			printf("\n\t\aux->minstamp %u",aux->minstamp);
			aux = aux->prox;
		}
		novo->prox = aux->prox;
		aux->prox = novo;
	}	
	return;
}

void dummy(int signal){}


int main(){
	t_processo *cabeca = NULL;
	t_processo *aux;
	t_msg msgrecebida;
	int key_msg;
	int tamanhoLista = 0;	
	int pid;
	
	signal(SIGALRM,dummy);
	key_msg = msgget(10,0x1FF);	
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
		getchar();
	
	}	
	int i ;
	printf( "\n\tTamanhoLista\t%d\n" , tamanhoLista);
	while(1){
		aux = cabeca;
		for( i = 0; i < tamanhoLista - 1; i++){
			printf("\n\tProcesso\t%d\t hora %u:%u x %u\t minstamp\t%u\n"
								  ,aux->pid
								  ,aux->hora
								  ,aux->min
								  ,aux->vezes
								  ,aux->minstamp);
			kill(aux->pid,SIGSTOP);
			kill(aux->prox->pid,SIGCONT);		
			aux = aux->prox;
			alarm(3);	
		}
		kill(aux->prox->pid,SIGSTOP);
		kill(cabeca->pid,SIGCONT);		
	}

	return 0 ;
}
