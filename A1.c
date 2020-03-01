// C Program to design a shell in Linux 
#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h> 

#define size 1000 // max number of letters to be supported 
#define size2 100 // max number of commands to be supported 

// Clearing the shell using escape sequences 
#define clear() printf("\033[H\033[J") 

// Greeting shell during startup 
void init_shell() { 
	clear(); 
	printf("\n\n\n\n******************"
		"************************"); 
	printf("\n\n\n\t**** Saif's Shell****"); 
	printf("\n\n\n\n*******************"
		"***********************"); 
	char* username = getenv("USER"); 
	printf("\n\n\nUSER is: @%s", username); 
	printf("\n"); 
	sleep(1); 
	clear(); 
} 

// Function to take input 
int user_input(char* str) { 
	char buf[15]; 

	printf("\nprompt>>>");
	fgets(buf, 15, stdin);
	buf[strcspn(buf, "\n")] = 0;   //removes \n from the string
	if (strlen(buf) != 0) { 
		add_history(buf); 
		strcpy(str, buf); 
		return 0;
	}
	else { 
		return 1;
	} 
} 

// Function to print Current Directory. 
void dir() { 
	char cwd[1024]; 
	getcwd(cwd, sizeof(cwd)); 
	printf("\nDir: %s", cwd); 
} 

// Function where the system command is executed 
void execArgs(char** parsed) { 
	// Forking a child 
	pid_t pid = fork(); 

	if (pid == 0) { 
		//printf("About to execute execvp\n");
		if (execvp(parsed[0], parsed) < 0) { 
			printf("\nUnable to execute the commmand"); 
		}

		exit(0); 
	} 
	else { 
		// waiting for child to terminate 
		wait(NULL); 
		//printf("Done with execute execvp\n");
		return;
	} 
}

// Function where the piped system commands is executed 
void execPipeArgs(char** parsed, char** parsedpipe) { 
	// 0 is read end, 1 is write end 
	int pipefd[2]; 
	pid_t p1, p2; 

	if (pipe(pipefd) < 0) { 
		printf("\nPipe could not be initialized"); 
		return; 
	} 
	p1 = fork(); 
	if (p1 < 0) { 
		printf("\nCould not fork"); 
		return; 
	} 

	if (p1 == 0) { 
		// Child 1 executing.. 
		// It only needs to write at the write end 
		close(pipefd[0]); 
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		if (execvp(parsed[0], parsed) < 0) { 
			printf("\nCould not execute command 1.."); 
			exit(0); 
		} 
	} else { 
		// Parent executing 
		p2 = fork(); 

		if (p2 < 0) { 
			printf("\nCould not fork"); 
			return;
		} 

		// Child 2 executing.. 
		// It only needs to read at the read end 
		if (p2 == 0) { 
			close(pipefd[1]); 
			dup2(pipefd[0], STDIN_FILENO); 
			close(pipefd[0]); 
			if (execvp(parsedpipe[0], parsedpipe) < 0) { 
				printf("\nCould not execute command 2.."); 
				exit(0); 
			} 
		} else { 
			// parent executing, waiting for two children 
			wait(NULL); 
			wait(NULL); 
		} 
	} 
} 

// Help command builtin 
void helper() { 
	puts("\n---WELCOME TO HELP---"
		"\nList of Commands supported:"
		"\n>cd"
		"\n>ls"
		"\n>exit"
		"\n>pipe handling"
		"\n>improper space handling");

	return; 
} 

// Function to execute builtin commands 
int myCMD(char** parsed) { 
	int NoOfOwnCmds = 3, i, switchOwnArg = 0; 
	char* mycmds[NoOfOwnCmds]; 
	char* username; 

	mycmds[0] = "exit"; 
	mycmds[1] = "cd"; 
	mycmds[2] = "help";

	for (i = 0; i < NoOfOwnCmds; i++) { 
		if (strcmp(parsed[0], mycmds[i]) == 0) {
			switchOwnArg = i + 1; 
			break;
		} 
	} 

	//printf("Value of switchOwnArg = %d\n", switchOwnArg);

	switch (switchOwnArg) { 
	case 1: 
		printf("\nGoodbye\n"); 
		exit(0); 
	case 2: 
		if(parsed[1]==NULL){
			chdir(".."); //goes back 1 folder
		}
		else{
		//printf("In chdir: %s\n", parsed[1]);
		chdir(parsed[1]);
		}
		return 1; 
	case 3: 
		helper(); 
		return 1;  
	default: 
		//printf("default mai hun\n");
		break;
	} 
	//printf("gonna return 0\n");
	return 0; 
} 

// function for finding pipe 
int parsePipe(char* str, char** strpiped) { 
	int i; 
	for (i = 0; i < 2; i++) { 
		strpiped[i] = strsep(&str, "|"); 
		//printf("strpiped[i] : %s\n", strpiped[i]);
		if (strpiped[i] == NULL) 
			break; 
	} 

	if (strpiped[1] == NULL) 
		return 0; // returns zero if no pipe is found. 
	else { 
		return 1; 
	} 
} 

// function for parsing command words 
void parseSpace(char* str, char** parsed) { 
	int i; 

	for (i = 0; i < size2; i++) { 
		parsed[i] = strsep(&str, " "); 
		printf("Parsed[i] : %s\n", parsed[i]);
		if (parsed[i] == NULL){
			//printf("Invalid command\n");
			break; 
		}
		if (strlen(parsed[i]) == 0){
			i--; 
		}
		//printf("leaving parseSpace ka for\n");
	}
	//printf("Leaving parseSpace\n"); 
} 

int processString(char* str, char** parsed, char** parsedpipe) { 

	char* strpiped[2]; 
	int piped = 0; 

	piped = parsePipe(str, strpiped); 

	if (piped) { 
		parseSpace(strpiped[0], parsed); //it may be a simple command so adding it to parsed
		printf("in next\n");
		parseSpace(strpiped[1], parsedpipe); 

	} else { 

		parseSpace(str, parsed); 
		//("str = %s and parsed = %s\n",str , parsed[0]);
	} 

	if (myCMD(parsed)){				// check here if the command is simple and can be handled
		//printf("inside command handler ka if\n");
		return 0; 
	}
	else{
		//printf("inside command handler ka else\n");
		return 1 + piped;			//confirms kay pipe command hi hai
	} 
} 






int main() { 
	char inputString[size], *parsedArgs[size2]; 
	char* pipeArgs[size2]; 
	int execFlag = 0; 
	init_shell(); 

	while (1) { 
		// print shell line 
		dir(); 
		// take input 
		if (user_input(inputString))
			continue;
		// process 
		//printf("outside take input ka if\n");
		execFlag = processString(inputString, parsedArgs, pipeArgs); 
		//printf("execFlag: %d\n", execFlag);
		// execflag returns zero if there is no command 
		// or it is a builtin command, 
		// 1 if it is a simple command 
		// 2 if it is including a pipe. 

		// execute 
		if (execFlag == 1) 
			execArgs(parsedArgs); 

		if (execFlag == 2) 
			execPipeArgs(parsedArgs, pipeArgs); 
	} 
	return 0; 
} 

