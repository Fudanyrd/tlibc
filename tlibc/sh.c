#include "sys.h"
#include "std.h"

/** Support at most 8 jobs in a single cmd. */
static job_t jobs[8];

static char *e1[] = {
    "_cat",
    "README.txt",
    NULL
};

static char *cat[] = {
    "_cat",
    NULL
};

static char *wc[] = {
    "_wc",
    NULL
};

static void eg1(void) {
    int pipe[2];
    if (sys_pipe(pipe) != 0) {
        sys_exit(1);
    }

    int pid = sys_fork();
    if (pid < 0) {
        sys_exit(1);
    }

    if (pid == 0) {
        // child
        sys_dup2(pipe[1], 1);
        sys_close(pipe[0]);
        sys_close(pipe[1]);
        sys_execve(e1[0], e1, NULL);
        sys_exit(1);
    }

    // parent
    sys_dup2(pipe[0], 0);
    sys_close(pipe[0]);
    sys_close(pipe[1]);
    sys_execve(wc[0], wc, NULL);
    sys_exit(1);
}

// cat README.txt | cat | wc
static void eg2(void) {
    int pipe[2];
    if (sys_pipe(pipe) != 0) {
        sys_exit(1);
    }

    int pid = sys_fork();
    if (pid < 0) {
        sys_exit(1);
    }

    if (pid == 0) {
        // parent 
        int pipe2[2];
        if (sys_pipe(pipe2) != 0) {
            sys_exit(1);
        }

        int pid2 = sys_fork();
        if (pid < 0) {
            sys_exit(1);
        }

        if (pid2 == 0) {
            sys_dup2(pipe2[1], 1);
            sys_close(pipe2[0]);
            sys_close(pipe2[1]);
            sys_execve(e1[0], e1, NULL);
            sys_exit(1);
        }

        sys_dup2(pipe[1], 1);
        sys_close(pipe[0]);
        sys_close(pipe[1]);
        sys_dup2(pipe2[0], 0);
        sys_close(pipe2[0]);
        sys_close(pipe2[1]);
        // cat 
        sys_execve(cat[0], cat, NULL);
        sys_exit(1);
    }

    // grand parent
    sys_dup2(pipe[0], 0);
    sys_close(pipe[0]);
    sys_close(pipe[1]);
    sys_execve(wc[0], wc, NULL);
    sys_exit(1);
}

int main(int argc, char **argv) {
    // eg2();

    job_t *job = &jobs[0];
    Strcpy(job->exe, e1[0]);
    job->argv[0] = e1[0];
    job->argv[1] = e1[1];
    job->argv[2] = NULL;
    job->pipe = false;

    job++;
    Strcpy(job->exe, cat[0]);
    job->argv[0] = cat[0];
    job->argv[1] = cat[1];

    job++;
    Strcpy(job->exe, wc[0]);
    job->argv[0] = wc[0];
    job->argv[1] = wc[1];
    job->argv[2] = NULL;
    
    exec_job(jobs, 3);
    return 0;
}
