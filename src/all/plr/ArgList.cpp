////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/ArgList.h>

#include <cstdlib>
#include <cstring>
#include <cctype>

using namespace plr;

bool Arg::Value::asBool() const
{
	return !(m_value[0] == 'f' || m_value[0] == 'F' || m_value[0] == '0');
}

sint64 Arg::Value::asInt() const
{
	return (sint64)atol(m_value);
}

double Arg::Value::asDouble() const
{
	return atof(m_value);
}

const char* Arg::Value::asString() const
{
	return m_value;
}

ArgList::ArgList(int _argc, char* _argv[])
{
	int currentArg = -1;
	for (int i = 1; i < _argc; ++i) {
		if (_argv[i][0] == '-' && !isdigit(_argv[i][1])) {
		 // '-' indicates a new arg unless the following digit is a number (in which case it's a negative value)
			m_args.push_back(Arg(&_argv[i][1]));
			++currentArg;
		} else {
			if (currentArg == -1) {
			 // no args, push a null argument
				m_args.push_back(Arg(0));
				++currentArg;
			}
		 // push value into the current argument
			m_args[currentArg].pushValue(_argv[i]);
		}
	}
}

const Arg* ArgList::find(const char* _name) const
{
	for (auto arg = m_args.begin(); arg != m_args.end(); ++arg) {
		if (strcmp(_name, arg->getName()) == 0) {
			return (Arg*)&(*arg);
		}
	}
	return 0;
}
