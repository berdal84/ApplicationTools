#pragma once
#ifndef plr_time_h
#define plr_time_h

#include <plr/def.h>
#include <plr/log.h>
#include <plr/static_initializer.h>

namespace plr {

class Timestamp;
class DateTime;

////////////////////////////////////////////////////////////////////////////////
/// \class Time
/// Global time functions.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class Time
{
public:
	/// \return High-resolution time stamp.
	/// \note This is not synchronised to an external time reference. Use for
	///   interval measurements.
	/// \ingroup plr_core
	static Timestamp GetTimestamp();
	
	/// \return High-resolution date time.
	/// \note This is synchronized to UTC.
	/// \ingroup plr_core
	static DateTime GetDateTime();
	
	/// \return Frequency of the system timer in ticks/second.
	/// \ingroup plr_core
	static sint64 GetSystemFrequency();
	
	/// \return Interval since the application began.
	/// \ingroup plr_core
	static Timestamp GetApplicationElapsed();

protected:
	static void Init();
	static void Shutdown();

}; // class Time
PLR_DECLARE_STATIC_INIT(Time);

////////////////////////////////////////////////////////////////////////////////
/// \class Timestamp
/// High-resolution unsynchronized timestamp, with functions for converting 
/// between ticks and seconds/milliseconds/microseconds. Use for interval
/// measurments.
/// \ingroup plr_core
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
/// \ingroup plr_core
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
	/// \note Returns a ptr to a thread-local static buffer - for normal use
	///   this should be fine, just print the string and don't keep the ptr.
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
/// interval in the dtor. Use PLR_TIME_DBG to declare an AutoTimer instance for 
/// debug builds only.
/// \ingroup plr_core
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
		PLR_LOG("%s -- %fms", m_name, interval.asMilliseconds());
	}
};

#define PLR_AUTOTIMER(name) plr::AutoTimer PLR_UNIQUE_NAME(_plrAutoTimer_)(name)
#ifdef PLR_DEBUG
	#define PLR_AUTOTIMER_DBG(name) PLR_AUTOTIMER(name)
#else
	#define PLR_AUTOTIMER_DBG(name) PLR_UNUSED(name)
#endif

} // namespace plr

#endif // plr_Time_h
