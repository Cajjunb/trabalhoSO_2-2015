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

void dummy(){}

int main(){
	
	int ppid = fork();
	if(ppid == 0 ){
		//FILHO
		char *listaAbs1;
		shmget(10,sizeof(int)*400,ICP_CREAT);
		signal(SIGALRM,dummy);
		listaAbs1 = shmat(shm_key,0,0x1FF);
		printf("\n\topa RECEBI ISSO DO PAI! %d",);

	}else{
		//PAI
		char *listaAbs2;
		int shm_key = shmget(10,sizeof(int)*400,0x1FF);
		listaAbs2= shmat(shm_key,0,0x1FF);
		listaAbs2 

	}



	return 0 ;
}