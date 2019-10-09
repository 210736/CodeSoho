#include "stdafx.h"
#include "math.h"

namespace ECore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 矩阵和四元数


	void MatrixPerspectiveFovLH_INF(Matrix4 *pOut,float fov,float aspect, float znear)
	{
		memset(pOut,0,sizeof(Matrix4));
		float h=1.0f/tanf(fov*0.5f);
		float w=h*aspect;

		pOut->_11=w;
		pOut->_22=h;
		pOut->_33=1.0f;
		pOut->_34=1.0f;
		pOut->_43=-znear;
	}

	void GetTranAndQuatFromMatrix4(const Matrix4& mat, Vector3& tran, Quaternion& quat)
	{
		// transformation
		tran.x = mat.m[3][0];
		tran.y = mat.m[3][1];
		tran.z = mat.m[3][2];

		//const MeReal *const t = (MeReal *) tm;
		float	s;

		// Check diagonal (trace)
		const float tr = mat.m[0][0] + mat.m[1][1] + mat.m[2][2];

		if( tr > 0.0f )
		{
			float InvS = 1.0f / FastSqrt(tr+1.0f);
			quat.w = 0.5f * (1.f / InvS);
			s = 0.5f * InvS;

			quat.x = (mat.m[1][2] - mat.m[2][1]) * s;
			quat.y = (mat.m[2][0] - mat.m[0][2]) * s;
			quat.z = (mat.m[0][1] - mat.m[1][0]) * s;
		} 
		else 
		{
			// diagonal is negative
			int i = 0;

			if(mat.m[1][1] > mat.m[0][0])
				i = 1;

			if (mat.m[2][2] > mat.m[i][i])
				i = 2;

			static const int nxt[3] = { 1, 2, 0 };
			const int j = nxt[i];
			const int k = nxt[j];

			s = mat.m[i][i] - mat.m[j][j] - mat.m[k][k] + 1.0f;

			float InvS = 1.0f / FastSqrt(s);

			float qt[4];
			qt[i] = 0.5f * (1.0f / InvS);

			s = 0.5f * InvS;

			qt[3] = (mat.m[j][k] - mat.m[k][j]) * s;
			qt[j] = (mat.m[i][j] + mat.m[j][i]) * s;
			qt[k] = (mat.m[i][k] + mat.m[k][i]) * s;

			quat.x = qt[0];
			quat.y = qt[1];
			quat.z = qt[2];
			quat.w = qt[3];
		}
	}

	void GetScaleFromMatrix4( Vector3* pScale, const Matrix4* pMat )
	{
		ASSERT( pScale && pMat );

		pScale->x = FastSqrt( pMat->_11 * pMat->_11 + pMat->_21 * pMat->_21 + pMat->_31 * pMat->_31 );
		pScale->y = FastSqrt( pMat->_12 * pMat->_12 + pMat->_22 * pMat->_22 + pMat->_32 * pMat->_32 );
		pScale->z = FastSqrt( pMat->_13 * pMat->_13 + pMat->_23 * pMat->_23 + pMat->_33 * pMat->_33 );
	}

	void GetEulerAngFromQuatO2I(const Quaternion&q, float* yaw, float* pitch, float* roll)
	{
		float sp = -2.0f * (q.y*q.z - q.w*q.x);

		// Check for Gimbal lock, giving slight tolerance for numerical imprecision
		if (fabs(sp) > 0.9999f)
		{
			// Looking straight up or down
			*pitch = 1.570796f * sp;	// pi/2

			// Compute heading, slam bank to zero
			*yaw = atan2(-q.x*q.z + q.w*q.y, 0.5f - q.y*q.y - q.z*q.z);
			*roll = 0.0f;
		}
		else
		{
			// Compute angles
			*pitch = asin(sp);
			*yaw = atan2(q.x*q.z + q.w*q.y, 0.5f - q.x*q.x - q.y*q.y);
			*roll = atan2(q.x*q.y + q.w*q.z, 0.5f - q.x*q.x - q.z*q.z);
		}
	}

	void GetEulerAngFromQuatI2O(const Quaternion&q, float* yaw, float* pitch, float* roll)
	{
		float sp = -2.0f * (q.y*q.z + q.w*q.x);

		// Check for Gimbal lock, giving slight tolerance for numerical imprecision
		if (fabs(sp) > 0.9999f)
		{
			// Looking straight up or down
			*pitch = 1.570796f * sp;	// pi/2

			// Compute heading, slam bank to zero
			*yaw = atan2(-q.x*q.z - q.w*q.y, 0.5f - q.y*q.y - q.z*q.z);
			*roll = 0.0f;
		}
		else
		{
			// Compute angles
			*pitch = asin(sp);
			*yaw = atan2(q.x*q.z - q.w*q.y, 0.5f - q.x*q.x - q.y*q.y);
			*roll = atan2(q.x*q.y - q.w*q.z, 0.5f - q.x*q.x - q.z*q.z);
		}
	}


	Quaternion RotateObjectToInertial(float yaw, float pitch, float roll)
	{
		Quaternion q;
		float sPitch, sRoll, sYaw;
		float cPitch, cRoll, cYaw;

		sinCos(&sPitch, &cPitch, pitch*0.5f);
		sinCos(&sRoll, &cRoll, roll*0.5f);
		sinCos(&sYaw, &cYaw, yaw*0.5f);

		// Compute values
		q.w = cYaw*cPitch*cRoll + sYaw*sPitch*sRoll;
		q.x = cYaw*sPitch*cRoll + sYaw*cPitch*sRoll;
		q.y = -cYaw*sPitch*sRoll + sYaw*cPitch*cRoll;
		q.z = -sYaw*sPitch*cRoll + cYaw*cPitch*sRoll;

		return q;
	}

	Quaternion RotateInertialToObject(float yaw, float pitch, float roll)
	{
		Quaternion q;
		float sPitch, sRoll, sYaw;
		float cPitch, cRoll, cYaw;

		sinCos(&sPitch, &cPitch, pitch*0.5f);
		sinCos(&sRoll, &cRoll, roll*0.5f);
		sinCos(&sYaw, &cYaw, yaw*0.5f);

		// Compute values
		q.w = cYaw*cPitch*cRoll + sYaw*sPitch*sRoll;
		q.x = -cYaw*sPitch*cRoll - sYaw*cPitch*sRoll;
		q.y = cYaw*sPitch*sRoll - sYaw*cPitch*cRoll;
		q.z = sYaw*sPitch*cRoll - cYaw*cPitch*sRoll;

		return q;
	}

	Quaternion GetRotationTo(const Vector3& from, const Vector3& to)
	{
		Quaternion q;
		//* NOTE: assumes normalized input
		Vector3 v1 = from;
		D3DXVec3Normalize(&v1, &v1);

		Vector3 v2 = to;
		D3DXVec3Normalize(&v2, &v2);

		float tx,ty,tz,temp,dist;
		float cost,len,ss;

		// get dot product of two vectors
		cost=v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;

		// check if parallel
		if (cost > 0.999999f)
		{
			D3DXQuaternionIdentity(&q);
			return q;
		}
		else if (cost < -0.999999f)
		{
			// check if opposite

			// check if we can use cross product of from vector with [1, 0, 0]
			tx=0.0f;
			ty=v1[0];
			tz= -v1[1];

			len=FastSqrt(ty*ty + tz*tz);

			if (len < 1e-6f)
			{
				// no, we need cross product of from vector with [0, 1, 0]
				tx= -v1[2];
				ty=0.0f;
				tz=v1[0];
			}

			// normalize
			temp=tx*tx + ty*ty + tz*tz;

			dist=(float)(1.0f/FastSqrt(temp));

			tx*=dist;
			ty*=dist;
			tz*=dist;

			q.w = 0.0f;
			q.x = tx;
			q.y = ty;
			q.z = tz;

			return q;
		}

		// else just cross two vectors

		tx=v1[1]*v2[2] - v1[2]*v2[1];
		ty=v1[2]*v2[0] - v1[0]*v2[2];
		tz=v1[0]*v2[1] - v1[1]*v2[0];

		temp=tx*tx + ty*ty + tz*tz;
		dist=1.0f/FastSqrt(temp);

		tx*=dist;
		ty*=dist;
		tz*=dist;

		// use half-angle formula (sin^2 t=( 1 - cos(2t) )/2)

		ss=FastSqrt(0.5f * (1.0f - cost));

		tx*=ss;
		ty*=ss;
		tz*=ss;

		// cos^2 t=( 1 + cos (2t) ) / 2
		// w part is cosine of half the rotation angle
		q.w = FastSqrt(0.5f * (1.0f + cost));
		q.x = tx;
		q.y = ty;
		q.z = tz;

		return q;
	}

	void SetPrjZScaleOffset(Matrix4& matPrj,float zScale,float zOffset)
	{
		matPrj._33=matPrj._33*zScale+zOffset;
		matPrj._43=matPrj._43*zScale;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Float相关


	INLINE void sinCos(float *returnSin, float *returnCos, float theta)
	{
		// For simplicity, we'll just use the normal trig functions
		// Note that on some platforms we may be able to do better

		*returnSin = sin(theta);
		*returnCos = cos(theta);
	}


}//namespace ECore