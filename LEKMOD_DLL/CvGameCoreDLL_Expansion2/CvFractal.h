






#pragma once



#ifndef CIV5_FRACTAL_H
#define CIV5_FRACTAL_H

#define FRACTAL_MAX_DIMS 255

class CvRandom;
class CvFractal
{

public:

	enum FracVals
	{
	    FRAC_WRAP_X					= (0x0001),
	    FRAC_WRAP_Y					= (0x0002),
	    FRAC_PERCENT				= (0x0004),
	    FRAC_POLAR					= (0x0008),
	    FRAC_CENTER_RIFT			= (0x0010),
	    FRAC_INVERT_HEIGHTS			= (0x0020),
	    DEFAULT_FRAC_X_EXP			= 7,
	    DEFAULT_FRAC_Y_EXP			= 6
	};








	void fracInit(int iNewXs, int iNewYs, int iGrain, CvRandom& random, int iFlags, CvFractal* pRifts=NULL, int iFracXExp=DEFAULT_FRAC_X_EXP, int iFracYExp=DEFAULT_FRAC_Y_EXP);
	void fracInitHinted(int iNewXs, int iNewYs, int iGrain, CvRandom& random, byte* pbyHints, int iHintsLength, int iFlags, CvFractal* pRifts, int iFracXExp=DEFAULT_FRAC_X_EXP, int iFracYExp=DEFAULT_FRAC_Y_EXP);

	int getHeight(int x, int y);
	int getHeightFromPercent(int iPercent);

	void reset();
	CvFractal();
	virtual ~CvFractal();
	void uninit();

























	void ridgeBuilder(CvRandom& random, int iNumVoronoiSeeds, int iRidgeFlags, int iBlendRidge, int iBlendFract);

protected:

	int m_iXs;
	int m_iYs;
	int m_iFlags;

	int m_iFracX;
	int m_iFracY;
	int m_iFracXExp;
	int m_iFracYExp;

	int m_iXInc;
	int m_iYInc;

	int m_aaiFrac[FRACTAL_MAX_DIMS+1][FRACTAL_MAX_DIMS+1];
	void fracInitInternal(int iNewXs, int iNewYs, int iGrain, CvRandom& random, byte* pbyHints, int iHintsLength, int iFlags, CvFractal* pRifts, int iFracXExp, int iFracYExp);
	void tectonicAction(CvFractal* pRifts);
	int yieldX(int iBadX);

};

#endif
