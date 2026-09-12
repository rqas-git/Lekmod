













#ifndef _INCLUDED_FStlContainerSerialization_H
#define _INCLUDED_FStlContainerSerialization_H


















#include <deque>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <unordered_set>


template<typename FirstType, typename SecondType>
FDataStream & operator<<(FDataStream & saveTo, const std::pair<FirstType, SecondType> & readFrom)
{
	saveTo << readFrom.first;
	saveTo << readFrom.second;
	return saveTo;
}

template<typename FirstType, typename SecondType>
FDataStream & operator>>(FDataStream & loadFrom, std::pair<FirstType, SecondType> & writeTo)
{
	loadFrom >> writeTo.first;
	loadFrom >> writeTo.second;
	return loadFrom;
}





template<typename ElementType, typename ContainerType>
struct SerializeFromSequenceContainer
{
	SerializeFromSequenceContainer(FDataStream & saveTo, const ContainerType & container) :
	m_saveTo(saveTo)
	, m_container(container)
	{
		m_saveTo << container.size();
	}

	void operator() (ElementType & i)
	{
		m_saveTo << i;
	}

	FDataStream & m_saveTo;
	const ContainerType & m_container;
};
template<typename ElementType, typename ContainerType>
void SerializeToSequenceContainer(FDataStream & loadFrom, ContainerType & container)
{
	container.clear();
#if defined(LEKMOD_MACOS)
	typename ContainerType::size_type count = 0;
#else
	ContainerType::size_type count = 0;
#endif
	loadFrom >> count;
#if defined(LEKMOD_MACOS)
	typename ContainerType::size_type i = 0;
#else
	ContainerType::size_type i = 0;
#endif
	for(i = 0; i < count; ++i)
	{
		ElementType v;
		loadFrom >> v;
		container.push_back(v);
	}
}
























template<typename ElementType, typename ContainerType>
struct SerializeFromAssociativeContainer
{
	SerializeFromAssociativeContainer(FDataStream & saveTo, ContainerType & container) :
	m_saveTo(saveTo)
	, m_container(container)
	{
		size_t count = container.size();
		m_saveTo << count;
	}

	void operator()(const ElementType & i)
	{
		m_saveTo << i;
	}

	FDataStream & m_saveTo;
	ContainerType & m_container;
};

template<typename ElementType, typename ContainerType>
void SerializeToAssociativeContainer(FDataStream & loadFrom, ContainerType & container)
{
	container.clear();
	size_t count = 0;
	loadFrom >> count;
	size_t i = 0;
	for(i = 0; i < count; ++i)
	{
		ElementType e;
		loadFrom >> e;
		container.insert(e);
	}
}

template<typename KeyType, typename ElementType>
FDataStream & operator<<(FDataStream & saveTo, const std::map<KeyType, ElementType> & readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromAssociativeContainer<std::pair<KeyType, ElementType>, const std::map<KeyType, ElementType> >(saveTo, readFrom));
	return saveTo;
}

template<typename KeyType, typename ElementType>
FDataStream & operator<<(FDataStream & saveTo, const stdext::hash_map<KeyType, ElementType> & readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromAssociativeContainer<std::pair<KeyType, ElementType>, const stdext::hash_map<KeyType, ElementType> >(saveTo, readFrom));
	return saveTo;
}

template<typename KeyType>
FDataStream & operator <<(FDataStream & saveTo, const std::tr1::unordered_set<KeyType>& readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromAssociativeContainer<KeyType, const std::tr1::unordered_set<KeyType> >(saveTo, readFrom));
	return saveTo;
}

template<typename ElementType>
FDataStream & operator<<(FDataStream & saveTo, const std::set<ElementType> & readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromAssociativeContainer<ElementType, const std::set<ElementType> >(saveTo, readFrom));
	return saveTo;
}

template<typename ElementType> 
FDataStream & operator<<(FDataStream & saveTo, const std::deque<ElementType> & readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromSequenceContainer<const ElementType, const std::deque<ElementType> >(saveTo, readFrom));
	return saveTo;
}

template<typename ElementType> 
FDataStream & operator<<(FDataStream & saveTo, const std::list<ElementType> & readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromSequenceContainer<const ElementType, const std::list<ElementType> >(saveTo, readFrom));
	return saveTo;
}

template<typename ElementType> 
FDataStream & operator<<(FDataStream & saveTo, const std::vector<ElementType> & readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromSequenceContainer<const ElementType, const std::vector<ElementType> >(saveTo, readFrom));
	return saveTo;
}

template<typename ElementType>
FDataStream & operator>>(FDataStream & loadFrom, std::deque<ElementType> & writeTo)
{



	SerializeToSequenceContainer<ElementType, std::deque<ElementType> >(loadFrom, writeTo);
	return loadFrom;
}

template<typename ElementType>
FDataStream & operator>>(FDataStream & loadFrom, std::list<ElementType> & writeTo)
{



	SerializeToSequenceContainer<ElementType, std::list<ElementType> >(loadFrom, writeTo);
	return loadFrom;
}

template<typename ElementType>
FDataStream & operator>>(FDataStream & loadFrom, std::vector<ElementType> & writeTo)
{



	SerializeToSequenceContainer<ElementType, std::vector<ElementType> >(loadFrom, writeTo);


	return loadFrom;
}

template<typename KeyType, typename ElementType>
FDataStream & operator>>(FDataStream & loadFrom, std::map<KeyType, ElementType> & writeTo)
{
	SerializeToAssociativeContainer<std::pair<KeyType, ElementType>, std::map<KeyType, ElementType> >(loadFrom, writeTo);
	return loadFrom;
}

template<typename KeyType, typename ElementType>
FDataStream & operator>>(FDataStream & loadFrom, stdext::hash_map<KeyType, ElementType> & writeTo)
{
	SerializeToAssociativeContainer<std::pair<KeyType, ElementType>, stdext::hash_map<KeyType, ElementType> >(loadFrom, writeTo);
	return loadFrom;
}

template<typename KeyType>
FDataStream & operator>>(FDataStream & loadFrom, std::tr1::unordered_set<KeyType> & writeTo)
{
	SerializeToAssociativeContainer<KeyType, std::tr1::unordered_set<KeyType> >(loadFrom, writeTo);
	return loadFrom;
}

template<typename ElementType>
FDataStream & operator>>(FDataStream & loadFrom, std::set<ElementType> & writeTo)
{
	SerializeToAssociativeContainer<ElementType, std::set<ElementType> >(loadFrom, writeTo);
	return loadFrom;
}

FDataStream & operator<<(FDataStream & saveTo, const std::string & readFrom);
FDataStream & operator>>(FDataStream & loadFrom, std::string & writeTo);

#endif
