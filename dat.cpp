#include "StdAfx.h"
#include "dat.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "math.h"
#include <ctime>
using namespace std;
re_dat_tet::re_dat_tet()
{
	N=0;

	E=0;

	n_of_btet=0;
	n_of_bt=0;

	boud_box=new float[6];
	pres_min_max=new float[2];
	v_win_min_max=new float[2];
	t_size=new int[3];

	for(int i=0;i<3;i++)
		t_size[i]=0;

	vert_co=NULL;
	v_wind=NULL;
	p=NULL;
	tet_in_ord=NULL;
	vert_neigb=NULL;
	tet_neigb = NULL;
	in_let_index=NULL;
	bt=NULL;
	presure_tex_data=NULL;
	p_tex_int=NULL;
	for(int i=0;i<6;i++)
		boud_box[i]=0;

	for(int i=0;i<2;i++)
	{
		pres_min_max[i]=0;
		v_win_min_max[i]=0;
	}
}

re_dat_tet::re_dat_tet(char *file)
{
	boud_box=new float[6];		
	pres_min_max=new float[2];		
	v_win_min_max=new float[2];	
	t_size=new int[3];

	for(int i=0;i<3;i++)
		t_size[i]=0;

	vert_co=NULL;
	v_wind=NULL;
	p=NULL;
	tet_in_ord=NULL;
	vert_neigb=NULL;
	tet_neigb=NULL;
	in_let_index=NULL;
	presure_tex_data=NULL;
	p_tex_int=NULL;
	bt=NULL;

	for(int i=0;i<6;i++)
		boud_box[i]=0;

	for(int i=0;i<2;i++)
	{
		pres_min_max[i]=0;
		v_win_min_max[i]=0;
	}

	read_dat_tet(file);
}

re_dat_tet::~re_dat_tet()
{
	clear_all();

	if(boud_box!=NULL)
		delete [] boud_box;
	boud_box=NULL;

	if(pres_min_max!=NULL)
		delete [] pres_min_max;
	pres_min_max=NULL;

	if(v_win_min_max)
		delete [] v_win_min_max;
	v_win_min_max=NULL;

	if(in_let_index!=NULL)
		delete [] in_let_index;
	in_let_index=NULL;

	if(bt!=NULL)
		delete [] bt;
	bt=NULL;

	if(presure_tex_data!=NULL)
		delete [] presure_tex_data;
	presure_tex_data=NULL;

	if(p_tex_int!=NULL)
		delete [] p_tex_int;
	p_tex_int=NULL;

	if(t_size!=NULL)
		delete [] t_size;
	t_size=NULL;
}

void  re_dat_tet::read_dat_tet(char *file)
{
	clear_all();

	char	*a;
	string  b;

	ifstream ifile(file);

	while(ifile>>b)
		if(b=="ZONE")
		{
			float tempd;
			char non[1024];

			ifile>>b;
			ifile>>b;
			b.assign(b.begin()+2,b.end()-1);
			a=&b[0];
			N=atoi(a);
			ifile>>b;
			b.assign(b.begin()+2,b.end()-1);
			a=&b[0];
			E=atoi(a);
			ifile.getline(non,1023);

			vert_co=new float[3*N];
			v_wind =new float[4*N];
			p	   =new float[N];

			tet_in_ord=new int[4*E];
			tet_neigb =new int[4*E];

			vert_neigb=new vector<int>[N];

			for(int i=0;i<N;i++)
			{
				//vector<float> tempv;
				vector<int>   temp_vecc;

				//float *tempv=new float[3];

				//vert_neigb.push_back(temp_vecc);

				//vert_co[i]=tempv;

				vert_neigb[i]=temp_vecc;

				ifile>>tempd;

				//boud_box[0]=(boud_box[0]>tempd)?(tempd):(boud_box[0]);
				//boud_box[1]=(boud_box[1]<tempd)?(tempd):(boud_box[1]);

				if(boud_box[0]>tempd)
					boud_box[0]=tempd;

				if(boud_box[1]<tempd)
					boud_box[1]=tempd;


				//tempv.push_back(tempd);
				//vert_co.push_back(tempv);

				vert_co[3*i]=tempd;
			}

			for(int i=0;i<N;i++)
			{
				ifile>>tempd;

				//boud_box[2]=(boud_box[2]>tempd)?(tempd):(boud_box[2]);
				//boud_box[3]=(boud_box[3]<tempd)?(tempd):(boud_box[3]);

				if(boud_box[2]>tempd)
					boud_box[2]=tempd;

				if(boud_box[3]<tempd)
					boud_box[3]=tempd;

				//vert_co[i].push_back(tempd);	
				vert_co[3*i+1]=tempd;
			}

			for(int i=0;i<N;i++)
			{
				ifile>>tempd;

				//boud_box[4]=(boud_box[4]>tempd)?(tempd):(boud_box[4]);
				//boud_box[5]=(boud_box[5]<tempd)?(tempd):(boud_box[5]);

				if(boud_box[4]>tempd)
					boud_box[4]=tempd;

				if(boud_box[5]<tempd)
					boud_box[5]=tempd;

				//vert_co[i].push_back(tempd);
				vert_co[3*i+2]=tempd;
			}

			for(int i=0;i<N;i++)
			{
				ifile>>tempd;

				//pres_min_max[0]=(pres_min_max[0]>tempd)?(tempd):(pres_min_max[0]);
				//pres_min_max[1]=(pres_min_max[1]<tempd)?(tempd):(pres_min_max[1]);

				if(pres_min_max[0]>tempd)
					pres_min_max[0]=tempd;

				if(pres_min_max[1]<tempd)
					pres_min_max[1]=tempd;

				//p.push_back(tempd);
				p[i]=tempd;
			}

			for(int i=0;i<N;i++)
			{
				//vector<float> tempv;
				//float *tempv=new float[4];

				//v_wind[i]=tempv;

				ifile>>tempd;
				//tempv.push_back(tempd);
				//v_wind.push_back(tempv);;
				v_wind[4*i]=tempd;
			}

			for(int i=0;i<N;i++)
			{
				ifile>>tempd;
				//v_wind[i].push_back(tempd);
				v_wind[4*i+1]=tempd;
			}

			for(int i=0;i<N;i++)
			{
				ifile>>tempd;
				//v_wind[i].push_back(tempd);

				v_wind[4*i+2]=tempd;

				float temp_v_wind;

				temp_v_wind=v_wind[4*i]*v_wind[4*i]+v_wind[4*i+1]*v_wind[4*i+1]+v_wind[4*i+2]*v_wind[4*i+2];

				temp_v_wind=sqrt(temp_v_wind);
				// 				if (temp_v_wind<1e-6)
				// 				{
				// 					v_wind[i][0] = 0;
				// 					v_wind[i][1] = 0;
				// 					v_wind[i][2] = 0;
				// 					v_wind[i][3] = 0;
				// 
				// 				}else
				// 				{
				// 					v_wind[i][0] = v_wind[i][0] / temp_v_wind;
				// 					v_wind[i][1] = v_wind[i][1] / temp_v_wind;
				// 					v_wind[i][2] = v_wind[i][2] / temp_v_wind;
				// 
				// 					//v_wind[i].push_back(temp_v_wind);
				// 					
				// 				}
				v_wind[4*i+3]=temp_v_wind;
				if(v_win_min_max[0]>temp_v_wind)
					v_win_min_max[0]=temp_v_wind;

				if(v_win_min_max[1]<temp_v_wind)
					v_win_min_max[1]=temp_v_wind;
			}

			for(int i=0;i<3*N;i++)
				ifile>>tempd;

			//for(int i=0;i<N;i++)
			//{
			//	vector<float> tempv;
			//	ifile>>tempd;
			//	tempv.push_back(tempd);
			//	temp_vec.push_back(tempv);	
			//}

			//for(int i=0;i<N;i++)
			//{
			//	ifile>>tempd;
			//	temp_vec[i].push_back(tempd);		
			//}

			//for(int i=0;i<N;i++)
			//{
			//	ifile>>tempd;
			//	temp_vec[i].push_back(tempd);
			//}

			for(int i=0;i<E;i++)
			{
				//vector<int> temp_vec;

				//int *temp_v=new int[4];

				//tet_in_ord[i]=temp_v;

				for(int j=0;j<4;j++)
				{
					int temp;

					ifile>>temp;
					tet_in_ord[4*i+j]=temp-1;
					vert_neigb[temp-1].push_back(i);
					//temp_vec.push_back(temp);
				}

				//tet_in_ord.push_back(temp_vec);

				//int *temp_nei=new int[4];

				for(int j=0;j<4;j++)
					tet_neigb[4*i+j]=-1;	
			}
		}

		ifile.close();

		vector<int> v_of_b;
		vector<int> v_of_boud_tet;

		for(int i=0;i<E;i++)
		{
			int count_num[6];

			for(int j=0;j<6;j++)
				count_num[j]=0;

			for(int j=0;j<4;j++)
				if(fabs(vert_co[3*tet_in_ord[4*i+j]]-boud_box[0])<0.000001)
					count_num[0]++;

			for(int j=0;j<4;j++)
				if(fabs(vert_co[3*tet_in_ord[4*i+j]]-boud_box[1])<0.000001)
					count_num[1]++;

			for(int j=0;j<4;j++)
				if(fabs(vert_co[3*tet_in_ord[4*i+j]+1]-boud_box[2])<0.000001)
					count_num[2]++;

			for(int j=0;j<4;j++)
				if(fabs(vert_co[3*tet_in_ord[4*i+j]+1]-boud_box[3])<0.000001)
					count_num[3]++;

			for(int j=0;j<4;j++)
				if(fabs(vert_co[3*tet_in_ord[4*i+j]+2]-boud_box[4])<0.000001)
					count_num[4]++;

			for(int j=0;j<4;j++)
				if(fabs(vert_co[3*tet_in_ord[4*i+j]+2]-boud_box[5])<0.000001)
					count_num[5]++;

			if(count_num[1]>=3)
				v_of_b.push_back(i);

			bool tcs=false;
			int  which_boud=-1;

			for(int j=0;j<6;j++)
				if(count_num[j]>=3)
				{
					tcs=true;
					which_boud=j;
					break;
				}

				if(tcs)
				{
					v_of_boud_tet.push_back(i);

					int tik=-1;

					int cor=-1;

					cor=which_boud/2;

					bool tp[4];

					for(int j=0;j<4;j++)
						tp[j]=false;

					for(int j=0;j<4;j++)
						if(fabs(vert_co[3*tet_in_ord[4*i+j]+cor]-boud_box[which_boud])<0.000001)
							tp[j]=true;

					for(int j=0;j<4;j++)
						if(!tp[j])
						{
							tik=j;
							break;
						}

						v_of_boud_tet.push_back(tik);

						bool tcs2=false;
						int which_boud2=-1;

						for(int j=which_boud+1;j<6;j++)
							if(count_num[j]>=3)
							{
								tcs2=true;
								which_boud2=j;
							}

							if(tcs2)
							{
								v_of_boud_tet.push_back(i);

								int tik2=-1;
								int cor2=-1;

								cor2=which_boud2/2;

								bool tp2[4];

								for(int j=0;j<4;j++)
									tp2[j]=false;

								for(int j=0;j<4;j++)
									if(fabs(vert_co[3*tet_in_ord[4*i+j]+cor2]-boud_box[which_boud2])<0.000001)
										tp2[j]=true;

								for(int j=0;j<4;j++)
									if(!tp2[j])
									{
										tik2=j;
										break;
									}

									v_of_boud_tet.push_back(tik2);
							}
				}
		}

		n_of_bt=v_of_b.size();
		n_of_boud_tet=v_of_boud_tet.size();

		bt=new int[n_of_bt];
		boud_tet=new int[n_of_boud_tet];

		for(int i=0;i<n_of_bt;i++)
			bt[i]=v_of_b[i];

		for(int i=0;i<n_of_boud_tet;i++)
			boud_tet[i]=v_of_boud_tet[i];

		for(int i=0;i<E;i++)
		{
			int *temp_s1=new int[4];

			for(int j=0;j<4;j++)
				temp_s1[j]=tet_in_ord[4*i+j];

			sort(temp_s1,temp_s1+4);

			for(int n=0;n<4;n++)
			{
				for(int j=0;j<vert_neigb[tet_in_ord[4*i+n]].size();j++)
				{
					int *temp_s2=new int[4];

					for(int k=0;k<4;k++)
						temp_s2[k]=tet_in_ord[4*vert_neigb[tet_in_ord[4*i+n]][j]+k];

					sort(temp_s2,temp_s2+4);

					vector<int> temp_vecn(8);

					vector<int>::iterator it_vec;

					it_vec=set_intersection(temp_s1,temp_s1+4,temp_s2,temp_s2+4,temp_vecn.begin());

					if((int(it_vec-temp_vecn.begin()))==3)		// the tet_in_ord[vert_neigb[tet_in_ord[i][n]-1][j]-1]  tet is one neigbor of tet_in_ord[i], so add it to tet_neigb[i]
					{
						int *temp_s3=new int[3];

						for(int ti=0;ti<3;ti++)
							temp_s3[ti]=temp_vecn[ti];

						vector<int> temp_v_t(4);

						vector<int>::iterator  it_re;

						it_re=set_difference(temp_s1,temp_s1+4,temp_s3,temp_s3+3,temp_v_t.begin());

						int temp_tet_ver=-1;

						int temp_which_ver=-1;

						if((int(it_re-temp_v_t.begin()))==1)
						{
							temp_tet_ver=temp_v_t[0];

							for(int tj=0;tj<4;tj++)
								if(temp_tet_ver==tet_in_ord[4*i+tj])
									temp_which_ver=tj;

							if(tet_neigb[4*i+temp_which_ver]==-1)
								tet_neigb[4*i+temp_which_ver]=vert_neigb[tet_in_ord[4*i+n]][j];
						}


						//if(tet_neigb[i][0]<4)
						//{
						//	bool exist=false;

						//	for(int k=1;k<=tet_neigb[i][0];k++)
						//		if(tet_neigb[i][k]==vert_neigb[tet_in_ord[i][n]-1][j])
						//			exist=true;

						//	if(!exist)		// the vert_neigb[tet_in_ord[i][n]-1][j]  tet has never been added to tet_neigb[i]'s neigbor
						//	{
						//		tet_neigb[i][0]++;

						//		tet_neigb[i][tet_neigb[i][0]]=vert_neigb[tet_in_ord[i][n]-1][j];
						//	}
						//}

						delete [] temp_s3;
						temp_s3=NULL;
					}

					if(temp_s2!=NULL)
						delete [] temp_s2;
					temp_s2=NULL;
				}
			}

			if(temp_s1!=NULL)
				delete [] temp_s1;
			temp_s1=NULL;
		}
		int n[4];
		for(int i=0;i<E;i++)
		{
			bool flag=false;

			vector<int>  n;

			for(int j=0;j<4;j++)
				if(tet_neigb[4*i+j]==-1)
				{
					flag=true;
					n.push_back(j);
				}

				if(flag)
				{
					for(int j=0;j<n.size();j++)
					{
						bool p_bound=true;

						int t=n[j];

						for(int k=0;k<4;k++)
						{
							if(k!=t)
							{
								if(!point_in_bound(tet_in_ord[4*i+k]))
								{
									p_bound=false;
									break;
								}
							}
						}

						if(p_bound)
							tet_neigb[4*i+t]=-2;

					}
				}
		}
}

void  re_dat_tet::read_dat_binary(char *file)
{
	clear_all();

	ifstream ifile(file,ios::binary);

	int *temp;

	//temp=new int[2];

	temp=new int[4];

	ifile.read((char *)temp,4*sizeof(int));

	N=temp[0];
	E=temp[1];
	n_of_bt=temp[2];
	n_of_boud_tet=temp[3];

	if(temp!=NULL)
		delete [] temp;
	temp=NULL;

	vert_co=new float[3*N];
	v_wind =new float[4*N];
	p	   =new float[N];

	tet_in_ord=new int[4*E];
	tet_neigb =new int[4*E];

	bt=new int[n_of_bt];
	boud_tet=new int[n_of_boud_tet];

	ifile.read((char *)vert_co,3*N*sizeof(float));

	ifile.read((char *)v_wind,4*N*sizeof(float));

	ifile.read((char *)p,N*sizeof(float));

	ifile.read((char *)boud_box,6*sizeof(float));

	ifile.read((char *)pres_min_max,2*sizeof(float));

	ifile.read((char *)v_win_min_max,2*sizeof(float));

	ifile.read((char *)tet_in_ord,4*E*sizeof(int));

	ifile.read((char *)tet_neigb,4*E*sizeof(int));	

	ifile.read((char *)bt,n_of_bt*sizeof(int));	

	ifile.read((char *)boud_tet,n_of_boud_tet*sizeof(int));	

	ifile.close();
}

void  re_dat_tet::write_to_mh3(char *file)
{
	ofstream ofile(file);

	ofile<<size_vert_co()<<endl;

	for(int i=0;i<size_vert_co();i++)
		ofile<<vert_co[3*i+0]<<" "<<vert_co[3*i+1]<<" "<<vert_co[3*i+2]<<" "<<2<<" "<<endl;

	ofile<<endl<<0<<endl<<endl;

	ofile<<-8<<" "<<size_tet_in_ord()<<endl;

	for(int i=0;i<size_tet_in_ord();i++)
	{
		for(int j=0;j<4;j++)
			ofile<<tet_in_ord[4*i+j]<<" ";

		ofile<<0<<" "<<0<<" "<<0<<" "<<-4<<endl;
	}

	ofile.close();
}

void  re_dat_tet::write_to_fre(char *file)
{
	ofstream ofile(file);

	ofile<<size_vert_co()<<" "<<size_tet_in_ord()<<endl;

	for(int i=0;i<size_vert_co();i++)
		ofile<<vert_co[3*i+0]<<" "<<vert_co[3*i+1]<<" "<<vert_co[3*i+2]<<endl;

	for(int i=0;i<size_tet_in_ord();i++)
	{
		for(int j=0;j<4;j++)
			ofile<<tet_in_ord[4*i+j]<<" ";

		ofile<<endl;
	}

	ofile.close();
}

void  re_dat_tet::write_vert_neigb(char *file)
{
	ofstream ofile(file);

	for(int i=0;i<size_vert_co();i++)
		ofile<<vert_co[3*i+0]<<" "<<vert_co[3*i+1]<<" "<<vert_co[3*i+2]<<" "<<endl;

	for(int i=0;i<size_vert_neigb();i++)
	{
		ofile<<"The "<<i+1<<" point has "<<vert_neigb[i].size()<<" neigbours: ";

		for(int j=0;j<vert_neigb[i].size();j++)
			ofile<<vert_neigb[i][j]<<" ";

		ofile<<"."<<endl;
	}
	ofile.close();
}

void  re_dat_tet::write_tet_neigb(char *file)
{
	ofstream ofile(file);

	for(int i=0;i<size_tet_neigb();i++)
	{
		ofile<<"   ";
		for(int j=0;j<4;j++)
			ofile<<tet_neigb[4*i+j]<<"  ";

		ofile<<endl;
	}

	ofile.close();
}

void  re_dat_tet::write_btet_off(char *file)
{
	ofstream ofile(file);

	ofile<<"OFF"<<endl;

	ofile<<size_vert_co()<<" "<<size_btet()<<" "<<0<<endl;

	for(int i=0;i<size_vert_co();i++)
		ofile<<vert_co[3*i+0]<<" "<<vert_co[3*i+1]<<" "<<vert_co[3*i+2]<<" "<<endl;

	for(int i=0;i<size_btet();i++)
		ofile<<4<<" "<<tet_in_ord[4*in_let_index[i]+0]<<" "<<tet_in_ord[4*in_let_index[i]+1]<<" "<<tet_in_ord[4*in_let_index[i]+2]<<" "<<tet_in_ord[4*in_let_index[i]+3]<<endl;

	ofile.close();
}

void  re_dat_tet::write_to_binary(char *file)
{
	//int temp[2];

	//temp[0]=N;
	//temp[1]=E;

	int temp[4];

	temp[0]=N;
	temp[1]=E;
	temp[2]=n_of_bt;
	temp[3]=n_of_boud_tet;

	ofstream ofile(file,ios::binary);

	ofile.write((char *)temp,4*sizeof(int));

	ofile.write((char *)vert_co,3*N*sizeof(float));

	ofile.write((char *)v_wind,4*N*sizeof(float));

	ofile.write((char *)p,N*sizeof(float));

	ofile.write((char *)boud_box,6*sizeof(float));

	ofile.write((char *)pres_min_max,2*sizeof(float));

	ofile.write((char *)v_win_min_max,2*sizeof(float));

	ofile.write((char *)tet_in_ord,4*E*sizeof(int));

	ofile.write((char *)tet_neigb,4*E*sizeof(int));

	ofile.write((char *)bt,n_of_bt*sizeof(int));

	ofile.write((char *)boud_tet,n_of_boud_tet*sizeof(int));

	ofile.close();
}
void  re_dat_tet::find_tet_in_sphere(float center_x,float center_y, float center_z,float radius)
{
	let_type=SPHERE_LET;
	let_center_x=center_x;
	let_center_y=center_y;
	let_center_z=center_z;
	let_radius=radius;
	let_radius2=radius*radius;

	DNoteType *dnot,*tp;

	dnot=NULL;

	int start_point;

	start_point=find_tet_id_of_point(center_x, center_y, center_z);

	if(start_point==(-1))
		return;

	insert_tet(dnot, start_point);

	while(!check_f(dnot))
	{
		DNoteType *t;

		t=find_f(dnot);

		if(t==NULL)
			break;

		int ids;

		ids=t->n;

		int neib[4];

		for(int i=0;i<4;i++)
			neib[i]=tet_neigb[4*ids+i];

		for(int i=0;i<4;i++)
		{
			if((!tet_in_dnote(dnot,neib[i]))&&(neib[i]>=0))
			{
				float xyz_dis[3];
				float dist2;

				for(int j=0;j<3;j++)
				{
					xyz_dis[j]=0;

					for(int k=0;k<4;k++)
						xyz_dis[j]+=vert_co[3*tet_in_ord[4*neib[i]+k]+j]/4;
				}

				dist2=0;

				for(int j=0;j<3;j++)
					dist2+=xyz_dis[j]*xyz_dis[j];

				if(dist2<=let_radius2)
					if(!tet_in_dnote(dnot,neib[i]))
						insert_tet(dnot,neib[i]);
			}
		}

		t->g=true;
	}

	vector<int> vec;

	tp=dnot;

	while(tp!=NULL)
	{
		vec.push_back(tp->n);

		tp=tp->next;
	}

	n_of_btet=vec.size();

	if (in_let_index)
	{
		delete [] in_let_index;
	}
	in_let_index=new int [n_of_btet];

	for(int i=0;i<n_of_btet;i++)
		in_let_index[i]=vec[i];
	vec.clear();

	del_link(dnot);

	/*
	let_type=SPHERE_LET;
	let_center_x=center_x;
	let_center_y=center_y;
	let_center_z=center_z;
	let_radius=radius;
	let_radius2=radius*radius;
	vector<int> vec;
	float x_dis,y_dis,z_dis;
	int vert_id;
	float dist2;

	for(int i=0;i<E;i++)
	{
	bool intersect=false;

	for(int j=0;j<4;j++)
	{
	vert_id=3*tet_in_ord[4*i+j];
	x_dis=vert_co[vert_id]-center_x;
	y_dis=vert_co[vert_id+1]-center_y;
	z_dis=vert_co[vert_id+2]-center_z;
	dist2=x_dis*x_dis+y_dis*y_dis+z_dis*z_dis;
	if(dist2<=let_radius2)
	{
	intersect=true;
	break;
	}
	}

	if(intersect)
	vec.push_back(i);
	}

	n_of_btet=vec.size();

	if (in_let_index)
	{
	delete [] in_let_index;
	}
	in_let_index=new int [n_of_btet];

	for(int i=0;i<n_of_btet;i++)
	in_let_index[i]=vec[i];
	vec.clear();
	*/
}
void  re_dat_tet::find_tet_in_box(float min_x,float max_x,float min_y,float max_y,float min_z,float max_z,float px,float py,float pz)
{
	let_type=BOX_LET;

	let_x_min=min_x-px;
	let_x_max=max_x+px;

	let_y_min=min_y-py;
	let_y_max=max_y+py;

	let_z_min=min_z-pz;
	let_z_max=max_z+pz;

	float c_xyz[3];

	c_xyz[0]=(min_x+max_x)/2;
	c_xyz[1]=(min_y+max_y)/2;
	c_xyz[2]=(min_z+max_z)/2;

	DNoteType *dnot,*tp;

	dnot=NULL;

	int start_point;

	start_point=find_tet_id_of_point(c_xyz[0], c_xyz[1], c_xyz[2]);

	if(start_point==(-1))
		return;

	insert_tet(dnot, start_point);

	while(!check_f(dnot))
	{
		DNoteType *t;

		t=find_f(dnot);

		if(t==NULL)
			break;

		int ids;

		ids=t->n;

		int neib[4];

		for(int i=0;i<4;i++)
			neib[i]=tet_neigb[4*ids+i];

		for(int i=0;i<4;i++)
		{
			if((!tet_in_dnote(dnot,neib[i]))&&(neib[i]>=0))
			{
				float xyz_dis[3];
				//float dist2;

				for(int j=0;j<3;j++)
				{
					xyz_dis[j]=0;

					for(int k=0;k<4;k++)
						xyz_dis[j]+=vert_co[3*tet_in_ord[4*neib[i]+k]+j]/4;
				}

				//dist2=0;

				//for(int j=0;j<3;j++)
				//	dist2+=xyz_dis[j]*xyz_dis[j];

				if((xyz_dis[0]>=min_x)&&(xyz_dis[0]<=max_x)&&(xyz_dis[1]>=min_y)&&(xyz_dis[1]<=max_y)&&(xyz_dis[2]>=min_z)&&(xyz_dis[2]<=max_z))
					if(!tet_in_dnote(dnot,neib[i]))
						insert_tet(dnot,neib[i]);
			}
		}

		t->g=true;
	}

	vector<int> vec;

	tp=dnot;

	while(tp!=NULL)
	{
		vec.push_back(tp->n);

		tp=tp->next;
	}

	n_of_btet=vec.size();

	if (in_let_index)
	{
		delete [] in_let_index;
	}
	in_let_index=new int [n_of_btet];

	for(int i=0;i<n_of_btet;i++)
		in_let_index[i]=vec[i];
	vec.clear();

	del_link(dnot);

	/*
	let_type=BOX_LET;

	let_x_min=min_x-px;
	let_x_max=max_x+px;

	let_y_min=min_y-py;
	let_y_max=max_y+py;

	let_z_min=min_z-pz;
	let_z_max=max_z+pz;

	vector<int> vec;
	float x,y,z;
	int vert_id;
	for(int i=0;i<E;i++)
	{
	bool intersect=false;

	for(int j=0;j<4;j++)
	{
	vert_id=3*tet_in_ord[4*i+j];
	x=vert_co[vert_id];
	y=vert_co[vert_id+1];
	z=vert_co[vert_id+2];
	if(x>=let_x_min&&x<=let_x_max
	&&y>=let_y_min&&y<=let_y_max
	&&z>=let_z_min&&z<=let_z_max)
	{
	intersect=true;
	break;
	}
	}

	if(intersect)
	vec.push_back(i);
	}

	n_of_btet=vec.size();

	if (in_let_index)
	{
	delete [] in_let_index;
	}
	in_let_index=new int [n_of_btet];

	for(int i=0;i<n_of_btet;i++)
	in_let_index[i]=vec[i];
	vec.clear();
	*/
}

void  re_dat_tet::clear_all()
{

	if(vert_co!=NULL)
	{

		delete [] vert_co;

		vert_co=NULL;
	}

	if(v_wind!=NULL)
	{
		delete [] v_wind;
		v_wind=NULL;
	}

	if(p!=NULL)
		delete [] p;
	p=NULL;

	if(vert_neigb!=NULL)
		delete [] vert_neigb;

	vert_neigb=NULL;

	//if(boud_box!=NULL)
	//	delete [] boud_box;

	//if(pres_min_max!=NULL)
	//	delete [] pres_min_max;

	//if(v_win_min_max)
	//	delete [] v_win_min_max;

	if(tet_in_ord!=NULL)
	{

		delete [] tet_in_ord;
		tet_in_ord=NULL;
	}

	if(tet_neigb!=NULL)
	{
		//for(int i=0;i<size_tet_neigb();i++)
		//{
		//	if(tet_neigb[i]!=NULL)
		//		delete [] tet_neigb[i];
		//}

		delete [] tet_neigb;

		tet_neigb=NULL;
	}

	if(in_let_index!=NULL)
		delete [] in_let_index;
	n_of_btet=0;
	in_let_index=NULL;

	if(presure_tex_data!=NULL)
		delete [] presure_tex_data;
	presure_tex_data=NULL;

	//if(t_size!=NULL)
	//	delete [] t_size;
	//t_size=NULL;

	E=0;

	N=0;

	//vert_co.clear();


	//v_wind.clear();
	//p.clear();
	//tet_in_ord.clear();
	//vert_neigb.clear();
	////pres_min_max.clear();
}

int  re_dat_tet::size_vert_co()
{
	return N;
}

int  re_dat_tet::size_vert_neigb()
{
	return N;
}

int  re_dat_tet::size_v_wind()
{
	return N;
}

int  re_dat_tet::size_p()
{
	return N;
}

int  re_dat_tet::size_tet_in_ord()
{
	return E;
}

int  re_dat_tet::size_boud_box()
{
	return 6;
}

int  re_dat_tet::size_pres_min_max()
{
	return 2;
}

int  re_dat_tet::size_v_win_min_max()
{
	return 2;
}

int re_dat_tet::size_tet_neigb()
{
	return E;
}

int re_dat_tet::size_btet()
{
	return n_of_btet;
}

bool re_dat_tet::point_in_bound(int point_id)
{
	float err=1e-6;

	if(vert_co[3*point_id+0]<=boud_box[0]+err
		||vert_co[3*point_id+1]<=boud_box[2]+err
		||vert_co[3*point_id+2]<=boud_box[4]+err
		||vert_co[3*point_id+0]>=boud_box[1]-err
		||vert_co[3*point_id+1]>=boud_box[3]-err
		||vert_co[3*point_id+2]>=boud_box[5]-err)
		return true;

	return false;
}

void re_dat_tet::write_tet_neigb_12(char *file1,char *file2)
{
	ofstream ofile1(file1);
	ofstream ofile2(file2);

	for(int i=0;i<N;i++)
	{
		ofile1<<"v "<<vert_co[3*i+0]<<" "<<vert_co[3*i+1]<<" "<<vert_co[3*i+2]<<" "<<endl;
		ofile2<<"v "<<vert_co[3*i+0]<<" "<<vert_co[3*i+1]<<" "<<vert_co[3*i+2]<<" "<<endl;
	}

	for(int i=0;i<E;i++)
	{
		int flag=0;

		for(int j=0;j<4;j++)
			if(tet_neigb[4*i+j]==-1)
				flag=1;
			else if(tet_neigb[4*i+j]==-2)
				flag=2;

		if(flag==1)
			ofile1<<"f "<<tet_in_ord[4*i+0]+1<<" "<<tet_in_ord[4*i+1]+1<<" "<<tet_in_ord[4*i+2]+1<<" "<<tet_in_ord[4*i+3]+1<<" "<<endl;

		if(flag==2)
			ofile2<<"f "<<tet_in_ord[4*i+0]+1<<" "<<tet_in_ord[4*i+1]+1<<" "<<tet_in_ord[4*i+2]+1<<" "<<tet_in_ord[4*i+3]+1<<" "<<endl;		
	}

	//for(int i=0;i<E;i++)
	//	ofile1<<"u "<<tet_neigb[i][0]<<" "<<tet_neigb[i][1]<<" "<<tet_neigb[i][2]<<" "<<tet_neigb[i][3]<<" "<<endl;

	ofile1.close();
	ofile2.close();
}

int re_dat_tet::find_tet_id_of_point_hel(float x,float y,float z,vector<float> se,int ids)
{

	if((x<boud_box[0])||(x>boud_box[1])||(y<boud_box[2])||(y>boud_box[3])||(z<boud_box[4])||(z>boud_box[5])||(E==0))
		return -1;

	int rand_id=ids;

	//rand_id=rand()%E;

	vector<float> point;

	point.push_back(x);point.push_back(y);point.push_back(z);

	//if(point_in_tet(point,rand_id)!=(-1))
	//	return rand_id;

	vector<float> p_start,p_current;


	for(int i=0;i<3;i++)
	{
		//float rmp=0;

		//for(int j=0;j<4;j++)
		//	rmp+=vert_co[3*tet_in_ord[4*rand_id+j]+i];

		p_start.push_back(se[i]);
		p_current.push_back(se[i]);

	}

	static vector<int> vot;
	static vector<bool> lins;

	//clock_t start_t,end_t;

	//start_t=clock();
	//
	while(point_in_tet(point,rand_id)==(-1))
	{
		//vector<float> lsy;

		//lsy=compute_new_point(p_current,p_start,point,rand_id);

		// lsy need to be not (FLT_MAX,FLT_MAX,FLT_MAX)

		bool tv_vv=false;

		//int fow=-1;

		float mer=bar_of_point(p_current,p_start,point);

		int ffg=-1;

		for(int i=0;i<4;i++)
		{
			//vector<float> sd;

			//for(int i=0;i<4;i++)
			//	sd.push_back(tet_neigb[4*rand_id+i]);

			if(!tv_vv)
			{

				vector<float> lsy;

				if((tet_neigb[4*rand_id+i]!=(-1))&&(tet_neigb[4*rand_id+i]!=(-2)))
				{

					lsy=compute_new_point(p_current,p_start,point,tet_neigb[4*rand_id+i]);

					if(lsy[0]!=FLT_MAX)
					{
						float umy=bar_of_point(lsy,p_start,point);

						if((umy>=mer)||(umy==1))
						{

							if(umy==1)
								return tet_neigb[4*rand_id+i];

							bool fgg=true;

							if(umy>mer)
							{
								vot.clear();
								lins.clear();
								tv_vv=true;
							}
							else
							{
								for(int tti=0;tti<vot.size();tti++)
									if(vot[tti]==tet_neigb[4*rand_id+i])
										fgg=false;

								if(fgg)
								{
									vot.push_back(tet_neigb[4*rand_id+i]);
									lins.push_back(false);
								}
							}

							mer=umy;

							if(fgg)
							{
								for(int j=0;j<3;j++)
									p_current[j]=lsy[j];

								ffg=i;
							}

						}
						//for(int j=0;j<3;j++)
						//	p_current[j]=lsy[j];

						//if(tv_vv)
						//{
						//	rand_id=tet_neigb[4*rand_id+i];
						//	//break;
						//}

						//tv_vv=true;
					}

					//if(point_in_tet(lsy,tet_neigb[4*rand_id+i])!=(-1))
					//	fow=i;

				}
			}
		}

		//if(!tv_vv)
		//	rand_id=tet_neigb[4*rand_id+ffg];

		bool gf=false;

		int tsi=-1;

		if(!tv_vv)
		{
			bool ft=false;

			while(!ft)
			{
				ft=true;

				int fi=-1;

				for(int ui=0;ui<vot.size();ui++)
				{
					if(!lins[ui])
					{
						fi=ui;
						break;
					}
				}

				for(int uj=0;uj<4;uj++)
				{
					if((vot.size()==0)||(fi==(-1)))
						break;

					if((tet_neigb[4*vot[fi]+uj]!=(-1))&&(tet_neigb[4*vot[fi]+uj]!=(-2)))
					{
						vector<float> sy;

						sy=compute_new_point(p_current,p_start,point,tet_neigb[4*vot[fi]+uj]);

						if(sy[0]!=FLT_MAX)
						{
							float umy=bar_of_point(sy,p_start,point);

							if(umy>=mer)
							{
								bool fgg=true;

								if(umy>mer)
								{
									tsi=tet_neigb[4*vot[fi]+uj];
									vot.clear();
									lins.clear();
									tv_vv=true;
								}
								else
								{
									for(int tti=0;tti<vot.size();tti++)
										if(vot[tti]==tet_neigb[4*vot[fi]+uj])
											fgg=false;

									if(fgg)
									{
										vot.push_back(tet_neigb[4*vot[fi]+uj]);
										lins.push_back(false);
									}
								}

								if((fgg)&&(umy>mer))
								{
									ffg=uj;
									for(int j=0;j<3;j++)
										p_current[j]=sy[j];
								}

								mer=umy;
							}

							if(ffg>-1)
								gf=true;
						}
					}
				}

				if(lins.size()>fi)
					lins[fi]=true;

				if(lins.size()>0)
					for(int ui=0;ui<vot.size();ui++)
					{
						if(!lins[ui])
						{
							ft=false;
							break;
						}
					}
			}

		}

		if(gf)
			rand_id=tsi;
		else
			rand_id=tet_neigb[4*rand_id+ffg];
		/*
		if(!tv_vv)
		{
		for(int i=0;i<4;i++)
		{
		if((tet_neigb[4*rand_id+i]!=(-1))&&(tet_neigb[4*rand_id+i]!=(-2)))
		{
		for(int j=0;j<4;j++)					
		{
		if(!tv_vv)
		{
		vector<float> lsy;

		//int ijksdf=tet_neigb[4*tet_neigb[4*rand_id+i]+j];

		lsy=compute_new_point(p_current,p_start,point,tet_neigb[4*tet_neigb[4*rand_id+i]+j]);

		if(lsy[0]!=FLT_MAX)
		{
		for(int ij=0;ij<3;ij++)
		p_current[ij]=lsy[ij];

		rand_id=tet_neigb[4*tet_neigb[4*rand_id+i]+j];

		tv_vv=true;
		}
		}
		}
		}
		}
		}

		if(!tv_vv)
		{
		for(int i=0;i<4;i++)
		{
		if((tet_neigb[4*rand_id+i]!=(-1))&&(tet_neigb[4*rand_id+i]!=(-2)))
		{
		for(int j=0;j<4;j++)
		{
		if((tet_neigb[4*tet_neigb[4*rand_id+i]+j]!=(-1))&&(tet_neigb[4*tet_neigb[4*rand_id+i]+j]!=(-2)))
		{
		for(int m=0;m<4;m++)
		{
		if(!tv_vv)
		{
		vector<float> lsy;

		//int ijksdf=tet_neigb[4*tet_neigb[4*tet_neigb[4*rand_id+i]+j]+m];

		lsy=compute_new_point(p_current,p_start,point,tet_neigb[4*tet_neigb[4*tet_neigb[4*rand_id+i]+j]+m]);

		if(lsy[0]!=FLT_MAX)
		{
		for(int tn=0;tn<3;tn++)
		p_current[tn]=lsy[tn];

		rand_id=tet_neigb[4*tet_neigb[4*tet_neigb[4*rand_id+i]+j]+m];

		tv_vv=true;
		}
		}
		}
		}
		}
		}
		}
		}

		if(!tv_vv)
		{
		for(int i=0;i<4;i++)
		{
		if((tet_neigb[4*rand_id+i]!=(-1))&&(tet_neigb[4*rand_id+i]!=(-2)))
		{
		for(int j=0;j<4;j++)
		{
		if((tet_neigb[4*tet_neigb[4*rand_id+i]+j]!=(-1))&&(tet_neigb[4*tet_neigb[4*rand_id+i]+j]!=(-2)))
		{
		for(int m=0;m<4;m++)
		{

		if((tet_neigb[4*tet_neigb[4*tet_neigb[4*rand_id+i]+j]+m])!=(-1)&&(tet_neigb[4*tet_neigb[4*tet_neigb[4*rand_id+i]+j]+m])!=(-2))
		{
		for(int n=0;n<4;n++)
		{
		if(!tv_vv)
		{
		vector<float> lsy;

		//int ijksdf=tet_neigb[4*tet_neigb[4*tet_neigb[4*tet_neigb[4*rand_id+i]+j]+m]+n];

		lsy=compute_new_point(p_current,p_start,point,tet_neigb[4*tet_neigb[4*tet_neigb[4*tet_neigb[4*rand_id+i]+j]+m]+n]);

		if(lsy[0]!=FLT_MAX)
		{
		for(int tn=0;tn<3;tn++)
		p_current[tn]=lsy[tn];

		rand_id=tet_neigb[4*tet_neigb[4*tet_neigb[4*tet_neigb[4*rand_id+i]+j]+m]+n];

		tv_vv=true;
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}

		*/

		//for(int i=0;i<3;i++)
		//	p_current[i]=lsy[i];

		//rand_id=tet_neigb[4*rand_id+fow];

		//end_t=clock()-start_t;

		//if(end_t>3000)
		//	return -1;
	}

	return rand_id;


	/*vector<float> p_vec;

	for(int i=0;i<3;i++)
	p_vec.push_back(point[i]-p_start[i]);

	float k=bar_of_point(p_current,p_start,point);

	vector<vector<float>> tet_po,intsec;

	bool interb[4];	

	for(int i=0;i<4;i++)
	{
	vector<float> tepm;

	for(int j=0;j<3;j++)
	tepm.push_back(vert_co[3*tet_in_ord[4*rand_id+i]+j]);

	tet_po.push_back(tepm);
	}

	vector<float> tepm;

	tepm=intersec_of_line_plain(point,p_start,tet_po[1],tet_po[2],tet_po[3]);

	if(point_in_tri( tepm,tet_po[1],tet_po[2],tet_po[3])==1)
	interb[0]=true;
	else
	interb[0]=false;

	intsec.push_back(tepm);

	tepm.clear();

	tepm=intersec_of_line_plain(point,p_start,tet_po[0],tet_po[2],tet_po[3]);

	if(point_in_tri( tepm,tet_po[0],tet_po[2],tet_po[3])==1)
	interb[1]=true;
	else
	interb[1]=false;

	intsec.push_back(tepm);

	tepm.clear();

	tepm=intersec_of_line_plain(point,p_start,tet_po[1],tet_po[0],tet_po[3]);

	if(point_in_tri( tepm,tet_po[1],tet_po[0],tet_po[3])==1)
	interb[2]=true;
	else
	interb[2]=false;

	intsec.push_back(tepm);

	tepm.clear();

	tepm=intersec_of_line_plain(point,p_start,tet_po[1],tet_po[2],tet_po[0]);

	if(point_in_tri( tepm,tet_po[1],tet_po[2],tet_po[0])==1)
	interb[3]=true;
	else
	interb[3]=false;

	intsec.push_back(tepm);

	tepm.clear();

	for(int i=0;i<4;i++)
	{
	if(interb[i])
	{
	float kk=bar_of_point(intsec[i],p_start,point);

	if((kk>0)&&(kk<=1)&&(kk>k))
	{
	tv_vv=true;

	for(int j=0;j<3;j++)
	p_current[j]=intsec[i][j];

	k=kk;

	rand_id=tet_neigb[4*rand_id+i];

	if(point_in_tet(point,rand_id)!=(-1))
	return rand_id;
	}
	}

	}*/
}

int re_dat_tet::find_tet_id_of_point(float x,float y,float z)
{
	if((x<boud_box[0])||(x>boud_box[1])||(y<boud_box[2])||(y>boud_box[3])||(z<boud_box[4])||(z>boud_box[5])||(E==0))
		return -1;

	int rand_id=rand()%E;

	vector<float> s_o,s_u,d_u,d_o;

	for(int i=0;i<3;i++)
	{
		float rmp=0;

		for(int j=0;j<4;j++)
			rmp+=vert_co[3*tet_in_ord[4*rand_id+j]+i];

		s_o.push_back(rmp/4);
		s_u.push_back(rmp/4);
	}

	s_u[1]=boud_box[3];

	d_o.push_back(x);	d_o.push_back(y);	d_o.push_back(z);
	d_u.push_back(x);	d_u.push_back(boud_box[3]);	d_u.push_back(z);

	int i_u,dd_u,dd_o;

	i_u=find_tet_id_of_point_hel(s_u[0],s_u[1],s_u[2],s_o,rand_id);
	//if(i_u==(-1))
	//	return -1;
	dd_u=find_tet_id_of_point_hel(d_u[0],d_u[1],d_u[2],s_u,i_u);
	//if(dd_u==(-1))
	//	return -1;
	dd_o=find_tet_id_of_point_hel(d_o[0],d_o[1],d_o[2],d_u,dd_u);
	//if(dd_o==(-1))
	//	return -1;

	return dd_o;
}

int re_dat_tet::point_in_tet(vector<float> point, int tet_id)
{
	vector<vector<float>> temp;
	for(int i=0;i<4;i++)
	{
		vector<float> tt;

		for(int j=0;j<3;j++)
			tt.push_back(vert_co[3*tet_in_ord[4*tet_id+i]+j]);

		temp.push_back(tt);
	}

	vector<vector<float>> tft;   //  v0p,v1p,v2p,v3p,v10,v20,v30

	for(int i=0;i<7;i++)
	{
		vector<float> tt;

		for(int j=0;j<3;j++)
			tt.push_back(0);

		tft.push_back(tt);
	}

	for(int i=0;i<3;i++)
	{
		tft[0][i]=temp[0][i]-point[i];
		tft[1][i]=temp[1][i]-point[i];
		tft[2][i]=temp[2][i]-point[i];
		tft[3][i]=temp[3][i]-point[i];
		tft[4][i]=temp[1][i]-temp[0][i];
		tft[5][i]=temp[2][i]-temp[0][i];
		tft[6][i]=temp[3][i]-temp[0][i];
	}

	float T=doc_cross(tft[4],tft[6],tft[5]);

	if(T==0)
		return false;

	float tmp[4];

	tmp[0]=doc_cross(tft[0],tft[1],tft[2])/T;
	tmp[1]=doc_cross(tft[0],tft[3],tft[1])/T;
	tmp[2]=doc_cross(tft[0],tft[2],tft[3])/T;
	tmp[3]=doc_cross(tft[1],tft[3],tft[2])/T;

	if((tmp[0]>=0)&&(tmp[0]<=1)&&(tmp[1]>=0)&&(tmp[1]<=1)&&(tmp[2]>=0)&&(tmp[2]<=1)&&(tmp[3]>=0)&&(tmp[3]<=1))
	{
		if((tmp[0]==0)||(tmp[0]==1)||(tmp[1]==0)||(tmp[1]==1)||(tmp[2]==0)||(tmp[2]==1)||(tmp[3]==0)||(tmp[3]==1))
			return 0;
		else
			return 1;
	}
	else
		return -1;
}

float re_dat_tet::doc_cross(vector<float> x,vector<float> y,vector<float> z)
{
	return ((x[0]*y[1]*z[2]+x[2]*y[0]*z[1]+x[1]*y[2]*z[0])-(x[2]*y[1]*z[0]+x[1]*y[0]*z[2]+x[0]*y[2]*z[1]));
}

float re_dat_tet::doc_prod(vector<float> v1,vector<float> v2) // return the doc product
{
	return (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2]);
}

vector<float> re_dat_tet::cross_prod(vector<float> v1,vector<float> v2) // return the cross product
{
	vector<float> temp;

	temp.push_back(v1[1]*v2[2]-v1[2]*v2[1]);
	temp.push_back(v1[2]*v2[0]-v1[0]*v2[2]);
	temp.push_back(v1[0]*v2[1]-v1[1]*v2[0]);

	return temp;
}

vector<float> re_dat_tet::intersec_of_line_plain(vector<float> p1,vector<float> p2,vector<float> v1,vector<float> v2,vector<float> v3)
{
	vector<float> temp,plain_norm,pt1,pt2,v_d,p1v1;

	float err=1e-9;

	for(int i=0;i<3;i++)
		temp.push_back(FLT_MAX);

	for(int i=0;i<3;i++)
	{
		pt1.push_back(v2[i]-v1[i]);
		pt2.push_back(v3[i]-v1[i]);
		v_d.push_back(p2[i]-p1[i]);
		p1v1.push_back(p1[i]-v1[i]);
	}

	plain_norm=cross_prod(pt1,pt2);

	float vnp,tnv;

	vnp=doc_prod(plain_norm,p1v1);
	tnv=doc_prod(plain_norm,v_d);

	if(tnv==0)
	{
		vector<float> te;

		for(int i=0;i<3;i++)
			te.push_back(p1[i]-v1[i]);

		float ui;

		ui=doc_prod(plain_norm,te);

		if(fabs(ui)>=err)
			return temp;
		else
		{
			vector<float> line_sec,line_sec1,line_sec2,line_sec3;

			line_sec1=intersec_of_line_line(p1, p2, v3, v2);
			line_sec2=intersec_of_line_line(p1, p2, v1, v3);
			line_sec3=intersec_of_line_line(p1, p2, v1, v2);

			bool flg1,flg2,flg3;

			flg1=line_in_out(line_sec1,v3,v2);
			flg2=line_in_out(line_sec2,v3,v1);
			flg3=line_in_out(line_sec3,v1,v2);

			if(line_sec1[0]==FLT_MAX)
				flg1=false;

			if(line_sec2[0]==FLT_MAX)
				flg2=false;

			if(line_sec3[0]==FLT_MAX)
				flg3=false;

			int cit[3];

			for(int i=0;i<3;i++)
				cit[i]=1;

			if(!flg1)
			{
				cit[0]=0;

				for(int i=0;i<3;i++)
					line_sec1[i]=0;
			}

			if(!flg2)
			{
				cit[1]=0;

				for(int i=0;i<3;i++)
					line_sec2[i]=0;
			}

			if(!flg3)
			{
				cit[2]=0;

				for(int i=0;i<3;i++)
					line_sec3[i]=0;
			}

			int tic=0;

			for(int i=0;i<3;i++)
				tic+=cit[i];

			vector<float> mas;

			for(int i=0;i<3;i++)
			{
				line_sec.push_back(line_sec1[i]+line_sec2[i]+line_sec3[i]);
				mas.push_back(FLT_MAX);
			}


			if((tic==2)||(tic==3))
			{
				for(int i=0;i<3;i++)
					line_sec[i]/=tic;

				return line_sec;
			}
			else
			{
				return mas;
			}
		}
	}
	else
	{
		float tk=-vnp/tnv;

		vector<float> xk;

		for(int i=0;i<3;i++)
			xk.push_back(tk*v_d[i]+p1[i]);

		return xk;
	}
}

vector<float> re_dat_tet::intersec_of_line_line(vector<float> p1,vector<float> p2,vector<float> v1,vector<float> v2)
{
	vector<float> temp,p,v;

	for(int i=0;i<3;i++)
	{
		temp.push_back(FLT_MAX);
		p.push_back(p2[i]-p1[i]);
		v.push_back(v2[i]-v1[i]);
	}

	vector<float> cro=cross_prod(p,v);

	if((cro[0]==0)&&(cro[1]==0)&&(cro[2]==0))
	{
		vector<float> p1v1,p1v2;

		for(int i=0;i<3;i++)
		{
			p1v1.push_back(p1[i]-v1[i]);
			p1v2.push_back(p1[i]-v2[i]);
		}

		float ut=doc_prod(p1v1,p1v1);

		if(ut!=0)
		{
			int co=0;

			for(int i=0;i<3;i++)
				if(p1v1[i]!=0)
					co=i;

			float ati=p1v2[co]/p1v1[co];

			bool fla=true;

			for(int i=0;i<3;i++)
				if((ati*p1v1[i]-p1v2[i])!=0)
					fla=false;

			if(!fla)
				return temp;
			else
			{
				vector<float> lins;

				for(int i=0;i<3;i++)
					lins.push_back((v1[i]+v2[i])/2);

				return lins;
			}

		}
		else
		{
			vector<float> lins;

			for(int i=0;i<3;i++)
				lins.push_back((v1[i]+v2[i])/2);

			return lins;
		}
	}
	else
	{
		float nn;

		if((p[0]*v[1]-p[1]*v[0])!=0)
		{
			nn=(v1[0]*v[1]-v1[1]*v[0]+p1[1]*v[0]-p1[0]*v[1])/(p[0]*v[1]-p[1]*v[0]);
		}
		else if((p[0]*v[2]-p[2]*v[0])!=0)
		{
			nn=(v1[0]*v[2]-v1[2]*v[0]+p1[2]*v[0]-p1[0]*v[2])/(p[0]*v[2]-p[2]*v[0]);
		}
		else	//(p[0]*v[1]-p[1]*v[0])!=0
		{
			nn=(v1[2]*v[1]-v1[1]*v[2]+p1[1]*v[2]-p1[2]*v[1])/(p[2]*v[1]-p[1]*v[2]);			
		}

		vector<float> tpp;

		for(int i=0;i<3;i++)
			tpp.push_back((1-nn)*p1[i]+nn*p2[i]);

		return tpp;
	}
}

bool re_dat_tet::line_in_out(vector<float> p,vector<float> v1,vector<float> v2)  // only in the same line
{
	bool fg=true;

	vector<float> p1,p2;

	for(int i=0;i<3;i++)
	{
		p1.push_back(p[i]-v1[i]);
		p2.push_back(p[i]-v2[i]);
	}

	float ts;

	ts=doc_prod(p1,p2);

	if(ts>0)
		return false;
	else
		return true;
}

int re_dat_tet::point_in_tri(vector<float> point, vector<float> tet1, vector<float> tet2, vector<float> tet3)		// only in the same plain
{
	vector<vector<float>> temp;
	for(int i=0;i<3;i++)
	{
		vector<float> tt;

		for(int j=0;j<3;j++)
			tt.push_back(0);

		temp.push_back(tt);
	}

	for(int i=0;i<3;i++)
	{
		temp[0][i]=tet1[i];
		temp[1][i]=tet2[i];
		temp[2][i]=tet3[i];
	}


	vector<float> v01,v12,v20,v0p,v1p,v2p;

	for(int i=0;i<3;i++)
	{
		v01.push_back(temp[1][i]-temp[0][i]);
		v12.push_back(temp[2][i]-temp[1][i]);
		v20.push_back(temp[0][i]-temp[2][i]);
		v0p.push_back(point[i]-temp[0][i]);
		v1p.push_back(point[i]-temp[1][i]);
		v2p.push_back(point[i]-temp[2][i]);
	}

	vector<float> s,s0,s1,s2;

	s=cross_prod(v01,v12);
	s0=cross_prod(v01,v1p);
	s1=cross_prod(v12,v2p);
	s2=cross_prod(v20,v0p);

	float ss=doc_prod(s,s);

	if(ss==0)
		return false;

	float ts[3];

	int ct=0;

	float teemp=0;

	for(int i=0;i<3;i++)
	{
		if(fabs(s[i])>teemp)
		{
			ct=i;
			teemp=fabs(s[i]);
		}
	}

	ts[0]=s0[ct]/s[ct];
	ts[1]=s1[ct]/s[ct];
	ts[2]=s2[ct]/s[ct];

	//if((ts[0]>=0)&&(ts[0]<=1)&&(ts[1]>=0)&&(ts[1]<=1)&&(ts[2]>=0)&&(ts[2]<=1))
	//{
	//	if((ts[0]==0)||(ts[0]==1)||(ts[1]==0)||(ts[1]==1)||(ts[2]==0)||(ts[2]==1))
	//		return 0;
	//	else
	//		return 1;
	//}
	//else
	//	return -1;

	float err=1e-6;

	if((ts[0]>=-err)&&(ts[0]<=1+err)&&(ts[1]>=-err)&&(ts[1]<=1+err)&&(ts[2]>=-err)&&(ts[2]<=1+err))
		//if((fabs(ts[0])>=0)&&(fabs(ts[0])<=1+err)&&(fabs(ts[1])>=0)&&(fabs(ts[1])<=1+err)&&(fabs(ts[2])>=0)&&(fabs(ts[2])<=1+err))
	{
		if((fabs(ts[0])<=err)||(fabs(ts[0])>=1-err)||(fabs(ts[1])<=err)||(fabs(ts[1])>=1-err)||(fabs(ts[2])<=err)||(fabs(ts[2])>=1-err))
			return 0;
		else
			return 1;
	}
	else
		return -1;
}

float re_dat_tet::bar_of_point(vector<float> p,vector<float> p0,vector<float> p1) // p0 and p1 are not the same point, the three are in the same line
{
	vector<float> te,tei;

	for(int i=0;i<3;i++)
	{
		te.push_back(p1[i]-p0[i]);
		tei.push_back(p[i]-p0[i]);
	}

	float rea=doc_prod(te,te);

	float fea=doc_prod(tei,te);

	if(rea==0)
		return FLT_MAX;
	else
		return fea/rea;
}

vector<float> re_dat_tet::compute_new_point(vector<float> p_current,vector<float> p_start,vector<float> point, int rand_id)  // return the new point ,if no, return vector3f(FLT_MAX)
{
	vector<float> tiy;

	tiy.push_back(FLT_MAX);
	tiy.push_back(FLT_MAX);
	tiy.push_back(FLT_MAX);

	if((rand_id==(-1))||(rand_id==(-2)))
		return tiy;

	vector<float> p_vec;

	for(int i=0;i<3;i++)
		p_vec.push_back(point[i]-p_start[i]);

	float k=bar_of_point(p_current,p_start,point);

	vector<vector<float>> tet_po,intsec;

	bool interb[4];	

	for(int i=0;i<4;i++)
	{
		vector<float> tepm;

		for(int j=0;j<3;j++)		
			tepm.push_back(vert_co[3*tet_in_ord[4*rand_id+i]+j]);


		tet_po.push_back(tepm);
	}

	vector<float> tepm;

	tepm=intersec_of_line_plain(point,p_start,tet_po[1],tet_po[2],tet_po[3]);

	if(point_in_tri( tepm,tet_po[1],tet_po[2],tet_po[3])!=(-1))
		interb[0]=true;
	else
		interb[0]=false;

	intsec.push_back(tepm);

	tepm.clear();

	tepm=intersec_of_line_plain(point,p_start,tet_po[0],tet_po[2],tet_po[3]);

	if(point_in_tri( tepm,tet_po[0],tet_po[2],tet_po[3])!=(-1))
		interb[1]=true;
	else
		interb[1]=false;

	intsec.push_back(tepm);

	tepm.clear();

	tepm=intersec_of_line_plain(point,p_start,tet_po[1],tet_po[0],tet_po[3]);

	if(point_in_tri( tepm,tet_po[1],tet_po[0],tet_po[3])!=(-1))
		interb[2]=true;
	else
		interb[2]=false;

	intsec.push_back(tepm);

	tepm.clear();

	tepm=intersec_of_line_plain(point,p_start,tet_po[1],tet_po[2],tet_po[0]);

	if(point_in_tri( tepm,tet_po[1],tet_po[2],tet_po[0])!=(-1))
		interb[3]=true;
	else
		interb[3]=false;

	intsec.push_back(tepm);

	tepm.clear();

	float flk=-FLT_MAX;
	int cflg=INT_MAX;
	for(int i=0;i<4;i++)
	{
		if(interb[i])
		{
			float kk=bar_of_point(intsec[i],p_start,point);

			if((kk>=0)&&(kk<=1)&&(kk>=k))
			{

				if(kk>flk)
				{
					flk=kk;
					cflg=i;
				}
				//k=kk;

				//return intsec[i];
				//tv_vv=true;

				//for(int j=0;j<3;j++)
				//	p_current[j]=intsec[i][j];

				//k=kk;

				//rand_id=tet_neigb[4*rand_id+i];

				//if(point_in_tet(point,rand_id)!=(-1))
				//	return rand_id;
			}
			else if(kk>1)
			{
				return point;
			}
		}

	}

	if(cflg<5)
		return intsec[cflg];	

	return tiy;
}

bool re_dat_tet::tet_in_dnote(DNoteType *dnot, int ids)
{
	DNoteType *t;

	//bool fg=false;

	//t=dnot;

	//while((!fg)&&(t!=NULL))
	//{
	//	if(t->n==ids)
	//		fg=true;
	//	else
	//		t=t->next;
	//}

	t=find_pos_link(dnot,ids);

	if(t==NULL)
		return false;
	else
		return true;

	//return fg;
}

void re_dat_tet::insert_tet(DNoteType *&dnot, int ids)
{
	DNoteType *t,*p;

	p=new DNoteType;
	p->n=ids;
	p->prev=NULL;
	p->next=NULL;
	p->g=false;

	t=dnot;

	while((t!=NULL)&&(t->n<ids))
		t=t->next;

	//t=find_pos_link(dnot,ids);

	if(t==NULL)
	{
		t=dnot;

		if(dnot==NULL)
		{
			dnot=p;			
		}
		else
		{
			while(t->next!=NULL)
				t=t->next;

			t->next=p;
			p->prev=t;
		}

	}
	else
	{
		p->prev=t->prev;
		p->next=t;
		t->prev=p;
		if(p->prev!=NULL)
			p->prev->next=p;
		else
			dnot=p;
	}
}

void re_dat_tet::del_link(DNoteType *&dnot)
{
	DNoteType *t,*temp;

	t=dnot;

	while(t!=NULL)
	{
		temp=t->next;

		delete t;

		t=temp;
	}
}

DNoteType * re_dat_tet::find_pos_link(DNoteType *dnot,int ids)
{
	DNoteType *t;

	t=dnot;

	while((t!=NULL)&&(t->n!=ids))
		t=t->next;

	return t;
}

bool re_dat_tet::check_f(DNoteType *dnot)
{
	if(dnot==NULL)
		return false;

	DNoteType *t;

	t=dnot;

	while(t!=NULL)
	{
		if(t->g==false)
			return false;

		t=t->next;
	}

	return true;
}

DNoteType * re_dat_tet::find_f(DNoteType *dnot)
{
	DNoteType *t;

	t=dnot;

	while((t!=NULL)&&(t->g==true))
		t=t->next;

	return t;
}

void re_dat_tet::find_tet_in_bvolumn(int num)
{
	let_type=VOLUMN_LET;
	vector<int> vec;

	srand(time(NULL));

	for(int i=0;i<num;i++)
	{
		int t=rand()%E;

		float tp[3];

		for(int j=0;j<3;j++)
			tp[j]=0;

		for(int j=0;j<3;j++)
			for(int k=0;k<4;k++)
				tp[j]+=vert_co[3*tet_in_ord[4*t+k]+j];

		for(int j=0;j<3;j++)
			tp[j]=tp[j]/4;

		tp[0]=boud_box[1];

		int tid=find_tet_id_of_point(tp[0],tp[1],tp[2]);

		vec.push_back(tid);
	}

	n_of_btet=vec.size();

	if (in_let_index)
	{
		delete [] in_let_index;
	}
	in_let_index=new int [n_of_btet];

	for(int i=0;i<n_of_btet;i++)
		in_let_index[i]=vec[i];
	vec.clear();
}

void re_dat_tet::find_tet_in_bplain(int num)
{
	let_type=PLAIN_LET;
	//vector<int> vec;

	//int num_p=sqrt((float)num);

	//if(num==0)
	//	return;

	//float tp[3];

	//tp[0]=boud_box[1];

	//for(int i=0;i<num_p;i++)
	//{
	//	tp[1]=boud_box[2]+i*(boud_box[3]-boud_box[2])/num_p+0.0000001;

	//	for(int j=0;j<num_p;j++)
	//	{
	//		tp[2]=boud_box[4]+j*(boud_box[5]-boud_box[4])/num_p;

	//		int tid=find_tet_id_of_point(tp[0],tp[1],tp[2]);

	//		if(tid>=0)
	//			vec.push_back(tid);
	//	}
	//}

	//n_of_btet=vec.size();

	if (in_let_index)
	{
		delete [] in_let_index;
	}

	n_of_btet=n_of_bt;

	in_let_index=new int [n_of_bt];

	//for(int i=0;i<num;i++)
	//{
	//	srand(time(NULL));

	//	int teppy=rand()%n_of_bt;

	//	in_let_index[i]=bt[teppy];
	//}

	for(int i=0;i<n_of_bt;i++)
		in_let_index[i]=bt[i];
	//vec.clear();
}

void re_dat_tet::presure_tex_comp(int size_x,int size_y,int size_z)
{
	t_size[0]=size_x;
	t_size[1]=size_y;
	t_size[2]=size_z;

	int DATA_SIZE=size_x*size_y*size_z;

	if(presure_tex_data!=NULL)
	{
		delete [] presure_tex_data;
		presure_tex_data=NULL;
	}

	presure_tex_data=new float[DATA_SIZE];

	if((size_x<=1)||(size_y<=1)||(size_z<=1))
	{
		cout<<"The texture size need to be lager than 1 !"<<endl;
		exit(-1);
	}

	float dx=(boud_box[1]-boud_box[0])/(size_x-1);
	float dy=(boud_box[3]-boud_box[2])/(size_y-1);
	float dz=(boud_box[5]-boud_box[4])/(size_z-1);

	int xid,yid,zid;
	float x,y,z;

	for(int i=0;i<DATA_SIZE;i++)
		presure_tex_data[i]=-100;

	for(int i=0;i<E;i++)
	{
		float tb[6];

		tb[0]=vert_co[3*tet_in_ord[4*i+0]+0];
		tb[1]=tb[0];
		tb[2]=vert_co[3*tet_in_ord[4*i+0]+1];
		tb[3]=tb[2];
		tb[4]=vert_co[3*tet_in_ord[4*i+0]+2];
		tb[5]=tb[4];

		for(int j=1;j<4;j++)
		{
			if(tb[0]>vert_co[3*tet_in_ord[4*i+j]+0])
				tb[0]=vert_co[3*tet_in_ord[4*i+j]+0];

			if(tb[1]<vert_co[3*tet_in_ord[4*i+j]+0])
				tb[1]=vert_co[3*tet_in_ord[4*i+j]+0];

			if(tb[2]>vert_co[3*tet_in_ord[4*i+j]+1])
				tb[2]=vert_co[3*tet_in_ord[4*i+j]+1];

			if(tb[3]<vert_co[3*tet_in_ord[4*i+j]+1])
				tb[3]=vert_co[3*tet_in_ord[4*i+j]+1];

			if(tb[4]>vert_co[3*tet_in_ord[4*i+j]+2])
				tb[4]=vert_co[3*tet_in_ord[4*i+j]+2];

			if(tb[5]<vert_co[3*tet_in_ord[4*i+j]+2])
				tb[5]=vert_co[3*tet_in_ord[4*i+j]+2];
		}

		int *xid,*yid,*zid;

		int xn,yn,zn;

		xn=(int)((tb[1]-tb[0])/dx)+3;
		yn=(int)((tb[3]-tb[2])/dy)+3;
		zn=(int)((tb[5]-tb[4])/dz)+3;

		xid=new int[xn];
		yid=new int[yn];
		zid=new int[zn];


		int xs,ys,zs;

		xs=(int)((tb[0]-boud_box[0])/dx)-1;
		ys=(int)((tb[2]-boud_box[2])/dy)-1;
		zs=(int)((tb[4]-boud_box[4])/dz)-1;

		for(int j=0;j<xn;j++)
		{
			if(xs<0)
				xs=0;

			if(xs>127)
				xs=127;

			xid[j]=xs++;
		}

		for(int j=0;j<yn;j++)
		{
			if(ys<0)
				ys=0;

			if(ys>127)
				ys=127;

			yid[j]=ys++;
		}

		for(int j=0;j<zn;j++)
		{
			if(zs<0)
				zs=0;

			if(zs>127)
				zs=127;

			zid[j]=zs++;
		}


		float w[4];

		float v0[3];
		float v1[3];
		float v2[3];
		float v3[3];

		for(int k=0;k<3;k++)
		{
			v0[k]=vert_co[3*tet_in_ord[4*i+0]+k];
			v1[k]=vert_co[3*tet_in_ord[4*i+1]+k];
			v2[k]=vert_co[3*tet_in_ord[4*i+2]+k];
			v3[k]=vert_co[3*tet_in_ord[4*i+3]+k];
		}

		float dop;

		dop =(v3[0]-v0[0])*((v1[1]-v0[1])*(v2[2]-v0[2])-(v1[2]-v0[2])*(v2[1]-v0[1]))+
			(v3[1]-v0[1])*((v1[2]-v0[2])*(v2[0]-v0[0])-(v1[0]-v0[0])*(v2[2]-v0[2]))+
			(v3[2]-v0[2])*((v1[0]-v0[0])*(v2[1]-v0[1])-(v1[1]-v0[1])*(v2[0]-v0[0]));


		for(int tx=0;tx<xn;tx++)
		{
			for(int ty=0;ty<yn;ty++)
			{
				for(int tz=0;tz<zn;tz++)
				{		
					if(presure_tex_data[xid[tx]*size_y*size_z+yid[ty]*size_z+zid[tz]]==(-100))
					{
						x=boud_box[0]+xid[tx]*dx;
						y=boud_box[2]+yid[ty]*dy;
						z=boud_box[4]+zid[tz]*dz;

						w[0]=(v3[0]-x)*((v1[1]-y)*(v2[2]-z)-(v1[2]-z)*(v2[1]-y))+
							(v3[1]-y)*((v1[2]-z)*(v2[0]-x)-(v1[0]-x)*(v2[2]-z))+
							(v3[2]-z)*((v1[0]-x)*(v2[1]-y)-(v1[1]-y)*(v2[0]-x));

						w[1]=(v3[0]-v0[0])*((y-v0[1])*(v2[2]-v0[2])-(z-v0[2])*(v2[1]-v0[1]))+
							(v3[1]-v0[1])*((z-v0[2])*(v2[0]-v0[0])-(x-v0[0])*(v2[2]-v0[2]))+
							(v3[2]-v0[2])*((x-v0[0])*(v2[1]-v0[1])-(y-v0[1])*(v2[0]-v0[0]));

						w[2]=(v3[0]-v0[0])*((v1[1]-v0[1])*(z-v0[2])-(v1[2]-v0[2])*(y-v0[1]))+
							(v3[1]-v0[1])*((v1[2]-v0[2])*(x-v0[0])-(v1[0]-v0[0])*(z-v0[2]))+
							(v3[2]-v0[2])*((v1[0]-v0[0])*(y-v0[1])-(v1[1]-v0[1])*(x-v0[0]));

						w[3]=(x-v0[0])*((v1[1]-v0[1])*(v2[2]-v0[2])-(v1[2]-v0[2])*(v2[1]-v0[1]))+
							(y-v0[1])*((v1[2]-v0[2])*(v2[0]-v0[0])-(v1[0]-v0[0])*(v2[2]-v0[2]))+
							(z-v0[2])*((v1[0]-v0[0])*(v2[1]-v0[1])-(v1[1]-v0[1])*(v2[0]-v0[0]));

						if((w[0]>=0)&&(w[1]>=0)&&(w[2]>=0)&&(w[3]>=0))
						{
							if(dop==0)
							{
								presure_tex_data[xid[tx]*size_y*size_z+yid[ty]*size_z+zid[tz]]=(p[tet_in_ord[4*i+0]]+p[tet_in_ord[4*i+1]]+p[tet_in_ord[4*i+2]]+p[tet_in_ord[4*i+3]])/4;
							}
							else
							{
								presure_tex_data[xid[tx]*size_y*size_z+yid[ty]*size_z+zid[tz]]=w[0]*p[tet_in_ord[4*i+0]]/dop+
									w[1]*p[tet_in_ord[4*i+1]]/dop+
									w[2]*p[tet_in_ord[4*i+2]]/dop+
									w[3]*p[tet_in_ord[4*i+3]]/dop;

							}
						}
					}
				}
			}
		}

		delete [] xid;
		delete [] yid;
		delete [] zid;
	}
}

void re_dat_tet::tex_ing(int *size_xyz,float pmin0, float pmax0,bool whi)
{
	int DATA_SIZE=size_xyz[0]*size_xyz[1]*size_xyz[2];

	if(p_tex_int!=NULL)
		delete [] p_tex_int;
	p_tex_int=NULL;

	p_tex_int=new int[DATA_SIZE];

	float pmin,pmax;

	if(whi)
	{
		pmin=pmin0;
		pmax=pmax0;
	}
	else
	{
		pmin=pres_min_max[0];
		pmax=pres_min_max[1];
	}

	for(int i=0;i<DATA_SIZE;i++)
	{
		if(presure_tex_data[i]==(-100))
		{
			p_tex_int[i]=-1;
		}
		else if(presure_tex_data[i]<pmin)
		{
			p_tex_int[i]=0;
		}
		else if(presure_tex_data[i]>pmax)
		{
			p_tex_int[i]=255;
		}
		else
		{
			p_tex_int[i]=(int)(255*(presure_tex_data[i]-pmin)/(pmax-pmin));
		}
	}
}

void re_dat_tet::tex_3d(int *size_xyz,float *pminmax,bool whi)
{
	presure_tex_comp(size_xyz[0],size_xyz[1],size_xyz[2]);
	tex_ing(size_xyz, pminmax[0],pminmax[1], whi);
}