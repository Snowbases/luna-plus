#include "StdAfx.h"
#include ".\dungeonprocessorbase.h"

CDungeonProcessorBase::CDungeonProcessorBase(void)
{
}

CDungeonProcessorBase::~CDungeonProcessorBase(void)
{
}

void CDungeonProcessorBase::Init(CDungeonMgr* pParent)
{
	m_pParent = pParent;
}