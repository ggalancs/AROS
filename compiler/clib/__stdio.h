#ifndef ___STDIO_H
#define ___STDIO_H

/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: internal header file for stdio
    Lang: English
*/

#include <stdio.h>
#include <stddef.h>
#include <exec/lists.h>

typedef struct
{
    struct MinNode Node;
    FILE File;
} FILENODE;

#if !defined(_CLIB_KERNEL_) && !defined(_CLIB_LIBRARY_)
extern struct MinList __stdio_files;
#else
#include <libraries/arosc.h>
#endif

#define FILENODE2FILE(fn)       (&((fn)->File))
#define FILE2FILENODE(f)        ((FILENODE *)(((char *)(f))-offsetof(FILENODE,File)))

int __smode2oflags(const char *mode);
int __oflags2sflags(int oflags);

#endif /* ___STDIO_H */
