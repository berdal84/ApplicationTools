#pragma once
#ifndef apt_time_h
#define apt_time_h

#include <apt/def.h>
#include <apt/log.h>
#include <apt/static_initializer.h>

namespace apt {

class Timestamp;
class DateTime;

////////////////////////////////////////////////////////////////////////////////
/// \class Time
/// Global time functions.
////////////////////////////////////////////////////////////////////////////////
class Time
{
public:
	/// \return High-resolution time stamp.
	/// \note This is not synchronised to an external time reference. Use for
	///   interval measurements.
	static Timestamp GetTimestamp();
	
	/// \return High-resolution date time.
	/// \note This is synchronized to UTC.
	static DateTime GetDateTime();
	
	/// \return Frequency of the system timer in ticks/second.
	static sint64 GetSystemFrequency();
	
	/// \return Interval since the application began.
	static Timestamp GetApplicationElapsed();

	static void Init();
	static void Shutdown();

}; // class Time
APT_DECLARE_STATIC_INIT(Time, Time::Init, Time::Shutdown);

////////////////////////////////////////////////////////////////////////////////
/// \class Timestamp
/// High-resolution unsynchronized timestamp, with functions for converting 
/// between ticks and seconds/milliseconds/microseconds. Use for interval
/// measurments.
////////////////////////////////////////////////////////////////////////////////
class Timestamp
{
	friend class Time;
public:
	/// Default/value-initializing ctor.
	Timestamp(sint64 _raw = 0ll): m_raw(_raw) {}

	/// \return raw time value in system-dependent units.
	sint64 getRaw() const { return m_raw; }

	/// \return time value in seconds.
	double asSeconds() const;

	/// \return time value in milliseconds (10^-3s).
	double asMilliseconds() const;

	/// \return time value in microseconds (10^-6s).
	double asMicroseconds() const;
	

	const Timestamp operator- (const Timestamp& rhs) const  { return m_raw -  rhs.m_raw; }
	const Timestamp operator+ (const Timestamp& rhs) const  { return m_raw +  rhs.m_raw; }
	Timestamp&      operator-=(const Timestamp& rhs)        { m_raw -= rhs.m_raw; return *this; }
	Timestamp&      operator+=(const Timestamp& rhs)        { m_raw += rhs.m_raw; return *this; }

	bool            operator> (const Timestamp& rhs) const  { return m_raw >  rhs.m_raw; }
	bool            operator>=(const Timestamp& rhs) const  { return m_raw >= rhs.m_raw; }
	bool            operator< (const Timestamp& rhs) const  { return m_raw <  rhs.m_raw; }
	bool            operator<=(const Timestamp& rhs) const  { return m_raw <= rhs.m_raw; }

private:
	sint64 m_raw;

}; // class Timestamp


////////////////////////////////////////////////////////////////////////////////
/// \class DateTime
/// High-resolution datetime, synchronized to UTC.
////////////////////////////////////////////////////////////////////////////////
class DateTime
{
	friend class Time;
public:
	/// Default/value-initializing ctor.
	DateTime(sint64 _raw = 0ll): m_raw(_raw) {}

	/// \return raw time value in system-dependent units.
	sint64 getRaw() const { return m_raw; }

	/// \return year.
	sint32 getYear() const;
	/// \return month in [1,12].
	sint32 getMonth() const;
	/// \return day of month in [1,31].
	sint32 getDay() const;
	/// \return hour of day in [0,23].
	sint32 getHour() const;
	/// \return minute of hour in [0,59].
	sint32 getMinute() const;
	/// \return second.
	sint32 getSecond() const;
	/// \return millisecond (system-dependent resolution).
	sint32 getMillisecond() const;

	/// \return a formatted string. The default formatting is ISO 8601, however
	///    a format string may be supplied using the following specifiers:
	///  <table>
	///    <tr><th>Specifier</th><th>Value</th></tr>
	///    <tr><td>%%d</td><td>Zero-padded day of month in [01,31]</td></tr>
	///    <tr><td>%%H</td><td>Zero-padded hour in [00,24]</td></tr>
	///    <tr><td>%%m</td><td>Zero-padded month in [01,12]</td></tr>	
	///    <tr><td>%%M</td><td>Zero-padded minute in [00,59]</td></tr>
	///    <tr><td>%%s</td><td>Zero-padded millisecond in [0000,9999]</td></tr>
	///    <tr><td>%%S</td><td>Zero-padded second in [00,59]</td></tr>
	///    <tr><td>%%Y</td><td>Year</td></tr>
	/// </table>
	/// E.g. ISO 8601 format would be "%Y-%m-%dT%H:%M:%SZ".
	/// \note Returns a ptr to a local static buffer - for normal use this should 
	///    be fine, just print the string and don't keep the ptr.
	const char*    asString(const char* _format = 0) const;

	const DateTime operator- (const DateTime& rhs) const  { return m_raw -  rhs.m_raw; }
	const DateTime operator+ (const DateTime& rhs) const  { return m_raw +  rhs.m_raw; }
	DateTime&      operator-=(const DateTime& rhs)        { m_raw -= rhs.m_raw; return *this; }
	DateTime&      operator+=(const DateTime& rhs)        { m_raw += rhs.m_raw; return *this; }

	bool           operator> (const DateTime& rhs) const  { return m_raw >  rhs.m_raw; }
	bool           operator>=(const DateTime& rhs) const  { return m_raw >= rhs.m_raw; }
	bool           operator< (const DateTime& rhs) const  { return m_raw <  rhs.m_raw; }
	bool           operator<=(const DateTime& rhs) const  { return m_raw <= rhs.m_raw; }

private:	
	sint64 m_raw;

}; // class DateTime

////////////////////////////////////////////////////////////////////////////////
/// \class AutoTimer
/// Scoped timer. Measures the time between ctor, dtor calls and logs the
/// interval in the dtor. Use APT_TIME_DBG to declare an AutoTimer instance for 
/// debug builds only.
////////////////////////////////////////////////////////////////////////////////
class AutoTimer
{
	Timestamp   m_start;
	const char* m_name;
public:
	AutoTimer(const char* _name)
		: m_name(_name) 
	{ 
		m_start = Time::GetTimestamp(); 
	}
	~AutoTimer() 
	{ 
		Timestamp interval = Time::GetTimestamp() - m_start;
		APT_LOG("%s -- %fms", m_name, interval.asMilliseconds());
	}
};

#define APT_AUTOTIMER(name) apt::AutoTimer APT_UNIQUE_NAME(_aptAutoTimer_)(name)
#ifdef APT_DEBUG
	#define APT_AUTOTIMER_DBG(name) APT_AUTOTIMER(name)
#else
	#define APT_AUTOTIMER_DBG(name) APT_UNUSED(name)
#endif

} // namespace apt

#endif // APT_Time_h
