#ifndef FTDI_HELPER_FUNCTIONS_H
#define FTDI_HELPER_FUNCTIONS_H

#include <string>
#include <array>
// #include<ftd2xx.h>
#include <algorithm>
#include<iostream>


template<size_t SIZE_T>
inline std::string convertArrayToString(std::array<bool, SIZE_T> arr)
{
	std::string ret = "";
	for (auto a:arr)
	{
		ret += std::to_string(a);
	}

	return ret;
}

template<class retSize, size_t SIZE_T>
inline retSize convertBinaryArrayToInt(std::array<bool, SIZE_T> arr)
{
	retSize ret = 0;
	int count = 0;
	for (auto a:arr)
	{
		ret |= int(a) << count;
		count++;
	}

	return ret;
}

inline std::string myConvertToBin(int val, int totalSize)
{
	std::string s = "";
	while(val)
	{
		s += std::to_string(val & 1);
		val = val >> 1;
	}
	int i = s.size();
	if (totalSize > 0)
	{
		if (i > totalSize)
		{
			//std::cout << "ConvertToBin: Size is more than the total size. Returning the actual value entered anyway" << std::endl;
			return s;
		}
	while(i < totalSize)
	{
		s += '0';
		i++;
	}
}
	reverse(s.begin(), s.end());
	return s;
}

inline int bin2dec(std::string str) {
 int n = 0;
 int size = (str.size());
        int count = 0;
 while ( count < size ) {
  // if ( str[count] == '1' ) 
      n |= int(str[count] - '0') << count;
  count++; 
 }
 return n;
}


#endif