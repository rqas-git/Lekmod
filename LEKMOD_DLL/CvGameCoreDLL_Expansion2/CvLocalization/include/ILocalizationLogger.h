#pragma once

namespace Localization
{
	class ILocalizationLogger
	{
	public:

		enum LogTypes
		{
			TYPE_MESSAGE = 0,
			TYPE_WARNING = 1,
			TYPE_ERROR = 2
		};


		virtual ~ILocalizationLogger(){};

		virtual void Log(const char* szMessage, LogTypes eType = TYPE_MESSAGE) = 0;
	};

}
