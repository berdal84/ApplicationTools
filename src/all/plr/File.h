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

#include <utility> // std::move

namespace plr {

////////////////////////////////////////////////////////////////////////////////
/// \class File
/// Noncopyable but movable.
/// Files loaded into memory via Load() have an implicit null character appended
/// to the internal data buffer, hence getData() can be interpreted directly as
/// C string.
/// \todo Checksum/hash util function.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class File
	: public internal::FileImpl
	, private non_copyable<File>
{
	typedef internal::FileImpl Impl;
public:
	File()  {}
	~File() {}
	
	File(File&& _file_): Impl(std::move(_file_))     {}
	File& operator=(File&& _file_)                   { Impl::swap(_file_); return *this; }

	/// \return true if _path exists.
	static bool Exists(const char* _path)            { return Impl::Exists(_path); }

	/// Load entire file into memory, use getData() to access the resulting 
	/// buffer. An implicit null is appended to the data buffer, hence getData()
	/// can be interpreted directly as a C string.
	/// \return false if an error occurred, in which case file_ remains unchanged.
	///    On success, any resources already associated with file_ are released.
	static bool Load(File* file_, const char* _path) { return Impl::Load(file_, _path); }

	
	const char* getPath() const                      { return Impl::getPath(); }
	const char* getData() const                      { return Impl::getData(); }
	char* getData()                                  { return Impl::getData(); }
	uint64 getDataSize() const                       { return Impl::getDataSize(); }
	void setPath(const char* _path)                  { Impl::setPath(_path); }
	void setData(const char* _data, uint64 _size)    { Impl::setData(_data, _size); }

}; // class File

} // namespace plr

#endif // plr_File_h
