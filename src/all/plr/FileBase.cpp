////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/File.h>

#include <cstring>
#include <utility>

using namespace plr;
using namespace internal;

// PUBLIC

void FileBase::setPath(const char* _path)
{
	strcpy(m_path, _path);
}

void FileBase::setData(const char* _data, uint _size)
{
	PLR_ASSERT(_data);
	PLR_ASSERT(_size > 0);
	if (m_data) {
		if (_size > m_dataSize) {
			delete[] m_data;
			m_data = new char[_size];
			PLR_ASSERT(m_data);
		}
	}
	memcpy(m_data, _data, _size);
}


// PROTECTED

FileBase::FileBase()
	: m_data(0)
	, m_dataSize(0u)
{
	memset(m_path, 0, PLR_ARRAY_LENGTH(m_path));
}

FileBase::~FileBase()
{
	if (m_data) {
		delete[] m_data;
	}
}

void FileBase::swap(FileBase& _file_)
{
	std::swap(m_path, _file_.m_path);
	std::swap(m_data, _file_.m_data);
	std::swap(m_dataSize, _file_.m_dataSize);
}