////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/File.h>

#include <plr/def.h>
#include <plr/win.h>

#include <utility> // std::swap

using namespace plr;
using namespace internal;

FileImpl::FileImpl()
	: m_handle((void*)INVALID_HANDLE_VALUE)
{
}

FileImpl::~FileImpl()
{
	if ((HANDLE)m_handle != INVALID_HANDLE_VALUE) {
		CloseHandle((HANDLE)m_handle);
	}
}

void FileImpl::swap(FileImpl& _file_)
{
	std::swap(m_handle, _file_.m_handle);
	FileBase::swap(_file_);
}

bool FileImpl::Exists(const char* _path)
{
	return GetFileAttributes(_path) != INVALID_FILE_ATTRIBUTES;
}

bool FileImpl::Load(FileImpl* file_, const char* _path)
{
	PLR_ASSERT(file_);
	return true;
}