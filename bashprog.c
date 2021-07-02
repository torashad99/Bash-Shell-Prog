#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

/*
static void the_handler(const int sig)
{
 char * str = strsignal(sig);
 write(2, str, strlen(str));
 write(2, "Detected", 1);
 write(2, "CS361 >", 1);
}
*/

static void int_handler(int sig){
	char msg[] = "caught sigint\nCS361 >";
  	write(1, msg, sizeof(msg));
}

static void tstp_handler(int sig){
	char msg[] = "caught sigtstp\nCS361 >";
  	write(1, msg, sizeof(msg));
}

/*
static void setup(void)
{
 
 struct sigaction sp;
 sp.sa_handler = tstp_handler;
 sigemptyset(&(sp.sa_mask));
 sp.sa_flags = SA_RESTART;
 sigaction(SIGTSTP, &sp, NULL);

 struct sigaction si;
 si.sa_handler = int_handler;
 sigemptyset(&(si.sa_mask));
 si.sa_flags = SA_RESTART;
 sigaction(SIGINT, &si, NULL);

}
*/



int main(){

  //create some space for our strings
  //setup();
  char line[500];
  /*
  char* argsarray[20];

  int k;
  for (k=0; k<20; k++) 
         argsarray[k] = (char *)malloc(100 * sizeof(char));
  */
  //signal(SIGINT, int_handler);
  //signal(SIGTSTP, tstp_handler);

  int flag = 0;

  while(flag == 0){

	signal(SIGINT, int_handler);
  signal(SIGTSTP, tstp_handler); 

	char* argsarray[20];
	char* semcol[20];
	int semflag = -999;
  int pipflag = -888;

  int k;
  for (k=0; k<20; k++)
      argsarray[k] = (char *)malloc(100 * sizeof(char));

	for (k=0; k<20; k++)
      semcol[k] = (char *)malloc(100 * sizeof(char));

  	//print prompt
  	printf("CS361 >");
  	//read line from terminal
  	fgets(line, 500, stdin);

  	//break the string up into words
  	char *word = strtok(line, " \n\t");
  	int i = 0;
	  int s = 0;
  	while (word) {
    		//printf("word: %s\n", word);

		  if(strcmp(word, ";") == 0){
			 semflag = 0;
			 word = strtok(NULL, " \n\t");
			 continue;
		  }

      if(strcmp(word, "|") == 0){
       pipflag = 0;
       word = strtok(NULL, " \n\t");
       continue;
      }

		  if(semflag != -999 || pipflag != -888){
			   strcpy(semcol[s], word);
			   s = s + 1;
		  }

		  if(semflag == -999 && pipflag == -888){
          strcpy(argsarray[i], word);
			    i = i + 1;
		  }

    		//get next word
    	word = strtok(NULL, " \n\t");
  	}

	 if(strcmp(argsarray[0], "exit") == 0){
		  //flag = 1;
	    break;
	}
	//Debugging functions
	/*
	int j=0;
  	for (;j<s;j++){
    	printf("semcol[%d]: %s\n", j, semcol[j]);
  	}

	for (j=0;j<i;j++){
        printf("argsarray[%d]: %s\n", j, argsarray[j]);
        }
	*/

  argsarray[i] = (char*)0;
	
	if(semflag != -999 || pipflag != -888)
		semcol[s] = (char*)0;

    int pipeio[2];
    pipe(pipeio);

	//Child1
  	int pid = fork();
  	if (pid == 0) {

          if(pipflag != -888){
              dup2(pipeio[1],STDOUT_FILENO);
              close(pipeio[0]);
              close(pipeio[1]);
              execvp(argsarray[0], argsarray);
          }
          else{
    			    execvp(argsarray[0], argsarray);
          }
  	}	
	  else {
                //Parent

                int status;

                int pidpip = pid;
                int statpip = WEXITSTATUS(status);

                if(pipflag != -888){
                    
                  //printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));

                  pid = fork();

                  if(pid == 0){
                    dup2(pipeio[0], STDIN_FILENO);
                    close(pipeio[1]);
                    close(pipeio[0]);
                    execvp(semcol[0], semcol);
                  }

                  else{
                    close(pipeio[0]);
                    close(pipeio[1]);
                    waitpid(pid, &status, 0);
                    printf("pid:%d status:%d\n", pidpip, statpip);
                    printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));
                  }
                  //waitpid(-1, &status, 0);
                  //printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));
                  //waitpid(-1, &status, 0);
                  //printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));
                }
                else{
                  waitpid(pid,&status,0);
                  printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));
                }  
       		      //Child2 
		            if(semflag != -999){
			           int pid2 = fork();

			           if(pid2 == 0)
				            execvp(semcol[0], semcol);
			         
                 else{
		                int status2;
                		waitpid(pid2,&status2,0);
                		printf("pid:%d status:%d\n", pid2, WEXITSTATUS(status));
			             }
		            }
	}

	for (k=0; k<20; k++)
                free(argsarray[k]);
	
	for (k=0; k<20; k++)
                free(semcol[k]);

  }
  
}