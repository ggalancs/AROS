/*
    (C) 1999 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: English
*/
#include "datatypes_intern.h"
#include <proto/utility.h>

/*****************************************************************************

    NAME */

        AROS_LH3(struct DTMethod *, FindTriggerMethod,

/*  SYNOPSIS */
	AROS_LHA(struct DTMethod *, methods, A0),
	AROS_LHA(STRPTR           , command, A1),
	AROS_LHA(ULONG            , method , D0),

/*  LOCATION */
	struct Library *, DTBase, 44, DataTypes)

/*  FUNCTION

    Search for a specific trigger method in a array of trigger methods (check
    if either 'command' or 'method' matches).

    INPUTS

    methods  --  array of trigger methods; may be NULL
    command  --  name of trigger method (may be NULL; if so, 'command'
                        is not matched against)
    method   --  id of trigger method to search for (may be ~0; if so, don't
                 match against 'method'.

    RESULT

    Pointer to trigger method table entry (struct DTMethod *) or NULL if the
    method wasn't found.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    GetDTTriggerMethods(), CopyDTTriggerMethods()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    if(methods == NULL)
	return NULL;

    while(methods->dtm_Method != STM_DONE)
    {
	if(command != NULL)
	{
	    if(Stricmp(methods->dtm_Command, command) != 0)
	    {
		methods++;
		break;
	    }
	    
	    return methods;
	}

	if(method != ~0UL)
	{
	    if(methods->dtm_Method != method)
	    {
		methods++;
		break;
	    }
	}

	return methods;
    }

    return NULL;

    AROS_LIBFUNC_EXIT
} /* FindTriggerMethod */

