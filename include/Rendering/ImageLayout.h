#pragma once

enum class ImageLayout
{
    Undefined,
    General,
    Preinitialized,
    Color_Attachment_Optimal,
    Depth_Attachment_Optimal,
    Depth_Stencil_Attachment_Optimal,
    Depth_Stencil_Read_Only_Optimal,
    Shader_Read_Only_Optimal,
    Transfer_Src_Optimal,
    Transfer_Dst_Optimal,
    Present_Src
};
