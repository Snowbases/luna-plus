#include "StdAfx.h"
#include "cPage.h"

cPage::cPage() :
mEmotion(0)
{
	mEmotion = 0;
}

cPage::~cPage()
{}

void cPage::Init(BYTE emotion )
{
	mEmotion = emotion;
}

void cPage::AddDialogue(DWORD dwId)
{
	mDialogMap[mDialogMap.size()] = dwId;
}

DWORD cPage::GetDialogue() const
{
	return GetDialogue(0);
}

DWORD cPage::GetDialogue(DWORD orderIndex) const
{
	const DialogMap::const_iterator iterator = mDialogMap.find(orderIndex);

	return mDialogMap.end() == iterator ? 0 : iterator->second;
}

DWORD cPage::GetRandomDialogue() const
{
	return GetDialogue(
		rand() % max(1, mDialogMap.size()));
}

void cPage::AddHyperLink(const HYPERLINK& link)
{
	mHyperLinkMap[mHyperLinkMap.size()] = link;
}

HYPERLINK* cPage::GetHyperText(DWORD orderIndex)
{
	HyperLinkMap::iterator iterator = mHyperLinkMap.find(orderIndex);

	return mHyperLinkMap.end() == iterator ? 0 : &(iterator->second);
}