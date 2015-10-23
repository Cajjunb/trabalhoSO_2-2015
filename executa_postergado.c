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
	if(argc != 5){
		printf("\n\tNumero de argumento invalido");
		exit(1);
	}else if(atoi(argv[1])<0){
		printf("\n\t Argumento invalido");
		exit(1);
	}else if(atoi(argv[2])<0){
		printf("\n\t Argumento invalido");
		exit(1);
	}else if(atoi(argv[3])<0){
		printf("\n\t Argumento invalido");
		exit(1);
	}else if(atoi(argv[4])<0){
		printf("\n\t Argumento invalido");
		exit(1);
	}
	msg1.mtype = 1;
	strcpy(msg1.msg,argv[1]);	
	msg1.hora = atoi(argv[2]);
	msg1.min = atoi(argv[3]);
	msg1.vezes = atoi(argv[4]);
	printf("\n\tMSG = %s",msg1.msg);
	printf("\n\thoras = %d",msg1.hora);
	printf("\n\tmin = %d",msg1.min);
	printf("\n\tvezs = %d\n",msg1.vezes);
	key_msg = msgget(10,IPC_CREAT|0x1FF);	
	msgsnd(key_msg,&msg1,size_msg, tipo);
	return 0;
}
