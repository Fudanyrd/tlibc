#pragma once
#ifndef _SYS_H_
#define _SYS_H_

/** This file contains syscall declaration. Their definition is
 * provided in sys.S as assembly code. */

#include <stddef.h>
#include <stdint.h>
#include "wait.h"

/** State Machine Operation */

extern void sys_exit(int code) __attribute__((noreturn));
extern void sys_execve(char *exe, char **argv, char **env);
extern int sys_fork(void);
extern void sys_pause(void);
extern int sys_waitid(uint32_t idtype, uint32_t id, siginfo_t *infop, int options);
extern int sys_kill(int pid, int sig);
extern int sys_getpid(void);

struct timespec {
    long tv_sec;  // seconds
    long tv_nsec;  // nanoseconds
};
extern int sys_nanosleep(const struct timespec *req, struct timespec *rem);

/** System IO */

extern int sys_chdir(const char *path);
extern char *sys_getcwd(char *buf, size_t siz);
extern long sys_write(int fd, const char *buf, size_t cnt);
extern long sys_read(int fd, char *buf, size_t cnt);

// Handle sys_open differently.
#ifdef __X86_64__
extern int sys_openat(int dirfd, const char *path, uint64_t mode);
extern int sys_open(const char *path, uint64_t mode);
#endif // __X86_64__

#ifdef __AARCH64__
#define AT_FDCWD (-100)
extern int sys_openat(int dirfd, const char *path, uint64_t mode);

static inline int sys_open(const char *path, uint64_t mode) {
    return *path == '/' ? sys_openat(0, path, mode) // absolute path
                        : sys_openat(AT_FDCWD, path, mode); 
}
#endif // __AARCH64__
extern void sys_close(int fd);
extern long sys_lseek(int fd, long off, int whence);
extern int sys_dup(int fd);
extern int sys_dup2(int oldfd, int newfd);
extern int sys_pipe(int *pip);
extern int sys_link(const char *oldpath, const char *newpath);
extern int sys_mkdir(const char *path, int mode);

struct stat {
    uint64_t st_dev;
    uint64_t st_ino;
    uint32_t st_mode;
    uint64_t st_nlink;
    uint32_t st_uid;
    uint32_t st_gid;
    uint64_t st_rdev;
    uint64_t st_size;
    uint64_t st_blksize;
    uint64_t st_blocks;
    uint8_t st_atime[16];
    uint8_t st_mtime[16];
    uint8_t st_ctime[16];
};

extern int sys_fstat(int fd, struct stat *statbuf);

/** Memory management */

extern void *sys_mmap(void *addr, size_t length, int prot, int flags, int fd, long offset);
extern void sys_munmap(void *addr, size_t length);

/** Returns current break pointer if addr if invalid. */
extern int sys_brk(void *addr);

/* these are defined by POSIX and also present in glibc's dirent.h */
#define DT_UNKNOWN	0
#define DT_FIFO		1
#define DT_CHR		2
#define DT_DIR		4
#define DT_BLK		6
#define DT_REG		8
#define DT_LNK		10
#define DT_SOCK		12
#define DT_WHT		14

/** Returns number of bytes read */
struct linux_dirent {
    unsigned long  d_ino;     /* Inode number */
    unsigned long  d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char           d_name[0]; /* Filename (null-terminated) */
                      /* length is actually (d_reclen - 2 -
                         offsetof(struct linux_dirent, d_name)) */
    /** Hidden two bytes:
     * char pad;
     * char d_type;
     * 
     * Hint: man getdents.
     */
};
extern long sys_getdents(int fd, struct linux_dirent *dirent, unsigned long count);

#endif // _SYS_H_
