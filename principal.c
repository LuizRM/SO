#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

void sigHandler(int);

#ifdef __linux__
#define BIN "/bin/"
#else
#ifdef __APPLE__
#define BIN "/sbin/"
#endif
#endif

#define TAMANHO_MAXIMO_COMANDO 100

unsigned short sinal = 0;

int
main (int argc, char **argv)
{
	/*Strings que irao receber os comandos a serem parseados ou já parseados.
		São necesssarias duas strings diferentes por conta da natureza da funcao
		strtok, que retorna um ponteiro. Isso tornaria os dois arrays de comandos
		iguais apos guardar o segundo argumento, caso se utilizasse o mesmo testador*/
	char testador1 [TAMANHO_MAXIMO_COMANDO];
	char testador2 [TAMANHO_MAXIMO_COMANDO];

	/*Caminho dos comandos
		Eles dependem do Sistema Operacional e são definidos no pré-processamento (linhas 8 a 14 deste programa)*/
	char path1[TAMANHO_MAXIMO_COMANDO] = BIN;
	char path2[TAMANHO_MAXIMO_COMANDO] = BIN;

	/*Variaveis auxiliares*/
	unsigned short comandoOK = 0;
	unsigned short i, spaces;
	const char separador [2] = " ";
	char *token;
	int erro;
	pid_t rc = 0;


/* ----------------------------------------------------- Aquisicao dos comandos --------------------------------------------------------*/
	while (!comandoOK){
		spaces = 0;
		strcpy(path1,BIN);
		printf ("Primeiro comando: ");
		fgets (testador1, TAMANHO_MAXIMO_COMANDO, stdin);
		/*Contagem de espacos em branco para descobrir a quantidade de argumentos.
			Retirado de https://stackoverflow.com/questions/21766778/count-number-of-spaces-and-words-in-an-integer
			Autor: rullof */
		for(i = 0; testador1[i] != '\0'; i++){
			if (testador1[i] == ' '){
				spaces++;
			}
 		}
		spaces++; /*Corrige spaces para representar o numero de argumentos. Ex: em uma string com 4 argumentos, existem 3 espaços, ou seja espacos+1*/

		token = strtok(testador1, separador); 	/*Separa os argumentos da string usando caracter de espaço como separador*/
		if (token[strlen(token)-1] == '\n') 	/*retira o pula linha do final da string se houver */
			token[strlen(token)-1] = '\0';
		strcat(path1,token);					/*Adiciona o caminho da pasta de comandos ao comando*/
		if (access(path1, F_OK) < 0){			/* access() retorna 0 caso o comando exista e -1 caso não.*/
			printf ("Erro! Comando não existe, tente novamente\n");
		} else {
			comandoOK = 1;
			printf ("Comando registrado com sucesso!\n");
		}
	}
	/*Passando o comando para o array de strings correspondente*/
	char *primeiroComando [spaces+2];
	primeiroComando[0] = path1;
	token = strtok (NULL, separador);			/* lê o restante dos argumentos de testador1 */
	for (i = 1; token != NULL;i++){
		if (token[strlen(token)-1] == '\n'){
			token[strlen(token)-1] = '\0';
		}
		primeiroComando[i] = token;
		token = strtok (NULL, separador);
	}
	primeiroComando[spaces] = NULL;				/* Ultima string deve ser nula para se adequar ao execvp()*/

	/*Registro do segundo comando, exatamente a mesma logica usada do bloco acima*/
	comandoOK = 0;
	while (!comandoOK){
		spaces = 0;
		strcpy(path2,BIN);
		printf ("Segundo comando: ");
		fgets (testador2, TAMANHO_MAXIMO_COMANDO, stdin);
		/*Contagem de espacos em branco para descobrir a quantidade de argumentos.
			Retirado de https://stackoverflow.com/questions/21766778/count-number-of-spaces-and-words-in-an-integer
			Autor: rullof */
		for(i = 0; testador2[i] != '\0'; i++){
			if (testador2[i] == ' '){
				spaces++;
			}
		}
		spaces++;

		token = strtok(testador2, separador);
		if (token[strlen(token)-1] == '\n')
			token[strlen(token)-1] = '\0';
		strcat(path2,token);
		if (access(path2, F_OK) < 0){
			printf ("Erro! Comando não existe, tente novamente\n");
		} else {
			comandoOK = 1;
			printf ("Comando registrado com sucesso!\n");
		}
	}
	/*Passando o comando para o array de strings correspondente*/
	char *segundoComando [spaces];
	segundoComando[0] = path2;
	token = strtok (NULL, separador);
	for (i = 1; token != NULL;i++){
		segundoComando[i] = token;
		if (token[strlen(token)-1] == '\n')
			token[strlen(token)-1] = '\0';
		token = strtok (NULL, separador);
	}
	segundoComando[spaces] = NULL;

/*---------------------------------------------------------- Espera e execucao dos sinais -----------------------------------------------*/

	printf ("Disparador incializado com pid=%d\n", getpid());
	signal(SIGUSR1, sigHandler);	/*Associa SIGUSR1 ao sigHandler. Ao receber este sinal, sigHandler() sera chamada*/
	signal(SIGUSR2, sigHandler);
	signal(SIGTERM, sigHandler);
	while(1){
		if (sinal == 1 /*SIGUSR1*/){
			rc = fork();
			if (rc < 0){
				/* Fork falhou*/
				fprintf (stderr, "Fork failed\n");
				exit(1);
			} else if (rc == 0){
				/* Processo filho (ou seja, processo que será substituido pelo comando recebido)*/
				erro = execvp(primeiroComando[0], primeiroComando);
				if (erro){ /* Esta linha não ocorre caso o comando seja executado sem erros */
					printf ("Ocorreu um erro na execucao do comando! Erro #%u\n", erro);
				}
				exit(0);
			} else {
			/* Processo pai. Apenas espera o processo filho ser finalizado */
				wait(NULL);
			}
			sinal = 0;
		}

		if (sinal == 2 /*SIGUSR2*/){
			rc = fork();
			if (rc < 0){
				fprintf (stderr, "Fork failed\n");
				exit(1);
			} else if (rc == 0){
				printf ("Hello, i am child (pid:%d)\n", (int) getpid());
				erro = execvp(segundoComando[0], segundoComando);
				if (erro){
					printf ("Ocorreu um erro na execucao do comando!\n");
				}
				exit(0);
			} else {
				wait(NULL);
			}
			sinal = 0;
		}

		if (sinal == 3 /*SIGTERM*/){
			printf ("Finalizando o disparador...\n");
			return 0;
		}
		sleep(1); /*Evita que o processo pai utilize muito a CPU*/
	}

	return 0;
}

void sigHandler(int sigNum){
	if (sigNum == SIGUSR1){
		signal(SIGUSR1, sigHandler); /*Ao ser recebido e tratado, o sinal se desvincula de sigHandler(), aqui apenas os associa de novo*/
		sinal = 1;
	}

	if (sigNum == SIGUSR2){
		signal(SIGUSR2, sigHandler);
		sinal = 2;
	}

	if (sigNum == SIGTERM){
		signal(SIGTERM, sigHandler);
		sinal = 3;
	}

}
