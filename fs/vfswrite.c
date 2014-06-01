/* CHANGED 5-27-14 */

/* This file is the counterpart of "read.c".  It contains the code for writing
 * insofar as this is not contained in read_write().
 *
 * The entry points into this file are
 *   do_write:     call read_write to perform the WRITE system call
 */

#include "fs.h"
#include "file.h"


/*===========================================================================*
 *				do_write				     *
 *===========================================================================*/
int do_write()
{
/* Perform the write(fd, buffer, nbytes) system call. */
  return(do_read_write(WRITING));
}

/* CHANGE START */
/*===========================================================================*
*				do_metawrite				     *
*===========================================================================*/
int do_meta_read_write(int rw_flag);
int do_metawrite() {
    /* Perform the write(fd, buffer, nbytes) system call. */
    printf("inside vfs::do_metawrite()\n");
    return(do_meta_read_write(WRITING));
}
/* CHANGE END */