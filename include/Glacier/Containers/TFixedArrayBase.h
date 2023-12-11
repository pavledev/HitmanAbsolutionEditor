#pragma once

template <typename TElement, typename ZFixedArrayData>
class TFixedArrayBase : public ZFixedArrayData
{
public:
	unsigned int Size() const
	{
		return sizeof(ZFixedArrayData) / sizeof(TElement);
	}

	TElement& operator[](unsigned int nIndex)
	{
		return this->m_pStart[nIndex];
	}

	const TElement& operator[](unsigned int nIndex) const
	{
		return this->m_pStart[nIndex];
	}
};
