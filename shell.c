#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <stdio.h>

#define MAX_LEN_LINE    100
#define LEN_HOSTNAME	30

int getprompt(void)
{
    char hostname[LEN_HOSTNAME + 1];
    memset(hostname, 0x00, sizeof(hostname));
    printf("username: %s\n", getpwuid(getuid())->pw_name);

    gethostname(hostname, LEN_HOSTNAME);
    printf("hostname: %s\n", hostname);

    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    printf("cwd: %s\n", cwd);

    return 0;
}

int main(void)
{
    char command[MAX_LEN_LINE];
    char *arg;
    char *args[] = {command, arg, NULL};
    int ret, status;
    pid_t pid, cpid;
    
    getprompt();
    
    while (true) {
        char *s;
        int len;
        
        printf("MyShell $ ");
        s = fgets(command, MAX_LEN_LINE, stdin);
       
       	if (s == NULL) {
            fprintf(stderr, "fgets failed\n");
            exit(1);
        }
       

        len = strlen(command);
        printf("%d\n", len);
        if (command[len - 1] == '\n') {
            command[len - 1] = '\0'; 
        }
        
        printf("[%s]\n", command);

	// 'exit' command
	if (strcmp(command, "exit") == 0) {
		printf("%c[1;35m", 27);
		printf("\nExit program\n");
		printf("%c[0m\n", 27);
		break;
	}

	// 'clear' command
	if (strcmp(command, "clear") == 0) {
		printf("\e[1;1H\e[2J");
	}

	// 'cd' command
	if (strcmp(command, "cd") == 0) {	
	     chdir("..");
	     printf("%c[1;35m", 27);
	     printf("%s\n", getcwd(s, 100));
	     printf("%c[0m\n", 27);
	}

	arg = strtok(command, " ");


        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork failed\n");
            exit(1);
        } 
        if (pid != 0) {  /* parent */
            cpid = waitpid(pid, &status, 0);
            if (cpid != pid) {
                fprintf(stderr, "waitpid failed\n");        
            }
            printf("Child process terminated\n");
            if (WIFEXITED(status)) {
                printf("Exit status is %d\n", WEXITSTATUS(status)); 
            }
        }
        else {  /* child */

	    // 'pwd' command
	    if (strcmp(command, "pwd") == 0) {
		    arg = strtok(NULL, " ");
		    args[0] = "/bin/pwd";
		    args[1] = arg;
	    }

	    //'ls' command
	    else if (strcmp(command, "ls") == 0) {
		    arg = strtok(NULL, " ");
		    args[0] = "/bin/ls";
		    args[1] = arg;
	    }

            ret = execve(args[0], args, NULL);
            if (ret < 0) {

                fprintf(stderr, "execve failed\n");   
                return 1;
            }
        } 
    }
    return 0;
}
