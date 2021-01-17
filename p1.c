#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>


int
main (int argc, char **argv)
{
  char *myargs[5];

  printf ("Hello World!(pid:%d)\n", getpid());
  int rc = fork();
  if (rc < 0){
      /* fork failed*/
      fprintf (stderr, "Fork failed\n");
      exit(1);
  } else if (rc == 0){
    /* child */
    printf ("Hello, i am child (pid:%d)\n", (int) getpid());
    myargs[0] = "ping";
    myargs[1] = "8.8.8.8";
    myargs[2] = "-c";
		myargs[3] = "5";
		myargs[4] = NULL;
    printf ("Comando: %s\n", myargs[0]);
    execvp(myargs[0], myargs);
  } else {
    /* parent goes here */
    wait(NULL);
    /*printf ("Hello, I am parent of %d (my pid:%d)\n", rc, (int) getpid());*/
  }
  printf ("Teste\n");

  return 0;
}
