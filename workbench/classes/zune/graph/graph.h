#ifndef ZUNE_GRAPH_H
#define ZUNE_GRAPH_H

/*
    Copyright � 2017, The AROS Development Team. All rights reserved.
    $Id: $
*/

#include <libraries/mui.h>

/*** Name *******************************************************************/
#define MUIC_Graph                      "Graph.mcc"

/*** Identifier base ********************************************************/
#define MUIB_Graph                      (MUIB_AROS | 0x00000000)

/*** Attributes *************************************************************/
#define MUIA_Graph_InfoText	        (MUIB_Graph | 0x00000001) /* ---  CONST_STRPTR          */
#define MUIA_Graph_Entries  	        (MUIB_Graph | 0x00000002) /* ---  IPTR *                */
#define MUIA_Graph_UpdateInterval       (MUIB_Graph | 0x00000003) /* ---  IPTR                  */
#define MUIA_Graph_PeriodicTick  	(MUIB_Graph | 0x000000FF) /* ---  BOOL                  */

/*** Methods ****************************************************************/
#define MUIM_Graph_GetSourceHandle    	(MUIB_Graph | 0x00000001)
#define MUIM_Graph_SetSourceAttrib    	(MUIB_Graph | 0x00000002)
#define MUIM_Graph_Timer    	        (MUIB_Graph | 0x000000FF)

#define MUIV_Graph_Source_ReadHook      (0x00000001)
#define MUIV_Graph_Source_Pen           (0x00000002)
#define MUIV_Graph_Source_FillPen       (0x00000003)

/*** Macros *****************************************************************/
#define GraphObject MUIOBJMACRO_START(MUIC_Graph)

#endif /* ZUNE_GRAPH_H */
