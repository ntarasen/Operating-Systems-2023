#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

void handle_sig(int sig) {
    printf("Control-C was pressed. Exiting...");
    exit(0);
}

int main(int argc, char *argv[]) {
	int rc = fork();

	if (rc < 0) {
		fprintf(stderr, "fork failed\n");
		exit(1);
	} else if (rc == 0) { //Child process
		char *userin[1024];
		char buff[1024];
		char delimiter[] = " ";
		char *token;
		int i = 0;
		struct sigaction sa;
		sa.sa_handler = &handle_sig;
		sigaction(SIGINT, &sa, NULL);

		printf("Execute? ");
		fgets(buff, 1024, stdin);
		if (buff[strlen(buff)-1] == '\n') {
			buff[strlen(buff)-1] = '\0';
		}
		token = strtok(buff, delimiter);
		while (token != NULL) {
			userin[i] = token;
			i++;
			token = strtok(NULL, delimiter);
		}
		
		printf("Executing: %s\n", userin[0]);	
		int exec_return = execvp(userin[0], userin);		
		if (exec_return == -1) {
			perror("Error: ");
			exit(1);
		}
		
	} else { //Parent process
		wait(NULL);
		printf("Execution complete!\n");
	}

	return 0;
}
