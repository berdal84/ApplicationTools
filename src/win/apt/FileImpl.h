#ifndef apt_File_h
	#error apt: Do not include apt/FileImpl.h, include apt/File.h
#endif

#include <apt/FileBase.h>

namespace apt { namespace internal {
	 
////////////////////////////////////////////////////////////////////////////////
/// \class FileImpl
/// \internal \ingroup APT_core
////////////////////////////////////////////////////////////////////////////////
class FileImpl: public internal::FileBase
{
protected:
	FileImpl();
	~FileImpl();
	
	static bool Exists(const char* _path);
	static bool CreateDir(const char* _path);
	static bool Read(FileImpl& file_, const char* _path);
	static bool Write(const FileImpl& _file, const char* _path);

private:
	void* m_handle; // HANDLE

}; // class FileImpl

} } // namespace apt::intenral
