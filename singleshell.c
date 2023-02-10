#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

void handle_sig(int sig) { //signal handler to be called when user presses ctrl-C
    printf("Control-C was pressed. Exiting...\n");
    exit(0);//exit with return code 0
}

int main(int argc, char *argv[]) {
	struct sigaction sa; 
	sa.sa_handler = &handle_sig; //Tells OS what function to call when the user inputs a signal
	sigaction(SIGINT, &sa, NULL); //Signal is handled
	int rc = fork(); //Creates new process

	if (rc < 0) { //Fork failed
		fprintf(stderr, "fork failed\n");
		exit(1);
	} else if (rc == 0) { //Child process
		struct sigaction sa;
		/* next two lines got rid of a double call to handle_sig when I ran the code. It uses SIG_DFL which is the default behavior of the signal*/

		sa.sa_handler = SIG_DFL;
		sigaction(SIGINT, &sa, NULL);
		char *userin[1024];
		char buff[1024];
		char delimiter[] = " ";
		char *token;
		int i = 0;

		printf("Execute? ");
		fgets(buff, 1024, stdin); //puts user input into buff
		if (buff[strlen(buff)-1] == '\n') {//Replaces newline with null terminator. 
			buff[strlen(buff)-1] = '\0';
		}
		token = strtok(buff, delimiter); //Using strtok to split the string. Strtok parses the string up to the first instance of the delimiter char and replaces it with the null byte and returns the addr of the first char of the token.
		while (token != NULL) {
			userin[i] = token;
			i++;
			token = strtok(NULL, delimiter);
		}
		
		printf("Executing: %s\n", userin[0]);	
		int exec_return = execvp(userin[0], userin); //executes the command in userin[0] with the arguments stored at the address userin.	
		if (exec_return == -1) {
			perror("Error: ");
			exit(1);
		}
		
	} else { //Parent process
		wait(NULL); //waiting for child process to finish
		printf("Execution complete!\n");
	}

	return 0;
}
