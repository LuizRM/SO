#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#ifdef __linux__
#define BIN "/bin/"
#else
#ifdef __APPLE__
#define BIN "/sbin/"
#endif
#endif

#define TAMANHO_MAXIMO_COMANDO 100

int
main (int argc, char **argv)
{
	/*strings que irao receber os comandos a serem parseados ou já parseados.
		São necesssarias duas strings diferentes por conta da natureza da funcao
		strtok, que retorna um ponteiro. Isso tornaria os dois arrays de comandos
		iguais apos guardar o segundo argumento, caso se utilizasse o mesmo testador*/
	char testador1 [TAMANHO_MAXIMO_COMANDO];
	char testador2 [TAMANHO_MAXIMO_COMANDO];

	/*Caminho dos comandos
		Eles dependem do Sistema Operacional e são definidos no pré-processamento (linhas 8 a 14 deste programa)*/
	char path1[TAMANHO_MAXIMO_COMANDO] = BIN;
	char path2[TAMANHO_MAXIMO_COMANDO] = BIN;
	char *myargs[5];

	/*Variaveis auxiliares*/
	unsigned short comandoOK = 0;
	unsigned short i, spaces;
	const char separador [2] = " ";
	char *token;
	int erro;

	printf ("Disparador incializado com pid=%d\n", getpid());
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
		spaces++; /*Ex: em uma string com 4 argumentos, existem 3 espaços*/

		token = strtok(testador1, separador); /*Separa os argumentos da string*/
		if (token[strlen(token)-1] == '\n') /*retira o pula linha do final da string se houver */
			token[strlen(token)-1] = '\0';
		strcat(path1,token);
		if (access(path1, F_OK) < 0){
			printf ("Erro! Comando não existe, tente novamente\n");
		} else {
			comandoOK = 1;
			printf ("Comando registrado com sucesso!\n");
		}
	}
	/*Passando o comando para o array de strings correspondente*/
	char *primeiroComando [spaces+2];
	primeiroComando[0] = path1;
	token = strtok (NULL, separador);
	for (i = 1; token != NULL;i++){
		if (token[strlen(token)-1] == '\n'){
			token[strlen(token)-1] = '\0';
		}
		primeiroComando[i] = token;
		token = strtok (NULL, separador);
	}
	for (i=0; i < spaces; i++){
		printf ("%s", primeiroComando[i]);
	}
	primeiroComando[spaces] = NULL;

	/*Registro do segundo comando*/
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
		spaces++; /*Ex: em uma string com 4 argumentos, existem 3 espaços*/

		token = strtok(testador2, separador); /*Separa os argumentos da string*/
		if (token[strlen(token)-1] == '\n') /*retira o pula linha do final da string se houver */
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
	for (i=0; i < spaces; i++){
		printf ("%s", primeiroComando[i]);
	}
	printf("\n");
	for (i=0; i < spaces; i++){
		printf ("%s", segundoComando[i]);
	}
	printf("\n");

/*---------------------------------------------------------- Espera e execucao dos sinais -----------------------------------------------*/

	int rc = fork();
	if (rc < 0){
		/* fork failed*/
		fprintf (stderr, "Fork failed\n");
		exit(1);
	} else if (rc == 0){
		/* child */
		printf ("Hello, i am child (pid:%d)\n", (int) getpid());
		/*myargs[0] = "/bin/ping";
		myargs[1] = "8.8.8.8";
		myargs[2] = "-c";
		myargs[3] = "5";
		myargs[4] = NULL;
		printf ("Comando: %s\n", myargs[0]);
		execvp(myargs[0], myargs);*/
		printf ("Comando: %s\n", primeiroComando[0]);
		erro = execvp(primeiroComando[0], primeiroComando);
		if (erro){
			printf ("Ocorreu um erro!\n");
		}
	} else {
	/* parent goes here */
		wait(NULL);
		printf ("Hello, I am parent of %d (my pid:%d)\n", rc, (int) getpid());
	}

	return 0;
}
