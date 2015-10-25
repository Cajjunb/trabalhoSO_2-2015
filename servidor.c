#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/shm.h>
#include <ctime>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include "dados.h"
#include <sys/wait.h>


void insere_lista( t_processo **lista, t_msg *registro){
	t_processo *novo = (t_processo*)malloc(size_processo);
	t_processo *aux = (t_processo*)*lista;
	std::time_t tempoCorrente;
	struct tm * tempoInfo;
	unsigned int i;
	
	time(&tempoCorrente);
	tempoInfo = localtime(&tempoCorrente); //PEGA O TEMPO ATUAL DE FORMA SEPARADA
	if(aux == NULL ){
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
		while(aux->prox != NULL ){
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
    	for (j = 0; j < aux->pid.size(); ++j)
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

// Funcao que remove a ficha da lista de processos na posicao int posicao
void removerLista(t_processo **lista,int posicao){
	t_processo *aux  *lista;
	int i;
	if(posicao !=1){
		//loop que vai para a posicao necessaria
		for(*aux = *lista, i = 0; i < posicao && aux->prox != NULL ; i++,aux = aux->prox){}
		//remove
		aux-prox = aux-prox->prox;
	}
	else{
		// cabeca da lista eh agora outro
		*lista = aux-prox;
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
	int ppid;					// Parent Process ID
	int i;
	unsigned int j;
	int w;
	unsigned alarm_return = 0;
	int wait_pid_status = -1;
	unsigned int debug_var;
	
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
			debug_var = (unsigned) ((tempoInfo->tm_hour*60) + tempoInfo->tm_min);
			printf("_____________________aux->minstamp=%u, debug_var=%u, vezes = %d\n", aux->minstamp, debug_var, aux->vezes );
			if( aux->minstamp <= (unsigned) ((tempoInfo->tm_hour*60) + tempoInfo->tm_min) && aux->vezes > 0  ){
				printf("\tHORA DE EXECUTAR UM PROCESSO!\n");
				ppid = fork();
				// /*debug*/ printf("\tppid = %d \n",ppid);
				if(ppid == 0){	
					// /*debug*/ printf("end1\n");
					kill(getpid(),SIGSTOP);
					if(execl(aux->msg,aux->msg,EComercial, (char*)0) < 0){
						printf("Erro no execl! \n");
						exit(1);
					}
				}else{
					/*debug*/ printf("end222222222222222222222222222222222222222222222222222\n");
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
		
		// round robin:
		aux = cabeca;
		if(aux != NULL)
		{
			// /*debug*/ printf("aux eh diferente de null!\n");
			for( i = 0; i < tamanhoLista; i++)
			{
				/*debug*/ printf("i=%d, aux->pid.size()=%lu\n", i, aux->pid.size());
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
						/*debug*/ printf("1");
						/*debug*/ printf("@@@@@@@@@@@@@@@@1 mandei sigstop para aux pid[%u]=%d\n", j, aux->pid[j]);
						kill(aux->pid[j],SIGSTOP);

						while(aux->pid[j+1] == 0 && j < aux->pid.size() - 1)
						{
							j++;
						}

						//se ainda tenho pids na lista atual de processos filhos
						if ( j < aux->pid.size() - 1)
						{
							/*debug*/ printf("2");
							/*debug*/ printf("@@@@@@@@@@@@@@@@2 (if) - mandei SIGCONT para aux pid[%u]=%d\n", j+1, aux->pid[j+1]);
							kill(aux->pid[j+1],SIGCONT);
							kill(aux->pid[j+1],SIGCONT);
							kill(aux->pid[j+1],SIGCONT);
							kill(aux->pid[j+1],SIGCONT);
							kill(aux->pid[j+1],SIGCONT);
							kill(aux->pid[j+1],SIGCONT);
							kill(aux->pid[j+1],SIGCONT);
							kill(aux->pid[j+1],SIGCONT);
							kill(aux->pid[j+1],SIGCONT);
							kill(aux->pid[j+1],SIGCONT);
							kill(aux->pid[j+1],SIGCONT);
						}
						else	
						{
							if (aux->prox != NULL && aux->prox->pid[0] != 0)
							{
								/*debug*/ printf("3");
								/*debug*/ printf("@@@@@@@@@@@@@@@@3 (else if)mandei SIGCONT para aux prox pid[0]=%d\n", aux->prox->pid[0]);
								// primeiro do vetor seguinte. o processo que está na proxima ficah do vetor
								kill(aux->prox->pid[0],SIGCONT);
							}
							// se entra no else eh porque chegou ao final, entao voltamos a dar um cont pra cabeca
							else
							{
								/*debug*/ printf("4");
								/*debug*/ printf("@@@@@@@@@@@@@@@@4 (else else)mandei SIGCONT para cabeca pid[0]=%d\n", cabeca->pid[0]);
								// primeiro do vetor seguinte. o processo que está na proxima ficah do vetor
								kill(cabeca->pid[0],SIGCONT);
							}
						}

					}
					printf("#############################ALARME!         begin\n");		
					alarm_return = alarm(10);
					pause();
					waitpid(aux->pid[j], &wait_pid_status, 1);
					printf("alarm return:%u\n", alarm_return);
					printf("->>>>>>>>>>>>>>>%d\n", wait_pid_status);
					printf("#############################ALARME!         end\n");	
				}


				if(aux->prox != NULL)
					aux = aux->prox;
			}	
		}	
	}
	return 0 ;
}
