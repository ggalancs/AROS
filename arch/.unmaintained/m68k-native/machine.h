#ifndef AROS_MACHINE_H
#define AROS_MACHINE_H
/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$

    NOTE: This file must compile *without* any other header !

    Desc: machine.h include file for Amiga/m68k
    Lang: english
*/

/* Information generated by machine.c */
#define AROS_STACK_GROWS_DOWNWARDS 1 /* Stack direction */
#define AROS_BIG_ENDIAN            1 /* Big or little endian */
#define AROS_SIZEOFULONG           4 /* Size of an ULONG */
#define AROS_WORDALIGN             2 /* Alignment for WORD */
#define AROS_LONGALIGN             2 /* Alignment for LONG */
#define AROS_PTRALIGN              2 /* Alignment for PTR */
#define AROS_IPTRALIGN             2 /* Alignment for IPTR */
#define AROS_DOUBLEALIGN           2 /* Alignment for double */
#define AROS_WORSTALIGN            8 /* Worst case alignment */

/*
    How much do I have to add to sp to get the address of the first
    byte on the stack?
*/
#define SP_OFFSET 0

/*
    BCPL datatypes
*/
#define AROS_BPTR_TYPE	unsigned long
#define AROS_BSTR_TYPE  unsigned long
#define MKBADDR(a)	(((unsigned long)(a))>>2)
#define BADDR(a)	(((unsigned long)(a))<<2)

/*
    One entry in a libraries' jumptable. For assembler compatibility, the
    field jmp should contain the code for an absolute jmp to a 32bit
    address. There are also a couple of macros which you should use to
    access the vector table from C.
*/
struct JumpVec
{
    unsigned short jmp;
    unsigned char vec[4];
};

/* Internal macros */
#define __AROS_ASMJMP			0x4EF9
#define __AROS_SET_VEC(v,a)             (*(ULONG*)(v)->vec=(ULONG)(a))
#define __AROS_GET_VEC(v)               ((APTR)(*(ULONG*)(v)->vec))

/* Use these to acces a vector table */
#define LIB_VECTSIZE			(sizeof (struct JumpVec))
#define __AROS_GETJUMPVEC(lib,n)        ((struct JumpVec *)(((UBYTE *)lib)-(n*LIB_VECTSIZE)))
#define __AROS_GETVECADDR(lib,n)        (__AROS_GET_VEC(__AROS_GETJUMPVEC(lib,n)))
#define __AROS_SETVECADDR(lib,n,addr)   (__AROS_SET_VEC(__AROS_GETJUMPVEC(lib,n),(APTR)(addr)))
#define __AROS_INITVEC(lib,n)           __AROS_GETJUMPVEC(lib,n)->jmp = __AROS_ASMJMP, \
					__AROS_SETVECADDR(lib,n,_aros_not_implemented)

/*
    The following AROS_LVO_CALLs are not protected by #ifdef/#endif pairs.
    This is because these are really the ones to be defined, not the (protected)
    ones from <aros/libcall.h>. Better to have a compiler warning, than to
    define the wrong ones.

    The reason to define them this way (jsr offset(a6)), is to jump over the
    library vector, instead of getting the address from offset+2 and jumping
    to that address. Some libraries may be putting something else (e.g. rts)
    in the vector, instead of a 'jsr address'. Fetching the address from such
    a vector would crash the AROS_LVO_CALLs in <aros/libcall.h>

    Furthermore, this (and the shorter code) is exactly the reason
    AROS/include/clib/ *.h and AROS/include/proto/ *.h containing the
    non-registerized versions have to be removed from the directory tree.
    If they are removed, gcc will fetch the compiler-specific protos, with
    the inlines (in case of gcc).
*/

#define LVO_ARGT(t,v,r) t
#define LVO_ARGV(t,v,r) v
#define LVO_ARGR(t,v,r) r

#define AROS_LVO_CALL0(rt,bt,bn,offs,name) \
({							\
   {							\
      register rt _##name##_re __asm("d0");		\
      register struct Library *const _##name##_bn __asm("a6") = (struct Library*)(bn);\
      __asm volatile ("jsr a6@(-"#offs"*6:W)"		\
      : "=r" (_##name##_re)				\
      : "r" (_##name##_bn)				\
      : "d0", "d1", "a0", "a1", "cc", "memory");	\
      _##name##_re;					\
   }							\
})

#define AROS_LVO_CALL0NR(bt,bn,offs,name)		\
({							\
   {							\
      register struct Library *const _##name##_bn __asm("a6") = (struct Library*)(bn);\
      __asm volatile ("jsr a6@(-"#offs":W)"		\
      : /* no output */					\
      : "r" (_##name##_bn)				\
      : "d0", "d1", "a0", "a1", "cc", "memory");	\
   }							\
})

#define AROS_LVO_CALL1(rt,a1,bt,bn,offs,name) \
({							\
   LVO_ARGT(a1) _##name##_v1 = (LVO_ARGV(a1));	\
   {							\
      register rt _##name##_re __asm("d0");		\
      register struct Library *const _##name##_bn __asm("a6") = (struct Library*)(bn);\
      register LVO_ARGT(a1) _n1 __asm(LVO_ARGR(a1)) = _##name##_v1;	\
      __asm volatile ("jsr a6@(-"#offs"*6:W)"		\
      : "=r" (_##name##_re)				\
      : "r" (_##name##_bn), "r"(_n1)			\
      : "d0", "d1", "a0", "a1", "cc", "memory");	\
      _##name##_re;					\
   }							\
})

#define AROS_LVO_CALL1NR(a1,bt,bn,offs,name) \
({							\
   LVO_ARGT(a1) _##name##_v1 = (LVO_ARGV(a1));	\
   {							\
      register struct Library *const _##name##_bn __asm("a6") = (struct Library*)(bn);\
      register LVO_ARGT(a1) _n1 __asm(LVO_ARGR(a1)) = _##name##_v1;	\
      __asm volatile ("jsr a6@(-"#offs"*6:W)"		\
      : /* no output */                                 \
      : "r" (_##name##_bn), "r"(_n1)			\
      : "d0", "d1", "a0", "a1", "cc", "memory");	\
   }							\
})

#define AROS_LVO_CALL2(rt,a1,a2,bt,bn,offs,name) \
({							\
   LVO_ARGT(a1) _##name##_v1 = (LVO_ARGV(a1));	\
   LVO_ARGT(a2) _##name##_v2 = (LVO_ARGV(a2));	\
   {							\
      register rt _##name##_re __asm("d0");		\
      register struct Library *const _##name##_bn __asm("a6") = (struct Library*)(bn);\
      register LVO_ARGT(a1) _n1 __asm(LVO_ARGR(a1)) = _##name##_v1;	\
      register LVO_ARGT(a2) _n2 __asm(LVO_ARGR(a2)) = _##name##_v2;	\
      __asm volatile ("jsr a6@(-"#offs"*6:W)"		\
      : "=r" (_##name##_re)				\
      : "r" (_##name##_bn), "r"(_n1), "r"(_n2)		\
      : "d0", "d1", "a0", "a1", "cc", "memory");	\
      _##name##_re;					\
   }							\
})

#define AROS_LVO_CALL2NR(a1,a2,bt,bn,offs,name) \
({							\
   LVO_ARGT(a1) _##name##_v1 = (LVO_ARGV(a1));	\
   LVO_ARGT(a2) _##name##_v2 = (LVO_ARGV(a2));	\
   {							\
      register struct Library *const _##name##_bn __asm("a6") = (struct Library*)(bn);\
      register LVO_ARGT(a1) _n1 __asm(LVO_ARGR(a1)) = _##name##_v1;	\
      register LVO_ARGT(a2) _n2 __asm(LVO_ARGR(a2)) = _##name##_v2;	\
      __asm volatile ("jsr a6@(-"#offs"*6:W)"		\
      : /* no output */                                 \
      : "r" (_##name##_bn), "r"(_n1), "r"(_n2)		\
      : "d0", "d1", "a0", "a1", "cc", "memory");	\
   }							\
})

#define AROS_LVO_CALL3(rt,a1,a2,a3,bt,bn,offs,name) \
({							\
   LVO_ARGT(a1) _##name##_v1 = (LVO_ARGV(a1));	\
   LVO_ARGT(a2) _##name##_v2 = (LVO_ARGV(a2));	\
   LVO_ARGT(a3) _##name##_v3 = (LVO_ARGV(a3));	\
   {							\
      register rt _##name##_re __asm("d0");		\
      register struct Library *const _##name##_bn __asm("a6") = (struct Library*)(bn);\
      register LVO_ARGT(a1) _n1 __asm(LVO_ARGR(a1)) = _##name##_v1;	\
      register LVO_ARGT(a2) _n2 __asm(LVO_ARGR(a2)) = _##name##_v2;	\
      register LVO_ARGT(a3) _n3 __asm(LVO_ARGR(a3)) = _##name##_v3;	\
      __asm volatile ("jsr a6@(-"#offs"*6:W)"		\
      : "=r" (_##name##_re)				\
      : "r" (_##name##_bn), "r"(_n1), "r"(_n2), "r"(_n3)\
      : "d0", "d1", "a0", "a1", "cc", "memory");	\
      _##name##_re;					\
   }							\
})

#define AROS_LVO_CALL3NR(a1,a2,a3,bt,bn,offs,name) \
({							\
   LVO_ARGT(a1) _##name##_v1 = (LVO_ARGV(a1));	\
   LVO_ARGT(a2) _##name##_v2 = (LVO_ARGV(a2));	\
   LVO_ARGT(a3) _##name##_v3 = (LVO_ARGV(a3));	\
   {							\
      register struct Library *const _##name##_bn __asm("a6") = (struct Library*)(bn);\
      register LVO_ARGT(a1) _n1 __asm(LVO_ARGR(a1)) = _##name##_v1;	\
      register LVO_ARGT(a2) _n2 __asm(LVO_ARGR(a2)) = _##name##_v2;	\
      register LVO_ARGT(a3) _n3 __asm(LVO_ARGR(a3)) = _##name##_v3;	\
      __asm volatile ("jsr a6@(-"#offs"*6:W)"		\
      : /* no output */                                 \
      : "r" (_##name##_bn), "r"(_n1), "r"(_n2), "r"(_n3)\
      : "d0", "d1", "a0", "a1", "cc", "memory");	\
   }							\
})

#define AROS_LVO_CALL4(rt,a1,a2,a3,a4,bt,bn,offs,name) \
({							\
   LVO_ARGT(a1) _##name##_v1 = (LVO_ARGV(a1));	\
   LVO_ARGT(a2) _##name##_v2 = (LVO_ARGV(a2));	\
   LVO_ARGT(a3) _##name##_v3 = (LVO_ARGV(a3));	\
   LVO_ARGT(a4) _##name##_v4 = (LVO_ARGV(a4));	\
   {							\
      register rt _##name##_re __asm("d0");		\
      register struct Library *const _##name##_bn __asm("a6") = (struct Library*)(bn);\
      register LVO_ARGT(a1) _n1 __asm(LVO_ARGR(a1)) = _##name##_v1;	\
      register LVO_ARGT(a2) _n2 __asm(LVO_ARGR(a2)) = _##name##_v2;	\
      register LVO_ARGT(a3) _n3 __asm(LVO_ARGR(a3)) = _##name##_v3;	\
      register LVO_ARGT(a4) _n4 __asm(LVO_ARGR(a4)) = _##name##_v4;	\
      __asm volatile ("jsr a6@(-"#offs"*6:W)"		\
      : "=r" (_##name##_re)				\
      : "r" (_##name##_bn), "r"(_n1), "r"(_n2), "r"(_n3), "r"(_n4) \
      : "d0", "d1", "a0", "a1", "cc", "memory");	\
      _##name##_re;					\
   }							\
})

#define AROS_LVO_CALL4NR(a1,a2,a3,a4,bt,bn,offs,name) \
({							\
   LVO_ARGT(a1) _##name##_v1 = (LVO_ARGV(a1));	\
   LVO_ARGT(a2) _##name##_v2 = (LVO_ARGV(a2));	\
   LVO_ARGT(a3) _##name##_v3 = (LVO_ARGV(a3));	\
   LVO_ARGT(a4) _##name##_v4 = (LVO_ARGV(a4));	\
   {							\
      register struct Library *const _##name##_bn __asm("a6") = (struct Library*)(bn);\
      register LVO_ARGT(a1) _n1 __asm(LVO_ARGR(a1)) = _##name##_v1;	\
      register LVO_ARGT(a2) _n2 __asm(LVO_ARGR(a2)) = _##name##_v2;	\
      register LVO_ARGT(a3) _n3 __asm(LVO_ARGR(a3)) = _##name##_v3;	\
      register LVO_ARGT(a4) _n4 __asm(LVO_ARGR(a4)) = _##name##_v4;	\
      __asm volatile ("jsr a6@(-"#offs"*6:W)"		\
      : /* no output */                                 \
      : "r" (_##name##_bn), "r"(_n1), "r"(_n2), "r"(_n3), "r"(_n4) \
      : "d0", "d1", "a0", "a1", "cc", "memory");	\
   }							\
})

#define AROS_LVO_CALL5(rt,a1,a2,a3,a4,a5,bt,bn,offs,name) \
({							\
   LVO_ARGT(a1) _##name##_v1 = (LVO_ARGV(a1));	\
   LVO_ARGT(a2) _##name##_v2 = (LVO_ARGV(a2));	\
   LVO_ARGT(a3) _##name##_v3 = (LVO_ARGV(a3));	\
   LVO_ARGT(a4) _##name##_v4 = (LVO_ARGV(a4));	\
   LVO_ARGT(a5) _##name##_v5 = (LVO_ARGV(a5));	\
   {							\
      register rt _##name##_re __asm("d0");		\
      register struct Library *const _##name##_bn __asm("a6") = (struct Library*)(bn);\
      register LVO_ARGT(a1) _n1 __asm(LVO_ARGR(a1)) = _##name##_v1;	\
      register LVO_ARGT(a2) _n2 __asm(LVO_ARGR(a2)) = _##name##_v2;	\
      register LVO_ARGT(a3) _n3 __asm(LVO_ARGR(a3)) = _##name##_v3;	\
      register LVO_ARGT(a4) _n4 __asm(LVO_ARGR(a4)) = _##name##_v4;	\
      register LVO_ARGT(a5) _n5 __asm(LVO_ARGR(a5)) = _##name##_v5;	\
      __asm volatile ("jsr a6@(-"#offs"*6:W)"		\
      : "=r" (_##name##_re)				\
      : "r" (_##name##_bn), "r"(_n1), "r"(_n2), "r"(_n3), "r"(_n4), "r"(_n5) \
      : "d0", "d1", "a0", "a1", "cc", "memory");	\
      _##name##_re;					\
   }							\
})

#define AROS_LVO_CALL5NR(a1,a2,a3,a4,a5,bt,bn,offs,name) \
({							\
   LVO_ARGT(a1) _##name##_v1 = (LVO_ARGV(a1));	\
   LVO_ARGT(a2) _##name##_v2 = (LVO_ARGV(a2));	\
   LVO_ARGT(a3) _##name##_v3 = (LVO_ARGV(a3));	\
   LVO_ARGT(a4) _##name##_v4 = (LVO_ARGV(a4));	\
   LVO_ARGT(a5) _##name##_v5 = (LVO_ARGV(a5));	\
   {							\
      register struct Library *const _##name##_bn __asm("a6") = (struct Library*)(bn);\
      register LVO_ARGT(a1) _n1 __asm(LVO_ARGR(a1)) = _##name##_v1;	\
      register LVO_ARGT(a2) _n2 __asm(LVO_ARGR(a2)) = _##name##_v2;	\
      register LVO_ARGT(a3) _n3 __asm(LVO_ARGR(a3)) = _##name##_v3;	\
      register LVO_ARGT(a4) _n4 __asm(LVO_ARGR(a4)) = _##name##_v4;	\
      register LVO_ARGT(a5) _n5 __asm(LVO_ARGR(a5)) = _##name##_v5;	\
      __asm volatile ("jsr a6@(-"#offs"*6:W)"		\
      : /* no output */                                 \
      : "r" (_##name##_bn), "r"(_n1), "r"(_n2), "r"(_n3), "r"(_n4), "r"(_n5) \
      : "d0", "d1", "a0", "a1", "cc", "memory");	\
   }							\
})

/* ??? */
#define RDFCALL(hook,data,dptr) ((void(*)(UBYTE,APTR))(hook))(data,dptr);

/*
    Find the next valid alignment for a structure if the next x bytes must
    be skipped.
*/
#define AROS_ALIGN(x)        (((x)+AROS_WORSTALIGN-1)&-AROS_WORSTALIGN)

/* Prototypes */
extern void _aros_not_implemented (void);

/* How much stack do we need ? Lots :-) */
#define AROS_STACKSIZE	100000

/* The registers */
#define D0 "d0"
#define D1 "d1"
#define D2 "d2"
#define D3 "d3"
#define D4 "d4"
#define D5 "d5"
#define D6 "d6"
#define D7 "d7"
#define A0 "a0"
#define A1 "a1"
#define A2 "a2"
#define A3 "a3"
#define A4 "a4"
#define A5 "a5"
#define A6 "a6"

/* SAS/C compatible declarations for register variables */
#ifndef __SASC
#define __d0 __asm("d0")
#define __d1 __asm("d1")
#define __d2 __asm("d2")
#define __d3 __asm("d3")
#define __d4 __asm("d4")
#define __d5 __asm("d5")
#define __d6 __asm("d6")
#define __d7 __asm("d7")

#define __a0 __asm("a0")
#define __a1 __asm("a1")
#define __a2 __asm("a2")
#define __a3 __asm("a3")
#define __a4 __asm("a4")
#define __a5 __asm("a5")
#define __a6 __asm("a6")
#endif

/* What to do with the library base in header, prototype and call */
#define __AROS_LH_BASE(basetype,basename)   basetype basename __asm("a6")
#define __AROS_LP_BASE(basetype,basename)   void * __asm("a6")
#define __AROS_LC_BASE(basetype,basename)   basename

/* How to transform an argument in header, prototype and call */
#define __AROS_LHA(type,name,reg)     type name __asm(reg)
#define __AROS_LPA(type,name,reg)     type __asm(reg)
#define __AROS_LCA(type,name,reg)     name
#define __AROS_UFHA(type,name,reg)    type name __asm(reg)
#define __AROS_UFPA(type,name,reg)    type __asm(reg)
#define __AROS_UFCA(type,name,reg)    name

/* Prefix for library function in header, prototype and call */
#define __AROS_LH_PREFIX    /* eps */
#define __AROS_LP_PREFIX    /* eps */
#define __AROS_LC_PREFIX    /* eps */
#define __AROS_UFH_PREFIX   /* eps */
#define __AROS_UFP_PREFIX   /* eps */
#define __AROS_UFC_PREFIX   /* eps */

/* if this is defined, all AROS_LP*-macros will expand to nothing. */
#define __AROS_USE_MACROS_FOR_LIBCALL

#endif /* AROS_MACHINE_H */
