#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    pid_t pid = fork();
    if (pid == -1) return -1;
    if (pid == 0)
    {
        f();
        exit(0);
    }

    struct sigaction sa_old, sa_new;
    sigemptyset(&sa_new.sa_mask);
    sa_new.sa_handler = SIG_IGN;
    sa_new.sa_flags = 0;
    if (sigaction(SIGALRM, &sa_new, &sa_old) == -1)
        return -1;
    alarm(timeout);
    int status;
    pid_t result = waitpid(pid, &status, 0);
    alarm(0);
    sigaction(SIGALRM, &sa_old, NULL);

    if (result == -1)
    {
        if (errno == EINTR)
        {
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            if (verbose) printf("Bad function: timed out after %u seconds\n", timeout);
            return 0;
        }
        return -1;
    }

    if (WIFEXITED(status))
    {
        int exit_code = WEXITSTATUS(status);
        if (exit_code == 0)
        {
            if (verbose) printf("Nice function!\n");
            return 1;
        }
        if (verbose) printf("Bad function: exited with code %d\n", exit_code);
        return 0;
    }
    if (WIFSIGNALED(status))
    {
        int sig = WTERMSIG(status);
        if (verbose) printf("Bad function: %s\n", strsignal(sig));
        return 0;
    }
    return 0;
}