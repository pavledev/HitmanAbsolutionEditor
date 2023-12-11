#pragma once

#include <string.h>

#include "../ZString.h"
#include "ETypeInfoFlags.h"

struct STypeID;
struct STypeFunctions;

class IType
{
public:
	IType(unsigned short typeSize, unsigned char typeAlignment, unsigned char typeInfoFlags, const char* typeName, STypeID* typeID);
	~IType();
	unsigned int GetTypeSize() const;
	unsigned int GetTypeAlignment() const;
	ETypeInfoFlags GetTypeInfoFlags() const;
	const char* GetTypeName() const;
	const bool IsEnumType() const;
	const bool IsPrimitiveType() const;

private:
	const STypeFunctions* m_pTypeFunctions;
	unsigned short m_nTypeSize;
	unsigned char m_nTypeAlignment;
	unsigned char m_nTypeInfoFlags;
	const char* pszTypeName;
	STypeID* typeID;
	bool (*fromString)(void* param1, const IType* type, const ZString& string);
	unsigned int (*toString)(const void* param1, const IType* type, char* param3, unsigned int param4, const ZString& string);
};
