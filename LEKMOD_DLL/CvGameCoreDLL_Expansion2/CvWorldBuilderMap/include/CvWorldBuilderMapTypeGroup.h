















#pragma once
#ifndef CvWorldBuilderMapTypeGroup_h
#define CvWorldBuilderMapTypeGroup_h

class CvWorldBuilderMapTypeGroup
{
public:
	CvWorldBuilderMapTypeGroup();
	CvWorldBuilderMapTypeGroup(const CvWorldBuilderMapTypeGroup &kRhs);
	~CvWorldBuilderMapTypeGroup();

	const CvWorldBuilderMapTypeGroup &operator=(const CvWorldBuilderMapTypeGroup &kRhs);

	bool Contains(_In_z_ const char *szType) const;

	void Add(_In_z_ const char *szType);
	void Remove(_In_z_ const char *szType);
	void Clear();

	uint GetSerializedSize() const { return m_uiBufferSize; }
	void Serialize(void *p) const;
	void Deserialize(const void *p, uint uiSize);

private:
	char *m_szBuffer;
	uint m_uiBufferSize;
};

#endif
