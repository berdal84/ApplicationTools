#pragma once
#ifndef apt_FileSystem_h
#define apt_FileSystem_h

#include <apt/def.h>
#include <apt/File.h>
#include <apt/String.h>

namespace apt {

////////////////////////////////////////////////////////////////////////////////
/// \class FileSystem
/// Manage file access. A fixed number of directory 'roots' may be set, which 
/// are searched in reverse order when resolving a path (e.g. kApplication is
/// checked before kCommon).
////////////////////////////////////////////////////////////////////////////////
class FileSystem
	: private non_instantiable<FileSystem>
{
public:
	typedef File::PathStr PathStr;

	enum RootType
	{
		kRoot,         //< By default the working directory of the executable.
		kCommon,       //< Shared path for common files.
		kApplication,  //< Application-specific files.

		kRootTypeCount,
		kDefaultRootType = kApplication
	};

	/// Get/set a path for the specified root type.
	static const char* GetRoot(RootType _type)                    { return s_roots[_type]; }
	static void        SetRoot(RootType _type, const char* _path) { s_roots[_type].set(_path); }

	/// Read a file into memory. Each root is searched, beginning at _rootHint. 
	/// If _path is 0, the path from file_ is used.
	/// \return false if an error occurred, in which case file_ remains unchanged.
	///    On success, any resources already associated with file_ are released.
	static bool        Read(File& file_, const char* _path = 0, RootType _rootHint = kDefaultRootType);

	/// As Read() but first checks if the file exists.
	/// \return false if the file does not exist or if an error occurred.
	static bool        ReadIfExists(File& file_, const char* _path = 0, RootType _rootHint = kDefaultRootType);

	/// Write _file's data to _path. If _path is 0, the path from _file is used.
	/// \return false if an error occurred, in which case any existing file at 
	///    _path may or may not have been overwritten.
	static bool        Write(const File& _file, const char* _path = 0, RootType _root = kDefaultRootType);

	/// \retrn true if _path exists. Each root is searched, beginning at _rootHint.
	static bool        Exists(const char* _path, RootType _rootHint = kDefaultRootType);

	/// Concatenates _path + s_separator + s_root[_root].
	static void        MakePath(PathStr& ret_, const char* _path, RootType _root);

	/// Make _path relative to _root. It is safe for _path to point to the string
	/// buffer in ret_.
	static void        MakeRelative(PathStr& ret_, const char* _path, RootType _root = kRoot);
	static void        MakeRelative(PathStr& ret_, RootType _root = kRoot) { MakeRelative(ret_, ret_, _root); }

	/// Select a file/files via the platform UI. _filters is a null-separated list of filter strings.
	static bool        PlatformSelect(PathStr& result_, const char* _filters = "");
	static int         PlatformSelectMulti(PathStr* results_, int _maxResults, const char* _filters = "");

private:
	static PathStr    s_roots[kRootTypeCount];
	static const char s_separator; // per-platform
	
	/// Get a path to an existing file based on _path and _rootHint.
	/// \return false if no existing file was found.
	static bool FindExisting(PathStr& ret_, const char* _path, RootType _rootHint);

}; // class FileSystem

} // namespace apt

#endif // apt_FileSystem_h
