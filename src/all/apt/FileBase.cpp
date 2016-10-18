#include <apt/File.h>

#include <cstdlib> // malloc, free
#include <cstring> // memcpy
#include <utility> // swap

using namespace apt;
using namespace internal;

// PROTECTED

void FileBase::setData(const char* _data, uint64 _size)
{
	if (_size == 0) {
		return;
	}
	if (m_data) {
		if (_size > m_dataSize) {
			free(m_data);
			m_data = 0;
		}
	}
	if (!m_data) {
		m_data = (char*)malloc(_size);
		APT_ASSERT(m_data);
	}
	if (_data) {
		memcpy(m_data, _data, _size);
	}
	m_dataSize = _size;
}

void FileBase::appendData(const char* _data, uint64 _size)
{
	APT_ASSERT(_size > 0);
	m_data = (char*)realloc(m_data, m_dataSize + _size);
	if (_data) {
		memcpy(m_data + m_dataSize, _data, _size);
	}
	m_dataSize += _size;
}


// PROTECTED

FileBase::FileBase()
	: m_path("")
	, m_data(0)
	, m_dataSize(0)
{
}

FileBase::~FileBase()
{
	if (m_data) {
		free(m_data);
	}
}
