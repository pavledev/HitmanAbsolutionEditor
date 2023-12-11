#pragma once

#include <unordered_map>
#include <string>

class GlobalBoneRegistry
{
public:
	static GlobalBoneRegistry& GetInstance();
	void Load();
	unsigned int GetBoneID(const std::string& boneName);
	unsigned int RegisterBoneID(const std::string& boneName);

private:
	GlobalBoneRegistry() = default;
	GlobalBoneRegistry(const GlobalBoneRegistry& other) = delete;
	GlobalBoneRegistry& operator=(const GlobalBoneRegistry& other) = delete;

	std::unordered_map<std::string, unsigned int> boneNameToGlobalIDMap;
	const char* boneNames[63] =
	{
		"ground", "pelvis", "spine", "spine1", "spine2", "neck", "neck1", "head", "PONYTAIL", "l_thigh", "lthightwist", "lthightwist1", "lthightwist2",
		"l_calf", "l_foot", "l_toe0", "r_thigh", "rthightwist", "rthightwist1", "rthightwist2", "r_calf", "r_foot", "r_toe0", "l_clavicle", "l_upperarm",
		"luparmtwist", "luparmtwist1", "luparmtwist2", "l_forearm", "l_foretwist", "l_foretwist1", "l_foretwist2", "l_hand", "l_finger0", "r_clavicle",
		"r_upperarm", "ruparmtwist", "ruparmtwist1", "ruparmtwist2", "r_forearm", "r_foretwist", "r_foretwist1", "r_foretwist2", "r_hand", "r_finger0",
		"xtra_mus_r_hip", "xtra_mus_l_hip", "r_hand_attacher", "l_hand_attacher", "hip_attacher", "back1_attacher", "free_attacher", "camera_attacher_01",
		"camera_attacher_02", "free_attacher_prop_01", "free_attacher_prop_02", "fa_left_eye", "fa_left_eyelid", "fa_right_eye", "fa_right_eyelid",
		"fa_lower_lip", "fa_jaw", "holster_attacher"
	};
};
