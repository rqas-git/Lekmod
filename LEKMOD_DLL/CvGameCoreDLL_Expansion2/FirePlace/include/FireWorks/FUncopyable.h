













#ifndef _FUNCOPYABLE_H_
#define _FUNCOPYABLE_H_
#pragma once

class FUncopyableBase
{
public:
	FUncopyableBase(){}

private:

	FUncopyableBase(const FUncopyableBase& kRhs);
	const FUncopyableBase &operator=(const FUncopyableBase& kRhs);
};

class FUncopyable : private FUncopyableBase
{


















};

#endif
