#include<stdio.h>
#include <time.h>
int main(){
	time_t tempoCorrente;
	struct tm * tempoAtual,*tempoInicio ;
	time(&tempoCorrente);
	tempoInicio = localtime(&tempoCorrente);
	while(1){
	  	time(&tempoCorrente);
		tempoAtual = localtime(&tempoCorrente);
		printf("\n\tPID = %d\tDeltatime = %d:%d\t tempoInicio = %d:%d\n", getpid(),tempoAtual->tm_hour,tempoAtual->tm_min, tempoInicio->tm_hour,tempoInicio->tm_min);
		sleep(2);	
	}	
	return 0;
}
