#pragma once

enum class ETypeInfoFlags
{
    ETYPEINFOFLAG_TENTITYREF = 1,
    ETYPEINFOFLAG_TRESOURCEPTR = 2,
    ETYPEINFOFLAG_CLASSTYPE = 4,
    ETYPEINFOFLAG_ENUMTYPE = 8,
    ETYPEINFOFLAG_CONTAINERTYPE = 16,
    ETYPEINFOFLAG_ARRAYTYPE = 32,
    ETYPEINFOFLAG_FIXEDARRAYTYPE = 64
};

inline ETypeInfoFlags operator&(ETypeInfoFlags lhs, ETypeInfoFlags rhs)
{
    return static_cast<ETypeInfoFlags>(static_cast<int>(lhs) & static_cast<int>(rhs));
}
