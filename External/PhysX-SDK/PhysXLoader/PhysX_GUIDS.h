/*
 * Copyright 2009-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

// This PhysXUpdateLoader GUID header file.  It defines the GUIDS for 
// the 284 DLLs that can be update loaded by the PhysXUpdateLoader.
// Notify the Nvidia PhysX team if you need a new GUID to represent a 
// new unique version of one of the following DLLs.
// These GUIDS come from the excel file in Perforce.
#ifndef PHYSX_UPDATE_LOADER_GUIDS_H
#define PHYSX_UPDATE_LOADER_GUIDS_H

#define PHYSX_284_CORE32_GUID    "BE9042F0-ADC4-4b12-A93E-DB8A731FDBD5"
#define PHYSX_284_CORE64_GUID    "A68D0501-0C67-464c-9FEC-224FB3E1F47C"
#define PHYSX_284_COOKING32_GUID "8F2594AC-F8B3-47f2-BDFC-86310FED9255"
#define PHYSX_284_COOKING64_GUID "27E8F218-1860-4a70-87D7-FAF241B19951"
#define PHYSX_284_DEVICE32_GUID  "AC74A51E-8741-413e-92FB-36B255F9FDC3"
#define PHYSX_284_DEVICE64_GUID  "DF84AF09-2B86-46c8-BB9B-5D8D40436E8D"

#ifdef WIN64
#define PHYSX_284_CORE_GUID    PHYSX_284_CORE64_GUID
#define PHYSX_284_COOKING_GUID PHYSX_284_COOKING64_GUID
#define PHYSX_284_DEVICE_GUID  PHYSX_284_DEVICE64_GUID
#else
#define PHYSX_284_CORE_GUID    PHYSX_284_CORE32_GUID
#define PHYSX_284_COOKING_GUID PHYSX_284_COOKING32_GUID
#define PHYSX_284_DEVICE_GUID  PHYSX_284_DEVICE32_GUID
#endif

#endif
