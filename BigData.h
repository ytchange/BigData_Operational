#include <iostream>
#include <string>
using namespace std;

#define UN_INIT 0xcccccccccccccccc //随机值
#define MAX_INT64 0x7fffffffffffffff//64位最大值
#define MIN_INT64 0x8000000000000000//64位最小值

typedef long long INT64;

class BigData
{
public:
	BigData(INT64 data= UN_INIT);
	BigData(const char* pData);

	BigData operator+(BigData &bigdata);
	BigData operator-(BigData &bigdata);
	BigData operator*(BigData &bigdata);
	BigData operator/(BigData &bigdata);
	friend ostream& operator<<(ostream& os, const BigData &bigdata);
	bool IsINT64Overflow() const;
protected:
	string Add(string left, string right);
	string Sub(string left, string right);
	string Mul(string left, string right);
	string Div(string left, string right);
	bool IsLeftStrBig(char *pLeft, size_t LSize, char *pRight, size_t RSize);
	char SubLoop(char *pLeft, size_t LSize, char *pRight, size_t RSize);
	void INT64ToString();//将未溢出的long long类型转化为字符串
private:
	INT64 llvalue;  //未溢出long long类型
	string strvalue; //溢出，以字符串的方式存储
};
