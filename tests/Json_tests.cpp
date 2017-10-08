#include <catch.hpp>

#include <apt/memory.h>
#include <apt/Json.h>

using namespace apt;

TEST_CASE("ArrayOfArray", "[SerializerJson]")
{
	Json json;

	{	SerializerJson js(json, SerializerJson::Mode_Write);
		uint in = 4;
		js.beginArray(in, "ArrayOfArrays");
		for (int i = 0; i < in; ++i) {
			uint jn = 3;
			js.beginArray(jn);
				for (int j = 0; j < jn; ++j) {
					int v = i + j;
					js.value(v);
				}
			js.endArray();
		}
		js.endArray();
	}
	{	SerializerJson js(json, SerializerJson::Mode_Read);
		uint in = 4;
		js.beginArray(in, "ArrayOfArrays");
		REQUIRE(in== 4);
		for (int i = 0; i < in; ++i) {
			uint jn = 3;
			js.beginArray(jn);
			REQUIRE(jn == 3);
				for (int j = 0; j < jn; ++j) {
					int v;
					js.value(v);
					REQUIRE(v == i + j);
				}
			js.endArray();
		}
		js.endArray();
	}
}

TEST_CASE("Binary", "[SerializerJson]")
{
	const char* kSrcData = 
		"Man is distinguished, not only by his reason, but by this singular passion from "
		"other animals, which is a lust of the mind, that by a perseverance of delight "
		"in the continued and indefatigable generation of knowledge, exceeds the short "
		"vehemence of any carnal pleasure."
		;
	const uint kSrcDataSize = strlen(kSrcData);

	
	Json json;
	SerializerJson js(json, SerializerJson::Mode_Write);
	
	void* data = (void*)kSrcData;
	uint dataSize = kSrcDataSize;
	js.binary(data, dataSize, "BinaryTest");

	data = nullptr;
	js.setMode(SerializerJson::Mode_Read);
	js.binary(data, dataSize, "BinaryTest");
	
	REQUIRE(dataSize == kSrcDataSize);
	REQUIRE(memcmp(data, kSrcData, dataSize) == 0);
}

