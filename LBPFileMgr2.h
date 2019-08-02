
#pragma once

class CLBPString;

class CLBPFileMgr2 
{
private:
	CLBPFileMgr2() {};
	virtual ~CLBPFileMgr2() {};

public:
	static CLBPString	GetPathOnly(const CLBPString& sPath);	//Doesn't include trailing slash
	static CLBPString	Truncate(const CLBPString& sPath);		//Will first remove filename, then each folder until empty

	static CLBPString	GetWithTrailingSlashRemoved(const CLBPString& sPath);
	static void		RemoveTrailingSlash(CLBPString& sPath);
};

