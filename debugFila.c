#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/errno.h>

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
	int key_msg;
	t_msg msgrecebida;

	key_msg = msgget(10,0x1FF);	
	while(msgrcv(key_msg,&msgrecebida,size_msg,0,0) > 0){
		printf("\nMSG = %s\n\tHORAS=%d\t MIN = %d\t seg = %d\n",msgrecebida.msg, msgrecebida.hora,msgrecebida.min,msgrecebida.seg);
	}
	printf("\n\t ERRNO = %d",errno);
	return 0;
}
