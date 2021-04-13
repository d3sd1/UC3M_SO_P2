#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <math.h>


#define MAX_COMMANDS 8


// ficheros por si hay redirección
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param) {
    printf("****  Saliendo del MSH **** \n");
    //signal(SIGINT, siginthandler);
    exit(0);
}

/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char ***argvv, int num_command) {
    //reset first
    for (int j = 0; j < 8; j++)
        argv_execvp[j] = NULL;

    int i = 0;
    for (i = 0; argvv[num_command][i] != NULL; i++)
        argv_execvp[i] = argvv[num_command][i];
}

// STUDENTS CODE
void printMyCalcError() {
    if ((write(1, "[ERROR] La estructura del comando es <operando 1> <add/mod> <operando 2>\n",
               strlen("[ERROR] La estructura del comando es <operando 1> <add/mod> <operando 2>\n"))) <
        0) {
        perror("Error writing log output.\n");
    }
}

void printMyLsOpeningError() {
    if ((write(1, "[ERROR] Error al abrir el fichero origen : No such file or directory\n",
               strlen("[ERROR] Error al abrir el fichero origen : No such file or directory\n"))) < 0) {
        perror("Error writing log output.\n");
    }
}

void printMyLsSyntaxError() {
    if ((write(1, "[ERROR] La estructura del comando es mycp <fichero origen> <fichero destino>\n",
               strlen("[ERROR] La estructura del comando es mycp <fichero origen> <fichero destino>\n"))) <
        0) {
        perror("Error writing log output.\n");
    }
}


/**
 * Main sheell  Loop
 */
int main(int argc, char *argv[]) {
    /**** Do not delete this code.****/
    int end = 0;
    int executed_cmd_lines = -1;
    char *cmd_line = NULL;
    char *cmd_lines[10];

    if (!isatty(STDIN_FILENO)) {
        cmd_line = (char *) malloc(100);
        while (scanf(" %[^\n]", cmd_line) != EOF) {
            if (strlen(cmd_line) <= 0) return 0;
            cmd_lines[end] = (char *) malloc(strlen(cmd_line) + 1);
            strcpy(cmd_lines[end], cmd_line);
            end++;
            fflush(stdin);
            fflush(stdout);
        }
    }

    /*********************************/

    char ***argvv = NULL;
    int num_commands;

    int Acu = 0;

    while (1) {
        error:;
        int status = 0;
        int command_counter = 0;
        int isBackground = 0;
        signal(SIGINT, siginthandler);

        // Prompt
        write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

        // Get command
        //********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
        executed_cmd_lines++;
        if (end != 0 && executed_cmd_lines < end) {
            command_counter = read_command_correction(&argvv, filev, &isBackground, cmd_lines[executed_cmd_lines]);
        } else if (end != 0 && executed_cmd_lines == end)
            return 0;
        else
            command_counter = read_command(&argvv, filev, &isBackground); //NORMAL MODE
        //************************************************************************************************


        /************************ STUDENTS CODE ********************************/

        /**
         * BEGIN BLOCK: CHECK_PARAMETERS
         * DESCRIPTION: CHECK LIMITS FOR CURRENT OPERATION
         */
        if (command_counter > 0) {
            if (command_counter > MAX_COMMANDS)
                perror("Error: Max command counts reached.\n");
        }

        for (int i = 0; i < command_counter; i++) {
            getCompleteCommand(argvv, i);
        }
        /**
         * END BLOCK: CHECK_PARAMETERS
         */

        /**
         * BEGIN BLOCK: MYCALC
         * DESCRIPTION: IMPLEMENT CALCULATOR
         */
        if (strcmp(argv_execvp[0], "mycalc") == 0) {
            if (argv_execvp[1] != NULL && argv_execvp[2] != NULL && argv_execvp[3] != NULL) {

                /**
                 * BEGIN BLOCK: MYCALC_ADD
                 * DESCRIPTION: SUM FUNCTION
                 */
                if (strcmp(argv_execvp[2], "add") == 0) {
                    int x = atoi(argv_execvp[1]);
                    int y = atoi(argv_execvp[3]);
                    Acu += x + y;
                    char buf[20];
                    sprintf(buf, "%d", Acu);
                    const char *p = buf;

                    /**
                     * BEGIN BLOCK: MYCALC_ADD_PROCESS
                     * DESCRIPTION:
                     * 1. Handle environment.
                     * 2. Output it.
                     * 3. Handle errors.
                     */
                    if (setenv("Acc", p, 1) < 0) {
                        perror("Error al dar valor a la variable de entorno\n");
                        goto error;
                    }

                    char str[100];
                    snprintf(str, 100, "[OK] %d + %d = %d; Acc %s\n", x, y, x + y, getenv("Acc"));

                    if ((write(2, str, strlen(str))) < 0) {
                        perror("Error handling add.\n");
                        goto error;
                    }
                    /**
                     * END BLOCK: MYCALC_ADD_ENVIRONMENT
                     */
                    /**
                     * END BLOCK: MYCALC_ADD
                     */
                }
                    /**
                     * BEGIN BLOCK: MYCALC_MOD
                     * DESCRIPTION: EXTRACTS MODULE
                     * 1. Handle variables.
                     * 2. Output it.
                     * 3. Handle errors.
                     */
                else if (strcmp(argv_execvp[2], "mod") == 0) {
                    int x = atoi(argv_execvp[1]), y = atoi(argv_execvp[3]);
                    char str[100];

                    snprintf(str, 100, "[OK] %d %% %d = %d * %d + %d\n", x, y, y, abs(floor(x / y)), x % y);
                    if ((write(2, str, strlen(str))) < 0) {
                        perror("Error handling module.\n");
                        goto error;
                    }
                }
                    /**
                     * END BLOCK: MYCALC_MOD
                     */

                    /**
                     * BEGIN BLOCK: MYCALC_ERR_HANDLER
                     * DESCRIPTION: SUM FUNCTION
                     * 1. ADD ENVIRONMENT VARIABLES.
                     * 2. Output it.
                     * 3. Handle errors.
                     */
                else {
                    printMyCalcError();
                }
            } else {
                printMyCalcError();
            }
            /**
             * END BLOCK: MYCALC_ERR_HANDLER
             */
        }
            /**
            * END BLOCK: MYCALC
            */

            /**
             * BEGIN BLOCK: MYCP
             * DESCRIPTION: COPIES FILES FROM SOURCE TO DESTINATION
             */
        else if (strcmp(argv_execvp[0], "mycp") == 0) {
            if (argv_execvp[1] != NULL && argv_execvp[2] != NULL) {
                int sourceDescriptor = open(argv_execvp[1], O_RDONLY, 0644);
                if (sourceDescriptor >= 0) {
                    int destinationDescriptor = open(argv_execvp[2], O_TRUNC | O_WRONLY | O_CREAT, 0644);
                    char buf[1024];

                    /**
                     * BEGIN BLOCK: MYCP_READ
                     * DESCRIPTION: READ FILE LINE BY LINE AND PRINT IT WITH STDOUT_FILENO
                     */
                    int nRead, nWrite;
                    while ((nRead = read(sourceDescriptor, buf, 1024)) > 0) {
                        do {
                            nWrite = write(destinationDescriptor, buf, nRead);
                            if (nWrite > 0) {
                                nRead -= nWrite;
                            }
                        } while (nRead > 0);
                    }
                    if (nRead < 0) {
                        perror("Error reading file.\n");
                    }
                    /**
                     * END BLOCK: MYCP_READ
                     */

                    /**
                     * BEGIN BLOCK: MYCP_CLOSE
                     * DESCRIPTION: CLOSE FILES
                     */
                    if (close(sourceDescriptor) < 0 || close(destinationDescriptor) < 0) {
                        perror("Error while closing opened files.\n");
                    }
                    /**
                     * END BLOCK: MYCP_CLOSE
                     */


                    /**
                     * BEGIN BLOCK: MYCP_OUTUT
                     * DESCRIPTION: OUTPUT AND PRINT WHATEVER WE HAVE TO.
                     */
                    char str[100];
                    snprintf(str, 100, "[OK] Copiado con exito el fichero %s a %s\n", argv_execvp[1], argv_execvp[2]);
                    write(1, str, strlen(str));
                    /**
                     * END BLOCK: MYCP_OUTUT
                     */
                }
                    /**
                     * BEGIN BLOCK: MYCP_HANDLE_ERR_OPENING
                     * DESCRIPTION: PRINT ERROR IF WE COULD NOT OPEN FILE
                     */
                else {
                    printMyLsOpeningError();
                }
                /**
                 * END BLOCK: MYCP_HANDLE_ERR_OPENING
                 */
            }
                /**
                 * BEGIN BLOCK: MYCP_HANDLE_ERR_SYNTAX
                 * DESCRIPTION: PRINT ERROR IF  COMMAND SYNTAX IS WRONG.
                 */
            else {
                printMyLsSyntaxError();
            }
            /**
             * END BLOCK: MYCP_HANDLE_ERR_SYNTAX
             */
        }
            /**
            * END BLOCK: MYCP
            */

            /**
             * BEGIN BLOCK: COMMANDS_AND_PIPES
             * DESCRIPTION: HANDLE SIMPLE COMMANDS AND PIPES.
             */
        else if (command_counter == 1) {
            int pid = fork();
            if (pid == -1) {
                perror("Fork overwrite error.\n");
                return (-1);
            }

            int fileHandler = 0;
            int stat;

            /**
             * BEGIN BLOCK: COMMANDS_AND_PIPES_CHILD
             * DESCRIPTION: HANDLE CHILD PROCESS.
             */
            if (pid == 0) {
                /**
                 * BEGIN BLOCK: COMMANDS_AND_PIPES_CHILD_REDIRECTS
                 * DESCRIPTION: HANDLE CHILD PROCESS REDIRECTS (INPUT, OUTPUT, ERROR)
                 */
                if (strcmp(filev[1], "0") != 0) {
                    if ((close(1)) < 0) {
                        perror("Error closing descriptor.\n");
                        goto error;
                    }

                    if ((fileHandler = open(filev[1], O_TRUNC | O_WRONLY | O_CREAT, 0644)) < 0) {
                        perror("Error opening file.\n");
                        goto error;
                    }
                }
                /**
                 * END BLOCK: COMMANDS_AND_PIPES_CHILD_REDIRECTS
                 */
                /**
                * BEGIN BLOCK: COMMANDS_AND_PIPES_CHILD_ERR_HANDLER
                * DESCRIPTION: HANDLE ERRORS
                */
                if (strcmp(filev[0], "0") != 0) {
                    if ((close(0)) < 0) {
                        perror("Error closing descriptor.\n");
                        goto error;
                    }
                    if ((fileHandler = open(filev[0], O_RDWR, 0644)) < 0) {
                        perror("Error closing file.\n");
                        goto error;
                    }
                }
                if (strcmp(filev[2], "0") != 0) {
                    if ((close(2)) < 0) {
                        perror("Error closing descriptor.\n");
                        goto error;
                    }
                    if ((fileHandler = open(filev[2], O_TRUNC | O_WRONLY | O_CREAT, 0644)) < 0) {
                        perror("Error opening file.\n");
                        goto error;
                    }
                }
                /**
                 * END BLOCK: COMMANDS_AND_PIPES_CHILD_ERR_HANDLER
                 */

                /**
                * BEGIN BLOCK: COMMANDS_AND_PIPES_CHILD_EXEC
                * DESCRIPTION: EXECUTE CHILD
                */
                if (execvp(argv_execvp[0], argv_execvp) < 0) {
                    perror("Execution error.\n");
                    goto error;
                }
                /**
                * END BLOCK: COMMANDS_AND_PIPES_CHILD_EXEC
                */
            }
                /**
                 * END BLOCK: COMMANDS_AND_PIPES_CHILD
                 */

                /**
                * BEGIN BLOCK: COMMANDS_AND_PIPES_PARENT
                * DESCRIPTION: EXECUTE PARENT.
                */
            else {
                if (!isBackground) {
                    while (wait(&stat) > 0);
                    if (stat < 0) {
                        perror("Child execution error.\n");
                    }
                }
            }
            /**
             * END BLOCK: COMMANDS_AND_PIPES_PARENT
             */
        }
            /**
             * END BLOCK: COMMANDS_AND_PIPES
             */


            /**
            * BEGIN BLOCK: COMMON_ERROR_HANDLER
            * DESCRIPTION: Error handler and special use cases.
            */
        else {
            int n = command_counter;
            int fd[2];
            int pid, status2;
            int fileHandler=0;

            int in;

            if ((in = dup(0)) < 0) {
                perror("Error duplicating descriptor.\n");
                goto error;
            }

            for (int i = 0; i < n; i++) {
                // CREATE NEXT PIPE
                if (i != n - 1) {
                    if (pipe(fd) < 0) {
                        perror("Pipe error.\n");
                        exit(0);
                    }
                }

                // NEXT PROCESS
                switch (pid = fork()) {

                    case -1:
                        perror("Error while forking.\n");

                        if((close(fd[0])) <0){
                            perror("Error closing descriptor.\n");
                            goto error;
                        }
                        if((close(fd[1])) <0){
                            perror("Error closing descriptor.\n");
                            goto error;
                        }
                        exit(0);
                    // CHILD NESTED CALLS
                    case 0:
                        // HALF PROCESS ERROR REDIR
                        if (strcmp(filev[2], "0") != 0) {
                            if((close(2)) <0){
                                perror("Error closing descriptor.\n");
                                goto error;
                            }

                            if ((fileHandler = open(filev[2], O_TRUNC | O_WRONLY | O_CREAT, 0644)) < 0) {
                                perror("Error opening file.\n");
                                goto error;
                            }
                        }

                        if (i == 0 && strcmp(filev[0], "0") != 0) {
                            if((close(0)) <0){
                                perror("Error closing descriptor.\n");
                                goto error;
                            }
                            if ((fileHandler = open(filev[0], O_RDWR, 0644)) < 0) {
                                perror("Error opening file.\n");
                                goto error;
                            }
                        } else {
                            if((close(0)) <0){
                                perror("Error closing descriptor.\n");
                                goto error;
                            }
                            if (dup(in) < 0) {
                                perror("Error duplicating descriptor.\n");
                                goto error;
                            }
                            if((close(in)) <0){
                                perror("Error closing descriptor.\n");
                                goto error;
                            }
                        }

                        // CLOSE POUT IF NOT LATEST PROCESS.
                        if (i != n - 1) {

                            if((close(1)) <0){
                                perror("Error closing descriptor.\n");
                                goto error;
                            }

                            if (dup(fd[1]) < 0) {
                                perror("Error duplicating descriptor.\n");
                                goto error;
                            }
                            if((close(fd[0])) <0){
                                perror("Error closing descriptor.\n");
                                goto error;
                            }
                            if((close(fd[1])) <0){
                                perror("Error closing descriptor.\n");
                                goto error;
                            }
                        } else {
                            if (strcmp(filev[1], "0") != 0) {
                                if((close(1)) <0){
                                    perror("Error closing descriptor.\n");
                                    goto error;
                                }

                                if ((fileHandler = open(filev[1], O_TRUNC | O_WRONLY | O_CREAT, 0644)) < 0) {
                                    perror("Error opening file.\n");
                                    goto error;
                                }
                            }
                        }

                        getCompleteCommand(argvv, i);
                        if (isBackground) {
                            printf("[%d]\n", getpid());
                        }

                        if (execvp(argv_execvp[0], argv_execvp) < 0) {
                            perror("Execution error.\n");
                            goto error;
                        }
                        break;

                    // PARENT PROCESS -> GIVES IN TO CHILDS (IF NOT LATEST)
                    default:
                        if((close(in)) <0){
                            perror("Error closing descriptor.\n");
                            goto error;
                        }                      if (i != n - 1) {
                    if ((in = dup(fd[0])) < 0) {
                        perror("Error duplicating descriptor.\n");
                        goto error;
                    }
                    if (dup(fd[0]) < 0) {
                        perror("Error duplicating descriptor.\n");
                        goto error;
                    }
                    if((close(fd[1])) <0){
                        perror("Error closing descriptor.\n");
                        goto error;
                    }
                }
                }
            }
            if(fileHandler!=0){
                if((close(fileHandler)) <0){
                    perror("Error closing descriptor.\n");
                    goto error;
                }
            }
            // AFTER LOOP IS FINISHED, THE FIRST PROCESS WAITS FOR LATEST, AND THEN AWAKENS 'EM ALL
            if (!isBackground) {
                while (wait(&status2) > 0);
                if (stat < 0) {
                    perror("Child error.\n");
                }
            }
        }
        /**
        * END BLOCK: COMMON_ERROR_HANDLER
        */
    }
    return 0;
}
