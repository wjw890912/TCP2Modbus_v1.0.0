/*****************************************************************************/
/*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : disc.c                                                           *
* Description : This file contains the functions regarding the whole disc     *
*               such as loading the MBR and performing read/write tests.      *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; version 2                     *
* of the License.                                                             *
                                                                              *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* As a special exception, if other files instantiate templates or             *
* use macros or inline functions from this file, or you compile this          *
* file and link it with other works to produce a work based on this file,     *
* this file does not by itself cause the resulting work to be covered         *
* by the GNU General Public License. However the source code for this         *
* file must still be made available in accordance with section (3) of         *
* the GNU General Public License.                                             *
*                                                                             *
* This exception does not invalidate any other reasons why a work based       *
* on this file might be covered by the GNU General Public License.            *
*                                                                             *
*                                                    (c)2006 Lennart Yseboodt *
*                                                    (c)2006 Michael De Nil   */
/*****************************************************************************/

/*****************************************************************************/
#include "disc.h"
/*****************************************************************************/

/* ****************************************************************************  
 * void disc_initDisc(Disc *disc,hcInterface* source)
 * Description: This initialises the disc by loading the MBR and setting the
 * pointer to the hardware object.
*/
void disc_initDisc(Disc *disc,IOManager* ioman)
{
	disc->ioman=ioman; //????IO??????????????IO????????IO????????
	disc_setError(disc,DISC_NOERROR);//??????????????????
	disc_loadMBR(disc);	//????MBR?????? disc.partitions
	//?????????????????????????????????????????????? 
}
/*****************************************************************************/ 

/* ****************************************************************************  
 * void disc_loadMBR(Disc *disc)
 * Description: This functions copies the partitiontable to the partitions field.
*/
void disc_loadMBR(Disc *disc)
{

	euint8 x;
	euint8 *buf;
		//????????	LBA_ADDR_MBR??????0??????2.0??????????0??
		//????????0??????BPB??????????????????????????????????????????????????
		// ioman_getSector??????MBR??????????????????????0??????????????????????
	buf=ioman_getSector(disc->ioman,LBA_ADDR_MBR,IOM_MODE_READONLY|IOM_MODE_EXP_REQ);
	for(x=0;x<4;x++){
	//????MBR????????????????????MBR????4????????????????????4??
		ex_getPartitionField(buf,&(disc->partitions[x]),PARTITION_TABLE_OFFSET+(x*SIZE_PARTITION_FIELD));
	}
	ioman_releaseSector(disc->ioman,buf);
}
/*****************************************************************************/ 


