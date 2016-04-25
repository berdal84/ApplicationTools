////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#ifndef plr_File_h
	#error plr: Do not include plr/FileImpl.h, include plr/File.h
#endif

#include <plr/FileBase.h>

namespace plr { namespace internal {
	 
////////////////////////////////////////////////////////////////////////////////
/// \class FileImpl
/// \internal \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class FileImpl: public internal::FileBase
{
protected:
	FileImpl();
	~FileImpl();
	FileImpl(FileImpl&& _file_);
	void swap(FileImpl& _file_);
	
	static bool Exists(const char* _path);
	static bool Read(FileImpl* file_, const char* _path);
	static bool Write(const FileImpl* _file, const char* _path);

private:
	void* m_handle; // HANDLE

}; // class FileImpl

} }// namespace plr::intenral
