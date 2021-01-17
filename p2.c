void sigHandler(int);

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

unsigned short sinal = 0;

int
main(int argc, char **argv)
{
	pid_t rc;

	printf ("Disparador incializado com pid=%d\n", getpid());
	if (signal(SIGUSR1, sigHandler) == SIG_ERR){
		printf("Não consigo lidar com SIGUSR1\n");
	}
	if (signal(SIGUSR2, sigHandler) == SIG_ERR){
		printf("Não consigo lidar com SIGUSR2\n");
	}
	if (signal(SIGTERM, sigHandler) == SIG_ERR){
		printf("Não consigo lidar com SIGTERM\n");
	}
	while(1){
		if (sinal == 1 /*SIGUSR1*/){
			rc = fork();
			sinal = 0;
			if (rc < 0){
				/* fork falhou*/
				fprintf (stderr, "Fork failed\n");
				exit(1);
			} else if (rc == 0){
				/* processo filho */
				printf ("Hello, i am child (pid:%d)\n", (int) getpid());
				printf ("Executando comando 1\n");
				exit (0);
			} else {
			/* processo pai */
				wait(NULL);
				printf ("Processo pai\n");
			}
		}

		if (sinal == 2 /*SIGUSR2*/){
			rc = fork();
			sinal = 0;
			if (rc < 0){
				/* fork failed*/
				fprintf (stderr, "Fork failed\n");
				exit(1);
			} else if (rc == 0){
				/* child */
				printf ("Hello, i am child (pid:%d)\n", (int) getpid());
				printf ("Executando comando 2\n");
				exit (0);
			} else {
			/* parent goes here */
				wait(NULL);
				printf ("Processo pai\n");
			}

		}

		if (sinal == 3 /*SIGTERM*/){
			printf ("Finalizando o disparador\n");
			return 0;
		}
		sleep(1);
	}

	return 0;
}

void sigHandler(int sigNum){
	if (sigNum == SIGUSR1){
		signal(SIGUSR1, sigHandler);
		printf ("Recebi SIGUSR1\n");
		sinal = 1;
	}

	if (sigNum == SIGUSR2){
		signal(SIGUSR2, sigHandler);
		printf ("Recebi SIGUSR2\n");
		sinal = 2;
	}

	if (sigNum == SIGTERM){
		signal(SIGTERM, sigHandler);
		printf ("Recebi SIGTERM\n");
		sinal = 3;
	}

}
