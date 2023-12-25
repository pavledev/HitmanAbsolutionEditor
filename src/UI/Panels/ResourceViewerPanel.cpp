#include <filesystem>

#include "directxtk/DDSTextureLoader.h"
#include "directxtk/ScreenGrab.h"

//extern "C"
//{
//#include "libavcodec/avcodec.h"
//#include "libavformat/avformat.h"
//#include "libswscale/swscale.h"
//}

#include "misc/cpp/imgui_stdlib.h"

#include "UI/Panels/ResourceViewerPanel.h"
#include "Resources/Texture.h"
#include "Resources/MorphemeRuntimeNetwork.h"
#include "Resources/MorphemeRuntimeRig.h"
#include "Resources/MorphemeRuntimeEventTrack.h"
#include "Resources/RenderPrimitive.h"
#include "Registry/ResourceInfoRegistry.h"
#include "Resources/RenderMaterialInstance.h"
#include "Logger.h"
#include "IO/MemoryInputAVIOContext.h"
#include "Resources/FabricResourceEntity.h"
#include "Resources/Cloth.h"
#include "Resources/SoundDefinitions.h"
#include "Resources/SoundBlendContainerExternalParametersType.h"
#include "Utility/ResourceUtility.h"
#include "UI/Panels/SceneHierarchyPanel2.h"
#include "Editor.h"

ResourceViewerPanel::ResourceViewerPanel(const char* name, const char* icon, std::shared_ptr<Resource> resource) : BasePanel(name, icon)
{
    this->resource = resource;
    textureView = nullptr;
    texture = nullptr;

    textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::JSON());

    viewportWidth = 0;
    viewportHeight = 0;
}

void ResourceViewerPanel::Render()
{
    ImGuiWindowFlags windowFlags{};

    if (resource->GetResourceHeaderHeader().m_type == 'AIBB')
    {
        windowFlags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove;
    }
    else if (resource->GetResourceHeaderHeader().m_type == 'PRIM')
    {
        windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
    }

    if (!Begin(windowFlags))
    {
        return;
    }

    if (!resource->GetResourceData())
    {
        ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
        ImGui::Text("Loading data...");
        End();

        return;
    }

    switch (resource->GetResourceHeaderHeader().m_type)
    {
        case 'AIBB':
            DisplayBehaviorTreeEntityBlueprint();
            break;
        case 'AIRG':
            break;
        case 'ALOC':
            DisplayCollisionData();
            break;
        case 'BORG':
            DisplayBoneRigData();
            break;
        case 'CBLU':
            DisplayCppEntityBlueprint();
            break;
        case 'GIDX':
            DisplayGlobalIndex();
            break;
        case 'TEXT':
            DisplayTexture();
            break;
        case 'MRTN':
            DisplayMorphemeRuntimeNetwork();
            break;
        case 'MRTR':
            DisplayMorphemeRuntimeRig();
            break;
        case 'MRTE':
            DisplayMorphemeRuntimeEventTrack();
            break;
        case 'PRIM':
            DisplayModel();
            break;
        case 'VIDE':
            DisplayVideoPlayer();
            break;
        case 'CLOT':
            DisplayFabricResourceEntity();
            break;
        case 'CLOB':
            DisplayFabricResourceEntityBlueprint();
            break;
        case 'CLOS':
            DisplayCloth();
            break;
        case 'SDEF':
            DisplaySoundDefinitions();
            break;
        case 'SBPD':
            DisplaySoundBlendContainerExternalParametersType();
            break;
        default:
            break;
    }

    End();
}

void ImageViewer(ImTextureID texture, const ImVec2& texture_size, float& zoom, const ImVec2& canvas_size, ImVec2 anchor_pos = ImGui::GetMousePos())
{
    
}

void ResourceViewerPanel::DisplayTexture()
{
    if (!textureView)
    {
        ID3D11Resource* d3d11Texture = nullptr;
        unsigned short width;
        unsigned short height;
        DirectX::Blob blob;
        std::shared_ptr<Texture> texture = std::static_pointer_cast<Texture>(resource);

        texture->Deserialize();
        texture->ConvertTextureToDDS(blob, width, height);
        DirectX::CreateDDSTextureFromMemory(Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device(), static_cast<unsigned char*>(blob.GetBufferPointer()), blob.GetBufferSize(), &d3d11Texture, &textureView);

        this->textureWidth = static_cast<float>(width);
        this->textureHeight = static_cast<float>(height);
    }

    //ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - textureWidth) / 2, (ImGui::GetWindowSize().y - textureHeight) / 2));
    //ImGui::Image(textureView, ImVec2(textureWidth, textureHeight));
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    canvas_size.y -= ImGui::GetFrameHeightWithSpacing();
    static float zoom = 1.0f;

    ImageViewer(textureView, ImVec2((float)textureWidth, (float)textureHeight), zoom, canvas_size);
}

void ResourceViewerPanel::DisplayBehaviorTreeEntityBlueprint()
{
    if (!behaviorTreeEntityBlueprint)
    {
        behaviorTreeEntityBlueprint = std::make_shared<BehaviorTreeEntityBlueprint>();

        behaviorTreeEntityBlueprint->Deserialize(resource->GetResourceData(), resource->GetResourceDataSize());
    }

    SBehaviorTreeInfo* behaviorTreeInfo = behaviorTreeEntityBlueprint->GetBehaviorTreeInfo();
    static ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    if (ImGui::BeginTable("behaviorTreeEntityBlueprint", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
    {
        ImGui::PushID(&behaviorTreeInfo->m_references);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();

        bool isReferencesNodeOpen = ImGui::TreeNode("m_references");

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("");

        if (isReferencesNodeOpen)
        {
            for (unsigned int i = 0; i < behaviorTreeInfo->m_references.Size(); ++i)
            {
                ImGui::PushID(&behaviorTreeInfo->m_references[i]);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();

                bool isBehaviorTreeEntityReferenceNodeOpen = ImGui::TreeNode("object", "SBehaviorTreeEntityReference %d", i);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("");

                if (isBehaviorTreeEntityReferenceNodeOpen)
                {
                    ImGui::PushID(&behaviorTreeInfo->m_references[i].m_bList);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::AlignTextToFramePadding();

                    ImGui::TreeNodeEx("field", treeNodeFlags, "m_bList", i);

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Checkbox("##value", &behaviorTreeInfo->m_references[i].m_bList);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    ImGui::PushID(&behaviorTreeInfo->m_references[i].m_sName);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::AlignTextToFramePadding();

                    ImGui::TreeNodeEx("field", treeNodeFlags, "m_sName", i);

                    std::string name = behaviorTreeInfo->m_references[i].m_sName.ToCString();

                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##value", &name);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    ImGui::TreePop();
                }

                ImGui::NextColumn();

                ImGui::PopID();
            }

            ImGui::TreePop();
        }

        ImGui::PopID();

        ImGui::PushID(&behaviorTreeInfo->m_inputPinConditions);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();

        bool isInputPinConditionsNodeOpen = ImGui::TreeNode("m_inputPinConditions");

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("");

        if (isInputPinConditionsNodeOpen)
        {
            for (unsigned int i = 0; i < behaviorTreeInfo->m_inputPinConditions.Size(); ++i)
            {
                ImGui::PushID(&behaviorTreeInfo->m_inputPinConditions[i]);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();

                bool isBehaviorTreeInputPinConditionNodeOpen = ImGui::TreeNode("object", "SBehaviorTreeInputPinCondition %d", i);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("");

                if (isBehaviorTreeInputPinConditionNodeOpen)
                {
                    ImGui::PushID(&behaviorTreeInfo->m_references[i].m_sName);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::AlignTextToFramePadding();

                    ImGui::TreeNodeEx("field", treeNodeFlags, "m_sName", i);

                    std::string name = behaviorTreeInfo->m_inputPinConditions[i].m_sName.ToCString();

                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##value", &name);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    ImGui::TreePop();
                }

                ImGui::NextColumn();

                ImGui::PopID();
            }

            ImGui::TreePop();
        }

        ImGui::PopID();

        ImGui::EndTable();
    }
}

void ResourceViewerPanel::DisplayCollisionData()
{
    physics->Deserialize(resource->GetResourceData(), resource->GetResourceDataSize());
}

void ResourceViewerPanel::DisplayBoneRigData()
{
    boneRig->Deserialize(resource->GetResourceData(), resource->GetResourceDataSize());
}

void ResourceViewerPanel::DisplayCppEntityBlueprint()
{
    if (!cppEntityBlueprint->GetCppEntityBlueprint())
    {
        cppEntityBlueprint->Deserialize();
    }

    //SCppEntityBlueprint* cppEntityBlueprint2 = cppEntityBlueprint->GetCppEntityBlueprint();
    SCppEntityBlueprint* cppEntityBlueprint2;
    static ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

    if (ImGui::BeginTable("cppEntityBlueprint", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
    {
        ImGui::PushID(&cppEntityBlueprint2->typeName);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();

        ImGui::TreeNodeEx("field", treeNodeFlags, "typeName");

        std::string typeName = cppEntityBlueprint2->typeName->pTypeInfo->GetTypeName();

        ImGui::TableSetColumnIndex(1);
        ImGui::InputText("##value", &typeName);

        ImGui::PopID();

        ImGui::PushID(&cppEntityBlueprint2->subsets);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();

        bool isSubsetsNodeOpen = ImGui::TreeNode("subsets");

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("");

        if (isSubsetsNodeOpen)
        {
            for (unsigned int i = 0; i < cppEntityBlueprint2->subsets.Size(); ++i)
            {
                ImGui::PushID(&cppEntityBlueprint2->subsets[i]);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();

                bool isCppEntitySubsetInfoNodeOpen = ImGui::TreeNode("object", "SCppEntitySubsetInfo %d", i);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("");

                if (isCppEntitySubsetInfoNodeOpen)
                {
                    ImGui::PushID(&cppEntityBlueprint2->subsets[i].name);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::AlignTextToFramePadding();

                    ImGui::TreeNodeEx("field", treeNodeFlags, "name", i);

                    std::string name = cppEntityBlueprint2->subsets[i].name.ToCString();

                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##value", &name);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    ImGui::PushID(&cppEntityBlueprint2->subsets[i].type);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::AlignTextToFramePadding();

                    ImGui::TreeNodeEx("field", treeNodeFlags, "type", i);

                    std::string type = cppEntityBlueprint2->subsets[i].type->pTypeInfo->GetTypeName();

                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##value", &type);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    ImGui::PushID(&cppEntityBlueprint2->subsets[i].flags);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::AlignTextToFramePadding();

                    ImGui::TreeNodeEx("field", treeNodeFlags, "flags", i);

                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputScalar("##value", ImGuiDataType_U32, &cppEntityBlueprint2->subsets[i].flags);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    ImGui::TreePop();
                }

                ImGui::NextColumn();

                ImGui::PopID();
            }

            ImGui::TreePop();
        }

        ImGui::PopID();

        ImGui::EndTable();
    }
}

void ResourceViewerPanel::DisplayGlobalIndex()
{
    if (!globalResourceIndex->GetResourceIndex())
    {
        globalResourceIndex->Deserialize(resource->GetResourceData(), resource->GetResourceDataSize());
    }

    SResourceIndex* resourceIndex = globalResourceIndex->GetResourceIndex();
    static ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

    if (ImGui::BeginTable("globalResourceIndex", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
    {
        ImGui::PushID(&resourceIndex->folders);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();

        bool isFoldersNodeOpen = ImGui::TreeNode("folders");

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("");

        if (isFoldersNodeOpen)
        {
            for (unsigned int i = 0; i < resourceIndex->folders.Size(); ++i)
            {
                ImGui::PushID(&resourceIndex->folders[i]);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();

                bool isResourceIndexFolderNodeOpen = ImGui::TreeNode("object", "SResourceIndexFolder %d", i);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("");

                if (isResourceIndexFolderNodeOpen)
                {
                    ImGui::PushID(&resourceIndex->folders[i].name);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::AlignTextToFramePadding();

                    ImGui::TreeNodeEx("field", treeNodeFlags, "name", i);

                    std::string name = resourceIndex->folders[i].name.ToCString();

                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##value", &name);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    ImGui::PushID(&resourceIndex->folders[i].resourceIndices);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::AlignTextToFramePadding();

                    bool isResourceIndicesNodeOpen = ImGui::TreeNodeEx("resourceIndices");

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("");

                    if (isResourceIndicesNodeOpen)
                    {
                        for (unsigned int j = 0; j < resourceIndex->folders[i].resourceIndices.Size(); ++j)
                        {
                            ImGui::PushID(&resourceIndex->folders[i].resourceIndices[j]);
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::AlignTextToFramePadding();

                            ImGui::TreeNodeEx("item", treeNodeFlags, "Index %d", j);

                            std::string name = std::to_string(resourceIndex->folders[i].resourceIndices[j]);

                            ImGui::TableSetColumnIndex(1);
                            ImGui::InputText("##value", &name);
                            ImGui::NextColumn();
                            ImGui::PopID();
                        }

                        ImGui::TreePop();
                    }

                    ImGui::PopID();

                    ImGui::TreePop();
                }

                ImGui::PopID();
            }

            ImGui::TreePop();
        }

        ImGui::PopID();

        ImGui::EndTable();
    }
}

void ResourceViewerPanel::DisplayMorphemeRuntimeNetwork()
{
    if (json.empty())
    {
        MorphemeRuntimeNetwork morphemeRuntimeNetwork;

        morphemeRuntimeNetwork.Deserialize(resource->GetResourceData());
        morphemeRuntimeNetwork.SerializeToJson(json);

        textEditor.SetText(json);
    }

    textEditor.Render();
}

void ResourceViewerPanel::DisplayMorphemeRuntimeRig()
{
    if (json.empty())
    {
        MorphemeRuntimeRig morphemeRuntimeRig;

        morphemeRuntimeRig.Deserialize(resource->GetResourceData());
        morphemeRuntimeRig.SerializeToJson(json);

        textEditor.SetText(json);
    }

    textEditor.Render();
}

void ResourceViewerPanel::DisplayMorphemeRuntimeEventTrack()
{
    if (json.empty())
    {
        MorphemeRuntimeEventTrack morphemeRuntimeEventTrack;

        morphemeRuntimeEventTrack.Deserialize(resource->GetResourceData());
        morphemeRuntimeEventTrack.SerializeToJson(json);

        textEditor.SetText(json);
    }

    textEditor.Render();
}

void ResourceViewerPanel::DisplayModel()
{
    float width = ImGui::GetContentRegionAvail().x;
    float height = ImGui::GetContentRegionAvail().y;

    std::shared_ptr<RenderPrimitive> renderPrimitive = std::static_pointer_cast<RenderPrimitive>(resource);

    if (renderPrimitive->GetMeshes().size() == 0)
    {
        renderPrimitive->Deserialize(resource->GetResourceData(), resource->GetResourceDataSize());

        //unsigned char lod = renderPrimitive->GetMeshes()[0]->GetLODMask();
        //unsigned char lod2 = renderPrimitive->GetMeshes()[1]->GetLODMask();
        //renderPrimitive->ConvertToOBJ("Bottle.obj");
        //renderPrimitive->ConvertToOBJ("Guitar.obj");
        //renderPrimitive->ConvertToOBJ("Guard.obj");

        const unsigned int boneRigResourceIndex = renderPrimitive->GetBoneRigResourceIndex();

        if (boneRigResourceIndex != -1)
        {
            std::vector<std::shared_ptr<Resource>>& primReferences = resource->GetReferences();
            std::shared_ptr<Resource> borgResource = primReferences[boneRigResourceIndex];
            const ResourceInfoRegistry::ResourceInfo& borgResourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(borgResource->GetHash());

            borgResource->SetHeaderLibraries(&borgResourceInfo.headerLibraries);
            borgResource->LoadResource(0, borgResourceInfo.headerLibraries[0].chunkIndex, borgResourceInfo.headerLibraries[0].indexInLibrary, false, false, true);
            boneRig->Deserialize(borgResource->GetResourceData(), borgResource->GetResourceDataSize());
            //renderPrimitive->SetBoneRig(&boneRig);
        }

        //renderPrimitive->ConvertToGLB("model", "C:\\Users\\Pavle\\Desktop\\model\\model.glb", true);

        Resource* alocResource = nullptr;
        std::vector<std::shared_ptr<Resource>>& primBackReferences = resource->GetBackReferences();
        bool isAlocResourceFound = false;

        for (size_t i = 0; i < primBackReferences.size(); ++i)
        {
            std::shared_ptr<Resource> backReference = primBackReferences[i];
            const ResourceInfoRegistry::ResourceInfo& backReferenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(backReference->GetHash());

            if (backReferenceInfo.type == "TEMP")
            {
                backReference->SetHeaderLibraries(&backReferenceInfo.headerLibraries);
                backReference->LoadResource(0, backReferenceInfo.headerLibraries[0].chunkIndex, backReferenceInfo.headerLibraries[0].indexInLibrary, true, false, false);

                std::vector<std::shared_ptr<Resource>>& tempReferences = backReference->GetReferences();

                for (size_t j = 0; j < tempReferences.size(); ++j)
                {
                    std::shared_ptr<Resource> reference = tempReferences[j];
                    const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(reference->GetHash());

                    if (referenceInfo.type == "ALOC")
                    {
                        reference->SetHeaderLibraries(&referenceInfo.headerLibraries);
                        reference->LoadResource(0, referenceInfo.headerLibraries[0].chunkIndex, referenceInfo.headerLibraries[0].indexInLibrary, false, false, true);
                        physics->Deserialize(reference->GetResourceData(), reference->GetResourceDataSize());
                        //renderPrimitive->SetPhysics(&physics);

                        isAlocResourceFound = true;

                        break;
                    }
                }

                if (isAlocResourceFound)
                {
                    break;
                }
            }
        }

        if (!isAlocResourceFound)
        {
            Logger::GetInstance().Log(Logger::Level::Warning, "Couldn't find ALOC resource!");
        }
    }

    if (!SceneHierarchyPanel2::GetRootEntity())
    {
        return;
    }

    if (viewportWidth == 0 || viewportHeight == 0)
    {
        viewportWidth = width;
        viewportHeight = height;

        sceneRenderer.Setup(viewportWidth, viewportHeight, renderPrimitive);
    }

    if (viewportWidth != width || viewportHeight != height)
    {
        viewportWidth = width;
        viewportHeight = height;

        sceneRenderer.OnViewportSizeChanged(viewportWidth, viewportHeight);
    }

    sceneRenderer.Render();
}

void ResourceViewerPanel::DisplayVideoPlayer()
{
    static bool startThread = true;

    if (startThread)
    {
        videoThread = std::thread(&ResourceViewerPanel::DecodeVideo, this);

        videoThread.detach();

        startThread = false;
    }
}

void ResourceViewerPanel::DecodeVideo()
{
    //AVFormatContext* formatContext = nullptr;
    //AVCodecContext* codecContext = nullptr;
    //AVCodecParameters* codecParameters = nullptr;
    //int videoStreamIndex = -1;
    //AVFrame* frame = nullptr;

    //while (true)
    //{
    //    if (resource && resource->GetResourceData())
    //    {
    //        //if (!texture)
    //        //{
    //        BinaryReader binaryReader = BinaryReader(resource->GetResourceData(), resource->GetResourceDataSize());
    //        MemoryInputAVIOContext privateContext = MemoryInputAVIOContext(binaryReader);
    //        formatContext = avformat_alloc_context();

    //        formatContext->pb = privateContext.GetIOContext();

    //        int result = avformat_open_input(&formatContext, "", nullptr, nullptr);

    //        if (result < 0)
    //        {
    //            return;
    //        }

    //        result = avformat_find_stream_info(formatContext, nullptr);

    //        if (result < 0)
    //        {
    //            avformat_close_input(&formatContext);

    //            return;
    //        }

    //        for (unsigned int i = 0; i < formatContext->nb_streams; ++i)
    //        {
    //            if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
    //            {
    //                videoStreamIndex = i;

    //                break;
    //            }
    //        }

    //        if (videoStreamIndex == -1)
    //        {
    //            avformat_close_input(&formatContext);

    //            return;
    //        }

    //        codecParameters = formatContext->streams[videoStreamIndex]->codecpar;
    //        const AVCodec* pCodec = avcodec_find_decoder(codecParameters->codec_id);

    //        if (!pCodec)
    //        {
    //            avformat_close_input(&formatContext);

    //            return;
    //        }

    //        codecContext = avcodec_alloc_context3(pCodec);

    //        if (!codecContext)
    //        {
    //            avformat_close_input(&formatContext);

    //            return;
    //        }

    //        result = avcodec_parameters_to_context(codecContext, codecParameters);

    //        if (result < 0)
    //        {
    //            avcodec_free_context(&codecContext);
    //            avformat_close_input(&formatContext);

    //            return;
    //        }

    //        result = avcodec_open2(codecContext, pCodec, nullptr);

    //        if (result < 0)
    //        {
    //            avcodec_free_context(&codecContext);
    //            avformat_close_input(&formatContext);

    //            return;
    //        }

    //        frame = av_frame_alloc();

    //        if (!frame)
    //        {
    //            avcodec_free_context(&codecContext);
    //            avformat_close_input(&formatContext);

    //            return;
    //        }

    //        Editor::GetInstance().GetDirectXRenderer()->CreateTexture(codecParameters->width, codecParameters->height, &texture, &textureView);
    //        //}

    //        AVPacket packet;

    //        while (av_read_frame(formatContext, &packet) == 0)
    //        {
    //            if (packet.stream_index == videoStreamIndex)
    //            {
    //                int response = avcodec_send_packet(codecContext, &packet);

    //                if (response < 0)
    //                {
    //                    av_packet_unref(&packet);
    //                    av_frame_free(&frame);
    //                    avcodec_free_context(&codecContext);
    //                    avformat_close_input(&formatContext);

    //                    return;
    //                }

    //                while (response >= 0)
    //                {
    //                    response = avcodec_receive_frame(codecContext, frame);

    //                    if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
    //                    {
    //                        av_packet_unref(&packet);

    //                        continue;
    //                    }
    //                    else if (response < 0)
    //                    {
    //                        av_packet_unref(&packet);
    //                        av_frame_free(&frame);
    //                        avcodec_free_context(&codecContext);
    //                        avformat_close_input(&formatContext);

    //                        return;
    //                    }

    //                    D3D11_MAPPED_SUBRESOURCE mappedResource;

    //                    Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext()->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    //                    unsigned char* destination = static_cast<unsigned char*>(mappedResource.pData);
    //                    int destinationPitch = mappedResource.RowPitch;

    //                    for (int y = 0; y < codecContext->height; ++y)
    //                    {
    //                        memcpy(destination, frame->data[0] + y * frame->linesize[0], codecContext->width * 4);

    //                        destination += destinationPitch;
    //                    }

    //                    Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext()->Unmap(texture, 0);

    //                    ImGui::Image(textureView, ImVec2(codecParameters->width, codecParameters->height));
    //                }
    //            }

    //            av_packet_unref(&packet);
    //        }

    //        av_frame_free(&frame);
    //        avcodec_free_context(&codecContext);
    //        avformat_close_input(&formatContext);
    //    }
    //}
}

void ResourceViewerPanel::DisplayFabricResourceEntity()
{
    if (json.empty())
    {
        FabricResourceEntity fabricResourceEntity;
        std::vector<std::shared_ptr<Resource>>& clotReferences = resource->GetReferences();

        fabricResourceEntity.Deserialize(resource->GetResourceData(), resource->GetResourceDataSize());

        for (size_t i = 0; i < clotReferences.size(); ++i)
        {
            std::shared_ptr<Resource> reference = clotReferences[i];
            const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(reference->GetHash());

            if (referenceInfo.type == "CLOB")
            {
                FabricResourceEntityBlueprint fabricResourceEntityBlueprint;

                reference->SetHeaderLibraries(&referenceInfo.headerLibraries);
                reference->LoadResource(0, referenceInfo.headerLibraries[0].chunkIndex, referenceInfo.headerLibraries[0].indexInLibrary, true, false, true);
                fabricResourceEntityBlueprint.Deserialize(reference->GetResourceData(), reference->GetResourceDataSize());

                json = fabricResourceEntity.SerializeToJson(fabricResourceEntityBlueprint);

                reference->DeleteResourceData();

                break;
            }
        }

        textEditor.SetText(json);
    }

    textEditor.Render();
}

void ResourceViewerPanel::DisplayFabricResourceEntityBlueprint()
{
    if (json.empty())
    {
        FabricResourceEntityBlueprint fabricResourceEntityBlueprint;

        fabricResourceEntityBlueprint.Deserialize(resource->GetResourceData(), resource->GetResourceDataSize());

        json = fabricResourceEntityBlueprint.SerializeToJson();

        textEditor.SetText(json);
    }

    textEditor.Render();
}

void ResourceViewerPanel::DisplayCloth()
{
    if (json.empty())
    {
        Cloth cloth;

        cloth.Deserialize(resource->GetResourceData(), resource->GetResourceDataSize());
        cloth.SerializeToXML(json);

        textEditor.SetText(json);
    }

    textEditor.Render();
}

void ResourceViewerPanel::DisplaySoundDefinitions()
{
    if (json.empty())
    {
        SoundDefinitions soundDefinitions;

        //soundDefinitions.Deserialize(resource->GetResourceData(), resource->GetResourceDataSize());
        //soundDefinitions.SerializeToJson(resource, json);

        textEditor.SetText(json);
    }

    textEditor.Render();
}

void ResourceViewerPanel::DisplaySoundBlendContainerExternalParametersType()
{
    if (json.empty())
    {
        SoundBlendContainerExternalParametersType soundBlendContainerExternalParametersType;

        soundBlendContainerExternalParametersType.Deserialize();
        json = soundBlendContainerExternalParametersType.SerializeToJson();

        textEditor.SetText(json);
    }

    textEditor.Render();
}

SceneRenderer& ResourceViewerPanel::GetSceneRenderer()
{
    return sceneRenderer;
}
