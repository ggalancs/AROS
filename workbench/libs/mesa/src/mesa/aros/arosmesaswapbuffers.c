/*
    Copyright 2009-2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "arosmesa_funcs.h"
#include <proto/exec.h>
#include <proto/gallium.h>
#include <gallium/pipe/p_screen.h>
#include <gallium/util/u_inlines.h>

/*****************************************************************************

    NAME */

      AROS_LH1(void, AROSMesaSwapBuffers,

/*  SYNOPSIS */ 
      AROS_LHA(AROSMesaContext, amesa, A0),

/*  LOCATION */
      struct Library *, MesaBase, 9, Mesa)

/*  FUNCTION
        Swaps the back with front buffers. MUST BE used to display the effect
        of rendering onto the target RastPort, since AROSMesa always work in
        double buffer mode.
 
    INPUTS
        amesa - GL rendering context on which swap is to be performed.
 
    RESULT
 
    BUGS

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    SAVE_REG
    
    PUT_MESABASE_IN_REG

    if (amesa->framebuffer->render_surface) 
    {
        /* Flush rendering cache before blitting */
        amesa->st->flush(amesa->st, PIPE_FLUSH_RENDER_CACHE, NULL);

        BltPipeSurfaceRastPort(amesa->framebuffer->render_surface, 0, 0, 
            amesa->visible_rp, amesa->left, amesa->top, 
            amesa->framebuffer->width, amesa->framebuffer->height);
    }

    AROSMesaCheckAndUpdateBufferSize(amesa);

    RESTORE_REG

    AROS_LIBFUNC_EXIT
}


