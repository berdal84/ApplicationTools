////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#ifndef plr_File_h
	#error plr: Do not include plr/FileBase.h, include plr/File.h
#endif

#include <plr/def.h>
#include <plr/FilePath.h>

namespace plr { namespace internal {

////////////////////////////////////////////////////////////////////////////////
/// \class FileBase
/// Base class for File implementations.
/// \internal \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class FileBase
{
public:
	const char* getPath() const       { return m_path; }
	const char* getData() const       { return m_data; }
	char* getData()                   { return m_data; }
	uint getDataSize() const          { return m_dataSize; }

	void setPath(const char* _path);
	void setData(const char* _data, uint _size);

protected:
	FileBase();
	~FileBase();

	void swap(FileBase& _file_);

	char  m_path[PLR_MAX_PATH_LENGTH];
	char* m_data;
	uint  m_dataSize;

}; // class FileBase

} } // namespace plr::internal
