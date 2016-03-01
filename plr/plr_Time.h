////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#ifndef plr_Time_h
#define plr_Time_h

#include <plr/plr_def.h>

namespace plr {

class Timestamp;
class DateTime;

////////////////////////////////////////////////////////////////////////////////
/// \class Time
/// Global time functions.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class Time: private non_instantiable<Time>
{
public:
	/// \return high-resolution timestamp.
	/// \note This is not synchronised to an external time reference. Use for
	///    interval measurements.
	static Timestamp GetTimestamp();

	/// \return high-resolution datetime.
	/// \note This is synchronized to UTC.
	static DateTime GetDateTime();

	/// \return frequency of the system timer in ticks/second.
	static sint64 GetSystemFrequency();
	
}; // class time


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

	/// \return time value in milliseconds (thousandths of a second).
	double asMilliseconds() const;

	/// \return time value in microseconds (millionths of a second).
	double asMicroseconds() const;

	const Timestamp operator- (const Timestamp& rhs) const  { return m_raw -  rhs.m_raw; }
	const Timestamp operator+ (const Timestamp& rhs) const  { return m_raw +  rhs.m_raw; }
	Timestamp& operator-=(const Timestamp& rhs)             { m_raw -= rhs.m_raw; return *this; }
	Timestamp& operator+=(const Timestamp& rhs)             { m_raw += rhs.m_raw; return *this; }

	bool operator> (const Timestamp& rhs) const             { return m_raw >  rhs.m_raw; }
	bool operator>=(const Timestamp& rhs) const             { return m_raw >= rhs.m_raw; }
	bool operator< (const Timestamp& rhs) const             { return m_raw <  rhs.m_raw; }
	bool operator<=(const Timestamp& rhs) const             { return m_raw <= rhs.m_raw; }

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
	///  </table>
	/// So, for example, ISO 8601 format would be "%Y-%m-%dT%H:%M:%SZ".
	std::string asString(const char* _format = 0) const;

	const DateTime operator- (const DateTime& rhs) const  { return m_raw -  rhs.m_raw; }
	const DateTime operator+ (const DateTime& rhs) const  { return m_raw +  rhs.m_raw; }
	DateTime& operator-=(const DateTime& rhs)             { m_raw -= rhs.m_raw; return *this; }
	DateTime& operator+=(const DateTime& rhs)             { m_raw += rhs.m_raw; return *this; }

	bool operator> (const DateTime& rhs) const            { return m_raw >  rhs.m_raw; }
	bool operator>=(const DateTime& rhs) const            { return m_raw >= rhs.m_raw; }
	bool operator< (const DateTime& rhs) const            { return m_raw <  rhs.m_raw; }
	bool operator<=(const DateTime& rhs) const            { return m_raw <= rhs.m_raw; }

private:	
	sint64 m_raw;

}; // class DateTime

} // namespace plr

#endif // plr_Time_h
