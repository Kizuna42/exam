#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int picoshell(char **cmds[])
{
    int i = 0, prev_pipe = -1;
    pid_t *pids;
    
    // Count commands
    while (cmds[i]) i++;
    if (i == 0) return 0;
    
    pids = malloc(i * sizeof(pid_t));
    if (!pids) return 1;
    
    for (int j = 0; j < i; j++)
    {
        int pipefd[2];
        if (j < i - 1 && pipe(pipefd) == -1) { free(pids); return 1; }
        
        pid_t pid = fork();
        if (pid == -1) { free(pids); return 1; }
        
        if (pid == 0)
        {
            if (prev_pipe != -1) { dup2(prev_pipe, 0); close(prev_pipe); }
            if (j < i - 1) { dup2(pipefd[1], 1); close(pipefd[0]); close(pipefd[1]); }
            execvp(cmds[j][0], cmds[j]);
            exit(1);
        }
        
        pids[j] = pid;
        if (prev_pipe != -1) close(prev_pipe);
        if (j < i - 1) { close(pipefd[1]); prev_pipe = pipefd[0]; }
    }
    
    if (prev_pipe != -1) close(prev_pipe);
    
    for (int j = 0; j < i; j++)
        waitpid(pids[j], NULL, 0);
    
    free(pids);
    return 0;
} 