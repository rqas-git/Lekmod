























#ifndef		FINPUTDEVICE_H
#define		FINPUTDEVICE_H
#pragma		once

#include "FSTLBlockAllocatorMT.h"

class FInputDevice
{
public:

	enum
	{

		INPUT_NONE		= 0,


		UNINIT		= -1,
		UP,
		DOWN,
		DBL_CLICK
	};



	struct InputValue
	{
		int m_iState;
		POINT m_point;
		InputValue(int iState=UNINIT, int px=-1, int py=-1) : m_iState(iState) {m_point.x=px; m_point.y=py;  }
		bool operator==(const InputValue& in) const		{ return in.m_iState==m_iState && in.m_point.x==m_point.x && in.m_point.y==m_point.y; }
	};


	struct InputEvent
	{
		uint m_uiTimeStamp;
		uint m_uiModifiers;
		InputValue m_val;
		uint m_type;

		InputEvent() : m_uiTimeStamp(0), m_uiModifiers(0), m_type(INPUT_NONE) {}
		InputEvent(uint uiType, InputValue iv, uint uiTime, uint uiMod=0) : 
			m_uiTimeStamp(uiTime), m_uiModifiers(uiMod), m_val(iv), m_type(uiType)  {}
		bool operator==(const InputEvent& in) const
		{ return (in.m_val==m_val && in.m_type==m_type && in.m_uiModifiers==m_uiModifiers); }
		
		int GetControllerNum() const { return (m_uiModifiers & 0xffff); }


		bool IsKeyRepeat() const { return (m_uiModifiers & 0x8000000) != 0; }
		void SetKeyRepeat(bool bVal) { if (bVal) m_uiModifiers |= 0x8000000; else m_uiModifiers &= ~0x8000000; }
	};
	typedef std::vector<InputEvent, FSTL_Tagged_Allocator<InputEvent, 16,c_eMPoolTypeContainer> > InputEventsList;	

	FInputDevice();
	virtual ~FInputDevice();

	bool Init(unsigned long flags);
	bool UnInit();									
	virtual bool Update(uint uiCurTime) = 0;
	
	void SetEnabled(bool bEnabled);
	bool GetEnabled() const { return m_bEnabled; }
	unsigned long GetFlags() const { return m_ulFlags; }		


	const InputEventsList& GetInputEvents() const { return m_InputEvents;	}


	virtual bool GetInputImmediate(InputValue *pResultValue, uint uiInputType, int iControllerNum=0) const = 0;


	void AddHotKeyMapping(uint inputFromType, uint inputToType);
	bool RemoveHotKeyMapping(uint inputFromType);
	void ClearHotKeyMappings();
	uint TranslateHotKey(uint inputFromType) const;
protected:
	typedef std::map<uint              , uint              ,
#if defined(LEKMOD_MACOS)
		std::less<uint>, FSTL_Tagged_Allocator<std::pair<const uint, uint>, 16,c_eMPoolTypeContainer> > HotKeyMap;
#else
		std::less<uint>, FSTL_Tagged_Allocator<std::pair<uint, uint>, 16,c_eMPoolTypeContainer> > HotKeyMap;
#endif

	virtual bool IInit() = 0;
	virtual bool IUnInit() = 0;

	HotKeyMap m_HotKeyMap;
	InputEventsList m_InputEvents;
	bool m_bInitted;
	bool m_bEnabled;
	uint m_ulFlags;
};

#endif
