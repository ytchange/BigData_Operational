#include "BigData.h"
#include <assert.h>

BigData::BigData(INT64 data)//初始化llvalue和strvalue
	:llvalue(data)
	, strvalue("")
{
	INT64ToString();
}
void BigData::INT64ToString()//将llvalue转化为strvalue
{
	char Symbol = '+';//设置符号位
	INT64 tmp = llvalue;
	if (tmp < 0)  //llvalue为负数时，将其变为负数形式
	{
		Symbol = '-';
		tmp = 0 - tmp;
	}
	strvalue.append(1, Symbol);//将符号位追加在最高位（0位）
	//6789
	while (tmp)
	{
		strvalue.append(1, tmp % 10 + '0');
		tmp /= 10;
	}
	//插入完成时得到的是反向的数字，需要进行翻转调整
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
BigData::BigData(const char* pData) //初始化strvalue和llvalue
	:llvalue(0)
{
	assert(pData);
	char Symbol = pData[0];//获取字符串的第0位
	char *tmp = (char *)pData;//处理tmp始终指向出符号位的第一个数字
	//12345, +12345,-12345,abc12345,
	if (Symbol == '+' || Symbol == '-')//前面有符号的情况
	{
		tmp++;
	}
	else if (*tmp <= '9'&&*tmp >= '0')//第0位就是数字的情况
	{
		Symbol = '+';
	}
	else//当给出的第0位既不是+，-，也不是数字，则不能进行大数运算
	{
		llvalue = 0;
		strvalue = '\0';
		return;
	}
	//当给出的字符串为0000012345，以好多0开头，则需要去掉前置0；
	while (*tmp=='0')
	{
		tmp++;
	}

	//调整字符串的长度
	strvalue.resize(strlen(tmp) + 1);//tmp调整后符号位发生了变化，还有前置0，所以需要重新改变长度，节省空间
	strvalue[0] = Symbol;//存储符号位
	int icount = 1;
	while (tmp)
	{
		if (*tmp >= '0'&&*tmp <= '9')
		{
			llvalue = llvalue * 10 + *tmp - '0';
			strvalue[icount++] = *tmp;
		}
		else//中间截断的情况，1234abc567，这种情况只按照前一部分数字处理
		{
			break;
		}
		tmp++;
	}
	strvalue.resize(icount);//将size再重新调整，减少空间的浪费
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
		strTmp = "+9223372036854775807";//最大正数
	}
	else
	{
		strTmp = "-9223372036854775808";//最小负数
	}
	//若果大于最大数的size则溢出
	if (strvalue.size() > strTmp.size())
	{
		return true;
	}
	else if (strvalue.size() == strTmp.size() && strvalue > strTmp)
	{//必须要加size相等的条件，不能直接比大小，9224比+9223372036854775807大，但是并没有溢出
		return true;
	}
	return false;
}
BigData BigData::operator + (BigData &bigdata)
{
	if (!IsINT64Overflow() && !bigdata.IsINT64Overflow())//两者都没有溢出
	{
		if (strvalue[0] != bigdata.strvalue[0])//两者异号
		{
			return BigData(llvalue + bigdata.llvalue);
		}
		else//两者同号
		{
			//同号但相加结果没有溢出，采用的判断方法是，当一个数小于最大数减去另一个数，则两数之和肯定没有溢出
			if ((strvalue[0] == '+' && bigdata.strvalue[0] == '+' && llvalue <= (MAX_INT64 - bigdata.llvalue)) || \
				(strvalue[0] == '-' && bigdata.strvalue[0] == '-' && llvalue >= (MAX_INT64 - bigdata.llvalue)))
			{
				return BigData(llvalue + bigdata.llvalue);
			}
		}
	}
	//处理有溢出的情况
	string strtmp;
	if (strvalue[0] == bigdata.strvalue[0])
	{
		//无论两者是都溢出还是只有一个溢出，同号相加都会溢出
		strtmp= Add(strvalue, bigdata.strvalue);
	}
	else//两者不同号，列为相减的情况
	{
		strtmp = Sub(strvalue, bigdata.strvalue);
	}
	return BigData(strtmp.c_str());
}

//处理相加溢出的情况
string BigData::Add(string left, string right)
{
	int Lsize = left.size();
	int Rsize = right.size();
	if (Lsize < Rsize)//保证左边的串比右边的串size大
	{
		swap(left, right);
		swap(Lsize, Rsize);
	}
	string strRet;
	strRet.resize(Lsize + 1);//考虑到相加之后可能会产生进位，因此将左串的size增加一位
	strRet[0] = left[0];//先存储符号位

	char Step = 0;//设置进位

	for (int index = 1; index < Lsize; index++)
	{
		//ch存储每一位相加后的结果
		int ch = left[Lsize - index] - '0' + Step;
		if (index < Rsize)
		{
			ch += (right[Rsize - index] - '0');
		}
		strRet[Lsize - index - 1] = (ch % 10)+'0';
		Step = ch / 10;
	}
	//考虑相加后进位到了最高位，因为strret的size重置了，所以要单独判断一下
	strRet[1] = Step + '0';
	return strRet;
}
//处理相减溢出的情况
string BigData::Sub(string left, string right)
{
	int Lsize = left.size();
	int Rsize = right.size();
	char Symbol = left[0];//获取到符号位
	if (Lsize<Rsize || Lsize == Rsize && left>right)
	{
		//始终让左边的size大于右边的size，值小于右边
		swap(left, right);
		swap(Lsize, Rsize);
		//符号位应该和原right位保持一致
		//确定符号位
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
	strRet.resize(Lsize);//新建一个Lsize大小的空间
	strRet[0] = Symbol;
	//开始相减
	//从后往前取，用left-right，并保存结果
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
		if (strvalue[0] == bigdata.strvalue[0])//同号相减，结果肯定不会溢出
		{
			return BigData(llvalue - bigdata.llvalue);
		}
		else
		{
			//同号但相加结果没有溢出，采用的判断方法是，当一个数小于最大数减去另一个数，则两数之差肯定没有溢出
			if (('+' == strvalue[0] && (INT64)(MAX_INT64 + bigdata.llvalue) >= llvalue) ||
				('-' == strvalue[0] && (INT64)(MIN_INT64 + bigdata.llvalue) <= llvalue))
			{
				return BigData(llvalue - bigdata.llvalue);
			}
		}
	}
	//一个溢出或者两个都溢出，或者是两个相减溢出
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
	//如果其中一个为0，则结果为0
	if (llvalue == 0 || bigdata.llvalue == 0)
	{
		return BigData(INT64(0));
	}
	if (!IsINT64Overflow() && !bigdata.IsINT64Overflow())
	{
		//没有溢出，但是两者同号
		if (strvalue[0] == bigdata.strvalue[0])
		{
			if ((strvalue[0] == '+' && (MAX_INT64 / bigdata.llvalue) >= llvalue)||
				(strvalue[0] == '-' &&(MIN_INT64/bigdata.llvalue)<=llvalue))
			{
				return BigData(llvalue * bigdata.llvalue);
			}
		}
		else//两者异号
		{
			if ((strvalue[0] == '+' && (MIN_INT64 / bigdata.llvalue) >= llvalue) ||
				(strvalue[0] == '-' && (MIN_INT64 / bigdata.llvalue) <= llvalue))
			{
				return BigData(llvalue * bigdata.llvalue);
			}
		}
	}
	//处理相乘溢出的情况，两者都溢出，或者其一溢出，或者没有溢出但是相乘溢出
	return BigData(Mul(strvalue, bigdata.strvalue).c_str());
}

string BigData::Mul(string left, string right)
{
	int Rsize = right.size();//得到的size都带有符号位，所以是字符串的长度加1
	int Lsize = left.size();
	if (Lsize > Rsize)//left是size小的
	{
		swap(right,left);
		swap(Lsize, Rsize);
	}
	//确定符号位
	char Symbol = '+';
	if (left[0] != right[0])
	{
		Symbol = '-';
	}
	//A位数乘A位数，得到的最大数位数为A+B
	string strTmp;
	//此处多加了一个符号位，所以应该减1
	//将该长度所有为置为0；
	strTmp.assign(Lsize + Rsize - 1, '0');
	strTmp[0] = Symbol;
	int len = strTmp.size();

	int iOffset = 0;//乘法，分开乘后，各个是要错位相加的；只要乘数一换，ioffset就该改变  

	//将左数从右向左的每一位作为乘数乘以右数
	for (int Lindex = 1; Lindex < Lsize; Lindex++)
	{
		int cLeft = (left[Lsize - Lindex]-'0');//从右边向左开始获取left的位
		if (cLeft == 0)
		{
			iOffset++;//为0时不用乘，直接将偏移位加加
			continue;
		}

		char step = 0;//设置进位为0；
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
	if (!bigdata.IsINT64Overflow())//未溢出
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
		assert("除数不能为0");
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
	//除数比被除数大时直接返回0
	if (Lsize < Rsize || Lsize == Rsize&&strcmp(left.c_str() + 1, right.c_str() + 1) < 0)
	{
		return "0";
	}
	else
	{//如果除数为1或-1直接返回被除数
		if (right == "-1" || right == "+1")
		{
			left[0] = Symbol;
			return left;
		}
	}
	string strTmp;//保存商的字符串
	strTmp.append(1, Symbol);
	char *pLeft = (char *)left.c_str() + 1;
	char *pRight = (char *)right.c_str() + 1;
	int dataLen = 1;
	//32323232323 44
	for (int index = 0; index < Lsize - 1;)
	{
		//990000000000099 / 33  *pLeft为0时，就直接把0作为商
		if (*pLeft == '0')
		{
			strTmp.append(1, '0');
			pLeft++;
			index++;
			continue;
		}
		//找到能够除以除数的子串
		if (!IsLeftStrBig(pLeft, dataLen, pRight, Rsize - 1))
		{
			strTmp.append(1, '0');//不够除时商0
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
			while ('0' == *pLeft && dataLen > 0)//上述循环相减后，得到的余数可能会把被除数串的最前面的几位改为0，  
			{                                    //所以再找新的子串时，要把前面的0都跳过  
				 pLeft++;      
				 index++;
				 dataLen--;
			 }     
			dataLen++; //紧接着要从上面一位下来  
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
//第一步先是123/15，要除8次才能满足IsLeftStrBig不能再除了
//SubLoop返回8
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

		// "990000000000000000000000000099" 剔除被改变后的被除数中的前置0 
		while (*pLeft == '0'&&LSize > 0)
		{
			pLeft++;
			LSize--;
		}
		Ret++;
	}
	return Ret;
}
