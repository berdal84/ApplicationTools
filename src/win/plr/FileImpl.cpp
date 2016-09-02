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
#include <plr/String.h>

#include <cstdlib> // malloc, free
#include <utility> // swap

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

bool FileImpl::Exists(const char* _path)
{
	return GetFileAttributes(_path) != INVALID_FILE_ATTRIBUTES;
}

bool FileImpl::Read(FileImpl& file_, const char* _path)
{
	if (!_path) {
		_path = file_.getPath();
	}
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
		goto FileImpl_Read_end;
	}

	LARGE_INTEGER li;
	if (!GetFileSizeEx(h, &li)) {
		err = GetPlatformErrorString(GetLastError());
		goto FileImpl_Read_end;
	}
	DWORD dataSize = (DWORD)li.QuadPart; // ReadFile can only read DWORD bytes

	data = (char*)malloc(dataSize + 2); // +2 for null terminator
	PLR_ASSERT(data);
	DWORD bytesRead;
	if (!ReadFile(h, data, dataSize, &bytesRead, 0)) {
		err = GetPlatformErrorString(GetLastError());
		goto FileImpl_Read_end;
	}
	data[dataSize] = data[dataSize + 1] = 0;

	ret = true;
	file_.~FileImpl(); // close existing handle, release data
	file_.m_data = data;
	file_.m_dataSize = (uint64)dataSize;
	file_.setPath(_path);

FileImpl_Read_end:
	if (!ret) {
		if (data) {
			free(data);
		}
		PLR_LOG_ERR("Error reading '%s':\n\t%s", _path, err);
		PLR_ASSERT(false);
	}
	if (h != INVALID_HANDLE_VALUE) {
		PLR_PLATFORM_VERIFY(CloseHandle(h));
	}
	return ret;
}

bool FileImpl::Write(const FileImpl& _file, const char* _path)
{
	if (!_path) {
		_path = _file.getPath();
	}
	PLR_ASSERT(_path);

	bool ret = false;
	const char* err = "";
	char* data = 0;
	
 	HANDLE h = CreateFile(
		_path,
		GENERIC_WRITE,
		0, // prevent sharing while we write
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (h == INVALID_HANDLE_VALUE) {
		err = GetPlatformErrorString(GetLastError());
		goto FileImpl_Write_end;
	}

	DWORD bytesWritten;
	if (!WriteFile(h, _file.getData(), (DWORD)_file.getDataSize(), &bytesWritten, NULL)) {
		err = GetPlatformErrorString(GetLastError());
		goto FileImpl_Write_end;
	}
	PLR_ASSERT(bytesWritten == _file.getDataSize());

	ret = true;

FileImpl_Write_end:
	if (!ret) {
		PLR_LOG_ERR("Error writing '%s':\n\t%s", _path, err);
		PLR_ASSERT(false);
	}
	if (h != INVALID_HANDLE_VALUE) {
		PLR_PLATFORM_VERIFY(CloseHandle(h));
	}
	return ret;
}