/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$

    Desc: ANSI C function fscanf()
    Lang: english
*/
#include <stdarg.h>

/*****************************************************************************

    NAME */
#include <stdio.h>

	int fscanf (

/*  SYNOPSIS */
	FILE	   * fh,
	const char * format,
	...)

/*  FUNCTION
	Scan a string with the specified arguments and write the results
	in the specified parameters.

    INPUTS
	fh - Read from this stream
	format - How to convert the input into the arguments
	... - Write the result in these arguments

    RESULT
	The number of converted arguments.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	scanf()

    INTERNALS

    HISTORY
	10.12.1996 digulla created

******************************************************************************/
{
    int     retval;
    va_list args;

    va_start (args, format);

    retval = vfscanf (fh, format, args);

    va_end (args);

    return retval;
} /* fscanf */

