#include <plr/def.h>
#include <plr/log.h>

#include <plr/Time.h>

#include <new>

using namespace plr;

#include <plr/IniFile.h>
#include <cstring> // strcmp
void CheckTestIniProperties(IniFile& _ini, const char* _section = 0)
{
	if (_section) {
		PLR_ASSERT(_ini.getProperty("section", _section).getType() == IniFile::ValueType::kString);
		PLR_ASSERT(strcmp(_ini.getProperty("section", _section).asString(), _section) == 0);
	}

	PLR_ASSERT(_ini.getProperty("ival0", _section).getType() == IniFile::ValueType::kInt);
	PLR_ASSERT(_ini.getProperty("ival0", _section).asInt() == 7);
	PLR_ASSERT(_ini.getProperty("ival1", _section).getType() == IniFile::ValueType::kInt);
	PLR_ASSERT(_ini.getProperty("ival1", _section).asInt() == -7);
	PLR_ASSERT(_ini.getProperty("ival2", _section).getType() == IniFile::ValueType::kInt);
	PLR_ASSERT(_ini.getProperty("ival2", _section).asInt() == 0x7);

	PLR_ASSERT(_ini.getProperty("iarr0", _section).getType() == IniFile::ValueType::kInt);
	PLR_ASSERT(_ini.getProperty("iarr0", _section).getCount() == 3);
	PLR_ASSERT(_ini.getProperty("iarr0", _section).asInt(0) == 7);
	PLR_ASSERT(_ini.getProperty("iarr0", _section).asInt(1) == 8);
	PLR_ASSERT(_ini.getProperty("iarr0", _section).asInt(2) == 9);


	PLR_ASSERT(_ini.getProperty("fval0", _section).getType() == IniFile::ValueType::kDouble);
	PLR_ASSERT(_ini.getProperty("fval0", _section).asDouble() == 7.0);
	PLR_ASSERT(_ini.getProperty("fval1", _section).getType() == IniFile::ValueType::kDouble);
	PLR_ASSERT(_ini.getProperty("fval1", _section).asDouble() == -7.0);
	PLR_ASSERT(_ini.getProperty("fval2", _section).getType() == IniFile::ValueType::kDouble);
	PLR_ASSERT(_ini.getProperty("fval2", _section).asDouble() == 1e-7);
	
	PLR_ASSERT(_ini.getProperty("farr0", _section).getType() == IniFile::ValueType::kDouble);
	PLR_ASSERT(_ini.getProperty("farr0", _section).getCount() == 3);
	PLR_ASSERT(_ini.getProperty("farr0", _section).asDouble(0) == 7.0);
	PLR_ASSERT(_ini.getProperty("farr0", _section).asDouble(1) == 8.1);
	PLR_ASSERT(_ini.getProperty("farr0", _section).asDouble(2) == 9.2);


	PLR_ASSERT(_ini.getProperty("bval0", _section).getType() == IniFile::ValueType::kBool);
	PLR_ASSERT(_ini.getProperty("bval0", _section).asBool() == true);
	PLR_ASSERT(_ini.getProperty("barr0", _section).getType() == IniFile::ValueType::kBool);
	
	PLR_ASSERT(_ini.getProperty("barr0", _section).getCount() == 2);
	PLR_ASSERT(_ini.getProperty("barr0", _section).asBool(0) == true);
	PLR_ASSERT(_ini.getProperty("barr0", _section).asBool(1) ==	false);


	PLR_ASSERT(_ini.getProperty("sval0", _section).getType() == IniFile::ValueType::kString);
	PLR_ASSERT(strcmp(_ini.getProperty("sval0", _section).asString(), "seven") == 0);
	PLR_ASSERT(strcmp(_ini.getProperty("sval1", _section).asString(), "eight") == 0);
	PLR_ASSERT(strcmp(_ini.getProperty("sval2", _section).asString(), "nine") == 0);

	PLR_ASSERT(_ini.getProperty("sarr0", _section).getType() == IniFile::ValueType::kString);
	PLR_ASSERT(_ini.getProperty("sarr0", _section).getCount() == 3);
	PLR_ASSERT(strcmp(_ini.getProperty("sarr0", _section).asString(0), "seven") == 0);
	PLR_ASSERT(strcmp(_ini.getProperty("sarr0", _section).asString(1), "eight") == 0);
	PLR_ASSERT(strcmp(_ini.getProperty("sarr0", _section).asString(2), "nine") == 0);
}
void TestIniFile()
{
	PLR_TIME_DBG("TestIniFile()");
	IniFile ini;
	PLR_VERIFY(ini.load("test.ini"));
	
	CheckTestIniProperties(ini);
	CheckTestIniProperties(ini, "section0");
	CheckTestIniProperties(ini, "section1");
}

int main(int _argc, char** _argv)
{
	PLR_LOG("plr_open_tests\n--------------");
	//PLR_LOG("%s", GetDateTime().asString().c_str());
	PLR_LOG_DBG("DEBUG");
	PLR_TIME_DBG("plr_open_tests");

	#define print_typeinfo(t) PLR_LOG("sizeof(%s)\t%d\talignof(%s)\t%d", #t, sizeof(t), #t, PLR_ALIGNOF(t));
	print_typeinfo(char);
	print_typeinfo(char*);
	print_typeinfo(sint8);
	print_typeinfo(uint8);
	print_typeinfo(sint16);
	print_typeinfo(uint16);
	print_typeinfo(sint32);
	print_typeinfo(uint32);
	print_typeinfo(sint64);
	print_typeinfo(uint64);
	print_typeinfo(sint);
	print_typeinfo(uint);
	print_typeinfo(float);
	print_typeinfo(double);

	#undef print_typeinfo

	TestIniFile();

	return 0;
}