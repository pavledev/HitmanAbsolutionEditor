#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "Resources/SoundDefinitions.h"

void SoundDefinitions::Entry::SerializeToJson(const std::vector<std::shared_ptr<Resource>>& references, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool hasActorSoundDefinitions)
{
	writer.StartObject();

	writer.String("definition");

	if (hasActorSoundDefinitions)
	{
		writer.String(ConvertActorSoundDefinitionToString(static_cast<SActorSoundDefs::EDefinition>(definition)).c_str());
	}
	else
	{
		writer.String(ConvertDoorSoundDefinitionToString(static_cast<SDoorSoundDefs::EDefinition>(definition)).c_str());
	}

	writer.String("waveBank");

	if (referenceIndex != -1)
	{
		writer.String(references[referenceIndex]->GetResourceID().c_str());
	}
	else
	{
		writer.String("");
	}

	writer.String("attenuationOffset");
	writer.Int(attenuationOffset);

	writer.String("groupNumber");
	writer.Int(groupNumber);

	writer.String("selectionMode");
	writer.String(ConvertSoundPlayParametersToString(selectionMode).c_str());

	writer.String("noRepeatsCount");
	writer.Int(noRepeatsCount);

	writer.String("subSoundRepeatCounts");

	if (subSoundRepeatCounts.size() > 0)
	{
		writer.StartArray();

		for (size_t i = 0; i < subSoundRepeatCounts.size(); ++i)
		{
			writer.Uint(subSoundRepeatCounts[i]);
		}

		writer.EndArray();
	}
	else
	{
		writer.Null();
	}

	writer.EndObject();
}

std::string SoundDefinitions::Entry::ConvertActorSoundDefinitionToString(const SActorSoundDefs::SActorSoundDefs::EDefinition definition)
{
	switch (definition)
	{
		case SActorSoundDefs::EDefinition::_NoSound:
			return "_NoSound";
		case SActorSoundDefs::EDefinition::Dth:
			return "Dth";
		case SActorSoundDefs::EDefinition::Dth_BrkNck:
			return "Dth_BrkNck";
		case SActorSoundDefs::EDefinition::Dth_Fll:
			return "Dth_Fll";
		case SActorSoundDefs::EDefinition::Dth_GnSht:
			return "Dth_GnSht";
		case SActorSoundDefs::EDefinition::Dth_HdSht:
			return "Dth_HdSht";
		case SActorSoundDefs::EDefinition::Dth_Mpct:
			return "Dth_Mpct";
		case SActorSoundDefs::EDefinition::Dth_SltThrt:
			return "Dth_SltThrt";
		case SActorSoundDefs::EDefinition::Dth_strngl:
			return "Dth_strngl";
		case SActorSoundDefs::EDefinition::Dth_Xplo:
			return "Dth_Xplo";
		case SActorSoundDefs::EDefinition::Dth_PrpF:
			return "Dth_PrpF";
		case SActorSoundDefs::EDefinition::Dth_Electro:
			return "Dth_Electro";
		case SActorSoundDefs::EDefinition::Dth_Burn:
			return "Dth_Burn";
		case SActorSoundDefs::EDefinition::Dth_Crush:
			return "Dth_Crush";
		case SActorSoundDefs::EDefinition::Gen_GrtHMCiv:
			return "Gen_GrtHMCiv";
		case SActorSoundDefs::EDefinition::Gen_GrtHMCop:
			return "Gen_GrtHMCop";
		case SActorSoundDefs::EDefinition::Gen_GrtHMElite:
			return "Gen_GrtHMElite";
		case SActorSoundDefs::EDefinition::Gen_GrtHMGuard:
			return "Gen_GrtHMGuard";
		case SActorSoundDefs::EDefinition::Gen_GtHlp:
			return "Gen_GtHlp";
		case SActorSoundDefs::EDefinition::Gen_GtHlpLd:
			return "Gen_GtHlpLd";
		case SActorSoundDefs::EDefinition::Gen_Hrt:
			return "Gen_Hrt";
		case SActorSoundDefs::EDefinition::Gen_Mssng:
			return "Gen_Mssng";
		case SActorSoundDefs::EDefinition::Gen_HMHere:
			return "Gen_HMHere";
		case SActorSoundDefs::EDefinition::Gen_HMThere:
			return "Gen_HMThere";
		case SActorSoundDefs::EDefinition::Gen_RdoDrm:
			return "Gen_RdoDrm";
		case SActorSoundDefs::EDefinition::Gen_Srps:
			return "Gen_Srps";
		case SActorSoundDefs::EDefinition::Gen_SrpsLd:
			return "Gen_SrpsLd";
		case SActorSoundDefs::EDefinition::Gen_SrpsGrabFt:
			return "Gen_SrpsGrabFt";
		case SActorSoundDefs::EDefinition::Gen_SrpsGrabBk:
			return "Gen_SrpsGrabBk";
		case SActorSoundDefs::EDefinition::Gen_StndRsp:
			return "Gen_StndRsp";
		case SActorSoundDefs::EDefinition::Gen_StopLd:
			return "Gen_StopLd";
		case SActorSoundDefs::EDefinition::ClsCmbt_Ack:
			return "ClsCmbt_Ack";
		case SActorSoundDefs::EDefinition::ClsCmbt_Tnt:
			return "ClsCmbt_Tnt";
		case SActorSoundDefs::EDefinition::Cmbt_BackupCll:
			return "Cmbt_BackupCll";
		case SActorSoundDefs::EDefinition::Cmbt_BackupRsp:
			return "Cmbt_BackupRsp";
		case SActorSoundDefs::EDefinition::Cmbt_BadDsg:
			return "Cmbt_BadDsg";
		case SActorSoundDefs::EDefinition::Cmbt_Beg:
			return "Cmbt_Beg";
		case SActorSoundDefs::EDefinition::Cmbt_CllOut:
			return "Cmbt_CllOut";
		case SActorSoundDefs::EDefinition::Cmbt_ClsAck:
			return "Cmbt_ClsAck";
		case SActorSoundDefs::EDefinition::Cmbt_CvrFirLdr:
			return "Cmbt_CvrFirLdr";
		case SActorSoundDefs::EDefinition::Cmbt_CvrFirRsp:
			return "Cmbt_CvrFirRsp";
		case SActorSoundDefs::EDefinition::Cmbt_Fire:
			return "Cmbt_Fire";
		case SActorSoundDefs::EDefinition::Cmbt_FireLdr:
			return "Cmbt_FireLdr";
		case SActorSoundDefs::EDefinition::Cmbt_GtHit:
			return "Cmbt_GtHit";
		case SActorSoundDefs::EDefinition::Cmbt_GtHitCiv:
			return "Cmbt_GtHitCiv";
		case SActorSoundDefs::EDefinition::Cmbt_HitHM:
			return "Cmbt_HitHM";
		case SActorSoundDefs::EDefinition::Cmbt_HMClsCmbtAck:
			return "Cmbt_HMClsCmbtAck";
		case SActorSoundDefs::EDefinition::Cmbt_HMClsCmbtPrpAck:
			return "Cmbt_HMClsCmbtPrpAck";
		case SActorSoundDefs::EDefinition::Cmbt_HMCvr:
			return "Cmbt_HMCvr";
		case SActorSoundDefs::EDefinition::Cmbt_HMFire:
			return "Cmbt_HMFire";
		case SActorSoundDefs::EDefinition::Cmbt_HMFlnk:
			return "Cmbt_HMFlnk";
		case SActorSoundDefs::EDefinition::Cmbt_HMHeadPopr:
			return "Cmbt_HMHeadPopr";
		case SActorSoundDefs::EDefinition::Cmbt_HMKll:
			return "Cmbt_HMKll";
		case SActorSoundDefs::EDefinition::Cmbt_HMKllCiv:
			return "Cmbt_HMKllCiv";
		case SActorSoundDefs::EDefinition::Cmbt_HMKllName:
			return "Cmbt_HMKllName";
		case SActorSoundDefs::EDefinition::Cmbt_HMKllPrpTss:
			return "Cmbt_HMKllPrpTss";
		case SActorSoundDefs::EDefinition::Cmbt_HMKllXplo:
			return "Cmbt_HMKllXplo";
		case SActorSoundDefs::EDefinition::Cmbt_HMMssTnt:
			return "Cmbt_HMMssTnt";
		case SActorSoundDefs::EDefinition::Cmbt_HMRelod:
			return "Cmbt_HMRelod";
		case SActorSoundDefs::EDefinition::Cmbt_HMShrpShtr:
			return "Cmbt_HMShrpShtr";
		case SActorSoundDefs::EDefinition::Cmbt_HMSht2Blw:
			return "Cmbt_HMSht2Blw";
		case SActorSoundDefs::EDefinition::Cmbt_HMSpttd:
			return "Cmbt_HMSpttd";
		case SActorSoundDefs::EDefinition::Cmbt_HMTrwXplo:
			return "Cmbt_HMTrwXplo";
		case SActorSoundDefs::EDefinition::Cmbt_HMVnshd:
			return "Cmbt_HMVnshd";
		case SActorSoundDefs::EDefinition::Cmbt_Hold:
			return "Cmbt_Hold";
		case SActorSoundDefs::EDefinition::Cmbt_HoldLdr:
			return "Cmbt_HoldLdr";
		case SActorSoundDefs::EDefinition::Cmbt_HumShldFrndFir:
			return "Cmbt_HumShldFrndFir";
		case SActorSoundDefs::EDefinition::Cmbt_HumShldRls1:
			return "Cmbt_HumShldRls1";
		case SActorSoundDefs::EDefinition::Cmbt_HumShldRls2:
			return "Cmbt_HumShldRls2";
		case SActorSoundDefs::EDefinition::Cmbt_HumShldRls3:
			return "Cmbt_HumShldRls3";
		case SActorSoundDefs::EDefinition::Cmbt_HumShldRlsFem1:
			return "Cmbt_HumShldRlsFem1";
		case SActorSoundDefs::EDefinition::Cmbt_HumShldRlsFem2:
			return "Cmbt_HumShldRlsFem2";
		case SActorSoundDefs::EDefinition::Cmbt_HumShldRlsFem3:
			return "Cmbt_HumShldRlsFem3";
		case SActorSoundDefs::EDefinition::Cmbt_HumShldVctm:
			return "Cmbt_HumShldVctm";
		case SActorSoundDefs::EDefinition::Cmbt_HumShldLdr:
			return "Cmbt_HumShldLdr";
		case SActorSoundDefs::EDefinition::Cmbt_HumShldBrth:
			return "Cmbt_HumShldBrth";
		case SActorSoundDefs::EDefinition::Cmbt_KllHM:
			return "Cmbt_KllHM";
		case SActorSoundDefs::EDefinition::Cmbt_LngLst:
			return "Cmbt_LngLst";
		case SActorSoundDefs::EDefinition::Cmbt_LngLstRsp:
			return "Cmbt_LngLstRsp";
		case SActorSoundDefs::EDefinition::Cmbt_LstMnStn:
			return "Cmbt_LstMnStn";
		case SActorSoundDefs::EDefinition::Cmbt_LstSght:
			return "Cmbt_LstSght";
		case SActorSoundDefs::EDefinition::Cmbt_LstSghtRsp:
			return "Cmbt_LstSghtRsp";
		case SActorSoundDefs::EDefinition::Cmbt_NdrAttck:
			return "Cmbt_NdrAttck";
		case SActorSoundDefs::EDefinition::Cmbt_Relod:
			return "Cmbt_Relod";
		case SActorSoundDefs::EDefinition::Cmbt_Scrm:
			return "Cmbt_Scrm";
		case SActorSoundDefs::EDefinition::Cmbt_TkDwnLdr:
			return "Cmbt_TkDwnLdr";
		case SActorSoundDefs::EDefinition::Cmbt_VntAck:
			return "Cmbt_VntAck";
		case SActorSoundDefs::EDefinition::Cmbt_Whmp:
			return "Cmbt_Whmp";
		case SActorSoundDefs::EDefinition::Cmbt_StalemateHold:
			return "Cmbt_StalemateHold";
		case SActorSoundDefs::EDefinition::Cmbt_StaleMateTnt:
			return "Cmbt_StaleMateTnt";
		case SActorSoundDefs::EDefinition::AvoidXplo:
			return "AvoidXplo";
		case SActorSoundDefs::EDefinition::AvoidXplo_Blw:
			return "AvoidXplo_Blw";
		case SActorSoundDefs::EDefinition::AvoidXplo_Stnd:
			return "AvoidXplo_Stnd";
		case SActorSoundDefs::EDefinition::Ar_BadDsg:
			return "Ar_BadDsg";
		case SActorSoundDefs::EDefinition::Ar_BadDsgRpt:
			return "Ar_BadDsgRpt";
		case SActorSoundDefs::EDefinition::Ar_BlmeKll:
			return "Ar_BlmeKll";
		case SActorSoundDefs::EDefinition::Ar_CivRptHMKnownAck:
			return "Ar_CivRptHMKnownAck";
		case SActorSoundDefs::EDefinition::Ar_ChngDsg:
			return "Ar_ChngDsg";
		case SActorSoundDefs::EDefinition::Ar_ChngDsgRpt:
			return "Ar_ChngDsgRpt";
		case SActorSoundDefs::EDefinition::Ar_DrgBody:
			return "Ar_DrgBody";
		case SActorSoundDefs::EDefinition::Ar_DrgBodyRpt:
			return "Ar_DrgBodyRpt";
		case SActorSoundDefs::EDefinition::Ar_FkeSrrdrTnt:
			return "Ar_FkeSrrdrTnt";
		case SActorSoundDefs::EDefinition::Ar_HMCmply:
			return "Ar_HMCmply";
		case SActorSoundDefs::EDefinition::Ar_HMDoor:
			return "Ar_HMDoor";
		case SActorSoundDefs::EDefinition::Ar_HMSecTp:
			return "Ar_HMSecTp";
		case SActorSoundDefs::EDefinition::Ar_HMSecTpRpt:
			return "Ar_HMSecTpRpt";
		case SActorSoundDefs::EDefinition::Ar_HMFseBx:
			return "Ar_HMFseBx";
		case SActorSoundDefs::EDefinition::Ar_HMFseBxRpt:
			return "Ar_HMFseBxRpt";
		case SActorSoundDefs::EDefinition::Ar_KyCrd:
			return "Ar_KyCrd";
		case SActorSoundDefs::EDefinition::Ar_KyCrdRpt:
			return "Ar_KyCrdRpt";
		case SActorSoundDefs::EDefinition::Ar_RunAway:
			return "Ar_RunAway";
		case SActorSoundDefs::EDefinition::Ar_RptX:
			return "Ar_RptX";
		case SActorSoundDefs::EDefinition::Ar_Trspss:
			return "Ar_Trspss";
		case SActorSoundDefs::EDefinition::Ar_TrspssRpt:
			return "Ar_TrspssRpt";
		case SActorSoundDefs::EDefinition::Ar_WeapWrn1:
			return "Ar_WeapWrn1";
		case SActorSoundDefs::EDefinition::Ar_WeapWrn2:
			return "Ar_WeapWrn2";
		case SActorSoundDefs::EDefinition::Ar_WeapWrnMelee1:
			return "Ar_WeapWrnMelee1";
		case SActorSoundDefs::EDefinition::Ar_WeapWrnMelee2:
			return "Ar_WeapWrnMelee2";
		case SActorSoundDefs::EDefinition::Ar_Wrn1:
			return "Ar_Wrn1";
		case SActorSoundDefs::EDefinition::Ar_Wrn2:
			return "Ar_Wrn2";
		case SActorSoundDefs::EDefinition::Ar_Wrn3:
			return "Ar_Wrn3";
		case SActorSoundDefs::EDefinition::InCa_BackupCll:
			return "InCa_BackupCll";
		case SActorSoundDefs::EDefinition::InCa_BackupRun:
			return "InCa_BackupRun";
		case SActorSoundDefs::EDefinition::InCa_ChckCvr:
			return "InCa_ChckCvr";
		case SActorSoundDefs::EDefinition::InCa_ChckNdrCr:
			return "InCa_ChckNdrCr";
		case SActorSoundDefs::EDefinition::InCa_CivCmnd:
			return "InCa_CivCmnd";
		case SActorSoundDefs::EDefinition::InCa_CivRptAck:
			return "InCa_CivRptAck";
		case SActorSoundDefs::EDefinition::InCa_CivRptFail:
			return "InCa_CivRptFail";
		case SActorSoundDefs::EDefinition::InCa_CivUpset:
			return "InCa_CivUpset";
		case SActorSoundDefs::EDefinition::InCa_ClstTnt:
			return "InCa_ClstTnt";
		case SActorSoundDefs::EDefinition::InCa_HMAsCivAck:
			return "InCa_HMAsCivAck";
		case SActorSoundDefs::EDefinition::InCa_HMAsCopAck:
			return "InCa_HMAsCopAck";
		case SActorSoundDefs::EDefinition::InCa_HMAsGrdAck:
			return "InCa_HMAsGrdAck";
		case SActorSoundDefs::EDefinition::InCa_HMAsEliteAck:
			return "InCa_HMAsEliteAck";
		case SActorSoundDefs::EDefinition::InCa_HMSpttd:
			return "InCa_HMSpttd";
		case SActorSoundDefs::EDefinition::InCa_HMTnt:
			return "InCa_HMTnt";
		case SActorSoundDefs::EDefinition::InCa_Idle:
			return "InCa_Idle";
		case SActorSoundDefs::EDefinition::InCa_NitiateHMKnwn:
			return "InCa_NitiateHMKnwn";
		case SActorSoundDefs::EDefinition::InCa_SrchLdr:
			return "InCa_SrchLdr";
		case SActorSoundDefs::EDefinition::InCa_SrchRsp:
			return "InCa_SrchRsp";
		case SActorSoundDefs::EDefinition::InCa_Stnd:
			return "InCa_Stnd";
		case SActorSoundDefs::EDefinition::InCa_StndAgtd:
			return "InCa_StndAgtd";
		case SActorSoundDefs::EDefinition::InCa_StndAgtdLdr:
			return "InCa_StndAgtdLdr";
		case SActorSoundDefs::EDefinition::InCa_StndAgtdHMKnwn:
			return "InCa_StndAgtdHMKnwn";
		case SActorSoundDefs::EDefinition::InCa_StndAgtdHMKnwnLdr:
			return "InCa_StndAgtdHMKnwnLdr";
		case SActorSoundDefs::EDefinition::InCa_StndHMKnwn:
			return "InCa_StndHMKnwn";
		case SActorSoundDefs::EDefinition::InCa_StndHMKnwnLdr:
			return "InCa_StndHMKnwnLdr";
		case SActorSoundDefs::EDefinition::InCa_StndLdr:
			return "InCa_StndLdr";
		case SActorSoundDefs::EDefinition::InCa_VntTnt:
			return "InCa_VntTnt";
		case SActorSoundDefs::EDefinition::InCa_ClsCmbtAck:
			return "InCa_ClsCmbtAck";
		case SActorSoundDefs::EDefinition::InCa_ClsCmbtInv:
			return "InCa_ClsCmbtInv";
		case SActorSoundDefs::EDefinition::InCa_ClsCmbtInvLdr:
			return "InCa_ClsCmbtInvLdr";
		case SActorSoundDefs::EDefinition::InCa_ClsCmbtInvRsp:
			return "InCa_ClsCmbtInvRsp";
		case SActorSoundDefs::EDefinition::InCa_ClsCmbtLdr:
			return "InCa_ClsCmbtLdr";
		case SActorSoundDefs::EDefinition::InCa_ClsCmbtRpt:
			return "InCa_ClsCmbtRpt";
		case SActorSoundDefs::EDefinition::InCa_ClsCmbtRsp:
			return "InCa_ClsCmbtRsp";
		case SActorSoundDefs::EDefinition::InCa_DthAck:
			return "InCa_DthAck";
		case SActorSoundDefs::EDefinition::InCa_DthInv:
			return "InCa_DthInv";
		case SActorSoundDefs::EDefinition::InCa_DthInvLdr:
			return "InCa_DthInvLdr";
		case SActorSoundDefs::EDefinition::InCa_DthInvRsp:
			return "InCa_DthInvRsp";
		case SActorSoundDefs::EDefinition::InCa_DthLdr:
			return "InCa_DthLdr";
		case SActorSoundDefs::EDefinition::InCa_DthRpt:
			return "InCa_DthRpt";
		case SActorSoundDefs::EDefinition::InCa_DthRsp:
			return "InCa_DthRsp";
		case SActorSoundDefs::EDefinition::InCa_XploAck:
			return "InCa_XploAck";
		case SActorSoundDefs::EDefinition::InCa_XploInv:
			return "InCa_XploInv";
		case SActorSoundDefs::EDefinition::InCa_XploInvLdr:
			return "InCa_XploInvLdr";
		case SActorSoundDefs::EDefinition::InCa_XploInvRsp:
			return "InCa_XploInvRsp";
		case SActorSoundDefs::EDefinition::InCa_XploLdr:
			return "InCa_XploLdr";
		case SActorSoundDefs::EDefinition::InCa_XploRpt:
			return "InCa_XploRpt";
		case SActorSoundDefs::EDefinition::InCa_XploRsp:
			return "InCa_XploRsp";
		case SActorSoundDefs::EDefinition::InCa_XploSeeAck:
			return "InCa_XploSeeAck";
		case SActorSoundDefs::EDefinition::InCa_XploSeeInv:
			return "InCa_XploSeeInv";
		case SActorSoundDefs::EDefinition::InCa_XploSeeInvLdr:
			return "InCa_XploSeeInvLdr";
		case SActorSoundDefs::EDefinition::InCa_XploSeeInvRsp:
			return "InCa_XploSeeInvRsp";
		case SActorSoundDefs::EDefinition::InCa_XploSeeLdr:
			return "InCa_XploSeeLdr";
		case SActorSoundDefs::EDefinition::InCa_XploSeeRpt:
			return "InCa_XploSeeRpt";
		case SActorSoundDefs::EDefinition::InCa_XploSeeRsp:
			return "InCa_XploSeeRsp";
		case SActorSoundDefs::EDefinition::InCa_GnShtAck:
			return "InCa_GnShtAck";
		case SActorSoundDefs::EDefinition::InCa_GnShtInv:
			return "InCa_GnShtInv";
		case SActorSoundDefs::EDefinition::InCa_GnShtInvLdr:
			return "InCa_GnShtInvLdr";
		case SActorSoundDefs::EDefinition::InCa_GnShtInvRsp:
			return "InCa_GnShtInvRsp";
		case SActorSoundDefs::EDefinition::InCa_GnShtLdr:
			return "InCa_GnShtLdr";
		case SActorSoundDefs::EDefinition::InCa_GnShtRsp:
			return "InCa_GnShtRsp";
		case SActorSoundDefs::EDefinition::InCa_GnShtRpt:
			return "InCa_GnShtRpt";
		case SActorSoundDefs::EDefinition::InCa_ScrmAck:
			return "InCa_ScrmAck";
		case SActorSoundDefs::EDefinition::InCa_ScrmInv:
			return "InCa_ScrmInv";
		case SActorSoundDefs::EDefinition::InCa_ScrmInvLdr:
			return "InCa_ScrmInvLdr";
		case SActorSoundDefs::EDefinition::InCa_ScrmInvRsp:
			return "InCa_ScrmInvRsp";
		case SActorSoundDefs::EDefinition::InCa_ScrmLdr:
			return "InCa_ScrmLdr";
		case SActorSoundDefs::EDefinition::InCa_ScrmRpt:
			return "InCa_ScrmRpt";
		case SActorSoundDefs::EDefinition::InCa_ScrmRsp:
			return "InCa_ScrmRsp";
		case SActorSoundDefs::EDefinition::InCa_GenAck:
			return "InCa_GenAck";
		case SActorSoundDefs::EDefinition::InCa_GenInv:
			return "InCa_GenInv";
		case SActorSoundDefs::EDefinition::InCa_GenInvLdr:
			return "InCa_GenInvLdr";
		case SActorSoundDefs::EDefinition::InCa_GenInvRsp:
			return "InCa_GenInvRsp";
		case SActorSoundDefs::EDefinition::InCa_GenLdr:
			return "InCa_GenLdr";
		case SActorSoundDefs::EDefinition::InCa_GenRpt:
			return "InCa_GenRpt";
		case SActorSoundDefs::EDefinition::InCa_GenRsp:
			return "InCa_GenRsp";
		case SActorSoundDefs::EDefinition::InCa_GenLdAck:
			return "InCa_GenLdAck";
		case SActorSoundDefs::EDefinition::InCa_GenLdInv:
			return "InCa_GenLdInv";
		case SActorSoundDefs::EDefinition::InCa_GenLdInvLdr:
			return "InCa_GenLdInvLdr";
		case SActorSoundDefs::EDefinition::InCa_GenLdInvRsp:
			return "InCa_GenLdInvRsp";
		case SActorSoundDefs::EDefinition::InCa_GenLdLdr:
			return "InCa_GenLdLdr";
		case SActorSoundDefs::EDefinition::InCa_GenLdRpt:
			return "InCa_GenLdRpt";
		case SActorSoundDefs::EDefinition::InCa_GenLdRsp:
			return "InCa_GenLdRsp";
		case SActorSoundDefs::EDefinition::Accdnt_Inv:
			return "Accdnt_Inv";
		case SActorSoundDefs::EDefinition::Accdnt_Wtch:
			return "Accdnt_Wtch";
		case SActorSoundDefs::EDefinition::InDedBdy_BloodPllAck:
			return "InDedBdy_BloodPllAck";
		case SActorSoundDefs::EDefinition::InDedBdy_Ack:
			return "InDedBdy_Ack";
		case SActorSoundDefs::EDefinition::InDedBdy_Inv:
			return "InDedBdy_Inv";
		case SActorSoundDefs::EDefinition::InDedBdy_Rpt:
			return "InDedBdy_Rpt";
		case SActorSoundDefs::EDefinition::InDedBdy_BllPllRpt:
			return "InDedBdy_BllPllRpt";
		case SActorSoundDefs::EDefinition::InDedBdy_Massive:
			return "InDedBdy_Massive";
		case SActorSoundDefs::EDefinition::InDedBdy_PcfdInv:
			return "InDedBdy_PcfdInv";
		case SActorSoundDefs::EDefinition::InDsg_Ack:
			return "InDsg_Ack";
		case SActorSoundDefs::EDefinition::InDsg_FllwWrn1:
			return "InDsg_FllwWrn1";
		case SActorSoundDefs::EDefinition::InDsg_FllwWrn2:
			return "InDsg_FllwWrn2";
		case SActorSoundDefs::EDefinition::InDsg_FllwWrn3:
			return "InDsg_FllwWrn3";
		case SActorSoundDefs::EDefinition::InDsg_Pzzl:
			return "InDsg_Pzzl";
		case SActorSoundDefs::EDefinition::InDsg_Stnd:
			return "InDsg_Stnd";
		case SActorSoundDefs::EDefinition::InDsg_HdNPlnSght:
			return "InDsg_HdNPlnSght";
		case SActorSoundDefs::EDefinition::Trspss_Stnd:
			return "Trspss_Stnd";
		case SActorSoundDefs::EDefinition::Trspss_Wrn1:
			return "Trspss_Wrn1";
		case SActorSoundDefs::EDefinition::Trspss_Wrn2:
			return "Trspss_Wrn2";
		case SActorSoundDefs::EDefinition::Trspss_Wrn3:
			return "Trspss_Wrn3";
		case SActorSoundDefs::EDefinition::Trspss_Rsp:
			return "Trspss_Rsp";
		case SActorSoundDefs::EDefinition::InCu_CivRsp:
			return "InCu_CivRsp";
		case SActorSoundDefs::EDefinition::InCu_BlltMpctAck:
			return "InCu_BlltMpctAck";
		case SActorSoundDefs::EDefinition::InCu_BlltMpctInv:
			return "InCu_BlltMpctInv";
		case SActorSoundDefs::EDefinition::InCu_BlltMpctLdr:
			return "InCu_BlltMpctLdr";
		case SActorSoundDefs::EDefinition::InCu_BlltMpctRsp:
			return "InCu_BlltMpctRsp";
		case SActorSoundDefs::EDefinition::InCu_BlltMpctStnd:
			return "InCu_BlltMpctStnd";
		case SActorSoundDefs::EDefinition::InCu_BlltMpctStndRsp:
			return "InCu_BlltMpctStndRsp";
		case SActorSoundDefs::EDefinition::InCu_CrAlrmAck:
			return "InCu_CrAlrmAck";
		case SActorSoundDefs::EDefinition::InCu_CrAlrmInv:
			return "InCu_CrAlrmInv";
		case SActorSoundDefs::EDefinition::InCu_CrAlrmLdr:
			return "InCu_CrAlrmLdr";
		case SActorSoundDefs::EDefinition::InCu_CrAlrmRsp:
			return "InCu_CrAlrmRsp";
		case SActorSoundDefs::EDefinition::InCu_CrAlrmStnd:
			return "InCu_CrAlrmStnd";
		case SActorSoundDefs::EDefinition::InCu_CrAlrmStndRsp:
			return "InCu_CrAlrmStndRsp";
		case SActorSoundDefs::EDefinition::InCu_FtStpsAck:
			return "InCu_FtStpsAck";
		case SActorSoundDefs::EDefinition::InCu_FtStpsStnd:
			return "InCu_FtStpsStnd";
		case SActorSoundDefs::EDefinition::InCu_NpcBrkAck:
			return "InCu_NpcBrkAck";
		case SActorSoundDefs::EDefinition::InCu_NpcBrkCiv:
			return "InCu_NpcBrkCiv";
		case SActorSoundDefs::EDefinition::InCu_NpcBrkInv:
			return "InCu_NpcBrkInv";
		case SActorSoundDefs::EDefinition::InCu_NpcBrkStnd:
			return "InCu_NpcBrkStnd";
		case SActorSoundDefs::EDefinition::InCu_PrpTssHearAck:
			return "InCu_PrpTssHearAck";
		case SActorSoundDefs::EDefinition::InCu_PrpTssHearInv:
			return "InCu_PrpTssHearInv";
		case SActorSoundDefs::EDefinition::InCu_PrpTssHearLdr:
			return "InCu_PrpTssHearLdr";
		case SActorSoundDefs::EDefinition::InCu_PrpTssHearRsp:
			return "InCu_PrpTssHearRsp";
		case SActorSoundDefs::EDefinition::InCu_PrpTssHearStnd:
			return "InCu_PrpTssHearStnd";
		case SActorSoundDefs::EDefinition::InCu_PrpTssHearStndRsp:
			return "InCu_PrpTssHearStndRsp";
		case SActorSoundDefs::EDefinition::InCu_PrpTssSeeAck:
			return "InCu_PrpTssSeeAck";
		case SActorSoundDefs::EDefinition::InCu_PrpTssSeeCiv:
			return "InCu_PrpTssSeeCiv";
		case SActorSoundDefs::EDefinition::InCu_PrpTssSeeInv:
			return "InCu_PrpTssSeeInv";
		case SActorSoundDefs::EDefinition::InCu_PrpTssSeeLdr:
			return "InCu_PrpTssSeeLdr";
		case SActorSoundDefs::EDefinition::InCu_PrpTssSeeRsp:
			return "InCu_PrpTssSeeRsp";
		case SActorSoundDefs::EDefinition::InCu_PrpTssSeeStnd:
			return "InCu_PrpTssSeeStnd";
		case SActorSoundDefs::EDefinition::InCu_PrpTssSeeStndRsp:
			return "InCu_PrpTssSeeStndRsp";
		case SActorSoundDefs::EDefinition::InCu_RdoAck:
			return "InCu_RdoAck";
		case SActorSoundDefs::EDefinition::InCu_RdoInv:
			return "InCu_RdoInv";
		case SActorSoundDefs::EDefinition::InCu_RdoLdr:
			return "InCu_RdoLdr";
		case SActorSoundDefs::EDefinition::InCu_RdoRsp:
			return "InCu_RdoRsp";
		case SActorSoundDefs::EDefinition::InCu_RdoStnd:
			return "InCu_RdoStnd";
		case SActorSoundDefs::EDefinition::InCu_RdoStndRsp:
			return "InCu_RdoStndRsp";
		case SActorSoundDefs::EDefinition::InCu_GuardStnd:
			return "InCu_GuardStnd";
		case SActorSoundDefs::EDefinition::InSt_Ack:
			return "InSt_Ack";
		case SActorSoundDefs::EDefinition::InSt_HMAglty:
			return "InSt_HMAglty";
		case SActorSoundDefs::EDefinition::InSt_HMBz:
			return "InSt_HMBz";
		case SActorSoundDefs::EDefinition::InSt_HMBzStnd:
			return "InSt_HMBzStnd";
		case SActorSoundDefs::EDefinition::InSt_HMEntXit:
			return "InSt_HMEntXit";
		case SActorSoundDefs::EDefinition::InSt_HMInCvr:
			return "InSt_HMInCvr";
		case SActorSoundDefs::EDefinition::InSt_HMRun:
			return "InSt_HMRun";
		case SActorSoundDefs::EDefinition::InSt_HMSnkng:
			return "InSt_HMSnkng";
		case SActorSoundDefs::EDefinition::InSt_PrpTssSee:
			return "InSt_PrpTssSee";
		case SActorSoundDefs::EDefinition::InSt_Stnd:
			return "InSt_Stnd";
		case SActorSoundDefs::EDefinition::InSt_Wrn:
			return "InSt_Wrn";
		case SActorSoundDefs::EDefinition::InSt_HM2Cls:
			return "InSt_HM2Cls";
		case SActorSoundDefs::EDefinition::FseBx_Fixed:
			return "FseBx_Fixed";
		case SActorSoundDefs::EDefinition::FseBx_Fixing:
			return "FseBx_Fixing";
		case SActorSoundDefs::EDefinition::FseBx_GoFix:
			return "FseBx_GoFix";
		case SActorSoundDefs::EDefinition::FseBx_SabAck:
			return "FseBx_SabAck";
		case SActorSoundDefs::EDefinition::_Last:
			return "_Last";
		default:
			return "";
	};
}

std::string SoundDefinitions::Entry::ConvertDoorSoundDefinitionToString(const SDoorSoundDefs::EDefinition definition)
{
	switch (definition)
	{
		case SDoorSoundDefs::EDefinition::DoorOpen:
			return "DoorOpen";
		case SDoorSoundDefs::EDefinition::DoorClose:
			return "DoorClose";
		case SDoorSoundDefs::EDefinition::DoorSlam:
			return "DoorSlam";
		case SDoorSoundDefs::EDefinition::DoorOpenStop:
			return "DoorOpenStop";
		default:
			return "";
	}
}

std::string SoundDefinitions::Entry::ConvertSoundPlayParametersToString(const ESoundPlayParameters soundPlayParameters)
{
	switch (soundPlayParameters)
	{
		case ESoundPlayParameters::SOUND_PLAY_SINGLE:
			return "Single";
		case ESoundPlayParameters::SOUND_PLAY_RANDOM:
			return "Random";
		case ESoundPlayParameters::SOUND_PLAY_SHUFFLE:
			return "Shuffle";
		default:
			return "";
	}
}

void SoundDefinitions::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());
	const unsigned int entryCount = binaryReader.Read<unsigned int>();

	entries.reserve(entryCount);

	for (unsigned int i = 0; i < entryCount; ++i)
	{
		Entry entry;

		entry.definition = binaryReader.Read<int>();
		entry.referenceIndex = binaryReader.Read<unsigned int>();
		entry.attenuationOffset = binaryReader.Read<int>();
		entry.groupNumber = binaryReader.Read<int>();
		entry.selectionMode = static_cast<ESoundPlayParameters>(binaryReader.Read<unsigned int>());
		entry.noRepeatsCount = binaryReader.Read<int>();

		const int numSubSoundRepeatCounts = binaryReader.Read<int>();

		for (unsigned int j = 0; j < numSubSoundRepeatCounts; ++j)
		{
			entry.subSoundRepeatCounts.push_back(binaryReader.Read<unsigned char>());
		}

		entries.push_back(entry);
	}

	isResourceDeserialized = true;
}

void SoundDefinitions::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		SerializeToJson(outputPath);
	}
}

void SoundDefinitions::SerializeToJson(const std::string& outputFilePath)
{
	const bool hasActorSoundDefinitions = entries.size() - 1 == static_cast<int>(SActorSoundDefs::EDefinition::_Last);

	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	writer.StartObject();

	writer.String("entries");
	writer.StartArray();

	const std::vector<std::shared_ptr<Resource>>& references = GetReferences();

	for (size_t i = 0; i < entries.size(); ++i)
	{
		entries[i].SerializeToJson(references, writer, hasActorSoundDefinitions);
	}

	writer.EndArray();

	writer.EndObject();

	std::ofstream outputFileStream = std::ofstream(outputFilePath);

	outputFileStream << stringBuffer.GetString();

	outputFileStream.close();
}

std::vector<SoundDefinitions::Entry>& SoundDefinitions::GetEntries()
{
	return entries;
}
