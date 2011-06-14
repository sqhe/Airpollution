#pragma once

//Include
#include "StdAfx.h"
#include <list>
#include <vector>
#include <string>
#include <cmath> //i.e., #include <math.h>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>
#include <cassert>
#include "Defs.h"
#include "Geometry.h"
//#include <gdiplus.h>
//for understanding halfedge structure,  please read the article in http://www.flipcode.com/articles/article_halfedge.shtml

// using   namespace   Gdiplus;
// #pragma   comment   (lib,   "Gdiplus.lib")  

typedef struct vector2D{
	double x;
	double y;
	bool   outer;
} Point2D;

class HE_vert;
class HE_face;
class HE_edge;

class HE_vert
{   
public:
	float x,y,z;		//3d coordinates
	HE_edge* edge;		// one of the half-edges_list emanating from the vertex
	float normal[3];	// vertex normal
	int id;				// index 
	int degree;			// vertex degree
	bool tag;			// tag for programming easily
public:
	HE_vert(float _x=0.0f, float _y=0.0f, float _z=0.0f)
		:x(_x),y(_y),z(_z),edge(NULL),id(-1),degree(0),tag(false)
	{}
};

class HE_edge
{   
public:
	HE_vert* vert;   // vertex at the end of the half-edge
	HE_edge* pair;   // oppositely oriented adjacent half-edge
	HE_face* face;   // face the half-edge borders
	HE_edge* next;   // next half-edge around the face
	int id;			 // index
	bool tag;		 // tag for programming easily
public:
	HE_edge()
		:vert(NULL),pair(NULL),face(NULL),next(NULL),id(-1),tag(false)
	{  }
};

class HE_face
{   
public:
	HE_edge* edge;		// one of the half-edges_list bordering the face	
	int valence;		// the number of edges_list 
	float normal[3];	// face normal
	int id;				// index
	bool tag;           // tag for programming easily
public:
	HE_face()
		:edge(NULL),id(-1),tag(false)
	{}

};


//simple 3d vector
struct vector3f
{
	float v[3];
};

//redefine types for programming easily

typedef std::vector<HE_vert*> VERTEX_LIST;
typedef std::vector<HE_face*> FACE_LIST;
typedef std::vector<HE_edge*> EDGE_LIST;

typedef std::vector<HE_vert*>* PTR_VERTEX_LIST;
typedef std::vector<HE_face*>* PTR_FACE_LIST;
typedef std::vector<HE_edge*>* PTR_EDGE_LIST;

typedef std::vector<HE_vert*>::iterator VERTEX_ITER;
typedef std::vector<HE_face*>::iterator FACE_ITER;
typedef std::vector<HE_edge*>::iterator EDGE_ITER;

typedef std::vector<HE_vert*>::reverse_iterator VERTEX_RITER;
typedef std::vector<HE_face*>::reverse_iterator FACE_RITER;
typedef std::vector<HE_edge*>::reverse_iterator EDGE_RITER;


typedef std::pair<HE_vert*,HE_vert*> PAIR_VERTEX;


//Mesh3D class:  Half edge data structure
class Mesh3D
{
protected:

	PTR_VERTEX_LIST vertices_list;		//store vertices
	PTR_EDGE_LIST edges_list;			//store edges
	PTR_FACE_LIST faces_list;			//store faces

	bool m_closed;						//indidate whether the mesh is closed
	bool m_quad;						//indidate whether the mesh is quadrilateral
	bool m_tri;							//indidate whether the mesh is triangular

	double min_gcurvature;
	double max_gcurvature;

	double min_mcurvature;
	double max_mcurvature;

	std::map<PAIR_VERTEX, HE_edge*> m_edgemap;	//associate two end vertices with its edge: only useful in creating mesh

public:
	//bounding box
	float xmax,xmin,ymax,ymin,zmax,zmin;		//bounding box for rendering

public:
	Mesh3D(void);
	~Mesh3D(void);

	//get information of vertices, faces, edges
	PTR_VERTEX_LIST get_vertices_list()
	{
		return vertices_list;
	}

	PTR_EDGE_LIST get_edges_list()
	{
		return edges_list;
	}

	PTR_FACE_LIST get_faces_list()
	{
		return faces_list;
	}

	int get_num_of_vertices_list()
	{	
		return vertices_list?(int)vertices_list->size():0;
	}

	int get_num_of_faces_list()
	{
		return faces_list?(int)faces_list->size():0;
	}
	int get_num_of_edges_list()
	{
		return edges_list?(int)edges_list->size():0;
	}
	
	HE_vert* get_vertex(int id) //get a vertex by id
	{
		return id >= get_num_of_vertices_list()? NULL:(*vertices_list)[id];
	}

	HE_edge* get_edge(int id) //get an edge by id
	{
		return id >= get_num_of_edges_list()? NULL: (*edges_list)[id];
	}
	HE_face* get_face(int id) //get a face by id
	{
		return id >= get_num_of_faces_list()? NULL: (*faces_list)[id];
	}

	//create mesh

	//insert a vertex,  return a pointer to the created vertex
	HE_vert* insert_vertex(float x, float y, float z);

	//insert a face,  return a pointer to the created face
	//vec_hv stored the vertices list
	HE_face* insert_face(VERTEX_LIST& vec_hv);

	bool isvalid() //check whether the mesh is valid
	{
		if( get_num_of_vertices_list()==0 || get_num_of_faces_list() == 0 )
			return false;
		return true;
	}

	bool isclosed() //check whether the mesh is closed
	{
		return m_closed;
	}

	bool istriangular()
	{
		return m_tri;
	}

	bool isquad()
	{
		return m_quad;
	}

	bool isborder(HE_vert* hv); //check whether the vertex is on border
	bool isborder(HE_face* hf); //check whether the face is on border
	bool isborder(HE_edge* he); //check whether the edge is on border

	void update_mesh(); //update mesh: call it when you have created the mesh

	void update_adjmesh();

	//FILE IO
	bool load_data(FILE *fp);
	bool load_offdata(FILE *fp);
	void write_data(std::ofstream &fout);

	int& get_num_vertices() {return vsize;}
	int& get_num_faces() {return fsize;}


	//Rendering
	void gl_draw(bool smooth=false);
	void compute_all_normal();
	void DrawModel(int RenderMode, int RenderModel, int FlatNormal);
	//void compute_boundingbox();

	//Simplify

	int    Simplify_Vertex_Remove(int nFace);
	void   Gausscurvaturecolormap(void);
	void   Meancurvaturecolormap(void);

	CProgressCtrl	*m_pProgress;

private:
	HE_edge* insert_edge(HE_vert* vstart, HE_vert* vend); // it is called by insert_face automatically

	//clear data
	void clear_data();
	void clear_vertices();
	void clear_edges();
	void clear_faces();

	void set_nextedge_for_border_vertices(); 
	void check_closed();
	void check_meshtype();

	void compute_faces_list_normal(); 
	void compute_perface_normal(HE_face* hf);
	void compute_vertices_list_normal();
	void compute_pervertex_normal(HE_vert* hv); 
	void SetColorMode();

	double calvornoiarea(Coord3 v1, Coord3 v2, Coord3 v3);
	double calgausscurvature(HE_vert* hv);

	void   SetColorBarImage();

	bool    m_GCfinish;
	bool    m_MCfinish;
	//Bitmap* bitmap;

public:
	
private:
	void calcircumcenterC(Coord3 center, Coord3 v1, Coord3 v2, Coord3 v3);

	unsigned char colorbar[1024];
	unsigned char origcolorbar[1024];
	unsigned int  cbtex;
	float*        gaussvtex;
	float*        meanvtex;
	int           vsize, fsize;
public:
	double caltriarea(Coord3 v1, Coord3 v2, Coord3 v3);
	double calvangle(Coord3 v1, Coord3 v2, Coord3 v3);
	void processallvergcurvature(void);
private:
	double calmeancurvature(HE_vert* hv);
	void calabangles(Coord3 pi, Coord3 pj, Coord3 p1, Coord3 p2, double& alpha, double& beta);
public:
	void processallvermcurvature(void);
};