/*
    (C) 1997 AROS - The Amiga Replacement OS
    $Id$

    Desc:
    Lang: english
*/
#include "expansion_intern.h"

/*****************************************************************************

    NAME */
#include <clib/expansion_protos.h>

	AROS_LH2(void, ConfigBoard,

/*  SYNOPSIS */
	AROS_LHA(APTR              , board, A0),
	AROS_LHA(struct ConfigDev *, configDev, A1),

/*  LOCATION */
	struct ExpansionBase *, ExpansionBase, 10, Expansion)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    expansion_lib.fd and clib/expansion_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct ExpansionBase *,ExpansionBase)

#warning TODO: Write expansion/ConfigBoard()
    aros_print_not_implemented ("ConfigBoard");

    AROS_LIBFUNC_EXIT
} /* ConfigBoard */
