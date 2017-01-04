#include <apt/FileSystem.h>

#include <apt/log.h>
#include <apt/File.h>

using namespace apt;

// PUBLIC

bool FileSystem::Read(File& file_, const char* _path, RootType _rootHint)
{
	PathStr fullPath;
	if (!FindExisting(fullPath, _path ? _path : file_.getPath(), _rootHint)) {
		APT_LOG_ERR("Error loading '%s':\n\tFile not found", _path);
		APT_ASSERT(false);
		return false;
	}
	return File::Read(file_, (const char*)fullPath);
}

bool FileSystem::ReadIfExists(File& file_, const char* _path, RootType _rootHint)
{
	PathStr fullPath;
	if (!FindExisting(fullPath, _path ? _path : file_.getPath(), _rootHint)) {
		return false;
	}
	return File::Read(file_, (const char*)fullPath);
}

bool FileSystem::Write(const File& _file, const char* _path, RootType _root)
{
	PathStr fullPath;
	MakePath(fullPath, _path ? _path : _file.getPath(), _root);
	return File::Write(_file, (const char*)fullPath);
}

bool FileSystem::Exists(const char* _path, RootType _rootHint)
{
	PathStr buf;
	return FindExisting(buf, _path, _rootHint);
}


// PRIVATE

FileSystem::PathStr FileSystem::s_roots[kRootTypeCount];

void FileSystem::MakePath(PathStr& ret_, const char* _path, RootType _root)
{
	APT_ASSERT(_root < kRootTypeCount);
	bool useRoot = !s_roots[_root].isEmpty();
	if (useRoot) {
	 // check if the root already exists in path as a directory
		const char* r = strstr(s_roots[_root], _path);
		if (!r || *(r + strlen(s_roots[_root])) != s_separator) {
			ret_.setf("%s%c%s", (const char*)s_roots[_root], s_separator, _path);
			return;
		}
	}
	ret_.set(_path);
}

bool FileSystem::FindExisting(PathStr& ret_, const char* _path, RootType _rootHint)
{
	for (int r = (int)_rootHint; r != -1; --r) {
		MakePath(ret_, _path, (RootType)r);
		if (File::Exists((const char*)ret_)) {
			return true;
		}
	}
	return false;
}
