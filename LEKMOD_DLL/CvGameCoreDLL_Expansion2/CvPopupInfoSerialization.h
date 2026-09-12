






#pragma once

class FDataStream;
struct CvPopupInfo;

FDataStream& operator <<(FDataStream& kStream, const CvPopupInfo& kPopupInfo);
FDataStream& operator >>(FDataStream& kStream, CvPopupInfo& kPopupInfo);
