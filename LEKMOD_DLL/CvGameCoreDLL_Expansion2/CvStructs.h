






#pragma once

#ifndef CVSTRUCTS_H
#define CVSTRUCTS_H

struct GameTurnInfo
{
	int iMonthIncrement;
	int iNumGameTurnsPerIncrement;
};

struct OrderData
{
	OrderData()
		: eOrderType(NO_ORDER)
		, iData1(0)
		, iData2(0)
		, bSave(false)
		, bRush(false)
	{
	}

	OrderTypes eOrderType;
	int iData1;
	int iData2;
	bool bSave;
	bool bRush;
};




struct MissionData
{
	MissionData()
		: eMissionType(NO_MISSION)
		, iData1(0)
		, iData2(0)
		, iFlags(0)
		, iPushTurn(0)
	{
	}

	MissionTypes eMissionType;
	int iData1;
	int iData2;
	int iFlags;
	int iPushTurn;
};




struct VoteSelectionSubData
{
	VoteSelectionSubData()
		: eVote(NO_VOTE)
		, ePlayer(NO_PLAYER)
		, iCityId(-1)
		, eOtherPlayer(NO_PLAYER)
		, strText("")
	{
	}

	VoteTypes eVote;
	PlayerTypes ePlayer;
	int iCityId;
	PlayerTypes eOtherPlayer;
	CvString strText;
};

struct VoteSelectionData
{
	VoteSelectionData()
		: iId(0)
		, eVoteSource(NO_VOTESOURCE)
		, aVoteOptions()
	{
	}

	int iId;
	VoteSourceTypes eVoteSource;
	std::vector<VoteSelectionSubData> aVoteOptions;

	int  GetID() const;
	void SetID(int iID);
	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;
};

FDataStream& operator<<(FDataStream&, const VoteSelectionData&);
FDataStream& operator>>(FDataStream&, VoteSelectionData&);

struct VoteTriggeredData
{
	VoteTriggeredData()
		: iId(0)
		, eVoteSource(NO_VOTESOURCE)
		, kVoteOption()
	{
	}

	int iId;
	VoteSourceTypes eVoteSource;
	VoteSelectionSubData kVoteOption;

	int  GetID() const;
	void SetID(int iID);
	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;
};

FDataStream& operator<<(FDataStream&, const VoteTriggeredData&);
FDataStream& operator>>(FDataStream&, VoteTriggeredData&);

struct PlotExtraYield
{
	PlotExtraYield()
		: m_iX(-1)
		, m_iY(-1)
		, m_aeExtraYield()
	{
	}

	int m_iX;
	int m_iY;
	std::vector<int> m_aeExtraYield;
};
FDataStream& operator<<(FDataStream&, const PlotExtraYield&);
FDataStream& operator>>(FDataStream&, PlotExtraYield&);

struct PlotExtraCost
{
	PlotExtraCost()
		: m_iX(-1)
		, m_iY(-1)
		, m_iCost(0)
	{
	}

	int m_iX;
	int m_iY;
	int m_iCost;
};
FDataStream& operator<<(FDataStream&, const PlotExtraCost&);
FDataStream& operator>>(FDataStream&, PlotExtraCost&);

struct BuildingYieldChange
{
	BuildingYieldChange()
		: eBuildingClass(NO_BUILDINGCLASS)
		, eYield(NO_YIELD)
		, iChange(0)
	{
	}

	BuildingClassTypes eBuildingClass;
	YieldTypes eYield;
	int iChange;
};

FDataStream& operator<<(FDataStream&, const BuildingYieldChange&);
FDataStream& operator>>(FDataStream&, BuildingYieldChange&);

struct BuildingGreatWork
{
	BuildingGreatWork()
		: eBuildingClass(NO_BUILDINGCLASS)
#ifdef AUI_WARNING_FIXES
		, iSlot(MAX_UNSIGNED_INT)
#else
		, iSlot(-1)
#endif
		, iGreatWorkIndex(-1)
	{
	}

	BuildingClassTypes eBuildingClass;
#ifdef AUI_WARNING_FIXES
	uint iSlot;
#else
	int iSlot;
#endif
	int iGreatWorkIndex;
};

FDataStream& operator<<(FDataStream&, const BuildingGreatWork&);
FDataStream& operator>>(FDataStream&, BuildingGreatWork&);















































































class CvUnit;
class CvPlot;
class CvCity;

void checkBattleUnitType(BattleUnitTypes unitType);







class CvCombatInfo
{
public:
	CvCombatInfo();
	CvCombatInfo& operator=(const CvCombatInfo&);

	CvUnit* getUnit(BattleUnitTypes unitType) const { return m_pUnits[unitType]; };
	void setUnit(BattleUnitTypes unitType, CvUnit* unit) { m_pUnits[unitType] = unit; };

	CvCity* getCity(BattleUnitTypes unitType) const { return m_pCities[unitType]; };
	void setCity(BattleUnitTypes unitType, CvCity* pkCity) { m_pCities[unitType] = pkCity; };

	const CvCombatMemberEntry* getCombatMember(BattleUnitTypes unitType) const;

	CvPlot* getPlot() const { return m_pTargetPlot; };
	void setPlot(CvPlot* plot) { m_pTargetPlot = plot; };


	CvPlot* getFromPlot() const { return m_pFromPlot; };
	void setFromPlot(CvPlot* plot) { m_pFromPlot = plot; };

	bool getAttackerAdvances() const { return m_bAttackerAdvances; };
	void setAttackerAdvances(bool bAdvance) { m_bAttackerAdvances = bAdvance; };

	bool getDefenderRetaliates() const { return m_bDefenderRetaliates; };
	void setDefenderRetaliates(bool bRetaliate) { m_bDefenderRetaliates = bRetaliate; };

	bool getAttackIsRanged() const { return m_bAttackIsRanged; };
	void setAttackIsRanged(bool bRanged) { m_bAttackIsRanged = bRanged; };

	bool getAttackIsBombingMission() const { return m_bAttackIsBombingMission; };
	void setAttackIsBombingMission(bool bBombingMission) { m_bAttackIsBombingMission = bBombingMission; };

	bool getAttackIsAirSweep() const { return m_bAttackIsAirSweep; };
	void setAttackIsAirSweep(bool bAirSweep) { m_bAttackIsAirSweep = bAirSweep; };

	bool getDefenderCaptured() const { return m_bDefenderCaptured; };
	void setDefenderCaptured(bool bDefenderCaptured) { m_bDefenderCaptured = bDefenderCaptured; };

	bool getAttackIsNuclear() const { return m_iNuclearDamageLevel > 0; };
	int getAttackNuclearLevel() const { return m_iNuclearDamageLevel; };
	void setAttackNuclearLevel(int iNuclearDamageLevel) { m_iNuclearDamageLevel = iNuclearDamageLevel; };

	int getDamageInflicted(BattleUnitTypes unitType) const { return m_iDamageInflicted[unitType]; };
	void setDamageInflicted(BattleUnitTypes unitType, int iDamage) { m_iDamageInflicted[unitType] = iDamage; };

	int getFinalDamage(BattleUnitTypes unitType) const { return m_iFinalDamage[unitType]; };
	void setFinalDamage(BattleUnitTypes unitType, int iFinalDamage) { m_iFinalDamage[unitType] = iFinalDamage; };

	int getFearDamageInflicted(BattleUnitTypes unitType) const { return m_iFearDamageInflicted[unitType]; };
	void setFearDamageInflicted(BattleUnitTypes unitType, int iDamage) { m_iFearDamageInflicted[unitType] = iDamage; };

	int getExperience(BattleUnitTypes unitType) const { return m_iExperienceChange[unitType]; };
	void setExperience(BattleUnitTypes unitType, int iExperience) { m_iExperienceChange[unitType] = iExperience; };

	int getMaxExperienceAllowed(BattleUnitTypes unitType) const { return m_iMaxExperienceAllowed[unitType]; };
	void setMaxExperienceAllowed(BattleUnitTypes unitType, int iMaxExperience) { m_iMaxExperienceAllowed[unitType] = iMaxExperience; };

	bool getInBorders(BattleUnitTypes unitType) const { return m_bInBorders[unitType]; };
	void setInBorders(BattleUnitTypes unitType, bool bInBorders) { m_bInBorders[unitType] = bInBorders; };

	bool getUpdateGlobal(BattleUnitTypes unitType) const { return m_bUpdateGlobal[unitType]; };
	void setUpdateGlobal(BattleUnitTypes unitType, bool bUpdateGlobal) { m_bUpdateGlobal[unitType] = bUpdateGlobal; };

	bool getVisualizeCombat() const { return m_bVisualize; };
	void setVisualizeCombat(bool bVisualize) { m_bVisualize = bVisualize; };

	bool getAttackerAdvancedVisualization() const { return m_bAttackedAdvancedVis; };
	void setAttackerAdvancedVisualization(bool bAdvance) { m_bAttackedAdvancedVis = bAdvance; };

	bool IsCombatPrediction() const { return m_bCombatPrediction; };
	void setCombatPrediction(bool bCombatPrediction) { m_bCombatPrediction = bCombatPrediction; };

	int getExtraDamageTaken(BattleUnitTypes unitType) const { return m_iExtraDamageTaken[unitType]; };
	void setExtraDamageTaken(BattleUnitTypes unitType, int iDamage) { m_iExtraDamageTaken[unitType] = iDamage; };

	int getCombatSeed(BattleUnitTypes unitType) const { return m_iCombatSeed[unitType]; };
	void setCombatSeed(BattleUnitTypes unitType, int iSeed) { m_iCombatSeed[unitType] = iSeed; };


	void doRandomness(BattleUnitTypes unitType, int iWoundedRatio);

	double doStrengthRatio(int strength, int opponentStrength);

	void doExperience();


	bool IsCombatRandom() const;
	bool IsAttackerDead() const;
	bool IsDefenderDead() const;


	CvCombatMemberEntry* getDamageMembers() { return &m_kDamageMembers[0]; }
	const CvCombatMemberEntry* getDamageMembers() const { return &m_kDamageMembers[0]; }
	int getDamageMemberCount() const { return m_iDamageMemberCount; }
	int getMaxDamageMemberCount() const { return MAX_DAMAGE_MEMBER_COUNT; }
	void setDamageMemberCount(int iDamageMemberCount) { m_iDamageMemberCount = std::min(iDamageMemberCount, (int)MAX_DAMAGE_MEMBER_COUNT); }

protected:
	CvUnit* 	m_pUnits[BATTLE_UNIT_COUNT];
	CvCity* 	m_pCities[BATTLE_UNIT_COUNT];

	CvPlot* 	m_pTargetPlot;
	CvPlot* 	m_pFromPlot;
	bool		m_bAttackerAdvances;
	bool		m_bAttackIsRanged;
	bool		m_bAttackIsBombingMission;
	bool		m_bAttackIsAirSweep;
	bool		m_bDefenderRetaliates;
	bool		m_bDefenderCaptured;

	int			m_iNuclearDamageLevel;

	int			m_iFinalDamage[BATTLE_UNIT_COUNT];
	int			m_iDamageInflicted[BATTLE_UNIT_COUNT];
	int			m_iFearDamageInflicted[BATTLE_UNIT_COUNT];

	int			m_iExperienceChange[BATTLE_UNIT_COUNT];
	int			m_iMaxExperienceAllowed[BATTLE_UNIT_COUNT];
	bool		m_bInBorders[BATTLE_UNIT_COUNT];
	bool		m_bUpdateGlobal[BATTLE_UNIT_COUNT];

	bool		m_bVisualize;

	bool		m_bAttackedAdvancedVis;
	bool		m_bCombatPrediction;
	int 		m_iExtraDamageTaken[BATTLE_UNIT_COUNT];
	int			m_iCombatSeed[BATTLE_UNIT_COUNT];

	CvCombatMemberEntry	m_kCombatMembers[BATTLE_UNIT_COUNT];


	int			m_iDamageMemberCount;
	CvCombatMemberEntry	m_kDamageMembers[MAX_DAMAGE_MEMBER_COUNT];
};
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
struct CvCombatDamageRange
{
	int iMin;
	int iMax;
	int iAverage;
	CvCombatDamageRange() : iMin(0), iMax(0), iAverage(0) {}
};
struct CvCombatModifierEntry
{
	CvCombatModifierEntry()
		: m_iModifier(0)
		, m_bPercent(true)
		, m_bMiscellaneous(false)
	{
	}

	CvCombatModifierEntry(const CvString& strText, int iModifier, bool bPercent = true, bool bMiscellaneous = false)
		: m_strText(strText)
		, m_iModifier(iModifier)
		, m_bPercent(bPercent)
		, m_bMiscellaneous(bMiscellaneous)
	{
	}

	CvString m_strText;
	int m_iModifier;
	bool m_bPercent;
	bool m_bMiscellaneous;
};

struct CvCombatModifierList
{
	CvCombatModifierList(int iMaxLines = 0)
		: iMaxLines(iMaxLines)
		, iMiscModifier(0)
		, iMiscCount(0)
		, iMiscEntryIndex(-1)
		, iMiscModifierFlat(0)
		, iMiscCountFlat(0)
		, iMiscFlatEntryIndex(-1)
		, bAttackerSide(false)
	{
	}

	void AddEntry(const CvString& strText, int iModifier, bool bPercent = true);
	void RebuildMiscellaneous(bool bPercent);

	std::vector<CvCombatModifierEntry> m_kEntries;

	int iMaxLines;



	int iMiscModifier;
	int iMiscCount;
	int iMiscEntryIndex;
	int iMiscModifierFlat;
	int iMiscCountFlat;
	int iMiscFlatEntryIndex;

	bool bAttackerSide;
};
#endif




class CvMissionDefinition
{
public:
	CvMissionDefinition();

	MissionTypes getMissionType() const;
	void setMissionType(MissionTypes missionType);

	float getMissionTime() const;
	void setMissionTime(float time);

	CvUnit* getUnit(BattleUnitTypes unitType) const;
	void setUnit(BattleUnitTypes unitType, CvUnit* unit);

	const CvPlot* getPlot() const;
	void setPlot(const CvPlot* plot);

	const CvPlot* getSecondaryPlot() const;
	void setSecondaryPlot(const CvPlot* plot);

protected:
	MissionTypes		m_eMissionType;
	CvUnit*				m_aUnits[BATTLE_UNIT_COUNT];
	float				m_fMissionTime;
	const CvPlot*		m_pPlot;
	const CvPlot*		m_pSecondaryPlot;
};






class CvAirMissionDefinition : public CvMissionDefinition
{
public:
	CvAirMissionDefinition();
	CvAirMissionDefinition(const CvAirMissionDefinition& kCopy);

	int getDamage(BattleUnitTypes unitType) const;
	void setDamage(BattleUnitTypes unitType, int damage);
	bool isDead(BattleUnitTypes unitType) const;

private:
	int	m_aDamage[BATTLE_UNIT_COUNT];
};
#if defined(LEKMOD_GOLDEN_AGE_YIELD_THRESHOLD)
struct GoldenAgeYieldThreshold
{
	GoldenAgeYieldThreshold()
		: m_eThresholdYield(NO_YIELD)
		, m_iThresholdAmount(0)
		, m_eRwdYield(NO_YIELD)
		, m_iRwdAmount(0)
	{
	}
	YieldTypes m_eThresholdYield;
	int m_iThresholdAmount;
	YieldTypes m_eRwdYield;
	int m_iRwdAmount;
};

FDataStream& operator<<(FDataStream&, const GoldenAgeYieldThreshold&);
FDataStream& operator>>(FDataStream&, GoldenAgeYieldThreshold&);
#endif
#endif
