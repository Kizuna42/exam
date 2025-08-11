#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int ft_popen(const char *file, char *const argv[], char type)
{
    if (type != 'r' && type != 'w') return -1;
    if (!file || !argv || !argv[0]) return -1;
    
    int pipefd[2];
    if (pipe(pipefd) == -1) return -1;
    
    pid_t pid = fork();
    if (pid == -1) { close(pipefd[0]); close(pipefd[1]); return -1; }
    
    if (pid == 0)
    {
        if (type == 'r')
        {
            close(pipefd[0]);
            dup2(pipefd[1], 1);
            close(pipefd[1]);
        }
        else
        {
            close(pipefd[1]);
            dup2(pipefd[0], 0);
            close(pipefd[0]);
        }
        execvp(file, argv);
        exit(1);
    }
    
    if (type == 'r')
    {
        close(pipefd[1]);
        return pipefd[0];
    }
    else
    {
        close(pipefd[0]);
        return pipefd[1];
    }
} 