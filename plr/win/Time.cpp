////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/time.h>

#include <plr/win/win.h>

#include <iomanip>
#include <sstream>

using namespace plr;

/*******************************************************************************
	
                                 Time

*******************************************************************************/
struct Time_Globals
{
	sint64    m_sysFreq;
	Timestamp m_appInit;
};
Time_Globals* g_timeGlobals;

Timestamp Time::GetTimestamp() 
{
	LARGE_INTEGER t;
	/*PLR_SYS_VERIFY(*/QueryPerformanceCounter(&t)/*)*/;
	return Timestamp(t.QuadPart);
}

Timestamp Time::GetApplicationElapsed()
{
	return GetTimestamp() - g_timeGlobals->m_appInit;
}

DateTime Time::GetDateTime() 
{
	FILETIME ft;
	GetSystemTimePreciseAsFileTime(&ft);
	// MS docs recommend copying into a LARGE_INTEGER first
	LARGE_INTEGER li;
	li.LowPart  = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	return DateTime(li.QuadPart);
}

sint64 Time::GetSystemFrequency() 
{
	return g_timeGlobals->m_sysFreq;
}

void Time::Init()
{
	LARGE_INTEGER f;
	/*PLR_SYS_VERIFY(*/QueryPerformanceFrequency(&f)/*)*/;
	g_timeGlobals = new Time_Globals;
	g_timeGlobals->m_sysFreq= f.QuadPart;
	g_timeGlobals->m_appInit = GetTimestamp();
}

void Time::Shutdown()
{
	delete g_timeGlobals;
}
PLR_DEFINE_STATIC_INIT(Time);

/*******************************************************************************
	
                                 Timestamp

*******************************************************************************/

double Timestamp::asSeconds() const
{
	return asMicroseconds() / 1000000.0;
}

double Timestamp::asMilliseconds() const
{
	return asMicroseconds() / 1000.0;
}

double plr::Timestamp::asMicroseconds() const
{
	return (double)((m_raw * 1000000ll) / Time::GetSystemFrequency());
}



/*******************************************************************************
	
                                   DateTime

*******************************************************************************/

static SYSTEMTIME GetSystemTime(plr::sint64 raw)
{
	LARGE_INTEGER li;
	li.QuadPart = raw;
	FILETIME ft;
	ft.dwLowDateTime  = li.LowPart;
	ft.dwHighDateTime = li.HighPart;
	SYSTEMTIME st;
	FileTimeToSystemTime(&ft, &st);
	return st;
}

sint32 DateTime::getYear() const         { return (sint32)GetSystemTime(m_raw).wYear; }
sint32 DateTime::getMonth() const        { return (sint32)GetSystemTime(m_raw).wMonth; }
sint32 DateTime::getDay() const          { return (sint32)GetSystemTime(m_raw).wDay; }
sint32 DateTime::getHour() const         { return (sint32)GetSystemTime(m_raw).wHour; }
sint32 DateTime::getMinute() const       { return (sint32)GetSystemTime(m_raw).wMinute; }
sint32 DateTime::getSecond() const       { return (sint32)GetSystemTime(m_raw).wSecond; }
sint32 DateTime::getMillisecond() const  { return (sint32)GetSystemTime(m_raw).wMilliseconds; }

/*std::string plr::DateTime::asString(const char* _format) const
{
	SYSTEMTIME st = GetSystemTime(m_raw);
	std::stringstream ss;
	if (!_format) {
	 // default ISO 8601 format
		ss	<< std::setw(4) << st.wYear << '-' 
			<< std::setw(2) << std::setfill('0') << st.wMonth << '-' 
			<< std::setw(2) << std::setfill('0') << st.wDay << 'T'
			<< std::setw(2) << std::setfill('0') << st.wHour << ':' 
			<< std::setw(2) << std::setfill('0') << st.wMinute << ':' 
			<< std::setw(2) << std::setfill('0') << st.wSecond << 'Z';
	} else {
		for (int i = 0; _format[i] != 0; ++i) {
			if (_format[i] == '%') {
			 // insert date time component into stream
				switch (_format[++i]) {
				case 'Y': ss << std::setw(4) << st.wYear; break;
				case 'm': ss << std::setw(2) << std::setfill('0') << st.wMonth; break;
				case 'd': ss << std::setw(2) << std::setfill('0') << st.wDay; break;
				case 'H': ss << std::setw(2) << std::setfill('0') << st.wHour; break;
				case 'M': ss << std::setw(2) << std::setfill('0') << st.wMinute; break;
				case 'S': ss << std::setw(2) << std::setfill('0') << st.wSecond; break;
				case 's': ss << std::setw(4) << std::setfill('0') << st.wMilliseconds; break;
				default:
				 // write char if not null
					if (_format[i] != 0)
						ss << _format[i];
				};
			} else {
			 // otherwise write char
				ss << _format[i];
			}
		}
	}

	return ss.str();
}*/

