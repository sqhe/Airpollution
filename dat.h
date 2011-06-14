#pragma once

#ifndef _DATH_
#define _DATH_

#include <vector>
#include <float.h>


//using namespace std;
enum file_type{ZONE,WIND};
enum LET_TYPES{SPHERE_LET,BOX_LET,VOLUMN_LET,PLAIN_LET};
typedef struct DNoteType
{
	int n;
	DNoteType *prev,*next;
	bool g;
};

class re_dat_tet
{
public:
	re_dat_tet();
	re_dat_tet(char *file);
	~re_dat_tet();
	void						read_dat_tet(char *file);
	void						read_dat_binary(char *file);
	void						write_to_mh3(char *file);
	void						write_to_fre(char *file);
	void						write_vert_neigb(char *file);
	void						write_tet_neigb(char *file);
	void						write_btet_off(char *file);
	void						write_tet_neigb_12(char *file1,char *file2);
	void						write_to_binary(char *file);
	void						find_tet_in_box(float min_x,float max_x,float min_y,float max_y,float min_z,float max_z,float px,float py,float pz);
	void						find_tet_in_sphere(float center_x,float center_y, float center_z,float radius);
	void						find_tet_in_bvolumn(int num);
	void						find_tet_in_bplain(int num);
	void						clear_all();
	void						presure_tex_comp(int size_x,int size_y,int size_z);
	void						tex_ing(int *size_xyz,float pmin0,float pmax0,bool whi);
	void						tex_3d(int *size_xyz,float *pminmax,bool whi);
	int							size_vert_co();
	int							size_vert_neigb();
	int							size_v_wind();
	int							size_p();
	int							size_tet_in_ord();
	int							size_boud_box();
	int							size_pres_min_max();
	int							size_v_win_min_max();
	int							size_tet_neigb();
	int							size_btet();


	int							find_tet_id_of_point_hel(float x,float y,float z,std::vector<float> se,int ids); // return the tet id which contains the point (x,y,z), if (x,y,z) is out of bouding box, return -1
	int							find_tet_id_of_point(float x,float y,float z); // return the tet id which contains the point (x,y,z), if (x,y,z) is out of bouding box, return -1
	int							point_in_tri(std::vector<float> point, std::vector<float> tet1, std::vector<float> tet2, std::vector<float> tet3);	// only in the same plain
	int							point_in_tet(std::vector<float> point, int tet_id); // if a point is in a tet, return true, false otherwise
	float						doc_cross(std::vector<float> x,std::vector<float> y,std::vector<float> z); // return (doc(x,cross(y,z)))
	float						doc_prod(std::vector<float> v1,std::vector<float> v2); // return the doc product
	float						bar_of_point(std::vector<float> p,std::vector<float> p0,std::vector<float> p1); // p0 and p1 are not the same point, the three are in the same line
	std::vector<float>			cross_prod(std::vector<float> v1,std::vector<float> v2); // return the cross product
	std::vector<float>			intersec_of_line_plain(std::vector<float> p1,std::vector<float> p2,std::vector<float> v1,std::vector<float> v2,std::vector<float> v3);
	std::vector<float>			intersec_of_line_line(std::vector<float> p1,std::vector<float> p2,std::vector<float> v1,std::vector<float> v2);	// only in the same plain
	bool						line_in_out(std::vector<float> p,std::vector<float> v1,std::vector<float> v2);  // only in the same line
	std::vector<float>			compute_new_point(std::vector<float> p_current,std::vector<float> p_start,std::vector<float> point, int rand_id);  // return the new point ,if no, return vector3f(FLT_MAX)

public:
	//vector<vector<float>>		vert_co;
	float						*vert_co;
	//vector<vector<float>>		v_wind;
	float						*v_wind;
	//vector<float>				p;
	float						*p;
	//vector<vector<int>>			tet_in_ord;
	int							*tet_in_ord;
	//vector<vector<int>>			vert_neigb;
	std::vector<int>					*vert_neigb;
	int							*tet_neigb;
	//vector<float>				boud_box;                   // x_min,x_max,y_min,y_max,z_min,z_max
	float						*boud_box;
	//vector<float>				pres_min_max;				// p_min,p_max
	float						*pres_min_max;
	//vector<float>				v_win_min_max;
	float						*v_win_min_max;
	int							*in_let_index;
	int							*bt;
	int							*boud_tet;           
	float						*presure_tex_data;
	int							*p_tex_int;

	int							N;
	int							E;
	int							n_of_btet;					// the number of tets in the inlet bound
	int							n_of_bt;
	int							n_of_boud_tet;				// 2*(the number of tets in boud)
	int							*t_size;
	LET_TYPES					let_type;
	float let_x_min,let_x_max,let_y_min,let_y_max,let_z_min,let_z_max;//box let
	float let_center_x, let_center_y,  let_center_z, let_radius,let_radius2;//sphere let
private:
	//bool						in_btet(float x,float y,float z);
	bool						point_in_bound(int point_id);
	bool						tet_in_dnote(DNoteType *dnot, int ids);
	void						insert_tet(DNoteType *&dnot, int ids);
	void						del_link(DNoteType *&dnot);
	DNoteType *					find_pos_link(DNoteType *dnot,int ids);
	bool						check_f(DNoteType *dnot);	// if dnot==NULL or at least one false, return false; otherwise return true
	DNoteType *					find_f(DNoteType *dnot);


};

#endif