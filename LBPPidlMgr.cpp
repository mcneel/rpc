// PidlMgr.cpp: implementation of the CLBPPidlMgr class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#if !defined LBPLIB_DISABLE_SHELL
#include "lbppidlmgr.h"


////////////////////////////////////////////////////////////////
// CLBPPidlMgr : Class to manage pidls

void CLBPPidlMgr::Delete(LPITEMIDLIST pidl)
{
	if (pidl) ::ILFree(pidl);
}

LPCITEMIDLIST CLBPPidlMgr::GetNextItem(LPCITEMIDLIST pidl)
{
	if (pidl)
	{
		return ::ILGetNext(pidl);
		//return (LPITEMIDLIST)(LPBYTE) ( ((LPBYTE)pidl) + pidl->mkid.cb);
	}
	return (NULL);
}

LPITEMIDLIST CLBPPidlMgr::Copy(LPCITEMIDLIST pidlSrc)
{
	if(pidlSrc) return ::ILClone(pidlSrc);
	return 0;
}

UINT CLBPPidlMgr::GetSize(LPCITEMIDLIST pidl)
{
	if(pidl) return ::ILGetSize(pidl);
	return 0;
}


LPCITEMIDLIST CLBPPidlMgr::GetLastItem(LPCITEMIDLIST pidl)
{
	if(pidl) return ::ILFindLastID(pidl);
	return 0;
}

UINT CLBPPidlMgr::GetPidlLength(LPCITEMIDLIST pidl)
{
	UINT length = 0;

	if(pidl)
	{
		while(pidl->mkid.cb)
		{
			length++;
			pidl = GetNextItem(pidl);
		}  
	}
	return length;
}


LPITEMIDLIST CLBPPidlMgr::Concatenate(LPCITEMIDLIST pidl1, 
	LPCITEMIDLIST pidl2)
{
	return ::ILCombine(pidl1, pidl2);
}

LPITEMIDLIST CLBPPidlMgr::Truncate(LPCITEMIDLIST pidl)
{
	//TRACE("Truncate\n");
	if(pidl == NULL)
	{
		return NULL;
	}

	UINT cbTotal = 0;
	UINT cbThis	 = 0;
	UINT length = GetPidlLength(pidl);

	if(length < 2) return NULL;
	length--;
	
	LPCITEMIDLIST pidlTemp = (LPITEMIDLIST) pidl;

	for(UINT i = 0; i < length; i++)
	{
		cbThis		= pidlTemp->mkid.cb;
		cbTotal	   += cbThis;
		pidlTemp	= GetNextItem(pidlTemp);


	}

	LPITEMIDLIST pidlNew = (LPITEMIDLIST)CoTaskMemAlloc(cbTotal + sizeof(ITEMIDLIST));
	
	if(pidlNew)
	{
		ZeroMemory(pidlNew, cbTotal + sizeof(ITEMIDLIST));
		CopyMemory(pidlNew, pidl, cbTotal);
	}

	return pidlNew;
}







LPITEMIDLIST CLBPPidlMgr::GetSimplePidl(LPCITEMIDLIST pidl, int iItem)
{
	if(pidl == NULL)
	{
		return NULL;
	}

	LPCITEMIDLIST pidlTemp = (LPCITEMIDLIST)pidl;

	for (int i=0; i < iItem; i++)
	{
		pidlTemp = GetNextItem(pidlTemp);
	}

	pidl->mkid.cb;
	LPITEMIDLIST pidlNew = (LPITEMIDLIST)CoTaskMemAlloc(pidlTemp->mkid.cb + sizeof(ITEMIDLIST));
	if(pidlNew)
	{
		ZeroMemory(pidlNew, pidlTemp->mkid.cb + sizeof(ITEMIDLIST));
		CopyMemory(pidlNew, pidlTemp, pidlTemp->mkid.cb);
	}
	return pidlNew;

}

bool CLBPPidlMgr::IsDesktop(LPCITEMIDLIST pidl)
{
	if(!pidl) return true;

	if(GetPidlLength(pidl) == 0) return true;
	if(GetSize(pidl)== 0) return true;

	LPITEMIDLIST pidlDesktop = GetDesktopPidl();
	if(!pidlDesktop)
	{
		return false;
	}

	UINT iSize = GetSize(pidlDesktop);
	//UINT iLength = GetPidlLength(pidlDesktop);

	bool bSame = false;

	if(GetSize(pidl) == iSize)
	{
		bSame = 0==memcmp(pidl, pidlDesktop, iSize);
	}

	Delete(pidlDesktop);
	
	return bSame;

}

LPITEMIDLIST CLBPPidlMgr::GetDesktopPidl()
{
	LPITEMIDLIST pidlDesktop = NULL;
	::SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidlDesktop);

	return pidlDesktop;
}

LPITEMIDLIST CLBPPidlMgr::PidlFromPath(LPCTSTR pszPath, HWND hOwner)
{
	USES_CONVERSION;

	ULONG count = 0;
	LPITEMIDLIST pidlNew = NULL;

	IShellFolder* pDesktop;
	HRESULT hr = SHGetDesktopFolder(&pDesktop);

	if(FAILED(hr))
		return NULL;

	hr = pDesktop->ParseDisplayName(hOwner, NULL, T2OLE(const_cast<LPTSTR>(pszPath)), &count, &pidlNew, NULL);
	pDesktop->Release();

	if(FAILED(hr))
		return NULL;

	return pidlNew;
}

bool CLBPPidlMgr::IsEqual(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
	LPSHELLFOLDER psfThis = nullptr;
	LPSHELLFOLDER psfLast = nullptr;

	if (pidl1 == nullptr && pidl2 == nullptr)
	{
		return true;
	}

	if (pidl1 == nullptr || pidl2 == nullptr)
	{
		return false;
	}

	const auto iLength1 = GetPidlLength(pidl1);
	const auto iLength2 = GetPidlLength(pidl2);
	if (iLength1 != iLength2)
		return false;

	SHGetDesktopFolder(&psfLast);

	LPITEMIDLIST pidlSimple1 = nullptr, pidlSimple2 = nullptr;

	for (UINT i = 0; i < GetPidlLength(pidl1); i++)
	{
		pidlSimple1 = GetSimplePidl(pidl1, i);
		pidlSimple2 = GetSimplePidl(pidl2, i);

		if (psfLast->CompareIDs(0, pidlSimple1, pidlSimple2) != 0)
		{
			return false;
		}

		psfLast->BindToObject(pidlSimple1, NULL, IID_IShellFolder, (LPVOID*)&psfThis);
		psfLast->Release();
		psfLast = psfThis;

		Delete(pidlSimple1);
		Delete(pidlSimple2);
	}

	return true;
}



#endif
