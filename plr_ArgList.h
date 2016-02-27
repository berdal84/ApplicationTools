////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#ifndef plr_ArgList_h
#define plr_ArgList_h

#include "plr_def.h"
#include <vector>

namespace plr {

////////////////////////////////////////////////////////////////////////////////
/// \class Arg
/// Arg associates a name with one or more values.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class Arg
{
public:
	/// Init with _name (can be null).
	Arg(char* _name): m_name(_name ? _name : "")   {}

	const char* getName() const             { return m_name; }
	uint  getValueCount() const             { return (uint)m_values.size(); }
	void  pushValue(const char* _val)       { m_values.push_back(_val); }
	const char* getValue(uint i) const      { PLR_ASSERT(i < m_values.size()); return m_values[i]; }
	const char* operator[](uint i) const    { PLR_ASSERT(i < m_values.size()); return m_values[i]; }

private:
	const char* m_name;
	std::vector<const char*> m_values;
};

////////////////////////////////////////////////////////////////////////////////
/// \class ArgList
/// Elements of _argv beginning with '-' are interpreted as arg names; all other
/// elements are interpreted as arg values.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class ArgList
{
public:
	/// Init from command line arguments.
	ArgList(int _argc, char* _argv[]);

	uint getArgCount()                   { return (uint)m_args.size(); }
	const Arg& getArg(uint i)            { PLR_ASSERT(i < m_args.size()); return m_args[i]; }
	const Arg& operator[](uint i) const  { PLR_ASSERT(i < m_args.size()); return m_args[i]; }

	/// \return Arg with matching _name, or 0 if not present.
	const Arg* find(const char* _name) const;

private:
	std::vector<Arg> m_args;
};

} // namespace plr

#endif // plr_ArgList_h
