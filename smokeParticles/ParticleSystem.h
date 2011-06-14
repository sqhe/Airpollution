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

#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__

#include "particles_kernel.cuh"
#include "vector_functions.h"
#include "cudpp/cudpp.h"
#include "GpuArray.h"
#include "nvMath.h"
#include "cudpp/cudpp.h"
#include <vector>
#include "SmokeSystem.h"
using namespace nv;

// CUDA BodySystem: runs on the GPU
class ParticleSystem
{
public:
    ParticleSystem(uint numParticles, bool bUseVBO = true, bool bUseGL = true);
    ~ParticleSystem();
    
    void step(float deltaTime,unsigned int endIndex,float particleLifetime);
    void depthSort();
    bool reset(ParticleConfig config,unsigned int beginIndex,unsigned int num,float lifetime);
	void setAirData();
	bool hasSetAirData(){return hasSetAirData_;}
	uint getPosBuffer() { return m_pos.getVbo(); }
	float4* getPos(){ return m_pos.getHostPtr(); }
	float4* getVel(){return m_vel.getHostPtr();}
	//float4* getIntersections(){return m_debug_intersection.getHostPtr();}
	int1* getVerTetIds(){return m_air_ver_tetId.getHostPtr();}
	int4* getNeighbors(){return m_air_neighbor.getHostPtr();}
	int4* getTets(){return m_air_tet.getHostPtr();}
	float4* getVers(){return m_air_ver.getHostPtr();}
	std::vector<float3>* getPathes(){return particle_path;}
	std::vector<float>* getPathVel(){return pathVel;}
	float getPathVelMin(){return vel_min;}
	float getPathVelMax(){return vel_max;}
    uint getVelBuffer() { return m_vel.getVbo(); }
    uint getColorBuffer() { return 0; }
    uint getSortedIndexBuffer() { return m_indices.getVbo(); }
    uint *getSortedIndices();

    float getParticleRadius() { return m_particleRadius; }

    SimParams &getParams() { return m_params; }

    void setSorting(bool x) { m_doDepthSort = x; }
    void setModelView(float *m);
    void setSortVector(float3 v) { m_sortVector = v; }

    void addSphere(uint &index, vec3f pos, vec3f vel, int r, float spacing, float jitter, float lifetime);
    void discEmitter(uint &index, vec3f pos, vec3f vel, vec3f vx, vec3f vy, float r, int n, float lifetime, float lifetimeVariance);
    void sphereEmitter(uint &index, vec3f pos, vec3f vel, vec3f spread, float r, int n, float lifetime, float lifetimeVariance);

    void dumpParticles(uint start, uint count);
    void dumpBin(float4 **posData, float4 **velData);
	int getMaxPathN(){return maxPathN;}
protected: // methods
    ParticleSystem() {}

    void _initialize(uint numParticlesm, bool bUseGL=true);
    void _free();
	void initAirParticles(float lifetime,unsigned int beginIndex,unsigned int num);
    void initGrid(vec3f start, uint3 size, vec3f spacing, float jitter, vec3f vel, uint numParticles, float lifetime=100.0f);
	void initCubeRandom(vec3f origin, vec3f size, vec3f vel, float lifetime=100.0f);
	bool initParticlesInLet(float lifetime,unsigned int beginIndex,unsigned int num);
protected: // data
    bool m_bInitialized;
    bool m_bUseVBO;
	uint m_numAirVer;
	uint m_numAirTet;
	unsigned int maxPathN;
    float m_particleRadius;

	//GpuArray<float4> m_start_pos;
	//for tracing the path
	bool* pathCompleted;
	std::vector<float>* pathVel;
	float vel_min;
	float vel_max;
	int* preTetId;//the tet id in the previous step
    GpuArray<float4> m_pos;
    GpuArray<float4> m_vel;
	GpuArray<float4> m_air_ver;
	GpuArray<float4> m_air_wind;
	GpuArray<int4> m_air_tet;
	GpuArray<int4> m_air_neighbor;
	GpuArray<int1> m_air_ver_tetId;
	std::vector<float3>* particle_path;
	//////////////////////////////////////////////////////////////////////////
	//debug
	//GpuArray<float4> m_debug_intersection;
	//////////////////////////////////////////////////////////////////////////

    // params
    SimParams m_params;

    float4x4 m_modelView;
    float3 m_sortVector;
    bool m_doDepthSort;
	bool hasSetAirData_;
	//int m_record_step;//record one sample of the path every record_step steps
	
    CUDPPHandle m_sortHandle;
	GpuArray<float> m_sortKeys;
	GpuArray<uint> m_indices;   // sorted indices for rendering

    uint m_timer;
	float m_time;
};

#endif // __PARTICLESYSTEM_H__
