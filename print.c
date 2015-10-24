#include<stdio.h>
#include <time.h>
int main(){
	time_t tempoCorrente;
	time(&tempoCorrente);
	struct tm * tempoInfo;
	while(1){
	  	tempoInfo = localtime(&tempoCorrente);
		printf("\n\tPID = %d\ttime = %d:%d", getpid(),tempoInfo->tm_hour,tempoInfo->tm_min);
		sleep(2);	
	}	
	return 0;
}
