#include "stdafx.h"
#include "Mesh3D.h"
#include "MATRIX.h"
#include <windows.h>
#include <gl/gl.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <list>
#include <vector>
#include <string>
#include <stdio.h>

#include "math.h"
#include "ColorRamp.h"

//  #include "CGAL/Delaunay_triangulation_2.h"
//  #include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//  #include <CGAL/Triangulation_euclidean_traits_xy_3.h>
//  
//  typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//  typedef CGAL::Triangulation_euclidean_traits_xy_3<K> Gt;
//  typedef CGAL::Delaunay_triangulation_2<Gt> Delaunay;
//  
//  typedef Delaunay::Face_handle Face_handle;
//  typedef Delaunay::Face_iterator Face_iterator;
//  typedef Delaunay::Triangle Triangle2D;
//  
//  typedef K::Point_3 DPoint;
//  
//  Delaunay T;

std::vector<vector2D> limitPoint;

using   namespace   std;  
// using   namespace   Gdiplus;
// #pragma   comment   (lib,   "Gdiplus.lib")   

//--------------------------------------------------------------

Mesh3D::Mesh3D(void)
{
	//initialization
	vertices_list = NULL;
	faces_list = NULL;
	edges_list = NULL;
	gaussvtex = NULL;
	meanvtex = NULL;

	xmax = ymax = zmax = -100000.0f;
	xmin = ymin = zmin = 100000.0f;

	m_closed = true;
	m_quad = false;
	m_tri = false;

	min_gcurvature = 100000;
	max_gcurvature = -100000;

	m_GCfinish = false;

	min_mcurvature = 100000;
	max_mcurvature = -100000;

	m_MCfinish = false;
	//bitmap = NULL;
}

//--------------------------------------------------------------

Mesh3D::~Mesh3D(void)
{
	clear_data();
	if (gaussvtex)
		delete []gaussvtex;

	if (meanvtex)
		delete []meanvtex;
}
//--------------------------------------------------------------

void Mesh3D::clear_data()
{
	clear_vertices();
	clear_edges();
	clear_faces();
}


//--------------------------------------------------------------

void Mesh3D::clear_vertices()
{
	if (vertices_list==NULL) {
		return;
	}
	VERTEX_ITER viter = vertices_list->begin();

	for (;viter!=vertices_list->end(); viter++) {
		delete *viter;
	}

	delete vertices_list;
	vertices_list  = NULL;
}
//--------------------------------------------------------------
void Mesh3D::clear_edges()
{
	m_edgemap.clear();
	if (edges_list==NULL) {
		return;
	}
	EDGE_ITER eiter = edges_list->begin();

	for (;eiter!=edges_list->end(); eiter++) {
		delete *eiter;
	}

	delete edges_list;
	edges_list = NULL;
}
//--------------------------------------------------------------
void Mesh3D::clear_faces()
{

	if (faces_list==NULL) {
		return;
	}
	FACE_ITER fiter = faces_list->begin();

	for (;fiter!=faces_list->end(); fiter++) {
		delete *fiter;
	}

	delete faces_list;
	faces_list = NULL;
}

//-------------------------------------------------------------
//create Mesh 

HE_vert* Mesh3D::insert_vertex(float x, float y, float z)
{
	HE_vert* hv = new HE_vert(x,y,z);
	if (vertices_list==NULL) {
		vertices_list = new VERTEX_LIST;
	}
	hv->id = (int)vertices_list->size();
	vertices_list->push_back(hv);
	
	return hv;
}
//-------------------------------------------------------------
HE_face* Mesh3D::insert_face(VERTEX_LIST& vec_hv)
{
	int vsize = (int)vec_hv.size();
	if (vsize<3) {
		return NULL;
	}

	if (faces_list==NULL) {
		faces_list = new FACE_LIST;
	}
	HE_face* hf = new HE_face;
	hf->valence = vsize;
	VERTEX_ITER viter = vec_hv.begin();
	VERTEX_ITER nviter = vec_hv.begin();
	nviter++;

	HE_edge *he1, *he2;
	std::vector<HE_edge*> v_he;
	int i;
	for (i=0; i<vsize-1; i++) {
		he1 = insert_edge( *viter, *nviter);
		he2 = insert_edge( *nviter, *viter);

		if (hf->edge==NULL) {
			hf->edge = he1;
		}
		he1->face = hf;

		he1->pair = he2;
		he2->pair = he1;
		v_he.push_back(he1);
		viter++,nviter++;
	}

	nviter = vec_hv.begin();

	he1 = insert_edge(*viter, *nviter);
	he2 = insert_edge(*nviter , *viter);
	he1->face = hf;
	if (hf->edge==NULL) {
		hf->edge = he1;
	}
	he1->pair = he2;
	he2->pair = he1;
	v_he.push_back(he1);

	for (i=0; i<vsize-1; i++) {
		v_he[i]->next = v_he[i+1];
	}
	v_he[i]->next = v_he[0];

	hf->id = (int)faces_list->size();
	faces_list->push_back(hf);
	
	return hf;
}
//-------------------------------------------------------------
HE_edge* Mesh3D::insert_edge(HE_vert* vstart, HE_vert* vend)
{
	if (edges_list==NULL) {
		edges_list = new EDGE_LIST;
	}

	if( m_edgemap[PAIR_VERTEX(vstart,vend)] != NULL )
		return m_edgemap[PAIR_VERTEX(vstart,vend)];


	HE_edge* he = new HE_edge;

	he->vert = vend;

	he->vert->degree++;

	vstart->edge = he;

	m_edgemap[PAIR_VERTEX(vstart,vend)] = he;

	he->id = (int)edges_list->size();
	edges_list->push_back(he);

	return he;
}
//--------------------------------------------------------------

void Mesh3D::set_nextedge_for_border_vertices()
{

	if (!isvalid()) {
		return;
	}

	EDGE_ITER eiter = edges_list->begin();

	for (; eiter!=edges_list->end(); eiter++) 
	{
		if ((*eiter)->next==NULL&& (*eiter)->face==NULL) {
			(*eiter)->pair->vert->edge = *eiter;
		}
	}

	for (eiter = edges_list->begin(); eiter!=edges_list->end(); eiter++) 
	{
		if ( (*eiter)->next ==NULL ) {
			HE_vert* hv = (*eiter)->vert;
			if (hv -> edge != (*eiter)->pair)
			{
				(*eiter)->next = hv -> edge;
			}
		}
	}

}

//--------------------------------------------------------------

bool Mesh3D::isborder(HE_vert* hv)
{
	HE_edge* edge = hv->edge;     
	do {
		if (edge==NULL||edge->pair->face==NULL||edge->face==NULL) {
			return true;
		}
		edge = edge->pair->next;   

	} while (edge != hv->edge);

	return false;

}
//--------------------------------------------------------------

bool Mesh3D::isborder(HE_face* hf)
{
	HE_edge* edge = hf->edge;     

	do {

		if (isborder(edge)) {
			return true;
		}

		edge = edge->next;

	} while (edge != hf->edge);

	return false;
}
//--------------------------------------------------------------

bool Mesh3D::isborder(HE_edge* he)
{
	if(he->face==NULL||he->pair->face==NULL)
		return true;
	return false;
}
//--------------------------------------------------------------

void Mesh3D::check_closed()
{
	m_closed = get_num_of_vertices_list()-get_num_of_edges_list()/2 + get_num_of_faces_list() == 2; 
}
//--------------------------------------------------------------

void Mesh3D::check_meshtype()
{
	FACE_ITER fiter = faces_list->begin();
	m_tri = true;
	m_quad = true;
	for (;fiter!=faces_list->end(); fiter++) {
		if ((*fiter)->valence!=3) {
			m_tri = false;
			break;
		}
	}

	 fiter = faces_list->begin();
	for (;fiter!=faces_list->end(); fiter++) {
		if ((*fiter)->valence!=4) {
			m_quad = false;
			break;
		}
	}
}
//--------------------------------------------------------------
void Mesh3D::update_mesh()
{
	if (!isvalid()) {
		return;
	}
	set_nextedge_for_border_vertices();
    check_closed();
	check_meshtype();
	m_edgemap.clear();
	compute_all_normal();
}
//-------------------------------------------------------------
//FILE IO
//--------------------------------------------------------------

bool Mesh3D::load_data(FILE *fp)
{
	try{
		clear_data();

		fscanf(fp,"%d %d",&vsize,&fsize);

		if (m_pProgress)
			m_pProgress->SetRange(0, vsize+fsize);


		float x, y, z;
		for (int i = 0; i < vsize; i++) {
			fscanf(fp,"%f%f%f", &(x), &(y), &(z));
			//			fin>>x>>y>>z;
			HE_vert* hv = insert_vertex(x,y,z);
			m_pProgress->SetPos(i);
		}

		for (int i = 0; i < fsize; i++) {
			VERTEX_LIST v_list;
			int valence = 3;
			int id[3];
			fscanf(fp,"%d%d%d", &(id[0]), &(id[1]), &(id[2]));
			for (int j=0; j<valence; j++) {
				HE_vert* hv = get_vertex(id[j] - 1);
				if (hv==NULL) {
					throw 1;
				}
				v_list.push_back( hv );
			}
			insert_face(v_list);
			m_pProgress->SetPos(i + vsize);
		}

		update_mesh();
		compute_all_normal();

		return fsize;
	}
	catch (...) {
		//catch any error
		clear_data();
		xmax = ymax = zmax = 1.0f;
		xmin = ymin = zmin = -1.0f;
		return 0;
	}
}

bool Mesh3D::load_offdata(FILE *fp)
{
	try{
		clear_data();

		fscanf(fp, "OFF\n%d %d 0\n", &vsize,&fsize);

		if (m_pProgress)
		{
			m_pProgress->SetRange(0, vsize+fsize);
			m_pProgress->SetStep(1);
		}
		float box[2][3];
		float center[3];
		box[0][0] = box[0][1] = box[0][2] = MAXFLOAT;
		box[1][0] = box[1][1] = box[1][2] = -MAXFLOAT;
		float x, y, z;		
		for (int i = 0; i < vsize; i++) {
			fscanf(fp,"%f %f %f", &(x), &(y), &(z));
			HE_vert* hv = insert_vertex(x,y,z);
			if (box[0][0]>x)
				box[0][0] = x;
			if (box[1][0]<x)
				box[1][0] = x;

			if (box[0][1]>y)
				box[0][1] = y;
			if (box[1][1]<y)
				box[1][1] = y;

			if (box[0][2]>z)
				box[0][2] = z;
			if (box[1][2]<z)
				box[1][2] = z;
			m_pProgress->SetPos(i);
		}
		center[0] = (box[0][0]+box[1][0])/2.0;
		center[1] = (box[0][1]+box[1][1])/2.0;
		center[2] = (box[0][2]+box[1][2])/2.0;

		xmax = box[1][0];
		ymax = box[1][1];
		zmax = box[1][2];

		xmin = box[0][0];
		ymin = box[0][1];
		zmin = box[0][2];
		for (int i = 0; i < fsize; i++) {
			VERTEX_LIST v_list;
			int valence = 3;
			int id[3];
			fscanf(fp,"%d %d %d %d", &valence, &(id[0]), &(id[1]), &(id[2]));
			for (int j=0; j < valence; j++) {
				HE_vert* hv = get_vertex(id[j]);
				if (hv==NULL) {
					throw 1;
				}
				v_list.push_back( hv );
			}
			insert_face(v_list);
			m_pProgress->SetPos(i+vsize);
		}

		update_mesh();
		//compute_all_normal();

		

		
		

		//float scale = 1;
		//scale = FMAX(box[1][0]-box[0][0],FMAX(box[1][1]-box[0][1],box[1][2]-box[0][2]));
		//scale /= 2.0;

		//viter = vertices_list->begin();
		//for (;viter!=vertices_list->end(); viter++)
		//{
		//	(*viter)->x = ((*viter)->x - center[0]) / scale;
		//	(*viter)->y = ((*viter)->y - center[1]) / scale;
		//	(*viter)->z = ((*viter)->z - center[2]) / scale;
		//}

		//VEC3_VOPV_OP_S(box[0],box[0],-,center,/,scale);
		//VEC3_VOPV_OP_S(box[1],box[1],-,center,/,scale);
		//VEC3_ZERO(center);

		return fsize;
	}
	catch (...) {
		//catch any error
		clear_data();
		xmax = ymax = zmax = -100000.0f;
		xmin = ymin = zmin = 100000.0f;
		return 0;
	}
}
//--------------------------------------------------------------

void Mesh3D::write_data(std::ofstream &fout)
{

	fout<<"OFF\n";
	//output the number of vertices_list, faces_list-> edges_list
	fout<<(int)vertices_list->size()<<" "<<(int)faces_list->size()<<" "<<(int)edges_list->size()/2<<"\n";

	//output coordinates of each vertex
	VERTEX_ITER viter = vertices_list->begin();
	for (;viter!=vertices_list->end(); viter++) {
		fout<<(*viter)->x<<" "<<(*viter)->y<<" "<<(*viter)->z<<"\n";
	}

	//output the valence of each face and its vertices_list' id

	FACE_ITER fiter = faces_list->begin();

	for (;fiter!=faces_list->end(); fiter++) {

		fout<<(*fiter)->valence;

		HE_edge* edge = (*fiter)->edge;     

		do {
			fout<<" "<<edge->vert->id;
			edge = edge->next;

		} while (edge != (*fiter)->edge);
		fout<<"\n";
	}
}


//--------------------------------------------------------------
//-------------------------------------------------------------
//For rendering

//void Mesh3D::compute_boundingbox()
//{
//
//	if (vertices_list->size()<3) {
//		return;
//	}
//
//	xmax = ymax = zmax = -10e10f;
//	xmin = ymin = zmin = 10e10f;
//
//	VERTEX_ITER viter = vertices_list->begin();
//	for (; viter!=vertices_list->end(); viter++) {
//		xmin = min( (*viter)->x,xmin );
//		ymin = min( (*viter)->y,ymin );
//		zmin = min( (*viter)->z,zmin );
//		xmax = max( (*viter)->x,xmax );
//		ymax = max( (*viter)->y,ymax );
//		zmax = max( (*viter)->z,zmax );
//	}
//}

//--------------------------------------------------------------
void Mesh3D::compute_faces_list_normal()
{
	FACE_ITER fiter = faces_list->begin();

	for (;fiter!=faces_list->end(); fiter++) 
	{
		compute_perface_normal(*fiter);
	}
}
//--------------------------------------------------------------
void Mesh3D::compute_perface_normal(HE_face* hf)
{
	int i = 0;
	HE_edge* pedge = hf->edge;
	HE_edge* nedge = hf->edge->next;

	float nx=0, ny=0, nz=0;
	for (i=0; i<hf->valence; i++) {

		//cross product
		HE_vert* p = pedge->vert;
		HE_vert* c = pedge->next->vert;
		HE_vert* n = nedge->next->vert;
		float pcx,pcy,pcz,ncx,ncy,ncz;
		pcx = p->x - c->x; pcy = p->y - c->y; pcz = p->z - c->z;
		ncx = n->x - c->x; ncy = n->y - c->y; ncz = n->z - c->z;

		nx+= pcy*ncz-pcz*ncy; ny+= pcz*ncx-pcx*ncz; nz+= pcx*ncy-pcy*ncx;
		pedge = nedge;
		nedge = nedge->next;
		if (hf->valence==3) {
			break;
		}
	}

	float sumarea = sqrt(nx*nx+ny*ny+nz*nz);
	if (sumarea==0.0f) {
		hf->normal[0] = 0.0f;
		hf->normal[1] = 0.0f;
		hf->normal[2] = 0.0f;
	}
	else
	{
		hf->normal[0] = -nx/sumarea;
		hf->normal[1] = -ny/sumarea;
		hf->normal[2] = -nz/sumarea;
	}
}
//--------------------------------------------------------------
void Mesh3D::compute_vertices_list_normal()
{
	VERTEX_ITER viter = vertices_list->begin();

	for (; viter!=vertices_list->end(); viter++) 
	{
		compute_pervertex_normal(*viter);
	}

}
//--------------------------------------------------------------
void Mesh3D::compute_pervertex_normal(HE_vert* hv)
{

	HE_edge* edge = hv->edge; 
	
	if (edge==NULL) {
		hv->normal[0] = 0.0f;
		hv->normal[1] = 0.0f;
		hv->normal[2] = 0.0f;
		return;
	}

	float x = 0, y = 0, z = 0;

	do {
		if (edge->face!=NULL) {
			x += edge->face->normal[0];
			y += edge->face->normal[1];
			z += edge->face->normal[2];
		}

		edge = edge->pair->next;   

	} while (edge != hv->edge);

	float sqnorm = sqrt(x*x+y*y+z*z);
	if (sqnorm==0.0f) {
		hv->normal[0] = 0.0f;
		hv->normal[1] = 0.0f;
		hv->normal[2] = 0.0f;
	}
	else
	{
		hv->normal[0] = x/sqnorm;
		hv->normal[1] = y/sqnorm;
		hv->normal[2] = z/sqnorm;
	}

}

//--------------------------------------------------------------
void Mesh3D::compute_all_normal()
{
	compute_faces_list_normal();
	compute_vertices_list_normal();
}

//--------------------------------------------------------------

void Mesh3D::DrawModel(int RenderMode, int RenderModel, int FlatNormal)
{
	if (!isvalid()) {
		return;
	}
	
	if (RenderMode == 1)
	{
		//glEnable(GL_LIGHTING);

		glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glEnable(GL_POLYGON_OFFSET_POINT);
		glPolygonOffset(1.0, 1.0);

		glPolygonMode(GL_FRONT,GL_FILL);
		glPolygonMode(GL_BACK,GL_FILL);

		glShadeModel(GL_SMOOTH);
		glEnable(GL_NORMALIZE);

		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		
		if (RenderModel == 1)
		{
			FACE_ITER fiter = faces_list->begin();
			
			for (;fiter!=faces_list->end(); fiter++) 
			{
				HE_edge* edge = (*fiter)->edge;     
				
				if (FlatNormal) {
					glNormal3fv((*fiter)->normal);
				}
				glBegin(GL_POLYGON);
				do {
					if(!FlatNormal)
						glNormal3fv(edge->vert->normal);
					glVertex3f(edge->vert->x,edge->vert->y,edge->vert->z);
					edge = edge->next;
				} while (edge != (*fiter)->edge);
				glEnd();
			}
		}
	}

	else if (RenderMode == 0)
	{
		//glEnable(GL_LIGHTING);

		glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glEnable(GL_POLYGON_OFFSET_POINT);
		glPolygonOffset(1.0, 1.0);

		glPolygonMode(GL_FRONT,GL_FILL);
		glPolygonMode(GL_BACK,GL_FILL);

		glShadeModel(GL_SMOOTH);
		glEnable(GL_NORMALIZE);

		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		
		if (RenderModel == 1)
		{
			FACE_ITER fiter = faces_list->begin();
			
			for (;fiter!=faces_list->end(); fiter++) 
			{
				HE_edge* edge = (*fiter)->edge;     
				
				if (FlatNormal) {
					glNormal3fv((*fiter)->normal);
				}
				glBegin(GL_POLYGON);
				do {
					if(!FlatNormal)
						glNormal3fv(edge->vert->normal);
					glVertex3f(edge->vert->x,edge->vert->y,edge->vert->z);
					edge = edge->next;
				} while (edge != (*fiter)->edge);
				glEnd();
			}
		}

		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT,GL_LINE);
		glPolygonMode(GL_BACK,GL_LINE);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_NORMALIZE);
		glColor3f(0.,0.,0.);

		if (RenderModel == 1)
		{
			FACE_ITER fiter = faces_list->begin();
			
			for (;fiter!=faces_list->end(); fiter++) 
			{
				HE_edge* edge = (*fiter)->edge;     
				
				if ((*fiter)->tag)
					glColor3f(1.,0.,0.);

				if (FlatNormal) {
					glNormal3fv((*fiter)->normal);
				}
				glBegin(GL_POLYGON);
				do {
					if(!FlatNormal)
						glNormal3fv(edge->vert->normal);
					glVertex3f(edge->vert->x,edge->vert->y,edge->vert->z);
					edge = edge->next;
				} while (edge != (*fiter)->edge);
				glEnd();
			}
		}
	}
}

void Mesh3D::update_adjmesh()
{

}

double Mesh3D::calvornoiarea(Coord3 v1, Coord3 v2, Coord3 v3)
{
	double varea = 0;

	Coord3 midv12, midv13, midv23, center;
	Coord3 v12, v13, v23;
	double angle1, angle2, angle3;
	double d12, d13, d23;

	VEC3_V_OP_V_OP_S( midv12, v1, +, v2, /, 2.);
	VEC3_V_OP_V_OP_S( midv13, v1, +, v3, /, 2.);
	VEC3_V_OP_V_OP_S( midv23, v2, +, v3, /, 2.);

	VEC3_V_OP_V( v12, v2, -, v1);
	VEC3_V_OP_V( v13, v3, -, v1);
	d12 = sqrt(v12[0]*v12[0] + v12[1]*v12[1] + v12[2]*v12[2]);
	d13 = sqrt(v13[0]*v13[0] + v13[1]*v13[1] + v13[2]*v13[2]);
	angle1 = acos(DOTPROD3(v12, v13) / ( sqrt(v12[0]*v12[0] + v12[1]*v12[1] + v12[2]*v12[2]) * sqrt(v13[0]*v13[0] + v13[1]*v13[1] + v13[2]*v13[2])));

	VEC3_V_OP_V( v12, v1, -, v2);
	VEC3_V_OP_V( v13, v3, -, v2);
	d23 = sqrt(v13[0]*v13[0] + v13[1]*v13[1] + v13[2]*v13[2]);
	angle2 = acos(DOTPROD3(v12, v13) / ( sqrt(v12[0]*v12[0] + v12[1]*v12[1] + v12[2]*v12[2]) * sqrt(v13[0]*v13[0] + v13[1]*v13[1] + v13[2]*v13[2])));

	VEC3_V_OP_V( v12, v1, -, v3);
	VEC3_V_OP_V( v13, v2, -, v3);
	angle3 = acos(DOTPROD3(v12, v13) / ( sqrt(v12[0]*v12[0] + v12[1]*v12[1] + v12[2]*v12[2]) * sqrt(v13[0]*v13[0] + v13[1]*v13[1] + v13[2]*v13[2])));

	if ((angle1 - M_PI / 2.) > .0f){
		VEC3_ASN_OP(center, =, midv23);
		varea = (1./4.)*d12*d13*sin(angle1);
	}
	else if (((angle2 - M_PI / 2.) > .0f)){
		VEC3_ASN_OP(center, =, midv13);
		varea = (1./8.)*d12*d13*sin(angle1);
	}
	else if ((angle3 - M_PI / 2.) > .0f){
		VEC3_ASN_OP(center, =, midv12);
		varea = (1./8.)*d12*d13*sin(angle1);
	}
	else {
		calcircumcenterC(center, v1, v2, v3);

		Coord3 vcenter;
		VEC3_V_OP_V( vcenter, center, -, v1);
		double dvcenter = sqrt(vcenter[0]*vcenter[0] + vcenter[1]*vcenter[1] + vcenter[2]*vcenter[2]);

		double sqrl1 = dvcenter * dvcenter - (d12 / 2.) * (d12 / 2.);
		double sqrl2 = dvcenter * dvcenter - (d13 / 2.) * (d13 / 2.);
		double l1, l2;
		if (fabs(sqrl1) < 1e-8)
			l1 = 0;
		else
		    l1 = sqrt(dvcenter * dvcenter - (d12 / 2.) * (d12 / 2.));

		if (fabs(sqrl2) < 1e-8)
			l2 = 0;
		else
		    l2 = sqrt(dvcenter * dvcenter - (d13 / 2.) * (d13 / 2.));

		varea = (1./2.)*l1*(d12 / 2.) + (1./2.)*l2*(d13 / 2.);
	}
	
	return varea;
}

void Mesh3D::Gausscurvaturecolormap(void)
{
	if (! m_GCfinish)
		processallvergcurvature();

	glEnable(GL_LIGHTING);
	glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_POLYGON_OFFSET_POINT);
	glPolygonOffset(1.0, 1.0);

	glPolygonMode(GL_FRONT,GL_FILL);
	glPolygonMode(GL_BACK,GL_FILL);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable( GL_COLOR_MATERIAL );

	CColorRamp colormap;
	colormap.BuildRainbow();
	float red, green, blue;
	FACE_ITER fiter = faces_list->begin();
	for (;fiter!=faces_list->end(); fiter++) 
	{
		HE_edge* edge = (*fiter)->edge;     

		glBegin(GL_POLYGON);
		do {
			glNormal3fv(edge->vert->normal);
			int vid = edge->vert->id;
			red = min( max((float)colormap.Red(gaussvtex[vid])/255.f, 0.f), 1.f );
			green = min( max((float)colormap.Green(gaussvtex[vid])/255.f, 0.f), 1.f );
			blue = min( max((float)colormap.Blue(gaussvtex[vid])/255.f, 0.f), 1.f );

			glColor3f( red, green, blue );
			glVertex3f(edge->vert->x,edge->vert->y,edge->vert->z);

			edge = edge->next;
		} while (edge != (*fiter)->edge);
		glEnd();
	}

	glDisable( GL_COLOR_MATERIAL );
}

void Mesh3D::Meancurvaturecolormap(void)
{
	if (! m_MCfinish)
		processallvermcurvature();

	glEnable(GL_LIGHTING);
	glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_POLYGON_OFFSET_POINT);
	glPolygonOffset(1.0, 1.0);

	glPolygonMode(GL_FRONT,GL_FILL);
	glPolygonMode(GL_BACK,GL_FILL);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable( GL_COLOR_MATERIAL );

	CColorRamp colormap;
	colormap.BuildRainbow();
	float red, green, blue;
	FACE_ITER fiter = faces_list->begin();
	for (;fiter!=faces_list->end(); fiter++) 
	{
		HE_edge* edge = (*fiter)->edge;     

		glBegin(GL_POLYGON);
		do {
			glNormal3fv(edge->vert->normal);
			int vid = edge->vert->id;
			red = min( max((float)colormap.Red(meanvtex[vid])/255.f, 0.f), 1.f );
			green = min( max((float)colormap.Green(meanvtex[vid])/255.f, 0.f), 1.f );
			blue = min( max((float)colormap.Blue(meanvtex[vid])/255.f, 0.f), 1.f );
			glColor3f( red, green, blue );
			glVertex3f(edge->vert->x,edge->vert->y,edge->vert->z);

			edge = edge->next;
		} while (edge != (*fiter)->edge);
		glEnd();
	}

	glDisable( GL_COLOR_MATERIAL );
}

double Mesh3D::calgausscurvature(HE_vert* hv)    //////////////////calculate the gauss curvature for inner vertex/////////////////////////////////
{
	double gausscurvature = 0;

	Coord3 *adjvertices;
	Coord3 cvertex;
	int    adjvernum;

	adjvernum = 0;
	adjvertices   = new Coord3[(hv)->degree + 1];

	HE_edge* edge = hv->edge;
	cvertex[0] = hv->x;
	cvertex[1] = hv->y;
	cvertex[2] = hv->z;

	do {     // do something with edge, edge->pair or edge->face
		adjvertices[adjvernum][0] = edge->vert->x;
		adjvertices[adjvernum][1] = edge->vert->y;
		adjvertices[adjvernum][2] = edge->vert->z;

		adjvernum ++;
		edge = edge->pair->next;
	} while (edge != (hv)->edge);

	adjvertices[adjvernum][0] = adjvertices[0][0];
	adjvertices[adjvernum][1] = adjvertices[0][1];
	adjvertices[adjvernum][2] = adjvertices[0][2];
	adjvernum ++;

	double vorarea = 0;
	double aroundangle = 0;
	for (int vernum = 0; vernum < adjvernum - 1; vernum ++){
		vorarea += calvornoiarea( cvertex, adjvertices[vernum], adjvertices[vernum + 1] );
		aroundangle += calvangle( cvertex, adjvertices[vernum], adjvertices[vernum + 1] );
	}

	gausscurvature = (2.*M_PI - aroundangle) / vorarea;

	return gausscurvature;
}

void Mesh3D::calcircumcenterC(Coord3 center, Coord3 v1, Coord3 v2, Coord3 v3)
{

}

double Mesh3D::caltriarea(Coord3 v1, Coord3 v2, Coord3 v3)
{
	return 0;
}

double Mesh3D::calvangle(Coord3 v1, Coord3 v2, Coord3 v3)
{
	double vangle = 0;
	Coord3 v12, v13;
	double d12, d13;
	VEC3_V_OP_V( v12, v2, -, v1);
	VEC3_V_OP_V( v13, v3, -, v1);
	d12 = sqrt(v12[0]*v12[0] + v12[1]*v12[1] + v12[2]*v12[2]);
	d13 = sqrt(v13[0]*v13[0] + v13[1]*v13[1] + v13[2]*v13[2]);
	vangle = acos(DOTPROD3(v12, v13) / ( sqrt(v12[0]*v12[0] + v12[1]*v12[1] + v12[2]*v12[2]) * sqrt(v13[0]*v13[0] + v13[1]*v13[1] + v13[2]*v13[2])));
	return vangle;
}

void Mesh3D::processallvergcurvature(void)
{
	if (gaussvtex){
		delete []gaussvtex;
		gaussvtex = NULL;
	}

	gaussvtex = new float[vsize];

	VERTEX_ITER viter = vertices_list->begin();
	double currgcurvature;
	int currentcount = 0;

	std::ofstream fout("c:\\curout.txt");
	for (; viter != vertices_list->end(); viter ++){
		//currgcurvature = log10(fabs(calgausscurvature((*viter))) + 1e-10) / log10(1000.);

		currgcurvature = fabs(calgausscurvature(*viter));

		fout << currentcount << ' '<<  currgcurvature << std::endl;
		if (max_gcurvature < currgcurvature)
			max_gcurvature = currgcurvature;
		if (min_gcurvature > currgcurvature)
			min_gcurvature = currgcurvature;
		currentcount ++;
	}
	fout.close();

	float range = max_gcurvature - min_gcurvature;

	double newmin = min_gcurvature;
	double newmax = min_gcurvature + 100.;

	range = newmax - newmin;

	viter = vertices_list->begin();
	for (; viter != vertices_list->end(); viter ++){
		//currgcurvature = log10(fabs(calgausscurvature((*viter))) + 1e-10) / log10(1000.);

		currgcurvature = fabs(calgausscurvature(*viter));

		if (currgcurvature < newmin)
			currgcurvature = newmin;

		if (currgcurvature > newmax)
			currgcurvature = newmax;

		//int ivex = min ( max( int((currgcurvature - min_gcurvature) / range * 255.f + 0.5f), 0.f ), 255.f );

		int ivex = min ( max( int((currgcurvature - newmin) / range * 255.f + 0.5f), 0.f ), 200.f );

		gaussvtex[(*viter)->id] = ivex;//min(max(ivex,.0f),1.0f);
	}

	m_GCfinish = true;
}

double Mesh3D::calmeancurvature(HE_vert* hv)
{
	double mcurvature = 0;

	Coord3 *adjvertices;
	Coord3 cvertex;
	int    adjvernum;

	adjvernum = 1;
	adjvertices   = new Coord3[(hv)->degree + 2];

	HE_edge* edge = hv->edge;
	cvertex[0] = hv->x;
	cvertex[1] = hv->y;
	cvertex[2] = hv->z;

	do {     // do something with edge, edge->pair or edge->face
		adjvertices[adjvernum][0] = edge->vert->x;
		adjvertices[adjvernum][1] = edge->vert->y;
		adjvertices[adjvernum][2] = edge->vert->z;

		adjvernum ++;
		edge = edge->pair->next;
	} while (edge != (hv)->edge);

	adjvertices[0][0] = adjvertices[adjvernum - 1][0];
	adjvertices[0][1] = adjvertices[adjvernum - 1][1];
	adjvertices[0][2] = adjvertices[adjvernum - 1][2];

	adjvertices[adjvernum][0] = adjvertices[1][0];
	adjvertices[adjvernum][1] = adjvertices[1][1];
	adjvertices[adjvernum][2] = adjvertices[1][2];
	adjvernum ++;

	double vorarea = 0;
	double cotab = 0;
	double alpha, beta;
	Coord3 pij, sigmapij;
	sigmapij[0] = sigmapij[1] = sigmapij[2] = 0;
	for (int vernum = 1; vernum < adjvernum - 1; vernum ++){
		vorarea += calvornoiarea( cvertex, adjvertices[vernum], adjvertices[vernum + 1] );
		VEC3_V_OP_V( pij, adjvertices[vernum], -, cvertex );
		calabangles( cvertex, adjvertices[vernum], adjvertices[vernum - 1], adjvertices[vernum + 1], alpha, beta );
		sigmapij[0] += ( 1./tan(alpha) + 1./tan(beta) )*pij[0];
		sigmapij[1] += ( 1./tan(alpha) + 1./tan(beta) )*pij[1];
		sigmapij[2] += ( 1./tan(alpha) + 1./tan(beta) )*pij[2];
	}

	sigmapij[0] = sigmapij[0]/(2.*vorarea);
	sigmapij[1] = sigmapij[1]/(2.*vorarea);
	sigmapij[2] = sigmapij[2]/(2.*vorarea);

	mcurvature = sqrt(sigmapij[0]*sigmapij[0] + sigmapij[1]*sigmapij[1] + sigmapij[2]*sigmapij[2]) / 2.;

	return mcurvature;
}

void Mesh3D::calabangles(Coord3 pi, Coord3 pj, Coord3 p1, Coord3 p2, double& alpha, double& beta)
{
	Coord3 v12, v13;
	double d12, d13;
	VEC3_V_OP_V( v12, pi, -, p1);
	VEC3_V_OP_V( v13, pj, -, p1);
	d12 = sqrt(v12[0]*v12[0] + v12[1]*v12[1] + v12[2]*v12[2]);
	d13 = sqrt(v13[0]*v13[0] + v13[1]*v13[1] + v13[2]*v13[2]);
	alpha = acos(DOTPROD3(v12, v13) / ( sqrt(v12[0]*v12[0] + v12[1]*v12[1] + v12[2]*v12[2]) * sqrt(v13[0]*v13[0] + v13[1]*v13[1] + v13[2]*v13[2])));

	VEC3_V_OP_V( v12, pi, -, p2);
	VEC3_V_OP_V( v13, pj, -, p2);
	d12 = sqrt(v12[0]*v12[0] + v12[1]*v12[1] + v12[2]*v12[2]);
	d13 = sqrt(v13[0]*v13[0] + v13[1]*v13[1] + v13[2]*v13[2]);
	beta = acos(DOTPROD3(v12, v13) / ( sqrt(v12[0]*v12[0] + v12[1]*v12[1] + v12[2]*v12[2]) * sqrt(v13[0]*v13[0] + v13[1]*v13[1] + v13[2]*v13[2])));
}

void Mesh3D::processallvermcurvature(void)
{
	if (meanvtex){
		delete []meanvtex;
		meanvtex = NULL;
	}

	meanvtex = new float[vsize];

	VERTEX_ITER viter = vertices_list->begin();
	double currmcurvature;
	for (; viter != vertices_list->end(); viter ++){
		currmcurvature = calmeancurvature((*viter));

		if (max_mcurvature < currmcurvature)
			max_mcurvature = currmcurvature;
		if (min_mcurvature > currmcurvature)
			min_mcurvature = currmcurvature;
	}

	double range = max_mcurvature - min_mcurvature;

	viter = vertices_list->begin();
	for (; viter != vertices_list->end(); viter ++){
		currmcurvature = calmeancurvature((*viter));

		int ivex = min ( max( int((currmcurvature - min_mcurvature) / range * 255. + 0.5), 0.f ), 255.f );
		meanvtex[(*viter)->id] = ivex;//min(max(ivex,.0f),1.0f);
	}

	m_MCfinish = true;
}
