//
// Created by Andrei García on 12/4/21.
//

#ifndef P2_MINISHELL_COMMON_H
#define P2_MINISHELL_COMMON_H

/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char*** argvv, int num_command) {
    //reset first
    for(int j = 0; j < 8; j++)
        argv_execvp[j] = NULL;

    int i = 0;
    for ( i = 0; argvv[num_command][i] != NULL; i++)
        argv_execvp[i] = argvv[num_command][i];
}

// STUDENTS CODE
checkMaxCommands(int command_counter, int MAX_COMMANDS) {
    if (command_counter > 0) {
        if (command_counter > MAX_COMMANDS)
            perror("Error: Numero máximo de comandos\n");
    }
}
extractCommands(char ***argvv, int command_counter) {
    for (int i = 0; i < command_counter; i++) {
        getCompleteCommand(argvv, i);
    }
}
#endif //P2_MINISHELL_COMMON_H
