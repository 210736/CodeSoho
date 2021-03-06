//-----------------------------------------------------------------------------
// File: base64.h
// Desc: base64算法
// Auth: 王炜
// Date: 2012/10/15
//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
namespace ECore {
//-----------------------------------------------------------------------------
class ECORE_API CBase64
{
public:
	/*编码
	DataByte
	[in]输入的数据长度,以字节为单位
	*/
	string Encode(const unsigned char* Data,int DataByte);
	/*解码
	DataByte
	[in]输入的数据长度,以字节为单位
	OutByte
	[out]输出的数据长度,以字节为单位,请不要通过返回值计算
	输出数据的长度
	*/
	string Decode(const char* Data,int DataByte,int& OutByte);
};
//-----------------------------------------------------------------------------
}
