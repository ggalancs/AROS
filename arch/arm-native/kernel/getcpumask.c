/*
    Copyright � 2015, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
*/

#include <aros/kernel.h>
#include <aros/libcall.h>

#include <kernel_base.h>

AROS_LH1(uint32_t, KrnGetCPUMask,
	AROS_LHA(uint32_t, id, D0),
	struct KernelBase *, KernelBase, 38, Kernel)
{
    AROS_LIBFUNC_INIT

    return (1 << id);

    AROS_LIBFUNC_EXIT
}
