#pragma once
#ifndef _CONTEXTFUNC_H
#define _CONTEXTFUNC_H

#include "common.h"


typedef struct _STREAM_CONTEXT
{
	BOOLEAN bNeedEncrypt;
	BOOLEAN bNeedDecrypt;
	BOOLEAN bIsThereAFlag;		//如果有flag说明文件已经加密
	BOOLEAN bHasWrittenData;	//确定是否有数据写入文件

}STREAM_CONTEXT, *PSTREAM_CONTEXT;




#endif