#include <apt/FileSystem.h>

#include <apt/log.h>
#include <apt/platform.h>
#include <apt/win.h>
#include <apt/String.h>
#include <apt/TextParser.h>

#include <Shlwapi.h>
#include <commdlg.h>
#include <cstring>

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


// PUBLIC

void FileSystem::MakeRelative(StringBase& ret_, const char* _path, RootType _root)
{
 // construct the full root
	TCHAR root[MAX_PATH] = {};
	{
		TCHAR tmp[MAX_PATH] = {};
		APT_PLATFORM_VERIFY(GetModuleFileName(0, tmp, MAX_PATH));
		APT_PLATFORM_VERIFY(GetFullPathName(tmp, MAX_PATH, root, NULL)); // required to resolve a relative path (e.g. when launching from the ide)
		char* pathEnd = strrchr(root, (int)'\\');
		++pathEnd;
		strcpy(pathEnd, s_roots[_root]);
	}
 // construct the full path
	TCHAR path[MAX_PATH] = {};
	APT_PLATFORM_VERIFY(GetFullPathName(_path, MAX_PATH, path, NULL));

	char tmpbuf[MAX_PATH];
	char* tmp = tmpbuf;
	APT_ASSERT(PathRelativePathTo(tmp, root, FILE_ATTRIBUTE_DIRECTORY, path, PathIsDirectory(path) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL));
	if (strncmp(tmp, ".\\", 2) == 0) { // remove "./" from the path start - not strictly necessary but it makes nicer looking relative paths for the most common case
		tmp += 2;
	}	
	ret_.set(tmp);
	ret_.replace('\\', s_separator);
}

bool FileSystem::PlatformSelect(StringBase& ret_, const char* _filters)
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
			APT_LOG_ERR("GetOpenFileName failed (0x%x)", err);
			APT_ASSERT(false);
		}		
	}
	return false;
}

int FileSystem::PlatformSelectMulti(StringBase* retList_, int _maxResults, const char* _filters)
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
			APT_LOG_ERR("GetOpenFileName failed (0x%x)", err);
			APT_ASSERT(false);
		}		
	}
	return 0;
}

// PROTECTED

const char FileSystem::s_separator = '/';
