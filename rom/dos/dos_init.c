/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$
    $Log$
    Revision 1.10  1997/01/27 00:36:17  ldp
    Polish

    Revision 1.9  1996/10/24 15:50:26  aros
    Use the official AROS macros over the __AROS versions.

    Revision 1.8  1996/10/23 14:23:06  aros
    Use LIB_VECTSIZE over sizeof(struct JumpVec)

    Revision 1.7  1996/09/17 18:40:21  digulla
    Only one global DOSBase to avoid multiple opens without close
    InitSemaphore() needs SysBase

    Revision 1.6  1996/09/13 17:50:06  digulla
    Use IPTR

    Revision 1.5  1996/09/12 14:52:03  digulla
    Use correct way to access external names (was missing)

    Revision 1.4  1996/09/11 16:54:23  digulla
    Always use AROS_SLIB_ENTRY() to access shared external symbols, because
	some systems name an external symbol "x" as "_x" and others as "x".
	(The problem arises with assembler symbols which might differ)

    Revision 1.3  1996/08/13 13:52:53  digulla
    Replaced <dos/dosextens.h> by "dos_intern.h" or added "dos_intern.h"
    Replaced AROS_LA by AROS_LHA

    Revision 1.2  1996/08/01 17:40:49  digulla
    Added standard header for all files

    Desc:
    Lang:
*/
#include <exec/types.h>
#include <exec/resident.h>
#include <exec/execbase.h>
#include <exec/libraries.h>
#include <proto/exec.h>
#include <aros/libcall.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <proto/dos.h>
#include <utility/tagitem.h>
#include "dos_intern.h"

static const char name[];
static const char version[];
static const APTR Dos_inittabl[4];
static void *const Dos_functable[];
struct DosLibrary *AROS_SLIB_ENTRY(init,Dos) ();
extern const char Dos_end;

int Dos_entry(void)
{
    /* If the library was executed by accident return error code. */
    return -1;
}

const struct Resident Dos_resident=
{
    RTC_MATCHWORD,
    (struct Resident *)&Dos_resident,
    (APTR)&Dos_end,
    RTF_AUTOINIT,
    39,
    NT_LIBRARY,
    0,
    (char *)name,
    (char *)&version[6],
    (ULONG *)Dos_inittabl
};

static const char name[]="dos.library";

static const char version[]="$VER: dos 39.0 (28.3.96)\n\015";

static const APTR Dos_inittabl[4]=
{
    (APTR)sizeof(struct DosLibrary),
    (APTR)Dos_functable,
    NULL,
    &AROS_SLIB_ENTRY(init,Dos)
};

void LDDemon();
void AROS_SLIB_ENTRY(OpenLibrary,Dos)();
void AROS_SLIB_ENTRY(OpenDevice,Dos)();
void AROS_SLIB_ENTRY(CloseLibrary,Dos)();
void AROS_SLIB_ENTRY(CloseDevice,Dos)();
void AROS_SLIB_ENTRY(RemLibrary,Dos)();
void LDFlush();

#undef SysBase

AROS_LH2(struct DosLibrary *, init,
 AROS_LHA(struct DosLibrary *, DOSBase, D0),
 AROS_LHA(BPTR,               segList,   A0),
	   struct ExecBase *, SysBase, 0, Dos)
{
    AROS_LIBFUNC_INIT
    /* This function is single-threaded by exec by calling Forbid. */

    /* Store arguments */
    DOSBase->dl_SysBase=SysBase;
    DOSBase->dl_SegList=segList;

    InitSemaphore(&DOSBase->dl_DosListLock);
    InitSemaphore(&DOSBase->dl_LDSigSem);

    DOSBase->dl_UtilityBase=OpenLibrary("utility.library",39);
    if(DOSBase->dl_UtilityBase!=NULL)
    {
	static const struct TagItem tags[]=
	{
	    { NP_Entry, (IPTR)LDDemon },
	    { NP_Input, 0 },
	    { NP_Output, 0 },
	    { NP_Name, (IPTR)"lib & dev loader demon" },
	    { TAG_END, 0 }
	};

	DOSBase->dl_LDDemon=CreateNewProc((struct TagItem *)tags);

	if(DOSBase->dl_LDDemon!=NULL)
	{
	    (void)SetFunction(&SysBase->LibNode,-92*LIB_VECTSIZE,AROS_SLIB_ENTRY(OpenLibrary,Dos));
	    (void)SetFunction(&SysBase->LibNode,-74*LIB_VECTSIZE,AROS_SLIB_ENTRY(OpenDevice,Dos));
	    (void)SetFunction(&SysBase->LibNode,-69*LIB_VECTSIZE,AROS_SLIB_ENTRY(CloseLibrary,Dos));
	    (void)SetFunction(&SysBase->LibNode,-75*LIB_VECTSIZE,AROS_SLIB_ENTRY(CloseDevice,Dos));
	    (void)SetFunction(&SysBase->LibNode,-67*LIB_VECTSIZE,AROS_SLIB_ENTRY(RemLibrary,Dos));
	    (void)SetFunction(&SysBase->LibNode,-73*LIB_VECTSIZE,AROS_SLIB_ENTRY(RemLibrary,Dos));

	    DOSBase->dl_LDHandler.is_Node.ln_Name="lib & dev loader demon";
	    DOSBase->dl_LDHandler.is_Node.ln_Pri=0;
	    DOSBase->dl_LDHandler.is_Code=LDFlush;

	    AddMemHandler(&DOSBase->dl_LDHandler);

	    return DOSBase;
	}

	CloseLibrary(DOSBase->dl_UtilityBase);
    }

    return NULL;
    AROS_LIBFUNC_EXIT
}

#define SysBase     (DOSBase->dl_SysBase)

AROS_LH1(struct DosLibrary *, open,
 AROS_LHA(ULONG, version, D0),
	   struct DosLibrary *, DOSBase, 1, Dos)
{
    AROS_LIBFUNC_INIT
    /*
	This function is single-threaded by exec by calling Forbid.
	If you break the Forbid() another task may enter this function
	at the same time. Take care.
    */

    /* Keep compiler happy */
    version=0;

    /* I have one more opener. */
    DOSBase->dl_lib.lib_OpenCnt++;
    DOSBase->dl_lib.lib_Flags&=~LIBF_DELEXP;

    /* You would return NULL if the open failed. */
    return DOSBase;
    AROS_LIBFUNC_EXIT
}

AROS_LH0(BPTR, close,
	   struct DosLibrary *, DOSBase, 2, Dos)
{
    AROS_LIBFUNC_INIT
    /*
	This function is single-threaded by exec by calling Forbid.
	If you break the Forbid() another task may enter this function
	at the same time. Take care.
    */

    /* I have one fewer opener. */
    if(!--DOSBase->dl_lib.lib_OpenCnt)
    {
	/* Delayed expunge pending? */
	if(DOSBase->dl_lib.lib_Flags&LIBF_DELEXP)
	    /* Then expunge the library */
	    return expunge();
    }
    return 0;
    AROS_LIBFUNC_EXIT
}

AROS_LH0(BPTR, expunge,
	   struct DosLibrary *, DOSBase, 3, Dos)
{
    AROS_LIBFUNC_INIT

    BPTR ret;
    /*
	This function is single-threaded by exec by calling Forbid.
	Never break the Forbid() or strange things might happen.
    */

    /* Test for openers. */
    if(DOSBase->dl_lib.lib_OpenCnt)
    {
	/* Set the delayed expunge flag and return. */
	DOSBase->dl_lib.lib_Flags|=LIBF_DELEXP;
	return 0;
    }

    /* Get rid of the library. Remove it from the list. */
    Remove(&DOSBase->dl_lib.lib_Node);

    /* Get returncode here - FreeMem() will destroy the field. */
    ret=DOSBase->dl_SegList;

    /* Free the memory. */
    FreeMem((char *)DOSBase-DOSBase->dl_lib.lib_NegSize,
	    DOSBase->dl_lib.lib_NegSize+DOSBase->dl_lib.lib_PosSize);

    return ret;
    AROS_LIBFUNC_EXIT
}

AROS_LH0I(int, null,
	    struct DosLibrary *, DOSBase, 4, Dos)
{
    AROS_LIBFUNC_INIT
    return 0;
    AROS_LIBFUNC_EXIT
}
