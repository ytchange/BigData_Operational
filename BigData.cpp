#include "BigData.h"
#include <assert.h>

BigData::BigData(INT64 data)//��ʼ��llvalue��strvalue
	:llvalue(data)
	, strvalue("")
{
	INT64ToString();
}
void BigData::INT64ToString()//��llvalueת��Ϊstrvalue
{
	char Symbol = '+';//���÷���λ
	INT64 tmp = llvalue;
	if (tmp < 0)  //llvalueΪ����ʱ�������Ϊ������ʽ
	{
		Symbol = '-';
		tmp = 0 - tmp;
	}
	strvalue.append(1, Symbol);//������λ׷�������λ��0λ��
	//6789
	while (tmp)
	{
		strvalue.append(1, tmp % 10 + '0');
		tmp /= 10;
	}
	//�������ʱ�õ����Ƿ�������֣���Ҫ���з�ת����
	char *begin = (char *)(strvalue.c_str()+1);
	char *end = (char *)(strvalue.c_str() + strvalue.size() - 1);
	while (begin < end)
	{
		char tmp = *begin;
		*begin = *end;
		*end = tmp;
		begin++;
		end--;
	}
}
BigData::BigData(const char* pData) //��ʼ��strvalue��llvalue
	:llvalue(0)
{
	assert(pData);
	char Symbol = pData[0];//��ȡ�ַ����ĵ�0λ
	char *tmp = (char *)pData;//����tmpʼ��ָ�������λ�ĵ�һ������
	//12345, +12345,-12345,abc12345,
	if (Symbol == '+' || Symbol == '-')//ǰ���з��ŵ����
	{
		tmp++;
	}
	else if (*tmp <= '9'&&*tmp >= '0')//��0λ�������ֵ����
	{
		Symbol = '+';
	}
	else//�������ĵ�0λ�Ȳ���+��-��Ҳ�������֣����ܽ��д�������
	{
		llvalue = 0;
		strvalue = '\0';
		return;
	}
	//���������ַ���Ϊ0000012345���Ժö�0��ͷ������Ҫȥ��ǰ��0��
	while (*tmp=='0')
	{
		tmp++;
	}

	//�����ַ����ĳ���
	strvalue.resize(strlen(tmp) + 1);//tmp���������λ�����˱仯������ǰ��0��������Ҫ���¸ı䳤�ȣ���ʡ�ռ�
	strvalue[0] = Symbol;//�洢����λ
	int icount = 1;
	while (tmp)
	{
		if (*tmp >= '0'&&*tmp <= '9')
		{
			llvalue = llvalue * 10 + *tmp - '0';
			strvalue[icount++] = *tmp;
		}
		else//�м�ضϵ������1234abc567���������ֻ����ǰһ�������ִ���
		{
			break;
		}
		tmp++;
	}
	strvalue.resize(icount);//��size�����µ��������ٿռ���˷�
	if (Symbol == '-')
	{
		llvalue = 0 - llvalue;
	}
}

bool BigData::IsINT64Overflow() const
{
	string strTmp;
	if (strvalue[0] == '+')
	{
		strTmp = "+9223372036854775807";//�������
	}
	else
	{
		strTmp = "-9223372036854775808";//��С����
	}
	//���������������size�����
	if (strvalue.size() > strTmp.size())
	{
		return true;
	}
	else if (strvalue.size() == strTmp.size() && strvalue > strTmp)
	{//����Ҫ��size��ȵ�����������ֱ�ӱȴ�С��9224��+9223372036854775807�󣬵��ǲ�û�����
		return true;
	}
	return false;
}
BigData BigData::operator + (BigData &bigdata)
{
	if (!IsINT64Overflow() && !bigdata.IsINT64Overflow())//���߶�û�����
	{
		if (strvalue[0] != bigdata.strvalue[0])//�������
		{
			return BigData(llvalue + bigdata.llvalue);
		}
		else//����ͬ��
		{
			//ͬ�ŵ���ӽ��û����������õ��жϷ����ǣ���һ����С���������ȥ��һ������������֮�Ϳ϶�û�����
			if ((strvalue[0] == '+' && bigdata.strvalue[0] == '+' && llvalue <= (MAX_INT64 - bigdata.llvalue)) || \
				(strvalue[0] == '-' && bigdata.strvalue[0] == '-' && llvalue >= (MAX_INT64 - bigdata.llvalue)))
			{
				return BigData(llvalue + bigdata.llvalue);
			}
		}
	}
	//��������������
	string strtmp;
	if (strvalue[0] == bigdata.strvalue[0])
	{
		//���������Ƕ��������ֻ��һ�������ͬ����Ӷ������
		strtmp= Add(strvalue, bigdata.strvalue);
	}
	else//���߲�ͬ�ţ���Ϊ��������
	{
		strtmp = Sub(strvalue, bigdata.strvalue);
	}
	return BigData(strtmp.c_str());
}

//���������������
string BigData::Add(string left, string right)
{
	int Lsize = left.size();
	int Rsize = right.size();
	if (Lsize < Rsize)//��֤��ߵĴ����ұߵĴ�size��
	{
		swap(left, right);
		swap(Lsize, Rsize);
	}
	string strRet;
	strRet.resize(Lsize + 1);//���ǵ����֮����ܻ������λ����˽��󴮵�size����һλ
	strRet[0] = left[0];//�ȴ洢����λ

	char Step = 0;//���ý�λ

	for (int index = 1; index < Lsize; index++)
	{
		//ch�洢ÿһλ��Ӻ�Ľ��
		int ch = left[Lsize - index] - '0' + Step;
		if (index < Rsize)
		{
			ch += (right[Rsize - index] - '0');
		}
		strRet[Lsize - index - 1] = (ch % 10)+'0';
		Step = ch / 10;
	}
	//������Ӻ��λ�������λ����Ϊstrret��size�����ˣ�����Ҫ�����ж�һ��
	strRet[1] = Step + '0';
	return strRet;
}
//���������������
string BigData::Sub(string left, string right)
{
	int Lsize = left.size();
	int Rsize = right.size();
	char Symbol = left[0];//��ȡ������λ
	if (Lsize<Rsize || Lsize == Rsize && left>right)
	{
		//ʼ������ߵ�size�����ұߵ�size��ֵС���ұ�
		swap(left, right);
		swap(Lsize, Rsize);
		//����λӦ�ú�ԭrightλ����һ��
		//ȷ������λ
		if (Symbol == '+')
		{
			Symbol = '-';
		}
		else
		{
			Symbol = '+';
		}
	}
	string strRet;
	strRet.resize(Lsize);//�½�һ��Lsize��С�Ŀռ�
	strRet[0] = Symbol;
	//��ʼ���
	//�Ӻ���ǰȡ����left-right����������
	for (int index = 1; index < Lsize; index++)
	{
		int ch = left[Lsize - index] - '0';
		if (index < Rsize)
		{
			ch -= (right[Rsize - index] - '0');
		}
		if (ch < 0)
		{
			left[Lsize - index - 1] -= 1;
			ch += 10;
		}
		strRet[Lsize - index] = ch + '0';
	}
	return strRet;
}

BigData BigData::operator - (BigData &bigdata)
{
	if (!IsINT64Overflow() && !bigdata.IsINT64Overflow())
	{
		if (strvalue[0] == bigdata.strvalue[0])//ͬ�����������϶��������
		{
			return BigData(llvalue - bigdata.llvalue);
		}
		else
		{
			//ͬ�ŵ���ӽ��û����������õ��жϷ����ǣ���һ����С���������ȥ��һ������������֮��϶�û�����
			if (('+' == strvalue[0] && (INT64)(MAX_INT64 + bigdata.llvalue) >= llvalue) ||
				('-' == strvalue[0] && (INT64)(MIN_INT64 + bigdata.llvalue) <= llvalue))
			{
				return BigData(llvalue - bigdata.llvalue);
			}
		}
	}
	//һ������������������������������������
	string strRet;  
	if (strvalue[0] != bigdata.strvalue[0])  
	{  
		strRet = Add(strvalue, bigdata.strvalue);
	}  
	else  
	{  
		strRet = Sub(strvalue, bigdata.strvalue);
	}  
	 return BigData(strRet.c_str());  
}

BigData BigData::operator*(BigData& bigdata)
{
	//�������һ��Ϊ0������Ϊ0
	if (llvalue == 0 || bigdata.llvalue == 0)
	{
		return BigData(INT64(0));
	}
	if (!IsINT64Overflow() && !bigdata.IsINT64Overflow())
	{
		//û���������������ͬ��
		if (strvalue[0] == bigdata.strvalue[0])
		{
			if ((strvalue[0] == '+' && (MAX_INT64 / bigdata.llvalue) >= llvalue)||
				(strvalue[0] == '-' &&(MIN_INT64/bigdata.llvalue)<=llvalue))
			{
				return BigData(llvalue * bigdata.llvalue);
			}
		}
		else//�������
		{
			if ((strvalue[0] == '+' && (MIN_INT64 / bigdata.llvalue) >= llvalue) ||
				(strvalue[0] == '-' && (MIN_INT64 / bigdata.llvalue) <= llvalue))
			{
				return BigData(llvalue * bigdata.llvalue);
			}
		}
	}
	//��������������������߶������������һ���������û���������������
	return BigData(Mul(strvalue, bigdata.strvalue).c_str());
}

string BigData::Mul(string left, string right)
{
	int Rsize = right.size();//�õ���size�����з���λ���������ַ����ĳ��ȼ�1
	int Lsize = left.size();
	if (Lsize > Rsize)//left��sizeС��
	{
		swap(right,left);
		swap(Lsize, Rsize);
	}
	//ȷ������λ
	char Symbol = '+';
	if (left[0] != right[0])
	{
		Symbol = '-';
	}
	//Aλ����Aλ�����õ��������λ��ΪA+B
	string strTmp;
	//�˴������һ������λ������Ӧ�ü�1
	//���ó�������Ϊ��Ϊ0��
	strTmp.assign(Lsize + Rsize - 1, '0');
	strTmp[0] = Symbol;
	int len = strTmp.size();

	int iOffset = 0;//�˷����ֿ��˺󣬸�����Ҫ��λ��ӵģ�ֻҪ����һ����ioffset�͸øı�  

	//���������������ÿһλ��Ϊ������������
	for (int Lindex = 1; Lindex < Lsize; Lindex++)
	{
		int cLeft = (left[Lsize - Lindex]-'0');//���ұ�����ʼ��ȡleft��λ
		if (cLeft == 0)
		{
			iOffset++;//Ϊ0ʱ���óˣ�ֱ�ӽ�ƫ��λ�Ӽ�
			continue;
		}

		char step = 0;//���ý�λΪ0��
		for (int Rindex = 1; Rindex < Rsize; Rindex++)
		{
			int cRight = (right[Rsize - Rindex] - '0');
			int Ret = cLeft*cRight;
			Ret += step;
			Ret += strTmp[len - iOffset - Rindex] - '0';
			strTmp[len - iOffset - Rindex] = Ret % 10 + '0';
			step= Ret / 10;
		}
		strTmp[len - iOffset - Rsize] += step;
		iOffset++;
	}
	return strTmp;
}

ostream& operator<<(ostream& os, const BigData &bigdata)
{
	if (!bigdata.IsINT64Overflow())//δ���
	{
		os << bigdata.llvalue << endl;
	}
	else
	{
		char *ptr = (char *)bigdata.strvalue.c_str();
		if (ptr[0] == '+')
		{
			ptr++;
		}
		os << ptr << endl;
	}
	return os;
}

BigData BigData::operator / (BigData &bigdata)
{
	if (bigdata.llvalue == 0)
	{
		assert("��������Ϊ0");
		return BigData(INT64(0));
	}
	if (!IsINT64Overflow() && !bigdata.IsINT64Overflow())
	{
		return BigData(llvalue / bigdata.llvalue);
	}
	return BigData(Div(strvalue, bigdata.strvalue).c_str());
}

string BigData::Div(string left, string right)
{
	char Symbol = '+';
	if (left[0] != right[0])
	{
		Symbol = '-';
	}
	int Lsize = left.size();
	int Rsize = right.size();
	//�����ȱ�������ʱֱ�ӷ���0
	if (Lsize < Rsize || Lsize == Rsize&&strcmp(left.c_str() + 1, right.c_str() + 1) < 0)
	{
		return "0";
	}
	else
	{//�������Ϊ1��-1ֱ�ӷ��ر�����
		if (right == "-1" || right == "+1")
		{
			left[0] = Symbol;
			return left;
		}
	}
	string strTmp;//�����̵��ַ���
	strTmp.append(1, Symbol);
	char *pLeft = (char *)left.c_str() + 1;
	char *pRight = (char *)right.c_str() + 1;
	int dataLen = 1;
	//32323232323 44
	for (int index = 0; index < Lsize - 1;)
	{
		//990000000000099 / 33  *pLeftΪ0ʱ����ֱ�Ӱ�0��Ϊ��
		if (*pLeft == '0')
		{
			strTmp.append(1, '0');
			pLeft++;
			index++;
			continue;
		}
		//�ҵ��ܹ����Գ������Ӵ�
		if (!IsLeftStrBig(pLeft, dataLen, pRight, Rsize - 1))
		{
			strTmp.append(1, '0');//������ʱ��0
			dataLen++;
			if (index + dataLen > Lsize)
			{
				break;
			}
			continue;
		}
		else
		{
			strTmp.append(1, SubLoop(pLeft, dataLen, pRight, Rsize - 1));
			
			 // pLeft  
			while ('0' == *pLeft && dataLen > 0)//����ѭ������󣬵õ����������ܻ�ѱ�����������ǰ��ļ�λ��Ϊ0��  
			{                                    //���������µ��Ӵ�ʱ��Ҫ��ǰ���0������  
				 pLeft++;      
				 index++;
				 dataLen--;
			 }     
			dataLen++; //������Ҫ������һλ����  
			if (index + dataLen > Lsize)
            {
				break;
			} 
		}
	}
	return strTmp;
}

bool BigData::IsLeftStrBig(char *pLeft, size_t LSize, char *pRight, size_t RSize)
{
	 assert(NULL != pLeft && NULL != pRight);  
	 if (LSize > RSize ||  
	      LSize == RSize && strncmp(pLeft, pRight, LSize) >= 0)  
	  {  
	       return true;  
	  }  
	  return false;  
}
//"1234567891011000000000000000000000000000"/15
//��һ������123/15��Ҫ��8�β�������IsLeftStrBig�����ٳ���
//SubLoop����8
char BigData::SubLoop(char *pLeft, size_t LSize, char *pRight, size_t RSize)
{
	assert(pLeft != NULL&&pRight != NULL);
	char Ret = '0';
	while (1)
	{
		if (!IsLeftStrBig(pLeft, LSize, pRight, RSize))
		{
			break;
		}
		int LDataLen = LSize - 1;
		int RDataLen = RSize - 1;
		while (LDataLen >= 0 && RDataLen >= 0)
		{
			if (pLeft[LDataLen] < pRight[RDataLen])
			{
				pLeft[LDataLen - 1] -= 1;
				pLeft[LDataLen] += 10;
			}
			pLeft[LDataLen] = pLeft[LDataLen] - pRight[RDataLen] + '0';
			LDataLen--;
			RDataLen--;
		}

		// "990000000000000000000000000099" �޳����ı��ı������е�ǰ��0 
		while (*pLeft == '0'&&LSize > 0)
		{
			pLeft++;
			LSize--;
		}
		Ret++;
	}
	return Ret;
}
