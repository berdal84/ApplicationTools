#include <apt/def.h>
#include <apt/log.h>


#include <apt/math.h>
#include <apt/memory.h>
#include <apt/ArgList.h>
#include <apt/File.h>
#include <apt/Image.h>
#include <apt/IniFile.h>
#include <apt/String.h>
#include <apt/Time.h>

#include <new>
#include <cstring> // strcmp

using namespace apt;

void TestArgList(int _argc, char** _argv)
{
	APT_AUTOTIMER("TestArgList()");
	ArgList al(_argc, _argv);

	const Arg* arg;
	if (arg = al.find("bool")) {
		APT_LOG("bool = %s", arg->getValue().asBool() ? "true" : "false");
	}
	if (arg = al.find("intPos")) {
		APT_LOG("intPos = %i", arg->getValue().asInt());
	}	
	if (arg = al.find("intNeg")) {
		APT_LOG("intNeg = %i", arg->getValue().asInt());
	}
	if (arg = al.find("double")) {
		APT_LOG("double = %f", arg->getValue().asDouble());
	}
}

void CheckTestIniProperties(IniFile& _ini, const char* _section = 0)
{
	if (_section) {
		APT_ASSERT(_ini.getProperty("section", _section).getType() == IniFile::ValueType::kString);
		APT_ASSERT(strcmp(_ini.getProperty("section", _section).asString(), _section) == 0);
	}

	APT_ASSERT(_ini.getProperty("ival0", _section).getType() == IniFile::ValueType::kInt);
	APT_ASSERT(_ini.getProperty("ival0", _section).asInt() == 7);
	APT_ASSERT(_ini.getProperty("ival1", _section).getType() == IniFile::ValueType::kInt);
	APT_ASSERT(_ini.getProperty("ival1", _section).asInt() == -7);
	APT_ASSERT(_ini.getProperty("ival2", _section).getType() == IniFile::ValueType::kInt);
	APT_ASSERT(_ini.getProperty("ival2", _section).asInt() == 0x7);

	APT_ASSERT(_ini.getProperty("iarr0", _section).getType() == IniFile::ValueType::kInt);
	APT_ASSERT(_ini.getProperty("iarr0", _section).getCount() == 3);
	APT_ASSERT(_ini.getProperty("iarr0", _section).asInt(0) == 7);
	APT_ASSERT(_ini.getProperty("iarr0", _section).asInt(1) == 8);
	APT_ASSERT(_ini.getProperty("iarr0", _section).asInt(2) == 9);


	APT_ASSERT(_ini.getProperty("fval0", _section).getType() == IniFile::ValueType::kDouble);
	APT_ASSERT(_ini.getProperty("fval0", _section).asDouble() == 7.0);
	APT_ASSERT(_ini.getProperty("fval1", _section).getType() == IniFile::ValueType::kDouble);
	APT_ASSERT(_ini.getProperty("fval1", _section).asDouble() == -7.0);
	APT_ASSERT(_ini.getProperty("fval2", _section).getType() == IniFile::ValueType::kDouble);
	APT_ASSERT(_ini.getProperty("fval2", _section).asDouble() == 1e-7);
	
	APT_ASSERT(_ini.getProperty("farr0", _section).getType() == IniFile::ValueType::kDouble);
	APT_ASSERT(_ini.getProperty("farr0", _section).getCount() == 3);
	APT_ASSERT(_ini.getProperty("farr0", _section).asDouble(0) == 7.0);
	APT_ASSERT(_ini.getProperty("farr0", _section).asDouble(1) == 8.1);
	APT_ASSERT(_ini.getProperty("farr0", _section).asDouble(2) == 9.2);


	APT_ASSERT(_ini.getProperty("bval0", _section).getType() == IniFile::ValueType::kBool);
	APT_ASSERT(_ini.getProperty("bval0", _section).asBool() == true);
	APT_ASSERT(_ini.getProperty("barr0", _section).getType() == IniFile::ValueType::kBool);
	
	APT_ASSERT(_ini.getProperty("barr0", _section).getCount() == 2);
	APT_ASSERT(_ini.getProperty("barr0", _section).asBool(0) == true);
	APT_ASSERT(_ini.getProperty("barr0", _section).asBool(1) ==	false);


	APT_ASSERT(_ini.getProperty("sval0", _section).getType() == IniFile::ValueType::kString);
	APT_ASSERT(strcmp(_ini.getProperty("sval0", _section).asString(), "seven") == 0);
	APT_ASSERT(strcmp(_ini.getProperty("sval1", _section).asString(), "eight") == 0);
	APT_ASSERT(strcmp(_ini.getProperty("sval2", _section).asString(), "nine") == 0);

	APT_ASSERT(_ini.getProperty("sarr0", _section).getType() == IniFile::ValueType::kString);
	APT_ASSERT(_ini.getProperty("sarr0", _section).getCount() == 3);
	APT_ASSERT(strcmp(_ini.getProperty("sarr0", _section).asString(0), "seven") == 0);
	APT_ASSERT(strcmp(_ini.getProperty("sarr0", _section).asString(1), "eight") == 0);
	APT_ASSERT(strcmp(_ini.getProperty("sarr0", _section).asString(2), "nine") == 0);
}
void TestIniFile()
{
	APT_AUTOTIMER("TestIniFile()");
	IniFile ini;
	APT_VERIFY(IniFile::Read(ini, "test.ini"));
	
	CheckTestIniProperties(ini);
	CheckTestIniProperties(ini, "section0");
	CheckTestIniProperties(ini, "section1");

	APT_VERIFY(IniFile::Write(ini, "test_write.ini"));
	IniFile iniw;
	APT_VERIFY(IniFile::Read(iniw, "test_write.ini"));
	CheckTestIniProperties(iniw);
	CheckTestIniProperties(iniw, "section0");
	CheckTestIniProperties(iniw, "section1");
}

template <uint kCapacity>
void TestString()
{
	APT_AUTOTIMER("TestString()");
	String<0> heapAlways;
	heapAlways.set("heapAlways", 4u);
	heapAlways.append("1234567890", 300);
	APT_LOG("%s", (const char*)heapAlways);
	
	String<kCapacity> str;
	uint len = str.appendf("Int: %d", 12);
	len = str.appendf("Int: %d", 48);
    APT_LOG("%s = %d", (const char*)str, len);
}

void TestMalloc()
{
	uint align = 2;
	while (align < 512) {
		void* p = malloc_aligned(1024, align);
		p = realloc_aligned(p, 512, align);
		free_aligned(p);

		align *= 2;
	}
}

extern void TestTypes();

int main(int _argc, char** _argv)
{
	APT_LOG("APT_open_tests\n--------------");
	//APT_LOG("%s", GetDateTime().asString().c_str());
	APT_LOG_DBG("DEBUG");
	APT_AUTOTIMER("APT_open_tests");

	#define print_typeinfo(t) APT_LOG("sizeof(%s)\t\t%d\talignof(%s)\t\t%d", #t, sizeof(t), #t, APT_ALIGNOF(t));
	print_typeinfo(char);
	print_typeinfo(char*);
	print_typeinfo(sint8);
	print_typeinfo(sint16);
	print_typeinfo(sint32);
	print_typeinfo(sint64);
	print_typeinfo(uint8);
	print_typeinfo(uint16);
	print_typeinfo(uint32);
	print_typeinfo(uint64);
	//print_typeinfo(float16);
	print_typeinfo(float32);
	print_typeinfo(float64);
	print_typeinfo(sint);
	print_typeinfo(uint);
	// math types
	print_typeinfo(vec2);
	print_typeinfo(vec3);
	print_typeinfo(vec4);
	print_typeinfo(mat2);
	print_typeinfo(mat3);
	print_typeinfo(mat4);
	print_typeinfo(quat);

	#undef print_typeinfo

	//TestMalloc();
	//TestArgList(_argc, _argv);
	//TestIniFile();
	//TestString<8>();
	TestTypes();

	return 0;
}