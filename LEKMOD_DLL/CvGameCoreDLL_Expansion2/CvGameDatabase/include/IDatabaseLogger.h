
#pragma once

namespace Database
{
	class IDatabaseLogger
	{
	public:

		enum LogTypes
		{
			TYPE_MESSAGE = 0,
			TYPE_WARNING = 1,
			TYPE_ERROR = 2
		};


		virtual ~IDatabaseLogger(){};

		virtual void Log(const char* szMessage, LogTypes eType = TYPE_MESSAGE) = 0;
	};

}
