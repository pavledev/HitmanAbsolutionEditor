#include "Glacier/ZObjectRef.h"

ZObjectRef::ZObjectRef()
{
	m_TypeID = nullptr;
	m_pData = nullptr;
}

STypeID* ZObjectRef::GetTypeID() const
{
	return m_TypeID;
}

void* ZObjectRef::GetData() const
{
	return m_pData;
}

void ZObjectRef::SetTypeID(STypeID* typeID)
{
	m_TypeID = typeID;
}

void ZObjectRef::SetData(void* data)
{
	m_pData = data;
}
