#include <plr/def.h>

#include <plr/types.h>

using namespace plr;

void TestTypes()
{
	uint8N u8n = DataType::Convert<float32, uint8N>(0.5f);
}
