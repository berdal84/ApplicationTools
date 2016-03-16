#include <plr/def.h>
#include <plr/log.h>
#include <plr/memory.h>
#include <plr/Time.h>

#include <new>

using namespace plr;

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

	return 0;
}