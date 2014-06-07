/* CHANGED 5-27-14 */

#include <sys/cdefs.h>
#include <lib.h>
#include "namespace.h"

#include <unistd.h>

#ifdef __weak_alias
__weak_alias(write, _write)
#endif

ssize_t write(int fd, const void *buffer, size_t nbytes)
{
  message m;

  m.m1_i1 = fd;
  m.m1_i2 = nbytes;
  m.m1_p1 = (char *) __UNCONST(buffer);
  return(_syscall(VFS_PROC_NR, WRITE, &m));
}

/* CHANGE START */
/*  metawrite */
/*
 *  This function compiles a message to send to VFS for
 *  a metawrite system call.
 *
 *  @param  fd      File descriptor for the inode to write
 *          buffer  Buffer containing the data that was
 *                  requested
 *          nbytes  Number of bytes to write.
 *  @return number of bytes written if successful.
 *          -1 if there was an error.
 *
 */
ssize_t metawrite(int fd, const void *buffer, size_t nbytes) {
  message m;

  m.m1_i1 = fd;
  m.m1_i2 = nbytes;
  m.m1_p1 = (char *)__UNCONST(buffer);
  return(_syscall(VFS_PROC_NR, METAWRITE, &m));
}
/* CHANGE END */
