////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/plr_ArgList.h>

#include <cstring>

plr::ArgList::ArgList(int _argc, char* _argv[])
{
	int currentArg = -1;
	for (int i = 1; i < _argc; ++i) {
		if (_argv[i][0] == '-') {
		 // '-' indicates a new arg
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

const plr::Arg* plr::ArgList::find(const char* _name) const
{
	for (auto arg = m_args.begin(); arg != m_args.end(); ++arg) {
		if (strcmp(_name, arg->getName()) == 0) {
			return (Arg*)&(*arg);
		}
	}
	return 0;
}
