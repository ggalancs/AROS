/*
    (C) 1995-97 AROS - The Amiga Replacement OS
    $Id$

    Desc: dos.library function Examine().
    Lang: english
*/
#include <exec/memory.h>
#include <proto/exec.h>
#include <dos/filesystem.h>
#include <dos/exall.h>
#include "dos_intern.h"

/*****************************************************************************

    NAME */
#include <proto/dos.h>

	AROS_LH2(BOOL, Examine,

/*  SYNOPSIS */
	AROS_LHA(BPTR,                   lock, D1),
	AROS_LHA(struct FileInfoBlock *, fib,  D2),

/*  LOCATION */
	struct DosLibrary *, DOSBase, 17, Dos)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	29-10-95    digulla automatically created from
			    dos_lib.fd and clib/dos_protos.h

*****************************************************************************/

/*****************************************************************************

    NAME
#include <clib/dos_protos.h>

	AROS_LH2(BOOL, ExamineFH,

    SYNOPSIS
	AROS_LHA(BPTR                  , fh, D1),
	AROS_LHA(struct FileInfoBlock *, fib, D2),

    LOCATION
	struct DosLibrary *, DOSBase, 65, Dos)

    FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	29-10-95    digulla automatically created from
			    dos_lib.fd and clib/dos_protos.h

*****************************************************************************/
/*AROS alias ExamineFH Examine */
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct DosLibrary *,DOSBase)

    UBYTE buffer[512];
    struct ExAllData *ead=(struct ExAllData *)buffer;
    STRPTR src, dst;
    ULONG i;

    /* Get pointer to filehandle */
    struct FileHandle *fh=(struct FileHandle *)BADDR(lock);

    /* Get pointer to process structure */
    struct Process *me=(struct Process *)FindTask(NULL);

    /* Get pointer to I/O request. Use stackspace for now. */
    struct IOFileSys io,*iofs=&io;

    /* Prepare I/O request. */
    iofs->IOFS.io_Message.mn_Node.ln_Type=NT_REPLYMSG;
    iofs->IOFS.io_Message.mn_ReplyPort	 =&me->pr_MsgPort;
    iofs->IOFS.io_Message.mn_Length	 =sizeof(struct IOFileSys);
    iofs->IOFS.io_Device =fh->fh_Device;
    iofs->IOFS.io_Unit	 =fh->fh_Unit;
    iofs->IOFS.io_Command=FSA_EXAMINE;
    iofs->IOFS.io_Flags  =0;
    iofs->io_Union.io_EXAMINE.io_ead = (struct ExAllData *)buffer;
    iofs->io_Union.io_EXAMINE.io_Size = sizeof(buffer);
    iofs->io_Union.io_EXAMINE.io_Mode = ED_OWNER;

    /* Send the request. */
    DoIO(&iofs->IOFS);

    /* Set error code and return */
    SetIoErr(iofs->io_DosError);
    if(iofs->io_DosError)
	return DOSFALSE;
    else
    {
        /* in fib_DiskKey the result from telldir is being stored which
           gives us important info for a call to ExNext() */
	fib->fib_DiskKey	=iofs->io_DirPos;
	fib->fib_DirEntryType	=ead->ed_Type;

	src=ead->ed_Name;
	dst=fib->fib_FileName;
	if(src!=NULL)
	    for(i=0;i<MAXFILENAMELENGTH-1;i++)
		if(!(*dst++=*src++))
		    break;
	*dst++=0;

	fib->fib_Protection	=ead->ed_Prot;
	fib->fib_EntryType	=ead->ed_Type;
	fib->fib_Size		=ead->ed_Size;
	fib->fib_Date.ds_Days	=ead->ed_Days;
	fib->fib_Date.ds_Minute	=ead->ed_Mins;
	fib->fib_Date.ds_Tick	=ead->ed_Ticks;

	src=ead->ed_Comment;
	dst=fib->fib_Comment;
	if(src!=NULL)
	    for(i=0;i<79;i++)
		if(!(*dst++=*src++))
		    break;
	*dst++=0;
	fib->fib_OwnerUID	=ead->ed_OwnerUID;
	fib->fib_OwnerGID	=ead->ed_OwnerGID;
	return DOSTRUE;
    }

    AROS_LIBFUNC_EXIT
} /* Examine */
