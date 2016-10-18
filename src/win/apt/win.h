#pragma once
#ifndef apt_win_h
#define apt_win_h

#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN 1
#include <Windows.h>
#ifdef Yield
// WinBase.h defines a do-nothing Yield() macro which conflicts with Thread::Yield
	#undef Yield
#endif

#endif // apt_win_h
