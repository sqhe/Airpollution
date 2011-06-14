// matrix.h.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOLUMERENDERINGSYSTEM_H__D330A049_A82C_4E59_8D49_7502EB9887B0__INCLUDED_)
#define AFX_VOLUMERENDERINGSYSTEM_H__D330A049_A82C_4E59_8D49_7502EB9887B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
  
template <class S,class T> struct MATRIX
{
//public:
//	MATRIX();
//	virtual ~MATRIX();
//有关矩阵的函数，其中Nagative是矩阵求逆的函数
		static void Multiple(T tResult[2], T m[2][2], T tMul[2])
		{
			tResult[0] = m[0][0] * tMul[0] + m[0][1] * tMul[1];	
			tResult[1] = m[1][0] * tMul[0] + m[1][1] * tMul[1];	
		}
		static void Multiple(T tResult[2], T m[2][2])
		{
			T temp[2] = {tResult[0], tResult[1]};
			Multiple(tResult, m, temp);
		}	
		static void Multiple(S sResult[3][3], S sMul[3][3], T tMul[3][3])
		{
			S temp[3][3];
			int i,j;
			for ( i = 0; i < 3; i ++)
			{
				for ( j = 0; j < 3; j ++)
				{
					temp[i][j] = 0;
					for (int k = 0; k < 3; k ++)
						temp[i][j] += sMul[i][k] * tMul[k][j];
				}
			}
			for ( i = 0; i < 3; i ++)
			{
				for ( j = 0; j < 3; j ++)
				{
					sResult[i][j]=temp[i][j];
				}
			}
		}
		static void Multiple(S v[3], S m[3][3], T tMul[3])
		{
			v[0] = m[0][0] * tMul[0] + m[0][1] * tMul[1] + m[0][2] * tMul[2];
			v[1] = m[1][0] * tMul[0] + m[1][1] * tMul[1] + m[1][2] * tMul[2];
			v[2] = m[2][0] * tMul[0] + m[2][1] * tMul[1] + m[2][2] * tMul[2];
		}
		static void Multiple(S v[3], S sMul[3], T m[3][3])
		{
			v[0] = m[0][0] * sMul[0] + m[0][1] * sMul[1] + m[0][2] * sMul[2];
			v[1] = m[1][0] * sMul[0] + m[1][1] * sMul[1] + m[1][2] * sMul[2];
			v[2] = m[2][0] * sMul[0] + m[2][1] * sMul[1] + m[2][2] * sMul[2];
		}
		static void Multiple(S sResult[3], T m[3][3])
		{	
			S temp[3] = {sResult[0], sResult[1], sResult[2]};
			Multiple(sResult, temp, m);
		}
		static S Multiple(S s[], T t[], int size)
		{
			S sTotle = 0;
			for (int i = 0; i < size; i ++)
				sTotle += s[i] * t[i];
			return sTotle;
		}
		static void Multiple(S s[4], S sMatrix[][4], T t[4])
		{
			s[0] = Multiple(sMatrix[0], t, 4);
			s[1] = Multiple(sMatrix[1], t, 4);
			s[2] = Multiple(sMatrix[2], t, 4);
			s[3] = Multiple(sMatrix[3], t, 4);			
		}
		static void Multiple(S sresult[4][4],S sMatrix[4][4], S tMatrix[4][4])
		{
			S sre[4][4];
			for(int j=0;j<4;j++)
				for(int i=0;i<4;i++)
				{
					sre[i][j]=0;
					for(int k=0;k<4;k++)
						sre[i][j]=sre[i][j]+sMatrix[i][k]*tMatrix[k][j];
				}
			for(j=0;j<4;j++)
				for(int i=0;i<4;i++)
					sresult[i][j]=sre[i][j];
		}
		static void Reverse(T m[3][3])
		{
			T t[3][3];
			int i, j;
			for (i = 0; i < 3; i ++)
				for (j = 0; j < 3; j ++)
					t[i][j] = m[i][j];

			for (i = 0; i < 3; i ++)
				for (j = 0; j < 3; j ++)
					m[i][j] = t[j][i];
		}
		static T Evaluate(T m[2][2])
		{
			return (m[0][0] * m[1][1] - m[1][0] * m[0][1]);
		}
		static T Evaluate(T m[3][3])
		{
			double v = 0.0;
			for (int i = 0; i < 3; i ++)
				v += ((i % 2) == 0 ? 1 : -1) * m[0][i] * Simple(m, 0, i);
			return v;
		}
		static T Evaluate(T m[4][4])
		{
			T v = 0;
			for (int i = 0; i < 4; i ++)
				v += ((i % 2) == 0 ? 1 : -1) * m[0][i] * Simple(m, 0, i);
			return v;
		}
		static T Simple(T m[3][3], int x, int y)
		{
			T t[2][2];
			ZeroMemory(t, sizeof(t));
			T *p = (T *)m;
			T *q = (T *)t; 
			for (int i = 0; i < 3; i ++)
				for (int j = 0; j < 3; j ++)
				{
					if ((i != x) && (j != y))
						*q ++ = *p;
					p ++;
				}
			return Evaluate(t);
		}
		static T Simple(T m[4][4], int x, int y)
		{
			T t[3][3];
			ZeroMemory(t, sizeof(t));
			T *p = (T *)m;
			T *q = (T *)t; 
			for (int i = 0; i < 4; i ++)
				for (int j = 0; j < 4; j ++)
				{
					if ((i != x) && (j != y))
						*q ++ = *p;
					p ++;
				}
			return Evaluate(t);
		}
		static void Nagative(T m[2][2])
		{
			T t[2][2] = { m[0][0], m[0][1], m[1][0], m[1][1] };
			T v = t[0][0] * t[1][1] - t[0][1] * t[1][0];
			m[0][0] = t[1][1] / v;
			m[0][1] = -t[0][1] / v;
			m[1][0] = -t[1][0] / v;
			m[1][1] = t[0][0] / v;
		}	
		static void Nagative(T m[3][3])
		{
			T t[3][3];
			memcpy(t, m, sizeof(t));
			T v = Evaluate(m);
			ASSERT(v != 0);
			for (int i = 0; i < 3; i ++)
				for (int j = 0; j < 3; j ++)
				{
					m[i][j] = Simple(t, j, i) / v;
					if ((i + j) % 2 == 1)
						m[i][j] = -m[i][j];
				}
		}		
		static void Nagative(T m[4][4])
		{
			T t[4][4];
			memcpy(t, m, sizeof(t));
			T v = Evaluate(m);
			ASSERT(v != 0);
			for (int i = 0; i < 4; i ++)
				for (int j = 0; j < 4; j ++)
				{
					m[i][j] = Simple(t, j, i) / v;
					if ((i + j) % 2 == 1)
						m[i][j] = -m[i][j];
				}
		}
        static void Resolve(T reso[2],T co[2][2],T cons[2])
		{
			T delt[2][2];
			if(Evaluate(co)!=0)
			{
                delt[0][0]=cons[0][0];
				delt[1][0]=cons[1][0];
                delt[0][1]=co[0][1];
				delt[1][1]=co[1][1];
			    reso[0]=Ealuate(delt)/Evaluate(co);
                delt[0][0]=co[0][0];
				delt[1][0]=co[1][0];
                delt[0][1]=cons[0][1];
				delt[1][1]=cons[1][1];
			    reso[1]=Ealuate(delt)/Evaluate(co);
			}
			return;
		}
        static void Resolve(T reso[3],T co[3][3],T cons[3])
		{
			T delt[3][3];
			if(Evaluate(co)!=0)
			{
				for(int i=0; i<3; i++)
				{
					for(int j=0; j<3; j++)
						for(int k=0; k<3; k++)
						{
							if(k==i)
								delt[j][k]=cons[j];
							else
								delt[j][k]=co[j][k];
						}
				    reso[i]=Evaluate(delt)/Evaluate(co);
				}
			}
			return;
		}
	};

#endif // !defined(AFX_VOLUMERENDERINGSYSTEM_H__D330A049_A82C_4E59_8D49_7502EB9887B0__INCLUDED_)
