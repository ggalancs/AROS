/*
    Copyright � 1995-2012, The AROS Development Team. All rights reserved.
    $Id$

    C99 function remove().
*/

#include <proto/dos.h>
#include <errno.h>
#include "__upath.h"

/*****************************************************************************

    NAME */
#include <stdio.h>

	int remove (

/*  SYNOPSIS */
	const char * pathname)

/*  FUNCTION
	Deletes a file or directory.

    INPUTS
	pathname - Complete path to the file or directory.

    RESULT
	0 on success and -1 on error. In case of an error, errno is set.
	
    NOTES
    	Identical to unlink

    EXAMPLE

    BUGS

    SEE ALSO
    	unlink()

    INTERNALS

******************************************************************************/
{
    if (!DeleteFile (__path_u2a(pathname)))
    {
	errno = __stdc_ioerr2errno (IoErr());
	return -1;
    }

    return 0;
} /* remove */

