#pragma once


#ifndef _PROC_H
#define _PROC_H

#include "common.h"

#define MAX_PROC_LEN  32


extern LIST_ENTRY CipherProcList;

extern ULONG G_ProcNameOffset;

typedef struct _RuleNode
{
	CHAR procName[MAX_PROC_LEN];
	LIST_ENTRY next;
}List_RuleNode, *PList_RuleNode;


BOOLEAN InsertRuleToLinkList(PCHAR str);

VOID Dbg_OutRules(PLIST_ENTRY listHead);

VOID ClearList(PLIST_ENTRY head);


BOOLEAN SearchProcess(PCHAR procName, PLIST_ENTRY head);

BOOLEAN IsMonitoredProcess(PCHAR procName);


VOID InitializeCipherProcList();

VOID ClearCipherProcList();


ULONG GetProcessNameOffset();

PCHAR GetProcessName();







#endif