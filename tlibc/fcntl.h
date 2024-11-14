// source: <https://elixir.bootlin.com/glibc/glibc-2.40.9000/source/bits/fcntl.h#L25>
#ifndef __FCNTL_H_ 
#define __FCNTL_H_ 

/* File access modes for `open' and `fcntl'.  */
#define	O_RDONLY	0	/* Open read-only.  */
#define	O_WRONLY	1	/* Open write-only.  */
#define	O_RDWR		2	/* Open read/write.  */
#define O_CREAT 64
#define O_TRUNC 512


/* Bits OR'd into the second argument to open.  */
#define	O_EXCL		0x0800	/* Fail if file already exists.  */
#define	O_NOCTTY	0x8000	/* Don't assign a controlling terminal.  */
#define	O_ASYNC		0x0040	/* Send SIGIO to owner when data is ready.  */
#define	O_FSYNC		0x0080	/* Synchronous writes.  */
#define	O_SYNC		O_FSYNC
#ifdef	__USE_MISC
#define	O_SHLOCK	0x0010	/* Open with shared file lock.  */
#define	O_EXLOCK	0x0020	/* Open with shared exclusive lock.  */
#endif
#ifdef __USE_XOPEN2K8
# define O_DIRECTORY	0x00200000	/* Must be a directory.	 */
# define O_NOFOLLOW	0x00000100	/* Do not follow links.	 */
# define O_CLOEXEC	0x00400000      /* Set close_on_exec.  */
#endif
#if defined __USE_POSIX199309 || defined __USE_UNIX98
# define O_DSYNC	0x00010000	/* Synchronize data.  */
# define O_RSYNC	0x00020000	/* Synchronize read operations.	 */
#endif

/* All opens support large file sizes, so there is no flag bit for this.  */
#ifdef __USE_LARGEFILE64
# define O_LARGEFILE	0
#endif

/* File status flags for `open' and `fcntl'.  */
#define	O_APPEND	0x0008	/* Writes append to the file.  */
#define	O_NONBLOCK	0x0004	/* Non-blocking I/O.  */

#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

#define O_DSYNC		040000	/* used to be O_SYNC, see below */
#define O_DIRECTORY	0100000	/* must be a directory */
#define O_NOFOLLOW	0200000 /* don't follow links */
#define O_LARGEFILE	0400000 /* will be set by the kernel on every open */
#define O_DIRECT	02000000 /* direct disk access - should check with OSF/1 */
#define O_NOATIME	04000000
#define O_CLOEXEC	010000000 /* set close_on_exec */

#endif // __FCNTL_H_ 
