
#include "stdafx.h"
#include "LBPColor.h"

CLBPColor::CLBPColor()
{
	m_Array[0] = 0.0;
	m_Array[1] = 0.0;
	m_Array[2] = 0.0;
	m_Array[3] = 0.0;
}

CLBPColor::CLBPColor(float fRed, float fGreen, float fBlue, float fAlpha)
{
	Set(fRed, fGreen, fBlue, fAlpha);
}

CLBPColor::CLBPColor(double dRed, double dGreen, double dBlue, double dAlpha)
{
	Set(dRed, dGreen, dBlue, dAlpha);
}

CLBPColor::CLBPColor(int   iRed, int   iGreen, int   iBlue, int iAlpha)
{
	Set(iRed, iGreen, iBlue, iAlpha);
}

CLBPColor::CLBPColor(const CLBPColor& src)
{
	*this=src;
}

CLBPColor& CLBPColor::operator=(const CLBPColor& src)
{
	Set(src.DRed(), src.DGreen(), src.DBlue(), src.DAlpha());
	return *this;
}


void CLBPColor::Set(float fRed, float fGreen, float fBlue, float fAlpha)
{
	m_Array[0] = fRed;
	m_Array[1] = fGreen;
	m_Array[2] = fBlue;
	m_Array[3] = fAlpha;
}

void CLBPColor::Set(double dRed, double dGreen, double dBlue, double dAlpha)
{
	m_Array[0] = dRed;
	m_Array[1] = dGreen;
	m_Array[2] = dBlue;
	m_Array[3] = dAlpha;
}

void CLBPColor::Set(int   iRed, int   iGreen, int   iBlue, int iAlpha)
{
	m_Array[0] = (double)iRed	/ 255.0;
	m_Array[1] = (double)iGreen / 255.0;
	m_Array[2] = (double)iBlue	/ 255.0;
	m_Array[3] = (double)iAlpha / 255.0;
}


bool CLBPColor::operator== (const CLBPColor& c2) const
{
	if(DRed()!=c2.DRed())		return false;
	if(DGreen()!=c2.DGreen())	return false;
	if(DBlue()!=c2.DBlue())		return false;
	if(DAlpha()!=c2.DAlpha())	return false;

	return TRUE;
}

bool CLBPColor::operator!= (const CLBPColor& c2) const
{
	return !(*this==c2);
}

double	CLBPColor::DRed() const
{
	return m_Array[0];
}

double	CLBPColor::DGreen() const
{
	return m_Array[1];
}

double	CLBPColor::DBlue() const
{
	return m_Array[2];
}

double	CLBPColor::DAlpha() const
{
	return m_Array[3];
}

UINT	CLBPColor::IRed() const
{
	return (UINT)((DRed()*255.0)+0.01);
}

UINT	CLBPColor::IGreen() const
{
	return (UINT)((DGreen()*255.0)+0.01);
}

UINT	CLBPColor::IBlue() const
{
	return (UINT)((DBlue()*255.0)+0.01);
}

UINT	CLBPColor::IAlpha() const
{
	return (UINT)((DAlpha()*255.0+0.01));
}


