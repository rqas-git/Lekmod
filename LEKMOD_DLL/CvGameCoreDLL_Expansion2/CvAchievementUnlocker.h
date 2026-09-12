







#ifndef CVACHIEVEMENTUNLOCKER_H
#define CVACHIEVEMENTUNLOCKER_H







class CvAchievementUnlocker
{
public:
	static bool Check_PSG();
	static void FarmImprovementPillaged();
	static void EndTurn();
	static void AlexanderConquest(PlayerTypes ePlayer);

private:
	static int ms_iNumImprovementsPillagedPerTurn;
	 
};











class CvPlayerAchievements
{
public:
	CvPlayerAchievements(const CvPlayer& kPlayer);




	void AlliedWithCityState(PlayerTypes eCityStateAlly);
	void AddUnit(CvUnit* pUnit);
	void AttackedUnitWithUnit(CvUnit* pAttackingUnit, CvUnit* pDefendingUnit);
	void BoughtCityState(int iNumUnits);
	void KilledUnitWithUnit(CvUnit* pKillingUnit, CvUnit* pKilledUnit);
	void StartTurn();
	void EndTurn();
	void FinishedBuilding(CvCity* pkCity, BuildingTypes eBuilding);

	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

private:
	const CvPlayer& m_kPlayer;

	int m_iAchievement_XP1_32_Progress;
	int m_iAchievement_XP1_33_Progress;


	BeliefTypes m_ePapalPrimacyType;
	BuildingTypes m_eCollossusType;
	BuildingTypes m_ePetraType;
	BuildingTypes m_eCaravansaryType;
	BuildingTypes m_eHarborType;
	CivilizationTypes m_eEthiopiaType;
	CivilizationTypes m_eByzantinesType;
	CivilizationTypes m_eGreekType;
	CivilizationTypes m_eCarthageType;
	CivilizationTypes m_eRomeType;
	CivilizationTypes m_eSwedenType;
	UnitTypes m_eDromonType;
	UnitTypes m_eGreatProphetType;
	UnitTypes m_eAfricanForestElephantType;
	UnitTypes m_eGreatGeneralType;
	UnitTypes m_eHakkapelittaType;
	UnitTypes m_eSkyFortress;
};

#endif
