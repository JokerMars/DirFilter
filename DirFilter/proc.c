#include "proc.h"

LIST_ENTRY CipherProcList;

ULONG G_ProcNameOffset;

VOID InitializeCipherProcList()
{
	G_ProcNameOffset = GetProcessNameOffset();
	DbgPrint("\tProcess Name Offset: %d\n", G_ProcNameOffset);

	BOOLEAN isOk = FALSE;
	InitializeListHead(&CipherProcList);

	isOk = InsertRuleToLinkList("acad.exe");
	if (isOk)
	{
		Dbg_OutRules(&CipherProcList);
	}

}


VOID ClearCipherProcList()
{
	ClearList(&CipherProcList);
}

BOOLEAN InsertRuleToLinkList(PCHAR str)
{
	if (str == NULL || strlen(str) >= MAX_PROC_LEN)
	{
		return FALSE;
	}

	PList_RuleNode pNode = NULL;
	pNode = ExAllocatePool(NonPagedPool, sizeof(List_RuleNode));
	if (pNode == NULL)
	{
		DbgPrint("\tInsufficient Resources.\n");
		return FALSE;
	}

	RtlZeroMemory(pNode, sizeof(List_RuleNode));
	RtlCopyMemory(pNode->procName, str, strlen(str));

	InsertHeadList(&CipherProcList, &pNode->next);

	return TRUE;
}


VOID Dbg_OutRules(PLIST_ENTRY listHead)
{
	PLIST_ENTRY    thisEntry, nextEntry;
	PList_RuleNode pNode;

	for (thisEntry = listHead->Flink, nextEntry = thisEntry->Flink;
		thisEntry != listHead;
		thisEntry = nextEntry, nextEntry = thisEntry->Flink)
	{

		pNode = CONTAINING_RECORD(thisEntry, List_RuleNode, next);

		DbgPrint("The data of ProcList is: %s\n", &pNode->procName);

	}
}

BOOLEAN SearchProcess(PCHAR procName, PLIST_ENTRY head)
{
	PLIST_ENTRY pEntry;
	PList_RuleNode pNode;

	int len = strlen(procName);

	for (pEntry = head->Flink; pEntry != head; pEntry = pEntry->Flink)
	{
		pNode = CONTAINING_RECORD(pEntry, List_RuleNode, next);

		if (RtlCompareMemory(procName, pNode->procName, len) == len)
		{
			return TRUE;
		}

	}

	return FALSE;
}


BOOLEAN IsMonitoredProcess(PCHAR procName)
{
	if (procName == NULL)
	{
		return FALSE;
	}

	if (SearchProcess(procName, &CipherProcList))
	{
		return TRUE;
	}

	return FALSE;
}


VOID ClearList(PLIST_ENTRY head)
{
	PList_RuleNode pNode;
	PLIST_ENTRY pEntry;

	try
	{
		while (!IsListEmpty(head))
		{
			pEntry = RemoveTailList(head);
			pNode = CONTAINING_RECORD(pEntry, List_RuleNode, next);
			if (pNode->procName != NULL)
			{
				DbgPrint("    Remove From List: %s\n", pNode->procName);
			}

			ExFreePool(pNode);
		}

	}
	finally
	{

	}

}


ULONG GetProcessNameOffset()
{
	PEPROCESS curproc = NULL;
	int i = 0;

	curproc = PsGetCurrentProcess();

	for (i = 0; i<3 * PAGE_SIZE; i++)
	{
		if (!strncmp("System", (PCHAR)curproc + i, strlen("System")))
		{
			return i;
		}
	}

	return 0;
}


PCHAR GetProcessName()
{
	PEPROCESS curproc = NULL;
	curproc = PsGetCurrentProcess();

	if (curproc != NULL)
	{
		return (PCHAR)curproc + G_ProcNameOffset;
	}

	return NULL;
}





