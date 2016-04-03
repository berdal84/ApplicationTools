////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/File.h>

#include <plr/def.h>
#include <plr/log.h>
#include <plr/platform.h>
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
		PLR_PLATFORM_VERIFY(CloseHandle((HANDLE)m_handle));
	}
}

FileImpl::FileImpl(FileImpl&& _file_)
	: m_handle((void*)INVALID_HANDLE_VALUE)
{
	swap(_file_);
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
	PLR_ASSERT(_path);

	bool ret = false;
	const char* err = "";
	char* data = 0;

 	HANDLE h = CreateFile(
		_path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (h == INVALID_HANDLE_VALUE) {
		err = GetPlatformErrorString(GetLastError());
		goto FileImpl_Load_end;
	}

	LARGE_INTEGER li;
	if (!GetFileSizeEx(h, &li)) {
		err = GetPlatformErrorString(GetLastError());
		goto FileImpl_Load_end;
	}
	DWORD dataSize = (DWORD)li.QuadPart; // ReadFile can only read DWORD bytes

	data = new char[dataSize + 2]; // +2 for null terminator
	PLR_ASSERT(data);
	if (!ReadFile(h, data, dataSize, 0, 0)) {
		err = GetPlatformErrorString(GetLastError());
		goto FileImpl_Load_end;
	}
	data[dataSize] = data[dataSize + 1] = 0;

	ret = true;
	file_->~FileImpl(); // close existing handle, release data
	file_->m_data = data;
	file_->m_dataSize = (uint64)dataSize;
	file_->setPath(_path);

FileImpl_Load_end:
	if (!ret) {
		if (data) {
			delete[] data;
		}
		PLR_LOG_ERR("Error loading '%s':\n\t%s", _path, err);
		PLR_ASSERT(false);
	}
	if (h != INVALID_HANDLE_VALUE) {
		PLR_PLATFORM_VERIFY(CloseHandle(h));
	}
	return ret;
}