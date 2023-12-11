#include "Glacier/Cloth/ZFabricResourceEntity.h"

void ZFabricResourceEntity::SAddedPropertyValues::SClothPiecePropertySet::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.String("m_vGravity");
    m_vGravity.SerializeToJson(writer);

    writer.String("m_nNumIterations");
    writer.Uint(m_nNumIterations);

    writer.String("m_fDamping");
    writer.Double(m_fDamping);
}

void ZFabricResourceEntity::SAddedPropertyValues::SClothPieceExtendedPropertySet::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.String("m_MaterialInstance");
    writer.Null();

    writer.String("m_MaterialInstancePtr");
    writer.Null();

    writer.String("m_vGravity");
    m_vGravity.SerializeToJson(writer);

    writer.String("m_nNumIterations");
    writer.Uint(m_nNumIterations);

    writer.String("m_fDamping");
    writer.Double(m_fDamping);
}

void ZFabricResourceEntity::SAddedPropertyValues::STransformPropertySet::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.String("m_TransformHook");
    writer.Null();
}

void ZFabricResourceEntity::SAddedPropertyValues::SStrandsPropertySet::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.String("m_fStrandWidth");
    writer.Double(m_fStrandWidth);
}
