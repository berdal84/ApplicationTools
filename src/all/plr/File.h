////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_File_h
#define plr_File_h

#include <plr/def.h>
#include <plr/FileImpl.h>
#include <plr/FilePath.h>

#include <utility> // std::move

namespace plr {

////////////////////////////////////////////////////////////////////////////////
/// \class File
/// Noncopyable but movable.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class File
	: public internal::FileImpl
	, private non_copyable<File>
{
	typedef internal::FileImpl Impl;
public:
	File();
	~File();

	File(File&& _file_): Impl(std::move(_file_)) {}
	File& operator=(File&& _file_) { Impl::swap(_file_); return *this; }

	/// \return true if _path exists.
	static bool Exists(const char* _path) { return Impl::Exists(_path); }

	/// Load entire file into memory.
	/// \return false if an error occurred, in which case file_ remains unchanged.
	static bool Load(File* file_, const char* _path) { return Impl::Load(file_, _path); }

}; // class File

} // namespace plr

#endif // plr_File_h
