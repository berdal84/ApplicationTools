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
/// Files loaded into memory via Read() have an implicit null character appended
/// to the internal data buffer, hence getData() can be interpreted directly as
/// C string.
/// \todo API should include some interface for either writing to the internal 
///   buffer directly, or setting the buffer ptr without copying all the data
///   (prefer the former, buffer ownership issues in the latter case).
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
	
	File(File&& _file_): Impl(std::move(_file_))                {}
	File& operator=(File&& _file_)                              { Impl::swap(_file_); return *this; }

	/// \return true if _path exists.
	static bool Exists(const char* _path)                       { return Impl::Exists(_path); }

	/// Read file into memory from _path, or file_.getPath() if _path is 0.
	/// Use getData() to access the resulting buffer. 
	/// \note An implicit null is appended to the data buffer, hence getData()
	///    can be interpreted directly as a C string.
	/// \return false if an error occurred, in which case file_ remains unchanged.
	///    On success, any resources already associated with file_ are released.
	static bool Read(File& file_, const char* _path = 0)        { return Impl::Read(file_, _path); }

	/// Write file to _path, or _file.getPath() if _path is 0.
	/// \return false if an error occurred, in which case any existing file at 
	///    _path may or may not have been overwritten.
	static bool Write(const File& _file, const char* _path = 0) { return Impl::Write(_file, _path); }

	const char* getPath() const                                 { return Impl::getPath(); }
	void        setPath(const char* _path)                      { Impl::setPath(_path); }
	const char* getData() const                                 { return Impl::getData(); }
	char*       getData()                                       { return Impl::getData(); }
	void        setData(const char* _data, uint64 _size)        { Impl::setData(_data, _size); }
	uint64      getDataSize() const                             { return Impl::getDataSize(); }
	void        setDataSize(uint64 _size)                       { Impl::setData(0, _size); }

}; // class File

} // namespace plr

#endif // plr_File_h
