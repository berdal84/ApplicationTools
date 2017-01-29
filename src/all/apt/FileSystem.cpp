#include <apt/FileSystem.h>

#include <apt/log.h>
#include <apt/File.h>
#include <apt/String.h>

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

void FileSystem::StripRoot(PathStr& ret_, const char* _path)
{
	for (int r = 0; r < kRootTypeCount; ++r) {
		if (s_rootLengths[r] == 0) {
			continue;
		}
		const char* rootBeg = strstr(_path, s_roots[r]);
		if (rootBeg != nullptr) {
			ret_.set(rootBeg + s_rootLengths[r] + 1);
			return;
		}
	}
 // no root found, strip the whole path
	StripPath(ret_, _path);
}

void FileSystem::StripPath(PathStr& ret_, const char* _path)
{
	int i = 0, last = 0;
	while (_path[i] != '\0') {
		if (_path[i] == s_separator)
			last = i + 1;
		++i;
	}
	ret_.set(_path + i);
}

const char* FileSystem::GetExtension(const char* _path)
{
	const char* ret = strrchr(_path, (int)'.');
	return ret == nullptr ? nullptr : ret + 1;
}

// PRIVATE

FileSystem::PathStr FileSystem::s_roots[kRootTypeCount];
int FileSystem::s_rootLengths[kRootTypeCount];

void FileSystem::MakePath(PathStr& ret_, const char* _path, RootType _root)
{
	APT_ASSERT(_root < kRootTypeCount);
	bool useRoot = !s_roots[_root].isEmpty();
	if (useRoot) {
	 // check if the root already exists in path as a directory
		const char* r = strstr(s_roots[_root], _path);
		if (!r || *(r + s_rootLengths[_root]) != s_separator) {
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
