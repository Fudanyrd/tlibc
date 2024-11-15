#include "std.h"

/** 
 * Wrapper of sys_execve, will auto search exe path. 
 */
void Execve(char *exe, char **argv) {
    static char buf[128];
    // possible path to exe
    static const char *dir[] = {
        "/",
        "/bin/",
        "/usr/bin/",
        "/usr/local/bin/",
    };
    char *chp;

    sys_execve(exe, argv, NULL);
    for (size_t i = 0; i < sizeof(dir) / sizeof(dir[0]); i++) {
        const char *path = dir[i];
        chp = (char *)buf;

        // copy dir name
        chp = Strcpy(chp, path);
        // copy path name
        chp = Strcpy(chp, exe);
        sys_execve((char *)buf, argv, NULL);
    }
}

/** Execute a single job */
static void exec_single(job_t *job) __attribute__((noreturn));

/** Execute a job recursively(do not return) */
static void exec_recur_noret(job_t *job, int cur, int total, int stdout_fd) __attribute__((noreturn));

static void exec_single(job_t *job) {
    Assert(job != NULL);
    if (job->stdin_fo) {
        int fd = sys_open(job[0].stdin_fo, O_RDONLY);
        sys_dup2(fd, 0);
    }

    if (job->stdout_fo) {
        int fd = sys_open(job[0].stdout_fo, O_CREAT | O_RDWR);
        sys_dup2(fd, 1);
    }

    if (job->stderr_fo) {
        int fd = sys_open(job[0].stderr_fo, O_CREAT | O_RDWR);
        sys_dup2(fd, 2);
    }

    Execve((char *)job->exe, (char **)job->argv);
}

int exec_job(job_t *job, int cnt) {
    Assert(cnt >= 0);
    Assert(cnt == 0 || job != NULL);

    int pid = sys_fork();

    if (pid < 0) {
        return -1;
    }

    if (pid == 0) {
        exec_recur_noret(job, cnt - 1, cnt, 1);
        sys_exit(1);
    }

    return sys_waitid(P_ALL, 0, NULL, WEXITED);
}

static void exec_recur_noret(job_t *job, int cur, int total, int stdout_fd) {
    Assert(cur <= total);

    if (cur == 0) {
        exec_single(job);
        sys_exit(1);
    }

    int pip[2];
    if (sys_pipe((int *)pip) != 0) {
        sys_exit(1);
    }

    int pid = sys_fork();
    if (pid < 0) {
        sys_exit(1);
    }

    if (pid == 0) {
        // child proc
        sys_dup2(pip[1], 1);
        sys_close(pip[0]);
        sys_close(pip[1]);
        exec_recur_noret(job, cur - 1, total, 1);
    } else {
        // parent proc
        sys_dup2(pip[0], 0);
        sys_close(pip[0]);
        sys_close(pip[1]);
        exec_single(&job[cur]);
    }

    sys_exit(1);
}
