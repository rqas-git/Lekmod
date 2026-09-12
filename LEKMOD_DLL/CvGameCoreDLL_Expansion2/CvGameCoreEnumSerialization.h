






#pragma once

FDataStream& operator<<(FDataStream&, const YieldTypes&);
FDataStream& operator>>(FDataStream&, YieldTypes&);
namespace FSerialization
{
std::string toString(const YieldTypes&);
}
