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
 
 #include "particles_kernel.cuh"

extern "C"
{
void initCuda(bool bUseGL);
void setParameters(SimParams *hostParams);
void setActiveArea(float *area);
void createNoiseTexture(int w, int h, int d);

void 
integrateSystem(float4 *oldPos, float4 *newPos,
				float4 *oldVel, float4 *newVel,
				int1 *oldTetId, int1 *newTetId,
                float deltaTime,
                unsigned int numParticles,
				float particleLifetime);//, float4 *intersection
void bindAirData(float4 *verPtr,float4 *windPtr,
				 int4 *tetPtr,int4 *neighborPtr,
				 unsigned int numVer,unsigned int numTet);
void unbindAirData();
void bindStartPos(float4 *startPosPer,int1 *startTetId,unsigned int beginIndex,unsigned int num,unsigned int numParticles);
void unbindStartPos();
void createStartPos(unsigned int num);
void freeStartPos();
void 
calcDepth(float4*  pos, 
		  float*   keys,		// output
          uint*    indices,		// output 
          float3   sortVector,
          unsigned int      numParticles);
          
}