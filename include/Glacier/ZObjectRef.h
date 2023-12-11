#pragma once

#include "TypeInfo/STypeID.h"

class ZObjectRef
{
public:
	ZObjectRef();
	STypeID* GetTypeID() const;
	void* GetData() const;
	void SetTypeID(STypeID* typeID);
	void SetData(void* data);

protected:
	STypeID* m_TypeID;
	void* m_pData;
};
