#pragma once
#ifndef apt_FilePath_h
#define apt_FilePath_h

#include <apt/def.h>

#ifdef APT_PLATFORM_WIN
	// max path length should match MAX_PATH (minwindef.h)
	#define APT_MAX_PATH_LENGTH 260
	// max path dirs is arbitrary but should be as small as reasonbly possible
	#define APT_MAX_PATH_DIRS 16
#else
#endif

namespace apt {

////////////////////////////////////////////////////////////////////////////////
/// \class FilePath
/// Path manipulation.
/// No memory allocation is performed, all interfaces are const char*.
/// \todo Use the String class to reduce the footprint.
////////////////////////////////////////////////////////////////////////////////
class FilePath
{
public:
	/// \param _path Raw path (copied to an internal buffer) up to
	///    APT_MAX_PATH_LENGTH chars (including the null terminator).
	FilePath(const char* _path = 0);

	/// Re-initialize from _path.
	void set(const char* _path);

	/// Replace the specified directory (leading separators are ignored)
	void setDirectory(int i, const char* _dir);

	/// Replace the filename.
	void setFileName(const char* _name)      { replace(getFileName(), _name); }

	/// Replace the extension (leading separators are ignored).
	void setExtension(const char* _ext);

	/// Implicit conversion to const char*.
	operator const char*() const             { return getPath(); }


	int         getLength() const            { return m_len; }
	const char* getEnd() const               { return &m_buf[m_len]; }

	const char* getPath() const              { return m_pth; }

	int         getDirectoryCount() const    { return m_dirsCount; }
	const char* getDirectory(int i) const    { APT_ASSERT(i < m_dirsCount); return m_dirs[i];  }

	const char* getFileName() const          { return m_fname; }
	const char* getExtension() const         { return m_ext; }

private:
	char m_pth[APT_MAX_PATH_LENGTH]; // original, raw path
	int  m_len;
	char m_buf[APT_MAX_PATH_LENGTH]; // tokenized path

	// the following point into m_buf
	char* m_dirs[APT_MAX_PATH_DIRS];
	int   m_dirsCount;
	char* m_fname;
	char* m_ext;


	// Tokenize raw path into m_buf.
	void parse();

	// Replaces the path substring starting at _beg with _in.
	void replace(const char* _beg, const char* _in);
};

} // namespace apt

#endif // apt_FilePath_h
