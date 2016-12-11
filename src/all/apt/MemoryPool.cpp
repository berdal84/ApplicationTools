#include <apt/MemoryPool.h>

#include <apt/memory.h>

#include <algorithm> // std::copy
#include <new>

using namespace apt;

// PUBLIC

MemoryPool::MemoryPool(uint _objectSize, uint _objectAlignment, uint _blockSize)
	: m_objectSize(_objectSize)
	, m_objectAlignment(_objectAlignment)
	, m_blockSize(_blockSize)
	, m_nextFree(0)
	, m_usedCount(0)
	, m_blocks(0)
	, m_blockCount(0)
{
	APT_ASSERT(m_objectSize >= sizeof(void*)); // objects must be at least the size of a ptr
}

MemoryPool::~MemoryPool()
{
	APT_ASSERT(m_usedCount == 0); // not all objects were freed
	for (uint i = 0; i < m_blockCount; ++i) {
		apt::free_aligned(m_blocks[i]);
	}
	delete[] m_blocks;
}

void* MemoryPool::alloc()
{
	if (m_nextFree == 0) {
		allocBlock();
	}
	void* ret = m_nextFree;
	m_nextFree = *((void**)m_nextFree);
	++m_usedCount;
	return ret;
}

void MemoryPool::free(void* _object)
{
	APT_ASSERT(_object);
	APT_ASSERT(m_usedCount > 0);
	*((void**)_object) = m_nextFree;
	m_nextFree = _object;
	--m_usedCount;
}


// PRIVATE

void MemoryPool::allocBlock()
{
	// allocate a new block list
	void** tmp = new(std::nothrow) void*[m_blockCount + 1];
	if (m_blocks) {
		std::copy(m_blocks, m_blocks + m_blockCount, tmp);
		delete[] m_blocks;
	}
	m_blocks = tmp;
	
	// allocate new block at list end
	m_blocks[m_blockCount] = apt::malloc_aligned(m_objectSize * m_blockSize, m_objectAlignment);

	// init free ptrs; if m_nextFree initially points to locX, after initializing
	// the new block (starting new0) is initialized as follows:
	//
	// m_nextFree -> new0 -> new1 -> new2 -> new3 -> locX
	//
	uint p = (uint)m_blocks[m_blockCount];
	for (uint i = 0, n = m_blockSize - 1; i < n; ++i) {
		*((uint*)p) = p + m_objectSize;
		p += m_objectSize;
	}
	*((uint*)p) = (uint)m_nextFree;
	m_nextFree = m_blocks[m_blockCount];
	
	++m_blockCount;
}

bool MemoryPool::isFromPool(const void* _ptr) const
{
	uint p = (uint)_ptr;
	for (uint i = 0; i < m_blockCount; ++i) {
		if (p >= (uint)m_blocks[i] && p < ((uint)m_blocks[i] + m_blockSize)) {
			return true;
		}
	}
	return false;
}

bool MemoryPool::validate() const
{
	uint freeCount = 0;
	void* p = m_nextFree;
	while (p != 0) {
		++freeCount;
		p = *((void**)p);
	}
	uint totalObjects = m_blockSize * m_blockCount;
	return m_usedCount == totalObjects - freeCount;
}
