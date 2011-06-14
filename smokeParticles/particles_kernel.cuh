/*
 * Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NVIDIA Corporation and its licensors retain all intellectual property and 
 * proprietary rights in and to this software and related documentation. 
 * Any use, reproduction, disclosure, or distribution of this software 
 * and related documentation without an express license agreement from
 * NVIDIA Corporation is strictly prohibited.
 *
 * Please refer to the applicable NVIDIA end user license agreement (EULA) 
 * associated with this source code for terms and conditions that govern 
 * your use of this NVIDIA software.
 * 
 */
 
 #ifndef PARTICLES_KERNEL_H
#define PARTICLES_KERNEL_H

#ifdef USE_TEX
#define FETCH(t, i) tex1Dfetch(t##Tex, i)
#else
#define FETCH(t, i) t[i]
#endif

#include "vector_types.h"
typedef unsigned int uint;

struct SimParams {
    float3 gravity;
    float globalDamping;
    float noiseFreq;
    float noiseAmp;
    float3 cursorPos;
    
    float time;
    float3 noiseSpeed;
};

struct float4x4 {
    float m[16];
};

#endif
