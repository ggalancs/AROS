
/*
    (C) 1995-99 AROS - The Amiga Research OS
    $Id$

    Desc: Draw the list of gels
    Lang: english
*/
#include "graphics_intern.h"

/*****************************************************************************

    NAME */
	#include <clib/graphics_protos.h>

	AROS_LH2(void, DrawGList,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp, A1),
	AROS_LHA(struct ViewPort *, vp, A0),

/*  LOCATION */
	struct GfxBase *, GfxBase, 19, Graphics)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct GfxBase *,GfxBase)

    struct VSprite * CurVSprite = rp->GelsInfo->gelHead;
    struct VSprite * AfterPathVSprite = NULL;
    struct BitMap bm;
    ULONG i;
    int followdrawpath;

#ifndef BLTBITMAPRASTPORT_EXISTS
    struct RastPort rp_bm;
    InitRastPort(&rp_bm);
    rp_bm.BitMap = &bm;
#endif

    /*
     * CurVSprite is not a valid VSprite but the "boundary vsprite"
     * Should I follow the DrawPath?
     */ 
    if (NULL != CurVSprite->DrawPath)
    {
      followdrawpath = TRUE;
      AfterPathVSprite = CurVSprite->NextVSprite;
      CurVSprite = CurVSprite->DrawPath;
    }
    else
    {
      followdrawpath = FALSE;
      CurVSprite = CurVSprite->NextVSprite;
    }
    
    /*
     * As long as I don't step on the last boundary vsprite
     */
    while (NULL != CurVSprite->NextVSprite)
    {
      /*
       * Check a few flags that must not be set.
       * The Bob should not be out of the rasrport or already be drawn
       */
      if ( 0 == (CurVSprite->Flags & (GELGONE)) ||
          (NULL != (CurVSprite->VSBob) && 0 == (CurVSprite->VSBob->Flags & BDRAWN)))	
      {
	UBYTE * imagedata = (UBYTE *)CurVSprite->ImageData;
	/*
	 * If this VSprite was overlapping with other VSprites
	 * the follow the ClearPath first and clear all the
	 * VSprites on that path. That routine will also reset
	 * the ClearPath variable on all encountered VSprites.
	 * If it was not overlapping with other VSprites but
	 * was visible before it will either clear or back up
	 * the previous background.
	 */
void _ClearBobAndFollowClearPath(struct VSprite *, struct RastPort *);

	if (NULL != CurVSprite->VSBob)
	  _ClearBobAndFollowClearPath(CurVSprite,rp);


	InitBitMap(&bm, 
	           CurVSprite->Depth,
		   CurVSprite->Width * 16,
		   CurVSprite->Height);

        /*
         * If I am supposed to back up the background then
         * I have to do it now!!
         */
        if (0 != (CurVSprite->Flags & SAVEBACK) &&
            NULL != CurVSprite->VSBob)
        {  
          UBYTE * savedata = (UBYTE *)CurVSprite->VSBob->SaveBuffer;
	  i=0;
	  while (i < bm.Depth && i < 8)
	  {
	    bm.Planes[i++] = savedata;
	    savedata += bm.Rows * bm.BytesPerRow;
	  }

#ifdef BLTRASTPORTBITMAP_EXISTS
          BltRastPortBitMap(rp,
                            CurVSprite->X,
                            CurVSprite->Y,
                            &bm,
                            0,
                            0,
                            CurVSprite->Width,
                            CurVSprite->Height,
                            0x0c0);
#else
#warning Since BltRastPortBitMap (or something similar) does not exist I need to use ClipBlit here which is pretty ugly.
          ClipBlit(rp,
                   CurVSprite->X,
                   CurVSprite->Y,
                   &rp_bm,
                   0,
                   0,
                   CurVSprite->Width << 4,
                   CurVSprite->Height,
                   0x0c0);
#endif
#warning Cannot set this Flag since ClipBlit seems to copy some garbage.
//          CurVSprite->Flags |= BACKSAVED;
        }


        if (0 == (CurVSprite->Flags & VSPRITE) &&
            BOBSAWAY == (CurVSprite->VSBob->Flags & BOBSAWAY))
        {
          /*
           * This Bob is to be removed...
           */
          CurVSprite->PrevVSprite->NextVSprite = CurVSprite->NextVSprite;
          CurVSprite->NextVSprite->PrevVSprite = CurVSprite->PrevVSprite;
          /*
           * This does not damage the drawpath and clearpath since
           * the structure is not freed.
           */
        }
        else
        {
  	  /*
	   * Now draw the VSprite/Bob at its current location.
	   * The bitmap has already been initialized!
	   */
	  i = 0;
	  while (i < bm.Depth && i < 8)
	  {
	    bm.Planes[i++] = imagedata;
	    imagedata += bm.Rows * bm.BytesPerRow;
	  }

	  BltBitMapRastPort(&bm,
                  	    0,
                	    0,
                	    rp,
                	    CurVSprite->X,
                	    CurVSprite->Y,
                	    CurVSprite->Width *16,
                	    CurVSprite->Height,
                	    0x0c0);

	  /*
	   * I will need to know the vsprite's coordinates
	   * that it has now the next time as well for clearing
	   * purposes.
	   */
          CurVSprite->OldX = CurVSprite->X;
          CurVSprite->OldY = CurVSprite->Y;
        
          if (CurVSprite->VSBob)
          {
            /*
             * it's a bob! mark it as drawn.
             */
            CurVSprite->VSBob->Flags |= BDRAWN;
          }
        }
      }	
      /*
       * Am I supposed to follow the drawpath.
       * If yes then follow it to its end.
       */
      if (TRUE == followdrawpath)
      {
         CurVSprite = CurVSprite->DrawPath;
         if (NULL == CurVSprite)
         {
           followdrawpath = FALSE;
           CurVSprite = AfterPathVSprite;
         }
      }
      else
      {
        CurVSprite = CurVSprite->NextVSprite;
        /*
         * Does a DrawPath start here?
         * If yes, then I will follow the DrawPath 
         * after I am done with this VSprite.
         */
        if (NULL != CurVSprite->DrawPath)
        {
          followdrawpath = TRUE;
          AfterPathVSprite = CurVSprite->NextVSprite;
        }
      }
    } /* while not all bobs/vsprites are drawn */

    AROS_LIBFUNC_EXIT
} /* DrawGList */


/*
 * Erase the VSprite from the list and follow the clear path
 * first, if necessary! This way of organizing the ClearPath
 * makes it easy to implement RemIBob but it leads to a recursion!
 * RemIBob could simply call this function here and then redraw
 * all cleared Bobs.
 * If a recursion is not what we want this can be easily
 * changed.
 */
void _ClearBobAndFollowClearPath(struct VSprite * CurVSprite, 
                                 struct RastPort * rp)
{
  if (NULL != CurVSprite->ClearPath)
  {
    /*
     * Clear the next one first. (recursion!!!)
     */
    _ClearBobAndFollowClearPath(CurVSprite->ClearPath, rp);
    CurVSprite->ClearPath = NULL;
  }

  /*
   * Only restore the background if the bob image
   * that is currently there is to be replaced by
   * the background. If SAVEBOB is set the user
   * might want some kind of a brush effect.
   */
  if (0 == (CurVSprite->Flags & SAVEBOB))
  {
    if (0 != (CurVSprite->Flags & BACKSAVED))
    {
      ULONG i;
      struct BitMap bm;
      UBYTE * imagedata = (UBYTE *)CurVSprite->VSBob->SaveBuffer;
    
      /*
       * Restore background!
       */	 
      InitBitMap(&bm, 
                 CurVSprite->Depth,
                 CurVSprite->Width * 16,
                CurVSprite->Height);
      i=0;
      while (i < bm.Depth && i < 8)
      {
        bm.Planes[i++] = imagedata;
        imagedata += bm.Rows * bm.BytesPerRow;
      }

      BltBitMapRastPort(&bm,
                        0,
                        0,
                        rp,
                        CurVSprite->OldX,
                        CurVSprite->OldY,
                        CurVSprite->Width << 4,
                        CurVSprite->Height,
                        0x0c0);

      CurVSprite->Flags &= ~BACKSAVED;
    }
    else
    {
      /*
       * No background was saved. So let's restore the
       * standard background!
       */
      EraseRect(rp,
                CurVSprite->OldX,
                CurVSprite->OldY,
                CurVSprite->OldX + ( CurVSprite->Width << 4 ) - 1,
                CurVSprite->OldY +   CurVSprite->Height       - 1);
    }
  }
}
