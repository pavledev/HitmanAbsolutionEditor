#pragma once

#include <d3d11.h>
#include <thread>

#include "BasePanel.h"
#include "Resources/HeaderLibrary.h"
#include "TextEditor.h"
#include "Rendering/DirectXRenderer.h"
#include "Resources/BehaviorTreeEntityBlueprint.h"
#include "Resources/ReasoningGrid.h"
#include "Resources/Physics.h"
#include "Resources/BoneRig.h"
#include "Resources/CppEntityBlueprint.h"
#include "Resources/GlobalResourceIndex.h"
#include "Rendering/Scene/SceneRenderer.h"

class ResourceViewerPanel : public BasePanel
{
public:
	ResourceViewerPanel(const char* name, const char* icon, std::shared_ptr<Resource> resource);
	void Render() override;
	void DisplayTexture();
	void DisplayBehaviorTreeEntityBlueprint();
	void DisplayCollisionData();
	void DisplayBoneRigData();
	void DisplayCppEntityBlueprint();
	void DisplayGlobalIndex();
	void DisplayMorphemeRuntimeNetwork();
	void DisplayMorphemeRuntimeRig();
	void DisplayMorphemeRuntimeEventTrack();
	void DisplayModel();
	void DisplayVideoPlayer();
	void DecodeVideo();
	void DisplayFabricResourceEntity();
	void DisplayFabricResourceEntityBlueprint();
	void DisplayCloth();
	void DisplaySoundDefinitions();
	void DisplaySoundBlendContainerExternalParametersType();
	SceneRenderer& GetSceneRenderer();

private:
	std::shared_ptr<Resource> resource;

	ID3D11ShaderResourceView* textureView;
	float textureWidth;
	float textureHeight;

	TextEditor textEditor;
	std::string json;

	ID3D11Texture2D* texture;
	std::thread videoThread;

	std::shared_ptr<BehaviorTreeEntityBlueprint> behaviorTreeEntityBlueprint;
	std::shared_ptr<ReasoningGrid> reasoningGrid;
	std::shared_ptr<Physics> physics;
	std::shared_ptr<BoneRig> boneRig;
	std::shared_ptr<CppEntityBlueprint> cppEntityBlueprint;
	std::shared_ptr<GlobalResourceIndex> globalResourceIndex;

	SceneRenderer sceneRenderer;
	float viewportWidth;
	float viewportHeight;
	//BoneRig boneRig;
};
