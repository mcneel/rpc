
#include "stdafx.h"
#include "LBPFileMgr2.h"
#include "LBPString.h"

#define LBP_DIRECTORY_SLASH L'\\'

CLBPString	CLBPFileMgr2::GetPathOnly(const CLBPString& sPath)
{
	return CLBPFileMgr2::Truncate(sPath);
}

CLBPString	CLBPFileMgr2::Truncate(const CLBPString& sPath)
{
	if(sPath.IsEmpty()) 
		return L"";

	int iPos = GetWithTrailingSlashRemoved(sPath).ReverseFind(LBP_DIRECTORY_SLASH);

	if(-1 == iPos)
	{
		//Not a path - just a name
		return L"";
	}

	return sPath.Left(iPos);
}

CLBPString	CLBPFileMgr2::GetWithTrailingSlashRemoved(const CLBPString& s)
{
	CLBPString sPath(s);
	RemoveTrailingSlash(sPath);
	return sPath;
}

void CLBPFileMgr2::RemoveTrailingSlash(CLBPString& sPath)
{
	if (sPath.IsEmpty())
		return;

	int iLast = sPath.GetLength()-1;

	if(LBP_DIRECTORY_SLASH == sPath.GetAtWide(iLast))
	{
		sPath = sPath.Left(iLast);
	}
}
