#ifndef DADOS_MOD
#define DADOS_MOD


#define MATRICULA 100033571
#include <vector>
/*CONSTANTES DE TIPO DE MENSAGEM*/
typedef enum {
	Agendamento = 1,
	Remocao,
	Cancelamento
}TiposMensagem;

/*Tipo mensagem, esse tipo de mensagem será usado para conversa entre programas como o agendador, o escalonador
removedor e etc*/
typedef struct{
	long int mtype;  
	char msg[300];
	unsigned int vezes;
	unsigned int hora;
	unsigned int min;
	unsigned int seg;
} t_msg;

typedef struct t_processo{
	long int mtype;  
	char msg[300];
	unsigned int vezes;
	unsigned int deltaHora;
	unsigned int deltaMin;
	unsigned int horaInsercao;
	unsigned int minInsercao;
	unsigned int minstamp;
	std::vector <int> pid;
	bool estaExecutando;
	struct t_processo* prox;
} t_processo;

/*DECLARAÇÕES DE FUNÇÕES DO IPCS*/
void exit(int algo);
int fork();
void sleep (int seg);
/*DECLARAÇÕES DE FUNÇÕES DO IPCS*/

/*DECLARAÇÕES DE CONSTANTES*/
char EComercial[2] = "&";
unsigned int const size_msg = sizeof(t_msg)-sizeof(long);
unsigned int const size_processo = sizeof(t_processo);
#endif
