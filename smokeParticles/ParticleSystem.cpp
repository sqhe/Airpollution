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
 
 #include "stdafx.h"
#include <assert.h>
#include <math.h>
#include <memory.h>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <GL/glew.h>
#include <fstream>
#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cutil_inline.h>

#include "ParticleSystem.h"
#include "ParticleSystem.cuh"
#include "particles_kernel.cuh"

#ifndef CUDART_PI_F
#define CUDART_PI_F         3.141592654f
#endif

/*
    This handles the particle simulation using CUDA
*/
bool (*checkInLet)(float, float,float);
bool checkInBoxLet(float x, float y,float z)
{
	if(x>=obj->let_x_min&&x<=obj->let_x_max
		&&y>=obj->let_y_min&&y<=obj->let_y_max
		&&z>=obj->let_z_min&&z<=obj->let_z_max)
		return true;
	return false;
}
bool checkInSphereLet(float x, float y,float z)
{
	float x_dis=x-obj->let_center_x;
	float y_dis=y-obj->let_center_y;
	float z_dis=z-obj->let_center_z;
	float dist2=x_dis*x_dis+y_dis*y_dis+z_dis*z_dis;
	if(dist2<=obj->let_radius2)
		return true;
	return false;
}
bool checkInVolumnLet(float x, float y,float z)
{
	return true;
}
bool checkInPlainLet(float x, float y,float z)
{
	return true;
}
ParticleSystem::ParticleSystem(uint numParticles, bool bUseVBO, bool bUseGL) :
    m_bInitialized(false),
    m_bUseVBO(bUseVBO),
    m_particleRadius(0.1),
    m_doDepthSort(false),
    m_timer(0),
    m_time(0.0f)
{
    m_params.gravity = make_float3(0.0f, 0.0f, 0.0f);
    m_params.globalDamping = 1.0f;
	m_params.noiseSpeed = make_float3(0.0f, 0.0f, 0.0f);
	maxPathN=100;
	if(maxPathN>numParticles)maxPathN=numParticles;
	hasSetAirData_=false;
	pathCompleted=new bool[maxPathN];
	pathVel=new std::vector<float>[maxPathN];
	preTetId=new int[maxPathN];
	particle_path=new std::vector<float3>[maxPathN];
	vel_min=1e6;
	vel_max=0;
    _initialize(numParticles, bUseGL);
	//m_record_step=1;
}

ParticleSystem::~ParticleSystem()
{
    _free();
}

void
ParticleSystem::_initialize(uint numParticles, bool bUseGL)
{
    assert(!m_bInitialized);

    initCuda(bUseGL);
    createNoiseTexture(64, 64, 64);
	createStartPos(numParticles);

	// allocate GPU arrays
    m_pos.alloc(numParticles, m_bUseVBO, true);    // create as VBO
    m_vel.alloc(numParticles, m_bUseVBO, true);
	m_air_ver_tetId.alloc(numParticles, false, true);
	//////////////////////////////////////////////////////////////////////////
	//debug
	//m_debug_intersection.alloc(numParticles, false);
	//////////////////////////////////////////////////////////////////////////
    // Create the CUDPP radix sort
    CUDPPConfiguration sortConfig;
    sortConfig.algorithm = CUDPP_SORT_RADIX;
    sortConfig.datatype = CUDPP_FLOAT;
    sortConfig.op = CUDPP_ADD;
    sortConfig.options = CUDPP_OPTION_KEY_VALUE_PAIRS;
    cudppPlan(&m_sortHandle, sortConfig, numParticles, 1, 0);

	m_sortKeys.alloc(numParticles);
    m_indices.alloc(numParticles, m_bUseVBO, false, true); // create as index buffer

    cutilCheckError(cutCreateTimer(&m_timer));
    setParameters(&m_params);

    m_bInitialized = true;
}

void
ParticleSystem::_free()
{
	unbindAirData();
	unbindStartPos();
	freeStartPos();
	delete [] preTetId;
	delete [] pathCompleted;
    assert(m_bInitialized);
    cudppDestroyPlan(m_sortHandle);
	for (int i=0;i<maxPathN;++i)
	{
		particle_path[i].clear();
		pathVel[i].clear();
	}
	delete [] particle_path;
	delete [] pathVel;
}
//////////////////////////////////////////////////////////////////////////debug
//bool outputFile=true;
//////////////////////////////////////////////////////////////////////////
// step the simulation

void 
ParticleSystem::step(float deltaTime,unsigned int endIndex, float particleLifetime)
{
	if (beingInit!=_NUM_CONFIGS)
	{
		++m_cur_step;
	}	
    assert(m_bInitialized);

	m_params.time = m_time;
    setParameters(&m_params);

   
	//////////////////////////////////////////////////////////////////////////
	//debug
//  	m_pos.copy(GpuArray<float4>::DEVICE_TO_HOST);
//  	m_vel.copy(GpuArray<float4>::DEVICE_TO_HOST);
//  	m_air_ver_tetId.copy(GpuArray<int1>::DEVICE_TO_HOST);
// 	float4* velPtr=m_vel.getHostPtr();
// 	for (int i=0;i<numParticles;++i)
// 	{
// 		printf("%d: (%f,%f,%f)",i,velPtr[i].x,velPtr[i].y,velPtr[i].z);
// 	}
	//////////////////////////////////////////////////////////////////////////
    // integrate particles
	m_pos.map();
	m_vel.map();
	m_air_ver_tetId.map();
	integrateSystem(m_pos.getDevicePtr(), m_pos.getDeviceWritePtr(),
					m_vel.getDevicePtr(), m_vel.getDeviceWritePtr(),
					m_air_ver_tetId.getDevicePtr(), m_air_ver_tetId.getDeviceWritePtr(),
                    deltaTime,
                    endIndex,
					particleLifetime);//,m_debug_intersection.getDeviceWritePtr()
	//////////////////////////////////////////////////////////////////////////
	//debug
//	m_debug_intersection.copy(GpuArray<float4>::DEVICE_TO_HOST);
	//////////////////////////////////////////////////////////////////////////
    m_pos.unmap();
    m_vel.unmap();
	m_air_ver_tetId.unmap();
	
    m_pos.swap();
    m_vel.swap();
	m_air_ver_tetId.swap();
	//record the path
	unsigned int  pathN=min_1(endIndex,maxPathN);
	m_pos.copy(GpuArray<float4>::DEVICE_TO_HOST,0,pathN);
	m_air_ver_tetId.copy(GpuArray<int1>::DEVICE_TO_HOST,0,pathN);
	m_vel.copy(GpuArray<float4>::DEVICE_TO_HOST,0,pathN);
	float4 *new_pos=m_pos.getHostPtr();
	int1* new_tetId=m_air_ver_tetId.getHostPtr();
	float4 *new_vel=m_vel.getHostPtr();
	float preVel;
	//record one sample each tet 
	for (int i=0;i<pathN;++i)
	{
		if (!pathCompleted[i])
		{
			if(preTetId[i]!=new_tetId[i].x)
			{
				preVel=pathVel[i].back();
				if (preVel<vel_min)
					vel_min=preVel;
				if (preVel>vel_max)
					vel_max=preVel;
				particle_path[i].push_back(make_float3(new_pos[i].x, new_pos[i].y, new_pos[i].z));
				pathVel[i].push_back(sqrt(new_vel[i].x*new_vel[i].x+new_vel[i].y*new_vel[i].y+new_vel[i].z*new_vel[i].z));
				if (new_tetId[i].x<0)
					pathCompleted[i]=true;
			}
			else
				particle_path[i].back()=make_float3(new_pos[i].x, new_pos[i].y, new_pos[i].z);
				pathVel[i].back()=sqrt(new_vel[i].x*new_vel[i].x+new_vel[i].y*new_vel[i].y+new_vel[i].z*new_vel[i].z);
		}	
		preTetId[i]=new_tetId[i].x;
	}
	m_time += deltaTime;
	//////////////////////////////////////////////////////////////////////////debug
// 	if (outputFile)
// 	{
// 		std::ofstream ofile1("error_particles.txt");
// 		for (int i=0;i<numParticles;++i)
// 		{
// 			
// 			if (new_pos[i].w==5)
// 			{
// 				//ofile1<<"error particle:bary("<<new_pos[i].x<<","<<new_pos[i].y<<","<<new_pos[i].z<<","<<new_vel[i].x<<")"<<"t3:"<<new_vel[i].y <<"t_back:"<<new_vel[i].z<<std::endl;
// 				ofile1<<"error particle:t("<<new_pos[i].x<<","<<new_pos[i].y<<","<<new_pos[i].z<<","<<new_vel[i].x<<") t_back:"<<new_vel[i].y<<std::endl;
// 				//ofile1<<"error particle:vel("<<new_vel[i].x<<","<<new_vel[i].y<<","<<new_vel[i].z<<") length:"<<sqrt(new_vel[i].x*new_vel[i].x+new_vel[i].y*new_vel[i].y+new_vel[i].z*new_vel[i].z)<<std::endl;
// 			}
// 			
// 		}
// 		ofile1.close();
// 		outputFile=false;
// 	}
	//////////////////////////////////////////////////////////////////////////
}

// depth sort the particles
void
ParticleSystem::depthSort()
{
    if (!m_doDepthSort||initCount==0)
        return;

    m_pos.map();
    m_indices.map();

    // calculate depth
    calcDepth(m_pos.getDevicePtr(), m_sortKeys.getDevicePtr(), m_indices.getDevicePtr(), m_sortVector, initCount);

    // radix sort
    cudppSort(m_sortHandle, m_sortKeys.getDevicePtr(), m_indices.getDevicePtr(), 32, initCount);

    m_pos.unmap();
    m_indices.unmap();
}

uint *
ParticleSystem::getSortedIndices()
{
	// copy sorted indices back to CPU
    m_indices.copy(GpuArray<uint>::DEVICE_TO_HOST);
    return m_indices.getHostPtr();
}

// random float [0, 1]
inline float frand()
{
    return rand() / (float) RAND_MAX;
}

// signed random float [-1, 1]
inline float sfrand()
{
    return frand()*2.0f-1.0f;
}

// random signed vector
inline vec3f svrand()
{
    return vec3f(sfrand(), sfrand(), sfrand());
}
// random unsigned vector
inline vec3f vrand()
{
	return vec3f(frand(), frand(), frand());
}

// random point in circle
inline vec2f randCircle()
{
    vec2f r;
    do {
        r = vec2f(sfrand(), sfrand());
    } while(length(r) > 1.0f);
    return r;
}

// random point in sphere
inline vec3f randSphere()
{
    vec3f r;
    do {
        r = svrand();
    } while(length(r) > 1.0f);
    return r;
}
inline vec3f float32vec3f(float3 f)
{
	return vec3f(f.x,f.y,f.z);
}
inline vec3f float32vec3f(float4 f)
{
	return vec3f(f.x,f.y,f.z);
}
inline vec4f float42vec4f(float4 f)
{
	return vec4f(f.x,f.y,f.z,f.w);
}
// random Bary Centric Coordinates
inline vec4f randBaryCentricCoord()
{
	float x,y,z,w;
	do {
		x = frand();
		y = frand();
		z = frand();
		w=1.0-x-y-z;
	} while( w < 0.0f);
	return vec4f(x,y,z,w);
}
// initialize in regular grid
void
ParticleSystem::initGrid(vec3f start, uint3 size, vec3f spacing, float jitter, vec3f vel, uint numParticles, float lifetime)
{
    srand(1973);

    float4 *posPtr = m_pos.getHostPtr();
    float4 *velPtr = m_vel.getHostPtr();

    for(uint z=0; z<size.z; z++) {
        for(uint y=0; y<size.y; y++) {
            for(uint x=0; x<size.x; x++) {
                uint i = (z*size.y*size.x) + (y*size.x) + x;

                if (i < numParticles) {
					vec3f pos = start + spacing*vec3f(x, y, z) + svrand()*jitter;

					posPtr[i] = make_float4(pos.x, pos.y, pos.z, 0.0f);
				    velPtr[i] = make_float4(vel.x, vel.y, vel.z, lifetime);
                }
            }
        }
    }
}
void
ParticleSystem::initAirParticles(float lifetime,unsigned int beginIndex,unsigned int num)
{
	float4 *posPtr = m_pos.getHostPtr();
	float4 *velPtr = m_vel.getHostPtr();
	float4 *verPtr = m_air_ver.getHostPtr();
	float4 *windPtr = m_air_wind.getHostPtr();
	int4 *tetPtr = m_air_tet.getHostPtr();
	int1* tetIdPtr=m_air_ver_tetId.getHostPtr();
	//uint4 *neighborPtr = m_air_neighbor.getHostPtr();
	uint i;
	int tet_id;
	uint endIndex=min_1(beginIndex+num,numParticles);
	for(uint i=beginIndex; i<endIndex; i++) 
	{
		tet_id=(int)(frand()*(m_numAirTet-1));
		int4 vel_id=tetPtr[tet_id];
		vec4f baryCentricCoord =randBaryCentricCoord();

		vec3f pos=baryCentricCoord.x*float32vec3f(verPtr[vel_id.x])+baryCentricCoord.y*float32vec3f(verPtr[vel_id.y])+baryCentricCoord.z*float32vec3f(verPtr[vel_id.z])+baryCentricCoord.w*float32vec3f(verPtr[vel_id.w]);
		vec3f vel=baryCentricCoord.x*float32vec3f(windPtr[vel_id.x])+baryCentricCoord.y*float32vec3f(windPtr[vel_id.y])+baryCentricCoord.z*float32vec3f(windPtr[vel_id.z])+baryCentricCoord.w*float32vec3f(windPtr[vel_id.w]);
		tetIdPtr[i].x=tet_id;//debug
		posPtr[i] = make_float4(pos.x, pos.y, pos.z, 0.0f);
		velPtr[i] = make_float4(vel.x, vel.y, vel.z, lifetime);
		if (i<maxPathN)
		{
			particle_path[i].push_back(make_float3(pos.x, pos.y, pos.z));
			pathVel[i].push_back(sqrt(vel.x*vel.x+vel.y*vel.y+vel.z*vel.z));
		}
		
		
// 		if(length(vel)<1e-6)
// 			pathCompleted[i]=true;

	}
}
bool
ParticleSystem::initParticlesInLet(float lifetime,unsigned int beginIndex,unsigned int num)
{
	if (!obj->in_let_index||obj->n_of_btet==0)
	{
		return false;
	}
	switch (obj->let_type)
	{
	case BOX_LET:
		checkInLet=checkInBoxLet;
		break;
	case SPHERE_LET:
		checkInLet=checkInSphereLet;
		break;
	case VOLUMN_LET:
		checkInLet=checkInVolumnLet;
		break;

	case PLAIN_LET:
		checkInLet=checkInPlainLet;
		break;
	}
	float4 *posPtr = m_pos.getHostPtr();
	float4 *velPtr = m_vel.getHostPtr();
	float4 *verPtr = m_air_ver.getHostPtr();
	float4 *windPtr = m_air_wind.getHostPtr();
	int4 *tetPtr = m_air_tet.getHostPtr();
	int1* tetIdPtr=m_air_ver_tetId.getHostPtr();
	//uint4 *neighborPtr = m_air_neighbor.getHostPtr();
	uint i;
	int tet_id;
	int4 vel_id;
	vec4f baryCentricCoord;
	vec3f pos,vel;
	uint endIndex=min_1(beginIndex+num,numParticles);
	if((obj->let_type!=PLAIN_LET)&&(obj->let_type!=VOLUMN_LET))
	{
	for(uint i=beginIndex; i<endIndex; i++) 
	//for(uint i=0; i<numParticles; i++) 
	{
		do
		{
			tet_id=obj->in_let_index[(int)(frand()*(obj->n_of_btet-1))];
			vel_id=tetPtr[tet_id];
			baryCentricCoord =randBaryCentricCoord();
			//if(obj->let_type==PLAIN_LET)
			//{
			//	baryCentricCoord.x=0.25;
			//	baryCentricCoord.y=0.25;
			//	baryCentricCoord.z=0.25;
			//	baryCentricCoord.w=0.25;
			//}
			pos=baryCentricCoord.x*float32vec3f(verPtr[vel_id.x])+baryCentricCoord.y*float32vec3f(verPtr[vel_id.y])+baryCentricCoord.z*float32vec3f(verPtr[vel_id.z])+baryCentricCoord.w*float32vec3f(verPtr[vel_id.w]);
		}while(!checkInLet(pos.x,pos.y,pos.z));
		vel=baryCentricCoord.x*float32vec3f(windPtr[vel_id.x])+baryCentricCoord.y*float32vec3f(windPtr[vel_id.y])+baryCentricCoord.z*float32vec3f(windPtr[vel_id.z])+baryCentricCoord.w*float32vec3f(windPtr[vel_id.w]);
		tetIdPtr[i].x=tet_id;//debug
		posPtr[i] = make_float4(pos.x, pos.y, pos.z, 0.0f);
		velPtr[i] = make_float4(vel.x, vel.y, vel.z, lifetime);
		if (i<maxPathN)
		{
			particle_path[i].push_back(make_float3(pos.x, pos.y, pos.z));
			pathVel[i].push_back(sqrt(vel.x*vel.x+vel.y*vel.y+vel.z*vel.z));
		}

	}
	}
	else
	{
	for(uint i=beginIndex; i<endIndex; i++) 
	//for(uint i=0; i<numParticles; i++) 
	{
		do
		{
			//tet_id=obj->in_let_index[(int)(frand()*(obj->n_of_btet-1))];
			tet_id=obj->in_let_index[i%obj->n_of_btet];
			vel_id=tetPtr[tet_id];
			baryCentricCoord =randBaryCentricCoord();
			//if(obj->let_type==PLAIN_LET)
			//{
			//	baryCentricCoord.x=0.25;
			//	baryCentricCoord.y=0.25;
			//	baryCentricCoord.z=0.25;
			//	baryCentricCoord.w=0.25;
			//}
			pos=baryCentricCoord.x*float32vec3f(verPtr[vel_id.x])+baryCentricCoord.y*float32vec3f(verPtr[vel_id.y])+baryCentricCoord.z*float32vec3f(verPtr[vel_id.z])+baryCentricCoord.w*float32vec3f(verPtr[vel_id.w]);
		}while(!checkInLet(pos.x,pos.y,pos.z));
		vel=baryCentricCoord.x*float32vec3f(windPtr[vel_id.x])+baryCentricCoord.y*float32vec3f(windPtr[vel_id.y])+baryCentricCoord.z*float32vec3f(windPtr[vel_id.z])+baryCentricCoord.w*float32vec3f(windPtr[vel_id.w]);
		tetIdPtr[i].x=tet_id;//debug
		posPtr[i] = make_float4(pos.x, pos.y, pos.z, 0.0f);
		velPtr[i] = make_float4(vel.x, vel.y, vel.z, lifetime);
		if (i<maxPathN)
		{
			particle_path[i].push_back(make_float3(pos.x, pos.y, pos.z));
			pathVel[i].push_back(sqrt(vel.x*vel.x+vel.y*vel.y+vel.z*vel.z));
		}
	}
	}
	return true;
}

// initalize in random positions within cube
void
ParticleSystem::initCubeRandom(vec3f origin, vec3f size, vec3f vel, float lifetime)
{
    float4 *posPtr = m_pos.getHostPtr();
    float4 *velPtr = m_vel.getHostPtr();

	for(uint i=0; i < numParticles; i++) 
	{
		vec3f pos = origin + svrand()*size;
		posPtr[i] = make_float4(pos.x, pos.y, pos.z, lifetime);
	    velPtr[i] = make_float4(vel.x, vel.y, vel.z, lifetime);
	}
}

// add sphere on regular grid
void
ParticleSystem::addSphere(uint &index, vec3f pos, vec3f vel, int r, float spacing, float jitter, float lifetime)
{
    float4 *posPtr = m_pos.getHostPtr();
    float4 *velPtr = m_vel.getHostPtr();

    uint start = index;
    uint count = 0; 
    for(int z=-r; z<=r; z++) {
        for(int y=-r; y<=r; y++) {
            for(int x=-r; x<=r; x++) {
				vec3f delta = vec3f(x, y, z)*spacing;
                float dist = length(delta);
                if ((dist <= spacing*r) && (index < numParticles)) {
					vec3f p = pos + delta + svrand()*jitter;

					posPtr[index] = make_float4(pos.x, pos.y, pos.z, 0.0f);
				    velPtr[index] = make_float4(vel.x, vel.y, vel.z, lifetime);

					index++;
                    count++;
                }
            }
        }
    }

    m_pos.copy(GpuArray<float4>::HOST_TO_DEVICE, start, count);
    m_vel.copy(GpuArray<float4>::HOST_TO_DEVICE, start, count);
}
void
ParticleSystem::setAirData()
{
	int i;
	uint numVer=obj->size_vert_co();
	uint numTet=obj->size_tet_in_ord();
	float* vert_co=obj->vert_co;
	float* v_wind=obj->v_wind;
	int* tet_in_ord=obj->tet_in_ord;
	int* tet_neigb=obj->tet_neigb;
	m_numAirVer=numVer;
	m_numAirTet=numTet;
	m_air_ver.alloc(numVer);
	m_air_wind.alloc(numVer);
	m_air_tet.alloc(numTet);
	m_air_neighbor.alloc(numTet);
	double offsetA=0.01;
	activeArea[0]=obj->boud_box[0]-offsetA;
	activeArea[1]=obj->boud_box[1]+offsetA;
	activeArea[2]=obj->boud_box[2]-offsetA;
	activeArea[3]=obj->boud_box[3]+offsetA;
	activeArea[4]=obj->boud_box[4]-offsetA;
	activeArea[5]=obj->boud_box[5]+offsetA;
	
	setActiveArea(activeArea);
	float4 *verPtr = m_air_ver.getHostPtr();
	float4 *windPtr = m_air_wind.getHostPtr();
	int4 *tetPtr = m_air_tet.getHostPtr();
	int4 *neighborPtr = m_air_neighbor.getHostPtr();
	for (i = 0; i < numVer; i ++)
	{
		verPtr[i] = make_float4(vert_co[3*i], vert_co[3*i+1], vert_co[3*i+2],1.0);
		windPtr[i] = make_float4(v_wind[4*i], v_wind[4*i+1], v_wind[4*i+2],1.0);		
	}
	for (i = 0; i < numTet; i ++)
	{
		tetPtr[i] = make_int4(tet_in_ord[4*i], tet_in_ord[4*i+1], tet_in_ord[4*i+2],tet_in_ord[4*i+3]);
		neighborPtr[i] = make_int4(tet_neigb[4*i], tet_neigb[4*i+1], tet_neigb[4*i+2],tet_neigb[4*i+3]);
	}
	m_air_ver.copy(GpuArray<float4>::HOST_TO_DEVICE);
	m_air_wind.copy(GpuArray<float4>::HOST_TO_DEVICE);
	m_air_tet.copy(GpuArray<int4>::HOST_TO_DEVICE);
	m_air_neighbor.copy(GpuArray<int4>::HOST_TO_DEVICE);

	// bind air data to texture memory
	bindAirData(m_air_ver.getDevicePtr(), m_air_wind.getDevicePtr(), m_air_tet.getDevicePtr(), m_air_neighbor.getDevicePtr(), numVer,numTet);
	hasSetAirData_=true;
}
bool
ParticleSystem::reset(ParticleConfig config,unsigned int beginIndex,unsigned int num,float lifetime)
{
	bool succeed=true;
	m_cur_step=0;
	uint endIndex=min_1(beginIndex+num,maxPathN);
	if (beginIndex==0)
	{
		vel_min=1e6;
		vel_max=0;
	}	
	for (int i=beginIndex;i<endIndex;++i)
	{
		particle_path[i].clear();
		pathVel[i].clear();
		
		pathCompleted[i]=false;
		preTetId[i]=-1;
	}
	switch(config)
	{
	default:
	case CONFIG_RANDOM:
		initCubeRandom(vec3f(0.0, 1.0, 0.0), vec3f(1.0, 1.0, 1.0), vec3f(0.0f), lifetime);
		m_pos.copy(GpuArray<float4>::HOST_TO_DEVICE,beginIndex,num);
		m_vel.copy(GpuArray<float4>::HOST_TO_DEVICE,beginIndex,num);
		m_pos.copy(GpuArray<float4>::DEVICE_TO_DEVICE,beginIndex,num);
		m_vel.copy(GpuArray<float4>::DEVICE_TO_DEVICE,beginIndex,num);
		break;

    case CONFIG_GRID:
        {
            float jitter = m_particleRadius*0.01f;
            uint s = (int) ceilf(powf((float) numParticles, 1.0f / 3.0f));
            uint gridSize[3];
            gridSize[0] = gridSize[1] = gridSize[2] = s;
            initGrid(vec3f(-1.0, 0.0, -1.0), make_uint3(s, s, s), vec3f(m_particleRadius*2.0f), jitter, vec3f(0.0), numParticles, lifetime);
        }
        break;
	case CONFIG_AIR_RANDOM:		
		initAirParticles(lifetime,beginIndex,num);		
		break;
	case CONFIG_INLET_RANDOM:
		if(!initParticlesInLet(lifetime,beginIndex,num))
		{
			succeed=false;
			//initAirParticles(100.0,beginIndex,num);
		}
		break;
	}
	
    m_pos.copy(GpuArray<float4>::HOST_TO_DEVICE,beginIndex,num);
    m_vel.copy(GpuArray<float4>::HOST_TO_DEVICE,beginIndex,num);
	m_air_ver_tetId.copy(GpuArray<int1>::HOST_TO_DEVICE,beginIndex,num);
	m_pos.map();
	m_air_ver_tetId.map();
	bindStartPos(m_pos.getDevicePtr(),m_air_ver_tetId.getDevicePtr(),beginIndex,num,numParticles);
	m_pos.unmap();
	m_air_ver_tetId.unmap();
	if (succeed)
	{
		initCount=beginIndex+num;
	}	
	return succeed;
}

// particle emitters
void
ParticleSystem::discEmitter(uint &index, vec3f pos, vec3f vel, vec3f vx, vec3f vy, float r, int n, float lifetime, float lifetimeVariance)
{
    float4 *posPtr = m_pos.getHostPtr();
    float4 *velPtr = m_vel.getHostPtr();

    uint start = index; 
    uint count = 0;
    for(int i=0; i<n; i++) {
        vec2f delta = randCircle() * r;
        if (index < numParticles) {
			vec3f p = pos + delta.x*vx + delta.y*vy;
			float lt = lifetime + frand()*lifetimeVariance;

			posPtr[index] = make_float4(p.x, p.y, p.z, 0.0f);
			velPtr[index] = make_float4(vel.x, vel.y, vel.z, lt);

			index++;
            count++;
        }
    }

    m_pos.copy(GpuArray<float4>::HOST_TO_DEVICE, start, count);
    m_vel.copy(GpuArray<float4>::HOST_TO_DEVICE, start, count);
}

void
ParticleSystem::sphereEmitter(uint &index, vec3f pos, vec3f vel, vec3f spread, float r, int n, float lifetime, float lifetimeVariance)
{
    float4 *posPtr = m_pos.getHostPtr();
    float4 *velPtr = m_vel.getHostPtr();

    uint start = index; 
    uint count = 0;
    for(int i=0; i<n; i++) {
        vec3f x = randSphere();
		//float dist = length(x);
        if (index < numParticles) {

			vec3f p = pos + x*r;
			float age = 0.0;

			float lt = lifetime + frand()*lifetimeVariance;

			vec3f dir = randSphere();
			dir.y = fabs(dir.y);
			vec3f v = vel + dir*spread;

			posPtr[index] = make_float4(p.x, p.y, p.z, age);
            velPtr[index] = make_float4(v.x, v.y, v.z, lt);

            index++;
            count++;
        }
    }

    m_pos.copy(GpuArray<float4>::HOST_TO_DEVICE, start, count);
    m_vel.copy(GpuArray<float4>::HOST_TO_DEVICE, start, count);
}

void
ParticleSystem::setModelView(float *m)
{
    for(int i=0; i<16; i++) {
        m_modelView.m[i] = m[i];
    }
}

// dump particles to sdout for debugging
void
ParticleSystem::dumpParticles(uint start, uint count)
{
    m_pos.copy(GpuArray<float4>::DEVICE_TO_HOST);
    float4 *pos = m_pos.getHostPtr();

    m_vel.copy(GpuArray<float4>::DEVICE_TO_HOST);
    float4 *vel = m_vel.getHostPtr();

    for(uint i=start; i<start+count; i++) {
        printf("%d: ", i);
        printf("pos: (%.4f, %.4f, %.4f, %.4f)\n", pos[i].x, pos[i].y, pos[i].z, pos[i].w);
        printf("vel: (%.4f, %.4f, %.4f, %.4f)\n", vel[i].x, vel[i].y, vel[i].z, vel[i].w);
    }
}

// dump particles to a system memory host
void
ParticleSystem::dumpBin( float4 **posData,
                         float4 **velData)
{
    m_pos.copy(GpuArray<float4>::DEVICE_TO_HOST);
    *posData = m_pos.getHostPtr();

    m_vel.copy(GpuArray<float4>::DEVICE_TO_HOST);
    *velData = m_vel.getHostPtr();
}
