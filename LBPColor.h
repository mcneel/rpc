
#pragma once

#ifndef LBPEXPORT
#define LBPEXPORT
#endif

class LBPEXPORT CLBPColor
{
public:
	CLBPColor();

	CLBPColor(float fRed, float fGreen, float fBlue, float fAlpha = 1.f);
	CLBPColor(double dRed, double dGreen, double dBlue, double dAlpha = 1.0);
	CLBPColor(int   iRed, int   iGreen, int   iBlue, int iAlpha = 255);

	CLBPColor(const CLBPColor& src);
	CLBPColor& operator=(const CLBPColor& src);

	bool operator== (const CLBPColor& c2) const;
	bool operator!= (const CLBPColor& c2) const;

	void Set(float fRed, float fGreen, float fBlue, float fAlpha = 1.f);
	void Set(double dRed, double dGreen, double dBlue, double fAlpha = 1.0);
	void Set(int   iRed, int   iGreen, int   iBlue, int iAlpha = 255);

	double	DRed() const;
	double	DGreen() const;
	double	DBlue() const;
	double	DAlpha() const;

	UINT	IRed() const;
	UINT	IGreen() const;
	UINT	IBlue() const;
	UINT	IAlpha() const;

protected:
	double m_Array[4];
};
