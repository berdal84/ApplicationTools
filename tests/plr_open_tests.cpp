#include <iostream>

#include <plr/def.h>
#include <plr/ArgList.h>
#include <plr/Path.h>
#include <plr/Time.h>

using namespace plr;

int main(int _argc, char** _argv)
{
	std::cout << "plr_open_tests\n--------------\n" << std::endl;

	#define print_typeinfo(t) std::cout \
		<< "sizeof(" #t ")\t" << sizeof(t) \
		<< "\talignof(" #t ")\t" << PLR_ALIGNOF(t) \
		<< std::endl
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