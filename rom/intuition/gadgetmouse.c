/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Intuition function GadgetMouse()
    Lang: english
*/
#include "intuition_intern.h"
#include "inputhandler_support.h"

/*****************************************************************************

    NAME */
#include <proto/intuition.h>

	AROS_LH3(void, GadgetMouse,

/*  SYNOPSIS */
	AROS_LHA(struct Gadget     *, gadget, A0),
	AROS_LHA(struct GadgetInfo *, ginfo, A1),
	AROS_LHA(WORD              *, mousepoint, A2),

/*  LOCATION */
	struct IntuitionBase *, IntuitionBase, 95, Intuition)

/*  FUNCTION
	Determines the current mouse position relative to the upper-left
	corner of a cusrom gadget.
	It is recommended not to call this function!

    INPUTS
	gadget - The gadget to take as origin
	ginfo - The GadgetInfo structure as passed to the custom gadget hook routine
	mousepoint - Pointer to an array of two WORDs or a structure of type Point

    RESULT
	None. Fills in the two WORDs pointed to by mousepoint.

    NOTES
	This function is useless, because programs which need this information
	can get it in a cleaner way.
	It is recommended not to call this function!

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct IntuitionBase *,IntuitionBase)

    struct gpInput dummy_gpi;
    
    ASSERT_VALID_PTR(gadget);
    ASSERT_VALID_PTR(ginfo);
    ASSERT_VALID_PTR(mousepoint);
    
    dummy_gpi.gpi_GInfo = ginfo;
    SetGPIMouseCoords(&dummy_gpi, gadget);

    mousepoint[0] = dummy_gpi.gpi_Mouse.X;
    mousepoint[1] = dummy_gpi.gpi_Mouse.Y;
    
    AROS_LIBFUNC_EXIT
    
} /* GadgetMouse */
