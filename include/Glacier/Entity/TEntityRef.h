#pragma once

#include "ZEntityRef.h"

template <typename T>
class TEntityRef
{
private:
	ZEntityRef m_entityRef;
	T* m_pInterfaceRef;
};
