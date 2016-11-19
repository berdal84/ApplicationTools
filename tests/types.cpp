/*#include <apt/def.h>
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
*/

#include <catch.hpp>

#include <apt/types.h>

using namespace apt;

TEST_CASE("Validate type sizes", "[types]")
{
	REQUIRE(sizeof(sint8)    == 1);
	REQUIRE(sizeof(uint8)    == 1);
	REQUIRE(sizeof(sint8N)   == 1);
	REQUIRE(sizeof(uint8N)   == 1);
	REQUIRE(sizeof(sint16)   == 2);
	REQUIRE(sizeof(uint16)   == 2);
	REQUIRE(sizeof(sint16N)  == 2);
	REQUIRE(sizeof(uint16N)  == 2);
	REQUIRE(sizeof(sint32)   == 4);
	REQUIRE(sizeof(uint32)   == 4);
	REQUIRE(sizeof(sint32N)  == 4);
	REQUIRE(sizeof(uint32N)  == 4);	
	REQUIRE(sizeof(sint64)   == 8);
	REQUIRE(sizeof(uint64)   == 8);
	REQUIRE(sizeof(sint64N)  == 8);
	REQUIRE(sizeof(uint64N)  == 8);
	REQUIRE(sizeof(float16)  == 2);
	REQUIRE(sizeof(float32)  == 4);
	REQUIRE(sizeof(float64)  == 8);
}

TEST_CASE("Validate DataType::GetSizeInBytes() matches sizeof()", "[types]")
{
	REQUIRE(DataType::GetSizeBytes(DataType::kSint8)    == sizeof(sint8));
	REQUIRE(DataType::GetSizeBytes(DataType::kUint8)    == sizeof(uint8));
	REQUIRE(DataType::GetSizeBytes(DataType::kSint8N)   == sizeof(sint8));
	REQUIRE(DataType::GetSizeBytes(DataType::kUint8N)   == sizeof(uint8));
	REQUIRE(DataType::GetSizeBytes(DataType::kSint16)   == sizeof(sint16));
	REQUIRE(DataType::GetSizeBytes(DataType::kUint16)   == sizeof(uint16));
	REQUIRE(DataType::GetSizeBytes(DataType::kSint16N)  == sizeof(sint16));
	REQUIRE(DataType::GetSizeBytes(DataType::kUint16N)  == sizeof(uint16));
	REQUIRE(DataType::GetSizeBytes(DataType::kSint32)   == sizeof(sint32));
	REQUIRE(DataType::GetSizeBytes(DataType::kUint32)   == sizeof(uint32));
	REQUIRE(DataType::GetSizeBytes(DataType::kSint32N)  == sizeof(sint32));
	REQUIRE(DataType::GetSizeBytes(DataType::kUint32N)  == sizeof(uint32));
	REQUIRE(DataType::GetSizeBytes(DataType::kSint64)   == sizeof(sint64));
	REQUIRE(DataType::GetSizeBytes(DataType::kUint64)   == sizeof(uint64));
	REQUIRE(DataType::GetSizeBytes(DataType::kSint64N)  == sizeof(sint64));
	REQUIRE(DataType::GetSizeBytes(DataType::kUint64N)  == sizeof(uint64));
	REQUIRE(DataType::GetSizeBytes(DataType::kFloat16)  == sizeof(float16));
	REQUIRE(DataType::GetSizeBytes(DataType::kFloat32)  == sizeof(float32));
	REQUIRE(DataType::GetSizeBytes(DataType::kFloat64)  == sizeof(float64));
}

TEST_CASE("Validate metadata functions", "[types]")
{
	REQUIRE(DataType::IsNormalized(DataType::kSint8)    == false);
	REQUIRE(DataType::IsNormalized(DataType::kUint8)    == false);
	REQUIRE(DataType::IsNormalized(DataType::kSint8N)   == true);
	REQUIRE(DataType::IsNormalized(DataType::kUint8N)   == true);
	REQUIRE(DataType::IsNormalized(DataType::kSint16)   == false);
	REQUIRE(DataType::IsNormalized(DataType::kUint16)   == false);
	REQUIRE(DataType::IsNormalized(DataType::kSint16N)  == true);
	REQUIRE(DataType::IsNormalized(DataType::kUint16N)  == true);
	REQUIRE(DataType::IsNormalized(DataType::kSint32)   == false);
	REQUIRE(DataType::IsNormalized(DataType::kUint32)   == false);
	REQUIRE(DataType::IsNormalized(DataType::kSint32N)  == true);
	REQUIRE(DataType::IsNormalized(DataType::kUint32N)  == true);
	REQUIRE(DataType::IsNormalized(DataType::kSint64)   == false);
	REQUIRE(DataType::IsNormalized(DataType::kUint64)   == false);
	REQUIRE(DataType::IsNormalized(DataType::kSint64N)  == true);
	REQUIRE(DataType::IsNormalized(DataType::kUint64N)  == true);
	REQUIRE(DataType::IsNormalized(DataType::kFloat16)  == false);
	REQUIRE(DataType::IsNormalized(DataType::kFloat32)  == false);
	REQUIRE(DataType::IsNormalized(DataType::kFloat64)  == false);

	REQUIRE(DataType::IsFloat(DataType::kSint8)    == false);
	REQUIRE(DataType::IsFloat(DataType::kUint8)    == false);
	REQUIRE(DataType::IsFloat(DataType::kSint8N)   == false);
	REQUIRE(DataType::IsFloat(DataType::kUint8N)   == false);
	REQUIRE(DataType::IsFloat(DataType::kSint16)   == false);
	REQUIRE(DataType::IsFloat(DataType::kUint16)   == false);
	REQUIRE(DataType::IsFloat(DataType::kSint16N)  == false);
	REQUIRE(DataType::IsFloat(DataType::kUint16N)  == false);
	REQUIRE(DataType::IsFloat(DataType::kSint32)   == false);
	REQUIRE(DataType::IsFloat(DataType::kUint32)   == false);
	REQUIRE(DataType::IsFloat(DataType::kSint32N)  == false);
	REQUIRE(DataType::IsFloat(DataType::kUint32N)  == false);
	REQUIRE(DataType::IsFloat(DataType::kSint64)   == false);
	REQUIRE(DataType::IsFloat(DataType::kUint64)   == false);
	REQUIRE(DataType::IsFloat(DataType::kSint64N)  == false);
	REQUIRE(DataType::IsFloat(DataType::kUint64N)  == false);
	REQUIRE(DataType::IsFloat(DataType::kFloat16)  == true);
	REQUIRE(DataType::IsFloat(DataType::kFloat32)  == true);
	REQUIRE(DataType::IsFloat(DataType::kFloat64)  == true);

	REQUIRE(DataType::IsSigned(DataType::kSint8)    == true);
	REQUIRE(DataType::IsSigned(DataType::kUint8)    == false);
	REQUIRE(DataType::IsSigned(DataType::kSint8N)   == true);
	REQUIRE(DataType::IsSigned(DataType::kUint8N)   == false);
	REQUIRE(DataType::IsSigned(DataType::kSint16)   == true);
	REQUIRE(DataType::IsSigned(DataType::kUint16)   == false);
	REQUIRE(DataType::IsSigned(DataType::kSint16N)  == true);
	REQUIRE(DataType::IsSigned(DataType::kUint16N)  == false);
	REQUIRE(DataType::IsSigned(DataType::kSint32)   == true);
	REQUIRE(DataType::IsSigned(DataType::kUint32)   == false);
	REQUIRE(DataType::IsSigned(DataType::kSint32N)  == true);
	REQUIRE(DataType::IsSigned(DataType::kUint32N)  == false);
	REQUIRE(DataType::IsSigned(DataType::kSint64)   == true);
	REQUIRE(DataType::IsSigned(DataType::kUint64)   == false);
	REQUIRE(DataType::IsSigned(DataType::kSint64N)  == true);
	REQUIRE(DataType::IsSigned(DataType::kUint64N)  == false);
	REQUIRE(DataType::IsSigned(DataType::kFloat16)  == true);
	REQUIRE(DataType::IsSigned(DataType::kFloat32)  == true);
	REQUIRE(DataType::IsSigned(DataType::kFloat64)  == true);
}
