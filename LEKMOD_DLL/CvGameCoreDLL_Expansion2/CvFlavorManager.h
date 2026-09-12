






#pragma once

#ifndef CIV5_FLAVOR_MANAGER_H
#define CIV5_FLAVOR_MANAGER_H










class CvFlavorRecipient
{
public:
	CvFlavorRecipient();
	~CvFlavorRecipient();
	virtual void Init();
	virtual void Uninit();

	bool IsCity();

	void SetFlavors(int* piUpdatedFlavorValues);
	void ChangeFlavors(int* piDeltaFlavorValues, bool bDontLog = false);

	int GetLatestFlavorValue(FlavorTypes eFlavor, bool bAllowNegative = false);

protected:

	virtual void FlavorUpdate() = 0;
	virtual void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR) = 0;

	bool m_bIsCity;

	int* m_piLatestFlavorValues;
};












class CvFlavorManager
{
public:
	CvFlavorManager(void);
	~CvFlavorManager(void);


	void Init(CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);


	void AddFlavorRecipient(CvFlavorRecipient* pTargetObject, bool bPropogateFlavorValues = true);
	void RemoveFlavorRecipient(CvFlavorRecipient* pTargetObject);


	void ChangeFlavors(int* piDeltaFlavorValues, bool bDontUpdateCityFlavors);
	void ResetToBasePersonality();
	void AdjustWeightsForMap();


	int GetIndividualFlavor(FlavorTypes eType);
	int* GetAllFlavors();


	int GetPersonalityIndividualFlavor(FlavorTypes eType);
	int* GetAllPersonalityFlavors();

	int GetAdjustedValue(int iOriginalValue, int iPlusMinus, int iMin, int iMax);

private:

	void RandomizeWeights();
	void BroadcastFlavors(int* piDeltaFlavorValues, bool bDontUpdateCityFlavors);
	void BroadcastBaseFlavors();

	void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR);

	int* m_piPersonalityFlavor;
	int* m_piActiveFlavor;
	CvPlayer* m_pPlayer;

	typedef FFastList<CvFlavorRecipient*, c_eCiv5GameplayDLL, 0> Flavor_List;
	Flavor_List m_FlavorTargetList;
};

#endif
