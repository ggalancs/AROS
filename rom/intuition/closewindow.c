/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$
    $Log$
    Revision 1.1  1996/08/13 15:37:26  digulla
    First function for intuition.library


    Desc:
    Lang: english
*/
#include "intuition_intern.h"

extern void intui_CloseWindow (struct Window *, struct IntuitionBase *);

/*****************************************************************************

    NAME */
	#include <clib/intuition_protos.h>

	__AROS_LH1(void, CloseWindow,

/*  SYNOPSIS */
	__AROS_LHA(struct Window *, window, A0),

/*  LOCATION */
	struct IntuitionBase *, IntuitionBase, 12, Intuition)

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
			    intuition_lib.fd and clib/intuition_protos.h

*****************************************************************************/
{
    __AROS_FUNC_INIT
    __AROS_BASE_EXT_DECL(struct IntuitionBase *,IntuitionBase)

    intui_CloseWindow (window, IntuitionBase);

    __AROS_FUNC_EXIT
} /* CloseWindow */
