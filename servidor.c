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
	unsigned int i;
	
	time(&tempoCorrente);
	tempoInfo = localtime(&tempoCorrente); //PEGA O TEMPO ATUAL DE FORMA SEPARADA
	if(aux == NULL || aux->minstamp > novo->minstamp ){
		//Insere na cabeca da lista o novo valor
		*lista = novo;
		(*lista)->vezes = registro->vezes;
		(*lista)->deltaHora = registro->hora;
		(*lista)->deltaMin = registro->min;
		(*lista)->minstamp = (registro->hora * 60) + registro->min+ (tempoInfo->tm_hour * 60) + tempoInfo->tm_min;
		strcpy((*lista)->msg,registro->msg);
		(*lista)->estaExecutando = false ;	
		(*lista)->estaExecutando = false;
		for (i = 0; i < registro->vezes; ++i)
		{
			(*lista)->pid.push_back(0);
		}
		//Concantena a ficha seguinte com o resto da fila
		if(aux != NULL)
			(*lista)->prox = aux;
	}else{	
		//Atribui para a nova ficha os valores
		strcpy(novo->msg,registro->msg);
		novo->vezes = registro->vezes;
		novo->deltaHora = registro->hora;
		novo->deltaMin = registro->min;
		novo->minstamp = (registro->hora * 60) + registro->min + (tempoInfo->tm_hour * 60) + tempoInfo->tm_min;
		novo->prox = NULL;
		novo->estaExecutando = false;
		for (i = 0; i < registro->vezes; ++i)
		{
			novo->pid.push_back(0);
		}
		
		// Procura o lugar onde colocar a nova ficha
		while(aux->prox != NULL && (aux->prox->minstamp < novo->minstamp )  ){
			aux = aux->prox;
		}
		// coloca 
 		novo->prox = aux->prox;
		aux->prox = novo;
	}	
	return;
}

void dummy(int signal){}

void imprimeLista(t_processo **cabeca){
    t_processo *aux = *cabeca;
    unsigned int j = 0;
    while(aux != NULL){
    	for (j = 0; j < count; ++j)
    	{
			printf("\tProcesso\t%u\t deltaHora %u:%u x %u\t minstamp\t%u\n"
						  ,aux->pid[j]
						  ,aux->deltaHora
						  ,aux->deltaMin
						  ,aux->vezes
						  ,aux->minstamp);

    	}
		aux = aux->prox;	
	}
	// getchar();
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
	int i;
	unsigned int j;
	int w;
	
	signal(SIGALRM,dummy);				//ROTINA DE SIGNAL_ALRM
	key_msg = msgget(10,0x1FF);			//CRIAR FILA DE MSG
	while(1)
	{
		while(msgrcv(key_msg,&msgrecebida,size_msg,0,IPC_NOWAIT) > 0){			//LOOP DE RECEBER MENSAGENS E INSERIR NA FILA PROCESSSOS
			insere_lista(&cabeca,&msgrecebida);
		}	
		aux = cabeca;
		tamanhoLista = 0;
		while(aux != NULL){
			time(&tempoCorrente);
			tempoInfo = localtime(&tempoCorrente); 					//PEGA O TEMPO ATUAL DE FORMA SEPARADA
			if( aux->minstamp == (unsigned) ((tempoInfo->tm_hour*60) + tempoInfo->tm_min) && aux->vezes > 0  ){
				printf("\tHORA DE EXECUTAR UM PROCESSO!\n");
				ppid = fork();
				// /*debug*/ printf("\tppid = %d \n",ppid);
				if(ppid == 0){	
					// /*debug*/ printf("end1\n");
					if(execl(aux->msg,aux->msg,EComercial, (char*)0) < 0){
						printf("Erro no execl! \n");
						exit(1);
					}
				}else{
					// /*debug*/ printf("end2\n");
					aux->vezes--;		
					aux->minstamp = ((tempoInfo->tm_hour*60) + tempoInfo->tm_min) + aux->deltaHora*60 + aux->deltaMin  ; //ATUALIZAR A PROX VEZ
					
					w=0;
					while(aux->pid[w] != 0)
					{
						w++;
					}
					aux->pid[w] = ppid;
					// /*debug*/ printf("\telse aux->pid = %u \n",aux->pid[w]);
				}
				// /*debug*/ printf("end\n");
			}
			tamanhoLista++;	
			aux = aux->prox;
			// /*debug*/ printf("\twhile(aux != NULL)FINAL\n");
		}
		
		
		
		// /*debug*/ printf("\tSAI DO LOOP DE EXEC\n");
		imprimeLista(&cabeca);
		// /*debug*/ printf("logo apos imprimir lista\n");
		
		aux = cabeca;
		if(aux != NULL)
		{
			printf("aux eh diferente de null!\n");
			for( i = 0; i < tamanhoLista - 1; i++)
			{
				printf("i=%d, aux->pid.size()=%u\n", i, aux->pid.size());
				for (j = 0; j < aux->pid.size(); ++j)
				{
					// printf("\tProcesso\t%d\t deltaHora %u:%u x %u\t minstamp\t%u\tprox\t=%p\n"
					// 				,aux->pid[j]
					// 				,aux->deltaHora
					// 				,aux->deltaMin
					// 				,aux->vezes
					// 				,aux->minstamp
					// 				,aux->prox);
					
					if(aux->pid[j] != 0)
					{
						/*debug*/ printf("1 mandei sigstop para aux pid[%u]=%d\n", j, aux->pid[j]);
						kill(aux->pid[j],SIGSTOP);

						//se ainda tenho pids na lista atual de processos filhos
						if ( j < aux->pid.size() - 1)
						{
							/*debug*/ printf("2 (if) - mandei SIGCONT para aux pid[%u]=%d\n", j+1, aux->pid[j+1]);
							kill(aux->pid[j+1],SIGCONT);
						}
						else	
						{
							/*debug*/ printf("3 (else)mandei SIGCONT para aux prox pid[0]=%d\n", aux->prox->pid[0]);
							// primeiro do vetor seguinte. o processo que está na proxima ficah do vetor
							kill(aux->prox->pid[0],SIGCONT);
						}

					}		
					alarm(3);
				}
				if(aux->prox != NULL)
					aux = aux->prox;
			}	
		}	
	}
	return 0 ;
}
