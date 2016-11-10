#pragma once
#ifndef apt_ArgList_h
#define apt_ArgList_h

#include <apt/def.h>

#include <vector>

namespace apt {

////////////////////////////////////////////////////////////////////////////////
/// \class Arg
/// Arg associates a name with one or more values.
////////////////////////////////////////////////////////////////////////////////
class Arg
{
public:
	class Value
	{
		friend class Arg;
		const char* m_value;
		Value(const char* _value): m_value(_value) {}
	public:
		operator const char*() const { return m_value; }

		bool asBool() const;
		sint64 asInt() const;
		double asDouble() const;
		const char* asString() const;
	};

	/// Init with _name (can be null).
	Arg(char* _name): m_name(_name ? _name : "")   {}

	const char* getName() const        { return m_name; }
	uint getValueCount() const         { return (uint)m_values.size(); }
	void pushValue(const char* _val)   { m_values.push_back(_val); }
	Value getValue(int _i = 0) const   { APT_ASSERT(_i < m_values.size()); return Value(m_values[_i]); }
	Value operator[](int _i) const     { APT_ASSERT(_i < m_values.size()); return Value(m_values[_i]); }
	
private:
	const char* m_name;
	std::vector<const char*> m_values;
};

////////////////////////////////////////////////////////////////////////////////
/// \class ArgList
/// Elements of _argv beginning with '-' are interpreted as arg names; all other
/// elements are interpreted as arg values.
////////////////////////////////////////////////////////////////////////////////
class ArgList
{
public:
	/// Init from command line arguments.
	ArgList(int _argc, char* _argv[]);

	uint getArgCount() const             { return (uint)m_args.size(); }
	const Arg& getArg(int _i)            { APT_ASSERT(_i < m_args.size()); return m_args[_i]; }
	const Arg& operator[](int _i) const  { APT_ASSERT(_i < m_args.size()); return m_args[_i]; }

	/// \return Arg with matching _name, or 0 if not present.
	const Arg* find(const char* _name) const;

private:
	std::vector<Arg> m_args;
};

} // namespace apt

#endif // apt_ArgList_h
