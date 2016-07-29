#include <iostream>
#include <string>
using namespace std;

#define UN_INIT 0xcccccccccccccccc //���ֵ
#define MAX_INT64 0x7fffffffffffffff//64λ���ֵ
#define MIN_INT64 0x8000000000000000//64λ��Сֵ

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
	void INT64ToString();//��δ�����long long����ת��Ϊ�ַ���
private:
	INT64 llvalue;  //δ���long long����
	string strvalue; //��������ַ����ķ�ʽ�洢
};
