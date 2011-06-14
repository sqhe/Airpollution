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
 
 /*
	This file contains simple wrapper functions that call the CUDA kernels
 */

#include <cutil_inline.h>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <GL/glew.h>
#include <cuda_gl_interop.h>

#include "particles_kernel.cu"
#include "ParticleSystem.cuh"

extern "C"
{

cudaArray *noiseArray;

void initCuda(bool bUseGL)
{
	if (bUseGL) {
	    cudaGLSetGLDevice( cutGetMaxGflopsDeviceId() );
	} else {
		cudaSetDevice( cutGetMaxGflopsDeviceId() );
	}
}

void setParameters(SimParams *hostParams)
{
    // copy parameters to constant memory
    cutilSafeCall( cudaMemcpyToSymbol(params, hostParams, sizeof(SimParams)) );
}
void setActiveArea(float *area)
{
    // copy parameters to constant memory
    cutilSafeCall( cudaMemcpyToSymbol(d_activeArea, area, sizeof(float)*6) );
}
//Round a / b to nearest higher integer value
int iDivUp(int a, int b){
    return (a % b != 0) ? (a / b + 1) : (a / b);
}

// compute grid and thread block size for a given number of elements
void computeGridSize(unsigned int n, int blockSize, int &numBlocks, int &numThreads)
{
    numThreads = min(blockSize, n);
    numBlocks = iDivUp(n, numThreads);
}

inline float frand()
{
    return rand() / (float) RAND_MAX;
}

// create 3D texture containing random values
void createNoiseTexture(int w, int h, int d)
{
    cudaExtent size = make_cudaExtent(w, h, d);
    uint elements = (uint) size.width*size.height*size.depth;

    float *volumeData = (float *)malloc(elements*4*sizeof(float));
    float *ptr = volumeData;
    for(uint i=0; i<elements; i++) {
        *ptr++ = frand()*2.0f-1.0f;
        *ptr++ = frand()*2.0f-1.0f;
        *ptr++ = frand()*2.0f-1.0f;
        *ptr++ = frand()*2.0f-1.0f;
    }


    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<float4>();
    cutilSafeCall( cudaMalloc3DArray(&noiseArray, &channelDesc, size) );

    cudaMemcpy3DParms copyParams = { 0 };
    copyParams.srcPtr   = make_cudaPitchedPtr((void*)volumeData, size.width*sizeof(float4), size.width, size.height);
    copyParams.dstArray = noiseArray;
    copyParams.extent   = size;
    copyParams.kind     = cudaMemcpyHostToDevice;
    cutilSafeCall( cudaMemcpy3D(&copyParams) );

    free(volumeData);

    // set texture parameters
    noiseTex.normalized = true;                      // access with normalized texture coordinates
    noiseTex.filterMode = cudaFilterModeLinear;      // linear interpolation
    noiseTex.addressMode[0] = cudaAddressModeWrap;   // wrap texture coordinates
    noiseTex.addressMode[1] = cudaAddressModeWrap;
    noiseTex.addressMode[2] = cudaAddressModeWrap;

    // bind array to 3D texture
    cutilSafeCall(cudaBindTextureToArray(noiseTex, noiseArray, channelDesc));
}
void bindAirData(float4 *verPtr,float4 *windPtr,int4 *tetPtr,int4 *neighborPtr,unsigned int numVer,unsigned int numTet)
{
	//cudaChannelFormatDesc channelDescf4 = cudaCreateChannelDesc<float4>();
	//cudaChannelFormatDesc channelDescui4 = cudaCreateChannelDesc<uint4>();
	cutilSafeCall(cudaBindTexture(0, air_ver_Tex, verPtr,numVer*sizeof(float4))); //, &channelDescf4
	cutilSafeCall(cudaBindTexture(0, air_wind_Tex, windPtr,numVer*sizeof(float4))); //,&channelDescf4,
	cutilSafeCall(cudaBindTexture(0, air_tet_Tex, tetPtr,numTet*sizeof(uint4))); //,&channelDescui4
	cutilSafeCall(cudaBindTexture(0, air_neighbor_Tex, neighborPtr,numTet*sizeof(uint4)));//,&channelDescui4
}
void createStartPos(unsigned int num)
{
	cutilSafeCall(cudaMalloc((void **) &d_start_pos, num*sizeof(float4)));
	cutilSafeCall(cudaMalloc((void **) &d_start_tetId, num*sizeof(int1)));
}
void freeStartPos()
{
	if (d_start_pos) {
        cutilSafeCall(cudaFree(d_start_pos));
        d_start_pos = 0;
    }
	if (d_start_tetId) {
        cutilSafeCall(cudaFree(d_start_tetId));
        d_start_tetId = 0;
    }
}
void bindStartPos(float4 *startPosPer,int1 *startTetId,unsigned int beginIndex,unsigned int num,unsigned int numParticles)
{
	cutilSafeCall(cudaMemcpy((void *)&d_start_pos[beginIndex], (void *) &startPosPer[beginIndex], num*sizeof(float4), cudaMemcpyDeviceToDevice));
	cutilSafeCall(cudaMemcpy((void *)&d_start_tetId[beginIndex], (void *) &startTetId[beginIndex], num*sizeof(int1), cudaMemcpyDeviceToDevice));
	cutilSafeCall(cudaBindTexture(0, start_pos_Tex, d_start_pos,numParticles*sizeof(float4))); //, &channelDescf4
	cutilSafeCall(cudaBindTexture(0, start_tetId_Tex, d_start_tetId,numParticles*sizeof(int1))); //, &channelDescf4
}
void unbindStartPos()
{
	cutilSafeCall(cudaUnbindTexture(start_pos_Tex)) ;
	cutilSafeCall(cudaUnbindTexture(start_tetId_Tex)) ;	
}
void unbindAirData()
{
	cutilSafeCall(cudaUnbindTexture(air_ver_Tex)) ;
	cutilSafeCall(cudaUnbindTexture(air_wind_Tex)) ;
	cutilSafeCall(cudaUnbindTexture(air_tet_Tex)) ;
	cutilSafeCall(cudaUnbindTexture(air_neighbor_Tex)) ;
}
void 
integrateSystem(float4 *oldPos, float4 *newPos,
				float4 *oldVel, float4 *newVel,
				int1 *oldTetId, int1 *newTetId,
                float deltaTime,
                unsigned int numParticles,
				float particleLifetime)//, float4 *intersection
{
    int numThreads;
	int numBlocks;
    computeGridSize(numParticles, 256, numBlocks, numThreads);

    // execute the kernel
    integrateD<<< numBlocks, numThreads >>>(newPos, newVel, newTetId,
                                            oldPos, oldVel,oldTetId,											
                                            deltaTime,
											numParticles,
											particleLifetime);//, intersection
    
    // check if kernel invocation generated an error
    cutilCheckMsg("Kernel execution failed");
}


void 
calcDepth(float4*  pos, 
		  float*   keys,		// output
          uint*    indices,		// output 
          float3   sortVector,
          unsigned int      numParticles)
{
    int numThreads;
	int numBlocks;
    computeGridSize(numParticles, 256, numBlocks, numThreads);

    // execute the kernel
    calcDepthD<<< numBlocks, numThreads >>>(pos, keys, indices, sortVector, numParticles);
    cutilCheckMsg("calcDepthD execution failed");
}

}   // extern "C"
