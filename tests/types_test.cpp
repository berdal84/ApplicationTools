#include <apt/def.h>

#include <apt/types.h>

using namespace apt;

void TestTypes()
{
	using std::numeric_limits;
	APT_STATIC_ASSERT(numeric_limits<sint8>::is_integer);
	APT_STATIC_ASSERT(numeric_limits<uint8>::is_integer);
	APT_STATIC_ASSERT(numeric_limits<uint8N>::is_integer);
	APT_STATIC_ASSERT(numeric_limits<sint8N>::is_integer);

	uint8N u8n = DataType::Convert<float32, uint8N>(0.5f);
}
