#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int picoshell(char **cmds[])
{
    int count = 0;
    while (cmds[count]) count++;
    if (count == 0) return 0;

    int prev_read = -1;
    for (int i = 0; i < count; i++)
    {
        int pipefd[2] = {-1, -1};
        if (i < count - 1 && pipe(pipefd) == -1)
        {
            if (prev_read != -1) close(prev_read);
            return 1;
        }

        pid_t pid = fork();
        if (pid == -1)
        {
            if (pipefd[0] != -1) { close(pipefd[0]); close(pipefd[1]); }
            if (prev_read != -1) close(prev_read);
            return 1;
        }

        if (pid == 0)
        {
            if (prev_read != -1)
            {
                dup2(prev_read, 0);
                close(prev_read);
            }
            if (i < count - 1)
            {
                dup2(pipefd[1], 1);
                close(pipefd[0]);
                close(pipefd[1]);
            }
            execvp(cmds[i][0], cmds[i]);
            exit(1);
        }

        if (prev_read != -1) close(prev_read);
        if (i < count - 1)
        {
            close(pipefd[1]);
            prev_read = pipefd[0];
        }
    }

    if (prev_read != -1) close(prev_read);
    for (int i = 0; i < count; i++)
        wait(NULL);
    return 0;
}