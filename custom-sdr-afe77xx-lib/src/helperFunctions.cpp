// #include <string>
// #include <array>
// // #include<ftd2xx.h>
// #include <algorithm>
// #include<iostream>


// using namespace std;

// template<size_t SIZE_T>
//  string convertArrayToString(array<bool, SIZE_T> arr)
// {
// 	string ret = "";
// 	for (auto a:arr)
// 	{
// 		ret += to_string(a);
// 	}

// 	return ret;
// }

// template<class retSize, size_t SIZE_T>
//  retSize convertBinaryArrayToInt(array<bool, SIZE_T> arr)
// {
// 	retSize ret = 0;
// 	int count = 0;
// 	for (auto a:arr)
// 	{
// 		ret |= a << count;
// 		count++;
// 	}

// 	return ret;
// }

//  string myConvertToBin(int val)
// {
// 	string s = "";
// 	while(val)
// 	{
// 		s += to_string(val & 1);
// 		val = val >> 1;
// 	}
// 	reverse(s.begin(), s.end());
// 	return s;
// }

