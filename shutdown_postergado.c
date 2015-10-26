#include <stdio.h>
#include <stdlib.h>     /* atoi */
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "dados.h"

int main(int argc, char *argv[]){
	int key_msg;
	t_msg msg1;
	TiposMensagem tipo = Agendamento;
	if(argc != 1){
		printf("\n\tNumero de argumento invalido");
		exit(1);
	}
	msg1.mtype = TIPOKILLSERVER;
	printf("\tCANCELAMENTO JOB ID = %ld\n",msg1.jobId);
	key_msg = msgget(10,IPC_CREAT|0x1FF);	
	msgsnd(key_msg,&msg1,size_msg, tipo);
	return 0;
}
