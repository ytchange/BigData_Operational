#include "BigData.h"

void test1()
{
	BigData bd1(123456789);
	//char *ptr = "-1234567891011000000000000000000000000000";
	//BigData bd2(ptr);
	BigData bd2(9223372035854775807);
	cout << bd1 + bd2 << endl;
}
void test2()
{
	char *ptr = "1234567891011000000000000000000000000000";
	BigData bd1(ptr);
	BigData bd2(15);
	cout << bd1 / bd2 ;
}
void test3()
{
	char *ptr = "1234567891011000000000000000000000000000";
	BigData bd1(ptr);
	BigData bd2(15);
	cout << bd1 * bd2;
}
void test4()
{
	char *ptr = "1234567891011000000000000000000000000000";
	BigData bd1(ptr);
	BigData bd2(15);
	cout << bd1 - bd2;
}
int main()
{
	test4();
	return 0;
}