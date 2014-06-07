/* CHANGED 5-27-14 */

#include <sys/cdefs.h>
#include <lib.h>
#include "namespace.h"

#include <unistd.h>

#ifdef __weak_alias
__weak_alias(read, _read)
#endif

ssize_t read(int fd, void *buffer, size_t nbytes)
{
  message m;

  m.m1_i1 = fd;
  m.m1_i2 = nbytes;
  m.m1_p1 = (char *) buffer;
  return(_syscall(VFS_PROC_NR, READ, &m));
}

/* CHANGE START */
/*  metaread */
/*  
 *  This function compiles a message to send to VFS for
 *  a metaread system call.
 *
 *  @param  fd      File descriptor for the inode to read
 *          buffer  Buffer to read data into on the top level
 *          nbytes  Number of bytes to read.
 *  @return number of bytes read if successful.
 *          -1 if there was an error.
 *
 */
ssize_t metaread(int fd, void *buffer, size_t nbytes) {
  message m;

  m.m1_i1 = fd;
  m.m1_i2 = nbytes;
  m.m1_p1 = (char *)buffer;
  return(_syscall(VFS_PROC_NR, METAREAD, &m));
}
/* CHANGE END */