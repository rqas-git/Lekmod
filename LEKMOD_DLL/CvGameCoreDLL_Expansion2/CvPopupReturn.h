






#pragma once

#ifndef CVPOPUPRETURN_h
#define CVPOPUPRETURN_h








class FDataStream;
#define CvPopup_SetAtGrow(kArray, iIdx, kValue)\
	if((int)kArray.size() <= iIdx) kArray.resize(iIdx+1);\
	kArray[iIdx] = kValue;

class PopupReturn
{

public:


	PopupReturn() {}


	PopupReturn(const PopupReturn& popupReturn);


	PopupReturn& operator=(const PopupReturn& source);


	void setSelectedRadioButton(int iValue, int iGroup = 0)
	{
		CvPopup_SetAtGrow(m_aiSelectedRadioButton, iGroup, iValue);

	}


	int getSelectedRadioButton(int iGroup = 0) const
	{
		return m_aiSelectedRadioButton[iGroup];
	}


	int getRadioButtonSize(void) const
	{
		return m_aiSelectedRadioButton.size();
	}


	void setCheckboxBitfield(int iValue, int iGroup = 0)
	{

		CvPopup_SetAtGrow(m_aiBitField, iGroup, iValue);
	}


	int getCheckboxBitfield(int iGroup = 0) const
	{
		return m_aiBitField[iGroup];
	}


	int getCheckboxSize(void) const
	{
		return m_aiBitField.size();
	}


#ifdef AUI_WARNING_FIXES
	void setEditBoxString(const CvString& strValue, int iGroup = 0)
#else
	void setEditBoxString(CvString strValue, int iGroup = 0)
#endif
	{

		CvPopup_SetAtGrow(m_aszEditBoxString, iGroup, strValue);
	}


	const char* getEditBoxString(int iGroup = 0) const
	{

		if((uint)iGroup<m_aszEditBoxString.size())
			return m_aszEditBoxString[iGroup];
		return NULL;
	}


	int getEditboxSize(void) const
	{
		return m_aszEditBoxString.size();
	}


	void setSpinnerWidgetValue(const int iValue, int iGroup = 0)
	{

		CvPopup_SetAtGrow(m_aiSpinnerWidgetValues, iGroup, iValue);
	}


	int getSpinnerWidgetValue(int iGroup = 0) const
	{
		return ((uint)iGroup<m_aiSpinBoxID.size()) ? m_aiSpinBoxID[iGroup] : -1;
	}


	int getSpinnerWidsize(void) const
	{
		return m_aiSpinnerWidgetValues.size();
	}


	void setSelectedPulldownValue(int iValue, int iGroup = 0)
	{
		CvPopup_SetAtGrow(m_aiPulldownID, iGroup, iValue);
	}


	int getSelectedPullDownValue(int iGroup = 0) const
	{
		return m_aiPulldownID[iGroup];
	}


	int getPulldownSize(void) const
	{
		return m_aiPulldownID.size();
	}


	void setSelectedListBoxValue(int iValue, int iGroup = 0)
	{
		CvPopup_SetAtGrow(m_aiListBoxID, iGroup, iValue);
	}


	int getSelectedListBoxValue(int iGroup = 0) const
	{
		return ((uint)iGroup<m_aiListBoxID.size()) ? m_aiListBoxID[iGroup] : -1;
	}


	int getListBoxSize(void) const
	{
		return m_aiListBoxID.size();
	}


	void setCurrentSpinBoxValue(int iValue, int iIndex = 0)
	{
		CvPopup_SetAtGrow(m_aiSpinBoxID, iIndex, iValue);
	}


	int getCurrentSpinBoxValue(int iIndex = 0) const
	{
		return ((uint)iIndex<m_aiSpinBoxID.size()) ? m_aiSpinBoxID[iIndex] : -1;
	}


	int getSpinBoxSize(void) const
	{
		return m_aiSpinBoxID.size();
	}


	void setButtonClicked(int iValue, int iGroup = 0)
	{
		CvPopup_SetAtGrow(m_aiButtonID, iGroup, iValue);
	}


	int GetButtonClicked(int iGroup = 0) const
	{
		return m_aiButtonID[iGroup];
	}


	int GetButtonSize(void) const
	{
		return m_aiButtonID.size();
	}


	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;

private:

	std::vector<int> m_aiSelectedRadioButton;
	std::vector<int> m_aiBitField;
	std::vector<CvString> m_aszEditBoxString;
	std::vector<int> m_aiSpinnerWidgetValues;
	std::vector<int> m_aiPulldownID;
	std::vector<int> m_aiListBoxID;
	std::vector<int> m_aiButtonID;
	std::vector<int> m_aiSpinBoxID;
};

#undef CvPopup_SetAtGrow

#endif
