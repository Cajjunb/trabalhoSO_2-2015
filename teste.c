#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MATRICULA 100033571

typedef struct{
	long int mtype;
	char msg[300];
	unsigned int vezes;
	unsigned int hora;
	unsigned int min;
	unsigned int seg;
} t_msg;

/*DECLARAÇÕES DE FUNÇÕES DO IPCS*/
int fork();

/*DECLARAÇÕES DE CONSTANTES*/

unsigned int const size_msg = sizeof(t_msg)-sizeof(long);


int main(){

int pid;
int key_msg;
t_msg msg1;
	msg1.mtype = 1;
	strcpy(msg1.msg,"OPA");	
	msg1.hora = 1;
	msg1.min = 2;
	msg1.seg = 3;
	msg1.vezes = 10;
	

	printf("\n\tMSG =%s",msg1.msg);

	key_msg = msgget(10,IPC_CREAT | IPC_EXCL |0x1FF);	
	pid = fork();
	if(pid == 0 ){		
		msgsnd(key_msg,&msg1,size_msg, 0);	
		sleep(20);
		printf("\n SOU O PAI");
	}else{
		t_msg msgrecebida;
		msgrcv(key_msg,&msgrecebida,size_msg,1,0);
		printf("\nSOU O FILHO %s\n\tHORAS=%d\t MIN = %d\t seg = %d",msgrecebida.msg, msgrecebida.hora,msgrecebida.min,msgrecebida.seg);
	}
	return 0;
}
