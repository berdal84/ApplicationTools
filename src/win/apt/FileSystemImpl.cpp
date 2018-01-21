#include <apt/FileSystem.h>

#include <apt/log.h>
#include <apt/platform.h>
#include <apt/win.h>
#include <apt/String.h>
#include <apt/TextParser.h>

#include <Shlwapi.h>
#include <commdlg.h>
#include <cstring>

#include <EASTL/vector.h>

#pragma comment(lib, "shlwapi")

using namespace apt;

// Windows expects a list of string pairs to pass to GetOpenFileName().
static void BuildFilterString(const char* _filters, StringBase& ret_)
{
	TextParser tp = _filters;
	while (*tp != '\0') {
		ret_.appendf("%s#%s#", (const char*)tp, (const char*)tp);
		tp.advanceToNext('\0');
		tp.advance();
	}
	ret_.replace('#', '\0'); // \hack
}

static DateTime FileTimeToDateTime(const FILETIME& _fileTime)
{
	LARGE_INTEGER li;
	li.LowPart  = _fileTime.dwLowDateTime;
	li.HighPart = _fileTime.dwHighDateTime;
	return DateTime(li.QuadPart);
}

static bool GetFileDateTime(const char* _fullPath, DateTime& created_, DateTime& modified_)
{
#if 0
	const char* err = nullptr;
	HANDLE h = CreateFile(
		_fullPath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
		);
	if (h == INVALID_HANDLE_VALUE) {
		err = GetPlatformErrorString(GetLastError());
		goto GetFileDateTime_End;
	}
	FILETIME created, modified;
	if (GetFileTime(h, &created, NULL, &modified) == 0) {
		err = GetPlatformErrorString(GetLastError());
		goto GetFileDateTime_End;
	}	
	created_ = FileTimeToDateTime(created);
	modified_ = FileTimeToDateTime(modified);
	
GetFileDateTime_End:
	if (err) {
		APT_LOG_ERR("GetFileDateTime: %s", err);
		APT_ASSERT(false);
	}
	APT_PLATFORM_VERIFY(CloseHandle(h));
	return err == nullptr;
#else
	const char* err = nullptr;
	WIN32_FILE_ATTRIBUTE_DATA attr;
	if (GetFileAttributesEx(_fullPath, GetFileExInfoStandard, &attr) == 0) {
		err = GetPlatformErrorString(GetLastError());
		goto GetFileDateTime_End;
	}
	created_ = FileTimeToDateTime(attr.ftCreationTime);
	modified_ = FileTimeToDateTime(attr.ftLastWriteTime);
GetFileDateTime_End:
	if (err) {
		APT_LOG_ERR("GetFileDateTime: %s", err);
		APT_ASSERT(false);
	}
	return err == nullptr;
#endif
}

static void GetAppPath(TCHAR ret_[MAX_PATH], const char* _append = nullptr)
{
	TCHAR tmp[MAX_PATH];

	APT_PLATFORM_VERIFY(GetModuleFileName(0, tmp, MAX_PATH));
	APT_PLATFORM_VERIFY(GetFullPathName(tmp, MAX_PATH, ret_, NULL)); // GetModuleFileName can return a relative path (e.g. when launching from the ide)

	if (_append) {
		APT_PLATFORM_VERIFY(GetFullPathName(_append, MAX_PATH, tmp, NULL));
		char* pathEnd = strrchr(ret_, (int)'\\');
		++pathEnd;
		strcpy(pathEnd, _append);
	} else {
		char* pathEnd = strrchr(ret_, (int)'\\');
		++pathEnd;
		*pathEnd = '\0';
	}
}

// PUBLIC

bool FileSystem::Delete(const char* _path)
{
	if (DeleteFile(_path) == 0) {
		DWORD err = GetLastError();
		if (err != ERROR_FILE_NOT_FOUND) {
			APT_LOG_ERR("DeleteFile(%s): %s", _path, GetPlatformErrorString(err));
		}
		return false;
	}
	return true;
}

DateTime FileSystem::GetTimeCreated(const char* _path, RootType _rootHint)
{
	PathStr fullPath;
	if (!FindExisting(fullPath, _path, _rootHint)) {
		return DateTime(); // \todo return invalid sentinel
	}
	DateTime created, modified;
	GetFileDateTime((const char*)fullPath, created, modified);
	return created;
}

DateTime FileSystem::GetTimeModified(const char* _path, RootType _rootHint)
{
	PathStr fullPath;
	if (!FindExisting(fullPath, _path, _rootHint)) {
		return DateTime(); // \todo return invalid sentinel
	}
	DateTime created, modified;
	GetFileDateTime((const char*)fullPath, created, modified);
	return modified;
}

void FileSystem::MakeRelative(StringBase& ret_, const char* _path, RootType _root)
{
	TCHAR root[MAX_PATH] = {};
	if (IsAbsolute((const char*)s_roots[_root])) {
		APT_PLATFORM_VERIFY(GetFullPathName((const char*)s_roots[_root], MAX_PATH, root, NULL));
	} else {
		GetAppPath(root, (const char*)s_roots[_root]);
	}

 // construct the full path
	TCHAR path[MAX_PATH] = {};
	APT_PLATFORM_VERIFY(GetFullPathName(_path, MAX_PATH, path, NULL));

	char tmpbuf[MAX_PATH];
	char* tmp = tmpbuf;
 // PathRelativePathTo will fail if tmp and root don't share a common prefix
	APT_VERIFY(PathRelativePathTo(tmp, root, FILE_ATTRIBUTE_DIRECTORY, path, PathIsDirectory(path) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL));
	if (strncmp(tmp, ".\\", 2) == 0) { // remove "./" from the path start - not strictly necessary but it makes nicer looking relative paths for the most common case
		tmp += 2;
	}	
	ret_.set(tmp);
	ret_.replace('\\', s_separator);
}

bool FileSystem::IsAbsolute(const char* _path)
{
	return PathIsRelative(_path) == FALSE;
}

void FileSystem::StripRoot(StringBase& ret_, const char* _path)
{
	TCHAR path[MAX_PATH] = {};
	APT_PLATFORM_VERIFY(GetFullPathName(_path, MAX_PATH, path, NULL));

	for (int r = 0; r < RootType_Count; ++r) {
		if (s_rootLengths[r] == 0) {
			continue;
		}
		TCHAR root[MAX_PATH];
		if (IsAbsolute((const char*)s_roots[r])) {
			APT_PLATFORM_VERIFY(GetFullPathName((const char*)s_roots[r], MAX_PATH, root, NULL));
		} else {
			GetAppPath(root, (const char*)s_roots[r]);
		}
		const char* rootBeg = strstr(path, root);
		if (rootBeg != nullptr) {
			ret_.set(path + strlen(root) + 1);
			ret_.replace('\\', s_separator);
			return;
		}
	}
 // no root found, strip the whole path if not absolute
	if (!IsAbsolute(_path)) {
		StripPath(ret_, _path);
	}
}

bool FileSystem::PlatformSelect(PathStr& ret_, const char* _filters)
{
	static DWORD       s_filterIndex = 0;
	static const DWORD kMaxOutputLength = MAX_PATH;
	static char        s_output[kMaxOutputLength];

	PathStr filters;
	BuildFilterString(_filters, filters);

	OPENFILENAMEA ofn   = {};
	ofn.lStructSize     = sizeof(ofn);
	ofn.lpstrFilter     = (LPSTR)filters;
	ofn.nFilterIndex    = s_filterIndex;
	ofn.lpstrInitialDir = (LPSTR)s_roots[RootType_Application];
	ofn.lpstrFile       = s_output;
	ofn.nMaxFile        = kMaxOutputLength;
	ofn.lpstrTitle      = "File";
	ofn.Flags           = OFN_DONTADDTORECENT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	if (GetOpenFileName(&ofn) != 0) {
		s_filterIndex = ofn.nFilterIndex;
	 // parse s_output into results_
		ret_.set(s_output);
		ret_.replace('\\', '/'); // sanitize path for display
		return true;
	} else {
		DWORD err = CommDlgExtendedError();
		if (err != 0) {
			APT_LOG_ERR("GetOpenFileName (0x%x)", err);
			APT_ASSERT(false);
		}		
	}
	return false;
}

int FileSystem::PlatformSelectMulti(PathStr retList_[], int _maxResults, const char* _filters)
{
	static DWORD       s_filterIndex = 0;
	static const DWORD kMaxOutputLength = 1024 * 4;
	static char        s_output[kMaxOutputLength];

	PathStr filters;
	BuildFilterString(_filters, filters);

	OPENFILENAMEA ofn   = {};
	ofn.lStructSize     = sizeof(ofn);
	ofn.lpstrFilter     = (LPSTR)filters;
	ofn.nFilterIndex    = s_filterIndex;
	ofn.lpstrInitialDir = (LPSTR)s_roots[RootType_Application];
	ofn.lpstrFile       = s_output;
	ofn.nMaxFile        = kMaxOutputLength;	
	ofn.lpstrTitle      = "File";
	ofn.Flags           = OFN_ALLOWMULTISELECT | OFN_DONTADDTORECENT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	if (GetOpenFileName(&ofn) != 0) {
		s_filterIndex = ofn.nFilterIndex;

	 // parse s_output into results_
		int ret = 0;
		TextParser tp(s_output);
		while (ret < _maxResults) {
			tp.advanceToNext('\0');
			tp.advance();
			if (*tp == '\0') {
				break;
			}
			retList_[ret].appendf("%s\\%s", s_output, (const char*)tp);
			retList_[ret].replace('\\', '/'); // sanitize path for display
			++ret;
		}
		return ret;

	} else {
		DWORD err = CommDlgExtendedError();
		if (err != 0) {
			APT_LOG_ERR("GetOpenFileName (0x%x)", err);
			APT_ASSERT(false);
		}		
	}
	return 0;
}

int FileSystem::ListFiles(PathStr retList_[], int _maxResults, const char* _path, const char* _filters, bool _recursive)
{
	eastl::vector<PathStr> dirs;
	dirs.push_back(_path);
	int ret = 0;
	while (!dirs.empty()) {
		PathStr root = (PathStr&&)dirs.back();
		dirs.pop_back();
		root.replace('/', '\\');
		PathStr search = root;
		search.appendf("\\*"); // ignore filter here, need to catch dirs for recursion

		WIN32_FIND_DATA ffd;
		HANDLE h = FindFirstFile((const char*)search, &ffd);
		if (h == INVALID_HANDLE_VALUE) {
			DWORD err = GetLastError();
			if (err != ERROR_FILE_NOT_FOUND) {
				APT_LOG_ERR("ListFiles (FindFirstFile): %s", GetPlatformErrorString(err));
			}
			continue;
		} 

		do {
			if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0) {
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if (_recursive) {
						dirs.push_back(root);
						dirs.back().appendf("\\%s", ffd.cFileName);
					}
				} else {
					if (MatchesMulti(_filters, (const char*)ffd.cFileName)) {
						if (ret < _maxResults) {
							retList_[ret].setf("%s\\%s", (const char*)root, ffd.cFileName);
						}
						++ret;
					}
				}
			}
	
        } while (FindNextFile(h, &ffd) != 0);

		DWORD err = GetLastError();
		if (err != ERROR_NO_MORE_FILES) {
			APT_LOG_ERR("ListFiles (FindNextFile): %s", GetPlatformErrorString(err));
		}

		FindClose(h);
    }

	return ret;
}

int FileSystem::ListDirs(PathStr retList_[], int _maxResults, const char* _path, const char* _filters, bool _recursive)
{
	eastl::vector<PathStr> dirs;
	dirs.push_back(_path);
	int ret = 0;
	// \note there are 2 choices of behavior here: 'direct' recursion (where sub directories appear immediately after the parent in the list), or
	// 'deferred' recursion, which is what is implemented. In theory, the latter is better because you can fill a small list of the first couple of levels
	// of the hierarchy and then manually recurse into those directories as needed.
	while (!dirs.empty()) {
		PathStr root = (PathStr&&)dirs.back();
		dirs.pop_back();
		root.replace('/', '\\');
		PathStr search = root;
		search.appendf("\\*");
	
		WIN32_FIND_DATA ffd;
		HANDLE h = FindFirstFile((const char*)search, &ffd);
		if (h == INVALID_HANDLE_VALUE) {
			DWORD err = GetLastError();
			if (err != ERROR_FILE_NOT_FOUND) {
				APT_LOG_ERR("ListFiles (FindFirstFile): %s", GetPlatformErrorString(err));
			}
			continue;
		}

		do {
			if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0) {
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if (_recursive) {
						dirs.push_back(root);
						dirs.back().appendf("\\%s", ffd.cFileName);
					}
					if (MatchesMulti(_filters, (const char*)ffd.cFileName)) {
						if (ret < _maxResults) {
							retList_[ret].setf("%s\\%s", (const char*)root, ffd.cFileName);
						}
						++ret;
					}
				}
			}
        } while (FindNextFile(h, &ffd) != 0);

		DWORD err = GetLastError();
		if (err != ERROR_NO_MORE_FILES) {
			APT_LOG_ERR("ListFiles (FindNextFile): %s", GetPlatformErrorString(err));
		}

		FindClose(h);
    }

	return ret;
}

bool FileSystem::CreateDir(const char* _path)
{
	TextParser tp(_path);
	while (tp.advanceToNext("\\/") != 0) {
		String<64> mkdir;
		mkdir.set(_path, tp.getCharCount());
		if (CreateDirectory((const char*)mkdir, NULL) == 0) {
			DWORD err = GetLastError();
			if (err != ERROR_ALREADY_EXISTS) {
				APT_LOG_ERR("CreateDirectory(%s): %s", _path, GetPlatformErrorString(err));
				return false;
			}
		}
		tp.advance(); // skip the delimiter
	}
	return true;
}

// PROTECTED

const char FileSystem::s_separator = '/';
