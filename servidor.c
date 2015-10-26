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


void insereListaProcesso( t_processo **lista, t_msg *registro){
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
		(*lista)->jobId =  0 ;
		(*lista)->vezes = registro->vezes;
		(*lista)->deltaHora = registro->hora;
		(*lista)->deltaMin = registro->min;
		(*lista)->minstamp = (registro->hora * 60) + registro->min+ (tempoInfo->tm_hour * 60) + tempoInfo->tm_min;
		strcpy((*lista)->executavel,registro->executavel);
		(*lista)->prox = NULL;
		for (i = 0; i < registro->vezes; ++i)
		{
			(*lista)->pid.push_back(0);
		}
	}else{	
		//Atribui para a nova ficha os valores
		strcpy(novo->executavel,registro->executavel);
		novo->vezes = registro->vezes;
		novo->deltaHora = registro->hora;
		novo->deltaMin = registro->min;
		novo->minstamp = (registro->hora * 60) + registro->min + (tempoInfo->tm_hour * 60) + tempoInfo->tm_min;
		novo->prox = NULL;
		for (i = 0; i < registro->vezes; ++i)
		{
			novo->pid.push_back(0);
		}
		//Reinicia o contador
		i = 0;
		// Procura o lugar onde colocar a nova ficha
		while(aux->prox != NULL ){
			i ++;
			aux = aux->prox;
		}
		//Coloca o id do job
		novo->jobId = i;
		// coloca 
 		novo->prox = aux->prox;
		aux->prox = novo;
	}	
	return;
}


void insereListaEstatistica( t_estatisticaProcesso **lista, t_processo *registro){
	t_estatisticaProcesso *novo = (t_estatisticaProcesso*)malloc(size_estatistica);
	t_estatisticaProcesso *aux = (t_estatisticaProcesso*)*lista;
	std::time_t tempoCorrente;
	struct tm * tempoInfo;
	unsigned int i;

	//debugprintf("\tINSERINDO ESTATISTISCA###############\n");
	time(&tempoCorrente);
	tempoInfo = localtime(&tempoCorrente); //PEGA O TEMPO ATUAL DE FORMA SEPARADA
	if(aux == NULL ){
		//debugprintf("\tIF ESTAT\n");
		//Insere na cabeca da lista o novo valor
		*lista = novo;
		(*lista)->vezes = registro->pid.size();
		(*lista)->hora = registro->horaInsercao;
		(*lista)->min = registro->minInsercao;
		strcpy((*lista)->executavel,registro->executavel);
		(*lista)->prox = NULL;
		//CONTAR QUANTOS VEZES FORAM EXECUTADAS
		for (i = 0; registro->pid[i] < 0 && i < registro->pid.size(); ++i){}
		(*lista)->nroProcessosExecutados = i ;
		
	}else{	
		//debugprintf("\tELSE ESTAT\n");
		//Atribui para a nova ficha os valores
		strcpy(novo->executavel,registro->executavel);
		novo->vezes = registro->pid.size();
		novo->hora = registro->horaInsercao;
		novo->min = registro->minInsercao;
		novo->prox = NULL;
		//CONTAR QUANTOS VEZES FORAM EXECUTADAS
		for (i = 0; registro->pid[i] < 0 && i < registro->pid.size(); ++i){}
		novo->nroProcessosExecutados = i ;
		// Procura o lugar onde colocar a nova ficha
		while(aux->prox != NULL ){
			aux = aux->prox;
		}
		// coloca 
 		novo->prox = aux->prox;
		aux->prox = novo;
	}	
	for (; registro->pid[i] >= 0 && i < registro->pid.size(); ++i){
		printf("\tSERVER MESSAGE\tPROCESSO jobID\t%lu\texec=%s\tNAO SERA EXECUTADO!CANCELADO\n",registro->jobId,registro->executavel);
	}
	//debugprintf("\tSAINDO ESTAT\n");
	return;
}

void dummy(int signal){}

void imprimeListaProcesso(t_processo **cabeca){
    t_processo *aux = *cabeca;
    unsigned int j = 0;
    while(aux != NULL){
    	//debug printf("no imprime lista\n");
    	for (j = 0; j < aux->pid.size(); ++j)
    	{
			printf("\tProcesso\tjobId=%lu\tpid=%d\t deltaHora %u:%u x %u\t minstamp\t%u\n"
						  ,aux->jobId
						  ,aux->pid[j]
						  ,aux->deltaHora
						  ,aux->deltaMin
						  ,aux->vezes
						  ,aux->minstamp);

    	}
		aux = aux->prox;	
	}
	getchar();
	system("clear");
	return;
}
void imprimeListaEstatistica(t_estatisticaProcesso **cabeca){
    t_estatisticaProcesso *aux = *cabeca;
    printf("\tSERVER MESSAGE: IMPRIMINDO ESTATISTICA\n");
    while(aux != NULL){
    	//debug printf("no imprime lista\n");
		printf("\tProcesso\tjobId=%lu\tInterrompido?=%d\texec=%s\thoraInicio%ux%u:%u\tminstamp\tVezesExecutada%u\t%u\n"
																	,aux->jobId
																	,aux->interrompido	
																	,aux->executavel
																	,aux->vezes
																	,aux->hora
																	,aux->min
																	,aux->minstamp
																	,aux->nroProcessosExecutados);
		aux = aux->prox;	
	}
	getchar();
	system("clear");
	return;
}


// Funcao que remove a ficha da lista de processos na posicao int posicao
int removerListaProcesso(t_processo **listaProcesso,t_estatisticaProcesso **listaEstatistica, int numero_da_lista, int unsigned posicao_no_vetor){
	t_processo *aux = *listaProcesso;
	int i;
	int retorno = 0;
	//debug printf("no removerlista\n");

	if(numero_da_lista != 0){
		//debug printf("no if do remover\n");
		//loop que vai para a posicao necessaria
		for(aux = *listaProcesso, i = 0; i < (numero_da_lista - 1) && aux->prox != NULL ; i++,aux = aux->prox){
			//debug printf("for remover lista i =%d\n", i);
		}
		
		// se for a ultima posicao, re
		if (posicao_no_vetor == aux->prox->pid.size() - 1 )
		{
			//debug printf("if if\n");
			//remove
			//INSERE NA LISTA DE ESTATISTICA!
			insereListaEstatistica(listaEstatistica,aux->prox);
			aux->prox = aux->prox->prox;
			retorno++;

		}
		else
		{
			//debug  printf("if else\n");
			aux->prox->pid[posicao_no_vetor] = -1;
		}
		
	}
	else{
		/*debug:*/printf("no else do remover\n");

		if (posicao_no_vetor == aux->pid.size() - 1 )
		{
			//debug  printf("else if\n");
			// cabeca da lista eh agora outro
			//INSERE NA LISTA DE ESTATISTICA!
			insereListaEstatistica(listaEstatistica,*listaProcesso);
			*listaProcesso = aux->prox;
			retorno++;

		}
		else
		{
			//debug printf("else else\n");
			aux->pid[posicao_no_vetor] = -1;
		}
	}
	//debug printf("removi!\n");
	return retorno;
}

void removeForcadoProcesso(t_processo **listaProcesso,t_processo *registro){
	t_processo *auxProc= *listaProcesso;
	printf("\tSERVER MESSAGE: REORGANIZANDO LISTA!\n");
	if(*listaProcesso == registro ){
		printf("\tSERVER MESSAGE: IF!\n");

		(*listaProcesso) = (*listaProcesso)->prox;
	}
	else{
		printf("\tSERVER MESSAGE: ELSE!\n");
		while(registro != auxProc->prox && auxProc->prox != NULL){
			auxProc = auxProc->prox;
		}
		if(auxProc!=NULL){
			printf("\tSERVER MESSAGE: IF 2!\n");
			t_processo *freeProc = auxProc->prox;
			auxProc->prox = auxProc->prox->prox;
			free(freeProc);
		}
		else
			printf("\tNAO FOI ENCONTRADO TAL PROCESSO\n");
	}
	printf("\tSERVER MESSAGE: FECHOU A REORGANIZACAO!\n");

	return;

}


//FUNCAO QUE DESTROI E INSERE TODO O RESTO NA LISTA
void destroiListaProcesso(t_processo **listaProcesso,t_estatisticaProcesso **listaEstatistica){
	t_processo *auxProc ;
	while(auxProc != NULL){
			auxProc = *listaProcesso;
			insereListaEstatistica(listaEstatistica,auxProc);
			removeForcadoProcesso(listaProcesso,auxProc);
			printf("\tSERVER MESSAGE: \tcabecada LISTA%p!\n",auxProc);
		}
	return;
}


//FUNCAO QUE DESTROI E INSERE PELO ID JOB !
void destroiProcessoJobId(t_processo **listaProcesso,t_estatisticaProcesso **listaEstatistica,  long int jobId){
	t_processo *auxProc = *listaProcesso ;
	while(auxProc->jobId != jobId && auxProc != NULL){
		auxProc = auxProc->prox;
	}
	if(auxProc == NULL){
		printf("\tSYSTEM MESSAGE: ERRO NAO EXISTE PROCESSO COM ESSE jobId!\n");
	}else{
		insereListaEstatistica(listaEstatistica,auxProc);
		printf("\tSERVER MESSAGE: COLOCADO NA ESTAT!\n");
		removeForcadoProcesso(listaProcesso,auxProc);
	}
	printf("\tSERVER MESSAGE: REMOVIDO JOB!\n");
	return;
}



void retiraProcessosFinalizados(t_processo **cabeca,t_estatisticaProcesso **listaEstatistica,unsigned tamanhoLista){
	t_processo *aux ;
	unsigned i;
	unsigned j;  
	int wait_pid_status = -1;
	bool flagRemover = true;
	//debugprintf("V#################VERIFICA SE REMOVE!\n");
	while(flagRemover){
		flagRemover = false;
		for (i = 0,aux = *cabeca; i < tamanhoLista && !flagRemover; ++i,aux = aux->prox)
		{
			for (j = 0; j < aux->pid.size() && !flagRemover; ++j)
			{
				//debugprintf("\t______________________i = %d\tj=%d\n",i,j );
				if (aux->pid[j] > 0){
					waitpid(aux->pid[j], &wait_pid_status, WNOHANG);
				}
				// Se um processo filho i terminou retira da lista
				//debug printf("->>>>>>>>>>>>>>>%d\n", wait_pid_status);
				if(WIFEXITED(wait_pid_status) ){
					//debug printf("chamando a remover lista com i=%d, j=%u, do aux->pid[j] = %d\n", i,j, aux->pid[j] );
					tamanhoLista -=removerListaProcesso(cabeca,listaEstatistica, i, j);	
					flagRemover = true;
					wait_pid_status = -1;

				}	
			}
		}
		//debugprintf("\tFINAL DOS FORS FLAG = %d\ttamanhoLista\t%d\ti=%d\tj=%d\n",flagRemover,tamanhoLista,i,j);
	}
	//debugprintf("\tACABOU A VERIFICACAO\n");
	return ;
}




/* PROGRAMA QUE VERIFICA A FILA DE MSGS, E TRATA A REQUISICAO!*/
int main(){
	t_processo *cabecaProcesso = NULL;
	t_processo *aux;
	t_estatisticaProcesso *cabecaEstatistica = NULL;
	t_msg msgrecebida;
	std::time_t tempoCorrente;
	struct tm * tempoInfo;
	int key_msg;
	int tamanhoLista;	
	int ppid;					// Parent Process ID
	int i;
	int w;
	unsigned int j;
	//debug unsigned alarm_return = 0;
	//debug unsigned int debug_var;
	bool flagPrimeiroRodou = false;
	
	signal(SIGALRM,dummy);				//ROTINA DE SIGNAL_ALRM
	key_msg = msgget(10,0x1FF);			//CRIAR FILA DE MSG
	while(1)
	{
		while(msgrcv(key_msg,&msgrecebida,size_msg,0,IPC_NOWAIT) > 0){			//LOOP DE RECEBER MENSAGENS E INSERIR NA FILA PROCESSSOS
			if(msgrecebida.mtype == TIPOCANCELAMENTO){	
				//FAZ VERIFICACAO SE RETIRA PROCESSOS!
				printf("\tSERVER MESSAGE\tCANCELANDO UM JOB\n");
				destroiProcessoJobId(&cabecaProcesso,&cabecaEstatistica,msgrecebida.jobId);
			}else if(msgrecebida.mtype == TIPOAGENDAMENTO){	
				printf("\tSERVER MESSAGE\tAGENDANDO UM JOB\n");
				insereListaProcesso(&cabecaProcesso,&msgrecebida);
			}
			else if(msgrecebida.mtype == TIPOKILLSERVER){
				printf("\tSERVER MESSAGE\tFECHANDO O SERVER\n");
				destroiListaProcesso(&cabecaProcesso,&cabecaEstatistica);
				imprimeListaEstatistica(&cabecaEstatistica);
				//FINALIZA SERVER
				return 0; 
			}
		}

		aux = cabecaProcesso;
		tamanhoLista = 0;
		while(aux != NULL){
			time(&tempoCorrente);
			tempoInfo = localtime(&tempoCorrente); 					//PEGA O TEMPO ATUAL DE FORMA SEPARADA
			//debug debug_var = (unsigned) ((tempoInfo->tm_hour*60) + tempoInfo->tm_min);
			//debugprintf("_____________________aux->minstamp=%u, debug_var=%u, vezes = %d\n", aux->minstamp, debug_var, aux->vezes );
			if( aux->minstamp <= (unsigned) ((tempoInfo->tm_hour*60) + tempoInfo->tm_min) && aux->vezes > 0  ){
				//debugprintf("\tHORA DE EXECUTAR UM PROCESSO!\n");
				ppid = fork();
				// //debug printf("\tppid = %d \n",ppid);
				if(ppid == 0){	
					// //debug printf("end1\n");
					kill(getpid(),SIGSTOP);
					if(execl(aux->executavel,aux->executavel,EComercial, (char*)0) < 0){
						printf("Erro no execl! \n");
						exit(1);
					}
				}else{
					//debug printf("end222222222222222222222222222222222222222222222222222\n");
					aux->vezes--;		
					aux->minstamp = ((tempoInfo->tm_hour*60) + tempoInfo->tm_min) + aux->deltaHora*60 + aux->deltaMin  ; //ATUALIZAR A PROX VEZ
					
					w=0;
					while(aux->pid[w] != 0)
					{
						w++;
					}
					aux->pid[w] = ppid;
					// //debug printf("\telse aux->pid = %u \n",aux->pid[w]);
				}
				// //debug printf("end\n");
			}
			tamanhoLista++;	
			aux = aux->prox;
			// //debug printf("\twhile(aux != NULL)FINAL\n");
		}
		
		
		
		// //debug printf("\tSAI DO LOOP DE EXEC\n");
		imprimeListaProcesso(&cabecaProcesso);

		// //debug printf("logo apos imprimir lista\n");
		// round robin:
		aux = cabecaProcesso;
		if(aux != NULL)
		{
			flagPrimeiroRodou = false;
			// //debug printf("aux eh diferente de null!\n");
			for( i = 0; i < tamanhoLista; i++)
			{
				//debug printf("i=%d, aux->pid.size()=%lu\n", i, aux->pid.size());
				for (j = 0; j < aux->pid.size(); ++j)
				{

					// printf("\tProcesso\t%d\t deltaHora %u:%u x %u\t minstamp\t%u\tprox\t=%p\n"
					// 				,aux->pid[j]
					// 				,aux->deltaHora
					// 				,aux->deltaMin
					// 				,aux->vezes
					// 				,aux->minstamp
					// 				,aux->prox);
					/*debug*/printf("\tCHECKANDO\ti = %d\tj=%d\n",i,j);
					
					if(aux->pid[j] > 0)
					{
						//Primeiro Processo de todos RODANDO?
						if(!flagPrimeiroRodou){
							//debug printf("0");
							//debug printf("@@@@@@@@@@@@@@@@1 mandei SIGCONT para aux pid[%u]=%d\n", j, aux->pid[j]);
							flagPrimeiroRodou =true;
							kill(aux->pid[j],SIGCONT);
							alarm(QUANTUM);
							pause();
						}
						//debug printf("1");
						//debug printf("@@@@@@@@@@@@@@@@1 mandei sigstop para aux pid[%u]=%d\n", j, aux->pid[j]);
						kill(aux->pid[j],SIGSTOP);

						while(aux->pid[j+1] <= 0 && j < aux->pid.size() - 1)
						{
							//debugprintf("while de pular coisas com j=%d\n", j);
							j++;
						}

						//se ainda tenho pids na lista atual de processos filhos
						if ( j < aux->pid.size() - 1)
						{
							//debug printf("2");
							//debug printf("@@@@@@@@@@@@@@@@2 (if) - mandei SIGCONT para aux pid[%u]=%d\n", j+1, aux->pid[j+1]);
							kill(aux->pid[j+1],SIGCONT);
						}
						else	
						{
							if (aux->prox != NULL && aux->prox->pid[0] > 0)
							{
								//debug printf("3");
								//debug printf("@@@@@@@@@@@@@@@@3 (else if)mandei SIGCONT para aux prox pid[0]=%d\n", aux->prox->pid[0]);
								// primeiro do vetor seguinte. o processo que está na proxima ficah do vetor
								kill(aux->prox->pid[0],SIGCONT);
							}
							// se entra no else eh porque chegou ao final, entao voltamos a dar um cont pra cabeca
							// else
							// {
							// 	//debug printf("4");
							// 	//debug printf("@@@@@@@@@@@@@@@@4 (else else)mandei SIGCONT para cabeca pid[0]=%d\n", cabeca->pid[0]);
							// 	// primeiro do vetor seguinte. o processo que está na proxima ficah do vetor
								
							// }
						}
						//debug printf("#############################ALARME!         begin\n");		
						alarm(QUANTUM);
						pause();
						//debug printf("alarm return:%u\n", alarm_return);
						//debug printf("#############################ALARME!         end\n");	
					}
				}
				if(aux->prox != NULL)
					aux = aux->prox;
			}
			//FAZ VERIFICACAO SE RETIRA PROCESSOS!
			retiraProcessosFinalizados(&cabecaProcesso,&cabecaEstatistica,tamanhoLista);
	
		}	
	}
	return 0 ;
}
