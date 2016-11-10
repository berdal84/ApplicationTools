#pragma once
#ifndef apt_File_h
#define apt_File_h

#include <apt/def.h>
#include <apt/FileImpl.h>

#include <utility> // std::move

namespace apt {

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
////////////////////////////////////////////////////////////////////////////////
class File
	: public internal::FileImpl
	, private non_copyable<File>
{
	typedef internal::FileImpl Impl;
public:
	File()  {}
	~File() {}

	/// \return true if _path exists.
	static bool Exists(const char* _path)                       { return Impl::Exists(_path); }

	/// Create the directory specified by _path, plus all parent directories
	/// if they do not exist.
	/// \note If _path contains only directory names, it must be appended by
	///   a path delimiter (e.g. "dir0/dir1/").
	/// \return false if an error occurred.
	static bool CreateDir(const char* _path)                    { return Impl::CreateDir(_path); }

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
	void        appendData(const char* _data, uint64 _size)     { Impl::appendData(_data, _size); }
	uint64      getDataSize() const                             { return Impl::getDataSize(); }
	void        setDataSize(uint64 _size)                       { Impl::setData(0, _size); }

}; // class File

} // namespace apt

#endif // apt_File_h
