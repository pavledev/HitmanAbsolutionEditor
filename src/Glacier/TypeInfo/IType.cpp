#include <string>

#include "Glacier/TypeInfo/IType.h"
#include "Glacier/TypeInfo/STypeID.h"

IType::IType(unsigned short typeSize, unsigned char typeAlignment, unsigned char typeInfoFlags, const char* typeName, STypeID* typeID)
{
	const size_t length = strlen(typeName);
	char* typeName2 = new char[length + 1];

	strncpy_s(typeName2, length + 1, typeName, length + 1);

	this->m_pTypeFunctions = nullptr;
	this->m_nTypeSize = typeSize;
	this->m_nTypeAlignment = typeAlignment;
	this->m_nTypeInfoFlags = typeInfoFlags;
	this->pszTypeName = typeName2;
	this->typeID = typeID;
	this->fromString = nullptr;
	this->toString = nullptr;
}

IType::~IType()
{
	delete[] pszTypeName;
}

unsigned int IType::GetTypeSize() const
{
	return m_nTypeSize;
}

unsigned int IType::GetTypeAlignment() const
{
	return m_nTypeAlignment;
}

ETypeInfoFlags IType::GetTypeInfoFlags() const
{
	return static_cast<ETypeInfoFlags>(m_nTypeInfoFlags);
}

const char* IType::GetTypeName() const
{
	return pszTypeName;
}

const bool IType::IsEnumType() const
{
	return (GetTypeInfoFlags() & ETypeInfoFlags::ETYPEINFOFLAG_ENUMTYPE) == ETypeInfoFlags::ETYPEINFOFLAG_ENUMTYPE;
}

const bool IType::IsPrimitiveType() const
{
	std::string typeName = pszTypeName;

	return typeID->typeNum >= 0 && typeID->typeNum <= 11;
}
