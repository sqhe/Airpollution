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
 * CUDA Device code for particle simulation.
 */

#ifndef _PARTICLES_KERNEL_H_
#define _PARTICLES_KERNEL_H_

#include "cutil_math.h"
#include "math_constants.h"
#include "particles_kernel.cuh"
#define EPSILON 0.000001
texture<float4, 3, cudaReadModeElementType> noiseTex;
texture<float4, 1, cudaReadModeElementType> air_ver_Tex;
texture<float4, 1, cudaReadModeElementType> air_wind_Tex;
texture<int4, 1, cudaReadModeElementType> air_tet_Tex;
texture<int4, 1, cudaReadModeElementType> air_neighbor_Tex;
texture<float4, 1, cudaReadModeElementType> start_pos_Tex;
texture<int1, 1, cudaReadModeElementType> start_tetId_Tex;
float4* d_start_pos;
int1 * d_start_tetId;
// simulation parameters
__constant__ SimParams params;
__constant__ float d_activeArea[6];
//get ray triangle intersection point
__device__
bool RayTriangleIntersect( float3 v0,   
						   float3 v1,
						   float3 v2,
						   float3 rayStart,
						   float3 rayDir,   
                           float&   t,
						   float previous_t)   
{   
    //Does ray parallel with triangle?   
    float3 e1, e2, p, s, q;   
    float u, v, tmp;   
    e1 = v1 - v0;   
    e2 = v2 - v0;   
    p=cross(rayDir, e2); 
    tmp = dot(p,e1);
//     if(tmp > -EPSILON && tmp < EPSILON)   
//     {   
// 		t=-1;//debug
//         return false;   
//     }   
    tmp = __fdividef(1.0f, tmp); 
    s = rayStart - v0;   
    u = dot(s, p) * tmp; 
    if(u < 0.0 || u > 1.0) 
	{
		t=-2;
        return false;   
	}
    q = cross(s, e1); 
    v = dot(rayDir, q) * tmp;  
    float uv = u+v; 
    if(v < 0.0||uv > 1.0)  
	{
		t=-3;
        return false;     
	}
    t = dot(e2, q) * tmp; 
    if(t-previous_t <= 1e-7)  
	//if(t<=previous_t)
        return false;   
    return true;   
}
//get barycentric coordinates of point inside a tet
__device__
float4 InterpolateTet(float3 p, float3 v0, float3 v1, float3 v2, float3 v3)
{

	float4 coord;

	float T = fabsf(dot(v1-v0,cross(v2-v0,v3-v0)));
	coord.w=__fdividef(fabsf(dot(v0-p,cross(v1-p,v2-p))),T);
	coord.z=__fdividef(fabsf(dot(v0-p,cross(v1-p,v3-p))),T);
	coord.y=__fdividef(fabsf(dot(v0-p,cross(v2-p,v3-p))),T);
	coord.x=__fdividef(fabsf(dot(v1-p,cross(v2-p,v3-p))),T);
	return coord;
}
// look up in 3D noise texture
__device__
float3 noise3D(float3 p)
{
    float4 n = tex3D(noiseTex, p.x, p.y, p.z);
    return make_float3(n.x, n.y, n.z);
}

__device__
float3 fractalSum3D(float3 p, int octaves, float lacunarity, float gain)
{
	float freq = 1.0f, amp = 0.5f;
	float3 sum = make_float3(0.0f);	
	for(int i=0; i<octaves; i++) {
		sum += noise3D(p*freq)*amp;
		freq *= lacunarity;
		amp *= gain;
	}
	return sum;
}

__device__
float3 turbulence3D(float3 p, int octaves, float lacunarity, float gain)
{
	float freq = 1.0f, amp = 0.5f;
	float3 sum = make_float3(0.0f);	
	for(int i=0; i<octaves; i++) {
		sum += fabs(noise3D(p*freq))*amp;
		freq *= lacunarity;
		amp *= gain;
	}
	return sum;
}

// integrate particle attributes
__global__ void
integrateD(float4* newPos, float4* newVel, int1 *newTetId,
           float4* oldPos, float4* oldVel, int1 *oldTetId, 		   
           float deltaTime,
		   int numParticles,
		   float particleLifetime)//, float4 *intersection
{
    int index = __mul24(blockIdx.x,blockDim.x) + threadIdx.x;
	if (index >= numParticles) return;

	volatile float4 posData = oldPos[index];	// ensure coalesced reads
    volatile float4 velData = oldVel[index];
	volatile int1 tetIdData = oldTetId[index];
    float3 pos = make_float3(posData.x, posData.y, posData.z);
    float3 vel = make_float3(velData.x, velData.y, velData.z);
	int4 tet,neighbors;
	float4 v0_4,v1_4,v2_4,v3_4;
	float3 v[4];
	int1 tetId,tetId_bak;
	tetId.x= tetIdData.x;
	float age = posData.w;	
	//float lifetime = velData.w;
	float lifetime = particleLifetime;
    //reset the paricle if it enter the buliding or go out of the bounding box, or the active area, or its age reaches the lifetime
	if(tetId.x==-2)
	{
		float4 startPos=tex1Dfetch(start_pos_Tex,index);
		pos= make_float3(startPos.x, startPos.y, startPos.z);
		tetId=tex1Dfetch(start_tetId_Tex,index);
		tet=tex1Dfetch(air_tet_Tex,tetId.x);
		v0_4=tex1Dfetch(air_ver_Tex,tet.x);
		v[0]=make_float3(v0_4.x,v0_4.y,v0_4.z);
		v1_4=tex1Dfetch(air_ver_Tex,tet.y);
		v[1]=make_float3(v1_4.x,v1_4.y,v1_4.z);
		v2_4=tex1Dfetch(air_ver_Tex,tet.z);
		v[2]=make_float3(v2_4.x,v2_4.y,v2_4.z);
		v3_4=tex1Dfetch(air_ver_Tex,tet.w);
		v[3]=make_float3(v3_4.x,v3_4.y,v3_4.z);
		float4 barryCoord=InterpolateTet(pos,v[0],v[1],v[2],v[3]);
	
		float4 vel_4=barryCoord.x*tex1Dfetch(air_wind_Tex,tet.x)+barryCoord.y*tex1Dfetch(air_wind_Tex,tet.y)+barryCoord.z*tex1Dfetch(air_wind_Tex,tet.z)+barryCoord.w*tex1Dfetch(air_wind_Tex,tet.w);
		vel_4.w=lifetime;
		newPos[index] = startPos;//make_float4(pos, 0);
		newVel[index] = vel_4;
		newTetId[index]=tetId;
		return;
	}
	if(age>=lifetime||length(vel)<1e-6||pos.x<d_activeArea[0]||pos.x>d_activeArea[1]||pos.y<d_activeArea[2]||pos.y>d_activeArea[3]||pos.z<d_activeArea[4]||pos.z>d_activeArea[5])
	{
		float4 startPos=make_float4(posData.x, posData.y, posData.z,lifetime);
		float4 vel_4=make_float4(velData.x, velData.y, velData.z,lifetime);
		newPos[index] = startPos;//make_float4(pos, 0);
		newVel[index] = vel_4;
		tetId.x=-2;
		newTetId[index]=tetId;
		return;
	}
	
    // update particle age
	age += deltaTime;
	if (age > lifetime) {
	    age = lifetime;
    }

    // apply accelerations
   // vel += params.gravity * deltaTime;

    // apply procedural noise
    //float3 noise = noise3D(pos*params.noiseFreq + params.time*params.noiseSpeed);
    //vel += noise * params.noiseAmp;

	
	
	//ray object intersection
	float t=0;
	float t_bak=t;
	int outIndex;
	/////////////////////////////////////////////////////////////////////////////////////debug
	//float3 tpos,tvel;
	/////////////////////////////////////////////////////////////////////////////////////
 	while(t<deltaTime)
	{
		tetId_bak=tetId;
		tet=tex1Dfetch(air_tet_Tex,tetId.x);
		neighbors=tex1Dfetch(air_neighbor_Tex,tetId.x);
		v0_4=tex1Dfetch(air_ver_Tex,tet.x);
		v[0]=make_float3(v0_4.x,v0_4.y,v0_4.z);
		v1_4=tex1Dfetch(air_ver_Tex,tet.y);
		v[1]=make_float3(v1_4.x,v1_4.y,v1_4.z);
		v2_4=tex1Dfetch(air_ver_Tex,tet.z);
		v[2]=make_float3(v2_4.x,v2_4.y,v2_4.z);
		v3_4=tex1Dfetch(air_ver_Tex,tet.w);
		v[3]=make_float3(v3_4.x,v3_4.y,v3_4.z);
		//find out the face out
		if(RayTriangleIntersect(v[1],v[2],v[3],pos,vel,t,t_bak))
		{
			tetId.x=neighbors.x;
			//tpos.x=t;
			outIndex=0;
		}
		else
		{
			//tpos.x=t;
			if(RayTriangleIntersect(v[0],v[2],v[3],pos,vel,t,t_bak))
			{
				tetId.x=neighbors.y;
				//tpos.y=t;
				outIndex=1;
			}
			else
			{
				//tpos.y=t;
				if(RayTriangleIntersect(v[0],v[1],v[3],pos,vel,t,t_bak))
				{
					tetId.x=neighbors.z;
					//tpos.z=t;
					outIndex=2;
				}
				else
				{
					//tpos.z=t;
					if(RayTriangleIntersect(v[0],v[1],v[2],pos,vel,t,t_bak))
					{
						tetId.x=neighbors.w;
						//tvel.x=t;
						outIndex=3;
					} 
					else
					{
						//tvel.x=t;
			// 			tetId.x=-1;			
			// 			t=0;
			// 			outIndex=-1;
						vel=noise3D(pos);
						float4 barryCoord=InterpolateTet(pos,v[0],v[1],v[2],v[3]);
						//newPos[index] = make_float4(barryCoord.x,barryCoord.y,barryCoord.z,5);
						//newPos[index] = make_float4(tpos,5);
						//tvel.y=t_bak;
						//newVel[index] = make_float4(tvel, lifetime);
						//newVel[index] = make_float4(barryCoord.w,tvel.x,tvel.y, lifetime);
						newPos[index] = make_float4(pos, lifetime-1);
						newVel[index] = make_float4(vel, lifetime);
						newTetId[index]=tetId_bak;
						return;
					}
				}
			}
		}
		//if the particle enter the building, change its direction.
		//if(tetId.x<0)
		if(tetId.x==-1)
		{
			pos += vel * t_bak;
			newPos[index] = make_float4(pos, age);
			++outIndex;
			float3 v0=v[outIndex%4];
			++outIndex;
			float3 v1=v[outIndex%4];
			++outIndex;
			float3 v2=v[outIndex%4];
			float3 face_normal=normalize(cross(v1-v0,v2-v0));
			vel-=dot(face_normal,vel)*face_normal;
			if(length(vel)<1e-6)
				vel=noise3D(pos);
			else
				vel=normalize(vel);
			newVel[index] = make_float4(vel, lifetime);
			newTetId[index]=tetId_bak;
			return;
		}
		else if(tetId.x==-2)//go out of the bounding box
		{
			pos += vel * t;
			newPos[index] = make_float4(pos, lifetime);
			newVel[index] = make_float4(vel, lifetime);
			newTetId[index]=tetId;
			return;
		}
		t_bak=t;
	}
	//intersection[index]=make_float4(pos+vel*t,t);
    // new position = old position + velocity * deltaTime
    pos += vel * deltaTime;
	//pos += vel * t;
	
	float4 barryCoord=InterpolateTet(pos,v[0],v[1],v[2],v[3]);
	
	float4 vel_4=barryCoord.x*tex1Dfetch(air_wind_Tex,tet.x)+barryCoord.y*tex1Dfetch(air_wind_Tex,tet.y)+barryCoord.z*tex1Dfetch(air_wind_Tex,tet.z)+barryCoord.w*tex1Dfetch(air_wind_Tex,tet.w);
	vel_4.w=lifetime;
    //vel *= params.globalDamping;

    // store new position and velocity
    newPos[index] = make_float4(pos, age);
    newVel[index] = vel_4;
	newTetId[index]=tetId_bak;
	
}

// calculate sort depth for each particle
__global__ void calcDepthD(float4* pos, float* keys, uint *indices, float3 vector, int numParticles)
{
	uint index = __mul24(blockIdx.x,blockDim.x) + threadIdx.x;
	if (index >= numParticles) return;

	volatile float4 p = pos[index];
	float key = -dot(make_float3(p.x, p.y, p.z), vector);        // project onto sort vector
	
	keys[index] = key;
	indices[index] = index;
}

#endif
