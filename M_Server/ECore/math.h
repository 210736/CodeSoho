#pragma once
#include "fast_code.h"
#include "stdafx.h"
namespace ECore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ������ѧ����


	#ifndef EMax
	#define EMax(a,b)            (((a) > (b)) ? (a) : (b))
	#endif

	#ifndef EMin
	#define EMin(a,b)            (((a) < (b)) ? (a) : (b))
	#endif

	#ifndef EAbs
	#define EAbs(a)		         ( (a)>=0 ? (a) : -(a) )
	#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// float���


	/** ��鸡���Ƿ���Ч������OPCODE��ײ����
	*/
	INLINE bool IsNAN(float value)				{ return (IR(value)&0x7f800000) == 0x7f800000;	}
	INLINE bool IsIndeterminate(float value)	{ return IR(value) == 0xffc00000;				}
	INLINE bool IsPlusInf(float value)			{ return IR(value) == 0x7f800000;				}
	INLINE bool IsMinusInf(float value)			{ return IR(value) == 0xff800000;				}

	INLINE bool IsValidFloat(float value)
	{
		if(IsNAN(value))			return false;
		if(IsIndeterminate(value))	return false;
		if(IsPlusInf(value))		return false;
		if(IsMinusInf(value))		return false;
		return true;
	}

#ifndef X64
	/** ��floatֵ����int����ʱ������������������ڲ�����_ftol()���ή������
		ʹ����Ƕ���ָ���FPUָ��
	*/
	INLINE int ftol_ambient(float f) 
	{
		int i;

		__asm 
		{
			fld f;
			fistp i;
		}
		return i;
	}
#endif

#ifndef X64
	/** ʹ��SSE���ٵĳ���float���Բ�ֵ
	*/
	INLINE void LinearLerp_SSE(float *pOut, const float *pV1, const float *pV2, float s, int count)
	{
		__declspec(align(16)) float factor[4]={s,s,s,s}, test1;
		float *pFactor=factor;
		int subCount=count%12;
		int mainCount=count-subCount;
		__asm
		{
			mov		ecx,	pFactor
			movaps	xmm0,	xmmword ptr [ecx]
			mov		ecx,	mainCount
			mov		edx,	pV1
			lea		edx,	[edx+ecx*4]
			mov		esi,	pV2
			lea		esi,	[esi+ecx*4]
			mov		edi,	pOut
			lea		edi,	[edi+ecx*4]
			neg		ecx
StartLoop:	// һ�δ���12�������ȸ���Ĳ�ֵ
			movups	xmm1,	xmmword ptr [edx+ecx*4]
			movups	xmm2,	xmmword ptr [edx+ecx*4+16]
			movups	xmm3,	xmmword ptr [edx+ecx*4+16*2]
			movups	xmm4,	xmmword ptr [esi+ecx*4]
			movups	xmm5,	xmmword ptr [esi+ecx*4+16]
			movups	xmm6,	xmmword ptr [esi+ecx*4+16*2]
			subps	xmm4,	xmm1
			subps	xmm5,	xmm2
			subps	xmm6,	xmm3
			mulps	xmm4,	xmm0
			mulps	xmm5,	xmm0
			mulps	xmm6,	xmm0
			addps	xmm1,	xmm4
			addps	xmm2,	xmm5
			addps	xmm3,	xmm6
			movaps	test1,	xmm1
			movups	xmmword ptr [edi+ecx*4],		xmm1
			movups	xmmword ptr [edi+ecx*4+16],		xmm2
			movups	xmmword ptr [edi+ecx*4+16*2],	xmm3
			add     ecx,	12

			jnz     StartLoop
		}

		// ����ʣ�µ�����0~11
		for (int i=0; i<subCount; i++)
			pOut[mainCount+i]=pV1[mainCount+i]+s*(pV2[mainCount+i]-pV1[mainCount+i]);
	}
#endif

	/** Float���Բ�ֵ
	*/
	INLINE float LinearLerp(float a,float b,float x)
	{
		return (a*(1-x)+b*x);
	}

	/** Float���Ҳ�ֵ
	*/
	INLINE float CosLerp(float a,float b,float x)
	{
		x=(1-cos(x*3.1415927f))*0.5f;
		return (a*(1-x)+b*x);
	}

	/** ����sin��cos
	*/
	void sinCos(float *returnSin, float *returnCos, float theta);

	/** ����ƽ��
	*/
	INLINE float SquareF(float f)
	{
		return f*f;
	}

	/** ����0�ж�
	*/
	INLINE bool IsZeroF(float f)
	{
		return fabsf(f)<FLOAT_EPSILON;
	}

	/** ����������ж�
	*/
	INLINE bool IsEqualF(float f1,float f2)
	{
		return fabsf(f1-f2)<FLOAT_EPSILON;
	}

	/** �ض�Ϊ0~1
	*/
	INLINE float Saturate( float f )
	{
		if( f < 0.0f )
			f = 0.0f;
		if( f > 1.0f )
			f = 1.0f;
		return f;
	}

	/** �ض�
	*/
	INLINE float Clamp( float f, float min, float max )
	{
		if( f < min ) f = min;
		if( f > max ) f = max;
		return f;
	}

	/** ����0~1
	*/
	INLINE FLOAT SmoothStep( float f, float min, float max )
	{
		return ( Clamp( f, min, max ) - min ) / ( max - min );
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �������



	/** Test to see if a number is an exact power of two (from Steve Baker's Cute Code Collection) 
	*/
	INLINE bool IsPowerOfTwo(unsigned int n)
	{ return ((n&(n-1))==0); }

#ifndef X64
	/** Returns the least power of 2 greater than or equal to "x".
	Note that for x=0 and for x>2147483648 this returns 0!	*/
	INLINE __declspec(naked) unsigned __fastcall CeilPowerOf2(unsigned x) 
	{
		__asm {
			xor eax,eax
				dec ecx
				bsr ecx,ecx
				cmovz ecx,eax
				setnz al
				inc eax
				shl eax,cl
				ret
		}
	}

	/** Returns the greatest power of 2 less than or equal to "x".
	Note that for x=0 this returns 0!*/
	INLINE __declspec(naked) unsigned __fastcall FloorPowerOf2(unsigned x) 
	{
		__asm {
			xor eax,eax
				bsr ecx,ecx
				setnz al
				shl eax,cl
				ret
		}
	}
#endif

	/** �ض�
	*/
	INLINE int Clamp( int n, int min, int max )
	{
		if( n < min ) n = min;
		if( n > max ) n = max;
		return n;
	}




	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �����


	/**	����һ�������0~1.0֮��ĸ�����
	*/
	INLINE float RandUnit()
	{
		return (float)rand()/RAND_MAX;
	}
	/** ���������
	*/
	INLINE int Rand(int nMin, int nMax)
	{
		if(nMax > nMin)
		{
			int nDiff = nMax - nMin + 1;
			return nMin + (rand( ) % nDiff);
		}
		else if(nMax < nMin)
		{
			int nDiff = nMin - nMax + 1;
			return nMax + (rand( ) % nDiff);
		}
		else
		{
			return nMin;
		}
	}
	/** ����Float�����
	*/
	INLINE float FRand(float fMin, float fMax)
	{
		float fUnit = float(rand( )) / RAND_MAX;
		float fDiff = fMax - fMin;
		return fMin + fUnit * fDiff;
	}
	/** ����Float�����
	*/
	INLINE float FRand()
	{
		return float(rand( )) / RAND_MAX;
	}


	//-----------------------------------------------------------------------------
	// �õ��������	code from GCC
	//-----------------------------------------------------------------------------
	INLINE INT BigRand()
	{
		static INT nRandHolder=(INT)GetTickCount();
		return (((nRandHolder = nRandHolder * 1103515245 + 12345) >> 1 ) & LONG_MAX);
	}
	// ���������
	INLINE int BigRand(int nMin, int nMax) { return nMin + (BigRand( ) % (nMax - nMin + 1)); }
	// ����Float�����
	INLINE float FBigRand(float fMin, float fMax) {	return fMin + (float(BigRand( )) / LONG_MAX) * (fMax - fMin); }
	// ����Float�����
	INLINE float FBigRand() { return float(BigRand( )) / LONG_MAX; }


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DWORD --> Vector3
	// Vector3 --> DWORD

	INLINE void DWORD2Vec( DWORD val, Vector3& out )
	{
		// 2 / 255 = 0.00784313772
		out.x = (val & 0x000000FF ) * 0.00784313772f - 1;
		out.y = ((val >> 8) & 0x000000FF) * 0.00784313772f - 1;
		out.z = ((val >> 16) & 0x000000FF) * 0.00784313772f - 1;
	}

	INLINE Vector3 DWORD2Vec( DWORD val )
	{
		Vector3 out; DWORD2Vec(val,out);
		return out;
	}

	INLINE DWORD Vec2DWORD( const Vector3& val )
	{
		DWORD out = 0;
		out |= (BYTE)((val.x+1)*127.5f);
		out |= (BYTE)((val.y+1)*127.5f) << 8;
		out |= (BYTE)((val.z+1)*127.5f) << 16;

		return out;
	}

	INLINE DWORD Vec2DWORD( const Vector4& val )
	{
		DWORD out = 0;
		out |= (BYTE)(val.x*255);
		out |= (BYTE)(val.y*255) << 8;
		out |= (BYTE)(val.z*255) << 16;
		out |= (BYTE)(val.w*255) << 24;

		return out;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Vector���


	/** ���Vector3�Ƿ���Ч
	*/
	INLINE bool IsValidVec3(const Vector3& v)
	{
		return IsValidFloat(v.x)
			&& IsValidFloat(v.y)
			&& IsValidFloat(v.z);
	}

	/** Vector���Բ�ֵ
	*/
	INLINE void LinearLerp(const Vector3& a,const Vector3& b,float x,Vector3& out)
	{
		out.x=(a.x*(1-x)+b.x*x);
		out.y=(a.y*(1-x)+b.y*x);
		out.z=(a.z*(1-x)+b.z*x);
	}

	INLINE void LinearLerp(const Vector2& a,const Vector2& b,float x,Vector2& out)
	{
		out.x=(a.x*(1-x)+b.x*x);
		out.y=(a.y*(1-x)+b.y*x);
	}

	INLINE void LinearLerp(const DWORD& a, const DWORD& b, float x, DWORD& out)
	{
		Vector3 vec3a = DWORD2Vec(a);
		Vector3 vec3b = DWORD2Vec(b);

		Vector3 vec3out;
		LinearLerp( vec3a, vec3b, x, vec3out );
		out = Vec2DWORD(vec3out);
	}

	/** ret=v*M,��D3D�Զ���ı任�Ĺ�ʽ */
	INLINE Vector3 operator * (const Vector3& v,const Matrix4& m)
	{
		return Vector3(v.x*m._11+v.y*m._21+v.z*m._31+m._41,
			v.x*m._12+v.y*m._22+v.z*m._32+m._42,
			v.x*m._13+v.y*m._23+v.z*m._33+m._43);
	}

	/** ��������Vector3�ľ���
	*/
	INLINE float Vec3Dist(const Vector3& p1,const Vector3& p2)
	{
		Vector3 t( 
			p1.x - p2.x, 
			p1.y - p2.y, 
			p1.z - p2.z );
		return D3DXVec3Length(&t);
	}

	/** ��������Vector3��2D����
	*/
	INLINE float Vec3Dist2D(const Vector3& p1,const Vector3& p2)
	{
		Vector2 t( 
			p1.x - p2.x, 
			p1.z - p2.z );
		return D3DXVec2Length(&t);
	}

	/** ��������Vector3�ľ���ƽ����û�п�������
	*/
	INLINE float Vec3DistSq(const Vector3& p1,const Vector3& p2)
	{
		return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y) + (p1.z - p2.z)*(p1.z - p2.z);
	}

	/** ��������Vector3��2D����ƽ����û�п�������
	*/
	INLINE float Vec3DistSq2D(const Vector3& p1,const Vector3& p2)
	{
		return (p1.x - p2.x)*(p1.x - p2.x) + (p1.z - p2.z)*(p1.z - p2.z);
	}

	/** ��������Vector3��2D���룬ʹ�ÿ��ٿ���
	*/
	INLINE FLOAT Vec3Dist2DFast(const Vector3& p1, const Vector3& p2)
	{
		return FastSqrt( Vec3DistSq2D( p1, p2 ) );
	}

	/** ����Vector3�ĳ���
	*/
	#define  Vec3Len	D3DXVec3Length

	/** ����Vector3�ĳ���ƽ��
	*/
	#define  Vec3LenSq	D3DXVec3LengthSq

	/** Vector3��һ��
	*/
	INLINE void Vec3Normalize(Vector3& v)
	{
		D3DXVec3Normalize(&v,&v);
	}
	
	/** Vector3�ĵ��
	*/
	INLINE float Vector3Dot(const Vector3& p1,const Vector3& p2)
	{
		return D3DXVec3Dot(&p1, &p2);
	}

	/** Vector3�Ĳ��
	*/
	INLINE Vector3 Vector3Cross(const Vector3& p1,const Vector3& p2)
	{
		Vector3 result;
		D3DXVec3Cross(&result,&p1,&p2);

		return result;
	}

	/** Vector3���������
	*/
	INLINE Vector3 Vector3Mul(const Vector3& p1,const Vector3& p2)
	{
		Vector3 result;

		result.x = p1.x * p2.x;
		result.y = p1.y * p2.y;
		result.z = p1.z * p2.z;

		return result;
	}

	/** ����Vector3��2D���
	*/
	INLINE FLOAT Vec3Dot2(const Vector3& p1, const Vector3& p2)
	{
		return p1.x * p2.x + p1.z * p2.z;
	}

	/** ����Vector3�ļ���
	*/
	INLINE VOID Vec3Sub(const Vector3& p1, const Vector3& p2, Vector3& out)
	{
		out.x = p1.x - p2.x;
		out.y = p1.y - p2.y;
		out.z = p1.z - p2.z;
	}

	/** ����Vector3��2D����
	*/
	INLINE VOID Vec3Sub2(const Vector3& p1, const Vector3& p2, Vector3& out)
	{
		out.x = p1.x - p2.x;
		out.z = p1.z - p2.z;
	}

	/** ����Vector3��2D�Ƚ�
	*/
	INLINE BOOL Vec3Equal2(const Vector3& p1, const Vector3& p2)
	{
		return p1.x == p2.x && p1.z == p2.z;
	}

	/** ����Vector3��2DͶӰ
	*/
	INLINE FLOAT Vec3Perp2D(const Vector3& u, const Vector3& v)
	{
		return u.z*v.x - u.x*v.z;
	}

	/** ����Vector2�ĳ���
	*/
	#define Vec2Len		D3DXVec2Length

	/** ����Vector2�ĳ���ƽ��
	*/
	#define  Vec2LenSq	D3DXVec2LengthSq

	/** Vector2��һ��
	*/
	INLINE void Vec2Normalize(Vector2& v)
	{
		D3DXVec2Normalize(&v,&v);
	}

	/** Vector2��ת
	*/
	INLINE void Vec2Rotate( Vector2& v, float fAngle )
	{
		float fSin = sinf( fAngle );
		float fCos = cosf( fAngle );

		v.x = v.x * fCos - v.x * fSin;
		v.y = v.y * fSin + v.y * fCos;
	}

	/**	����һ������ĵ�λ����
	*/
	INLINE Vector3 RandUnitVec3()
	{
		Vector3 tmp((float)rand()/(RAND_MAX/2.0f)-1.0f,
			(float)rand()/(RAND_MAX/2.0f)-1.0f,
			(float)rand()/(RAND_MAX/2.0f)-1.0f);
		Vec3Normalize(tmp);
		return tmp;
	}
	
	/** ����2�������ļн�
	*/
	INLINE float CalcAngle(Vector3 v1, Vector3 v2)
	{
		D3DXVec3Normalize(&v1, &v1);
		D3DXVec3Normalize(&v2, &v2);
		return acosf( D3DXVec3Dot(&v1, &v2) );
	}

	INLINE float CalcAngle(Vector2 v1, Vector2 v2)
	{
		D3DXVec2Normalize(&v1, &v1);
		D3DXVec2Normalize(&v2, &v2);
		return acosf( D3DXVec2Dot(&v1, &v2) );
	}

	/** ���
	*/
	#define  Vec3Dot D3DXVec3Dot
	#define  Vec2Dot D3DXVec2Dot

	/** ���
	*/
	#define  Vec3Cross D3DXVec3Cross

	/** ��������
	*/
	INLINE BOOL ComputeTangent(Vector3 &t,
		const Vector3 &p0, const Vector2 &c0,
		const Vector3 &p1, const Vector2 &c1,
		const Vector3 &p2, const Vector2 &c2)
	{
		const FLOAT epsilon = 0.0001f;
		BOOL	Ret = FALSE;
		Vector3 dp1 = p1 - p0;
		Vector3 dp2 = p2 - p0;
		FLOAT   du1 = c1.x - c0.x;
		FLOAT   dv1 = c1.y - c0.y;
		if(fabsf(dv1) < epsilon && fabsf(du1) >= epsilon)
		{
			t = dp1 / du1;
			Ret = TRUE;
		}
		else
		{
			FLOAT du2 = c2.x - c0.x;
			FLOAT dv2 = c2.y - c0.y;
			FLOAT det = dv1*du2 - dv2*du1;
			if(fabsf(det) >= epsilon)
			{
				t = (dp2*dv1-dp1*dv2)/det;
				Ret = TRUE;
			}
		}
		return Ret;
	}

	/** ��������Vector3��2D�е�
	*/
	INLINE VOID Vec3MidPt2D(const Vector3& v1, const Vector3& v2, Vector3& out)
	{
		out.x = ( v1.x + v2.x ) * 0.5f;
		out.z = ( v1.z + v2.z ) * 0.5f;
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Yaw - Roll - Pitch



	/** 
	 * ����3D�ռ��Yaw
	 */
	INLINE float CalcYaw(Vector3 dir)
	{
		dir.y = 0.f;
		Vec3Normalize(dir);

		// ����ת���غ���
		if( fabs(dir.x) < FLOAT_EPSILON 
			&& fabs(dir.z) < FLOAT_EPSILON )
			return 0;

		const Vector3 negZ(0.f, 0.f, -1.f);
		float yaw = D3DXVec3Dot(&negZ, &dir);

		if( yaw > 1.0f )
			yaw = 1.0f;
		if( yaw < -1.0f )
			yaw = -1.0f;

		yaw = acosf(yaw);
		if( dir.x > 0.0f )
		{
			yaw = FLOAT_PI * 2.f - yaw;
		}

		return yaw;
	}

	/** 
	 * ����2D�ռ��Yaw
	 */
	INLINE float CalcYaw(Vector2 dir)
	{
		D3DXVec2Normalize(&dir, &dir);

		const Vector2 negZ(0.f, -1.f);
		float yaw = D3DXVec2Dot(&negZ, &dir);

		if(yaw > 1.0f)
			yaw = 1.0f;
		if(yaw < -1.0f)
			yaw = -1.0f;

		yaw = acosf(yaw);
		if( dir.x > 0.0f )
		{
			yaw = FLOAT_PI * 2 - yaw;
		}

		return yaw;
	}

	/** 
	 * ����Yaw���㷽������
	 */
	INLINE Vector3 Yaw2Vec3(float fYaw)
	{
		float fSin = sinf( fYaw );
		float fCos = cosf( fYaw );

		return Vector3( -fSin, 0.f, -fCos );
	}

	INLINE void Yaw2Vec3(float fYaw, Vector3& v)
	{
		float fSin = sinf( fYaw );
		float fCos = cosf( fYaw );

		v.x = -fSin;
		v.y = 0.f;
		v.z = -fCos;
	}

	/** 
	 * ����Yaw,Pitch���㷽������
	 */
	INLINE Vector3 YawPitch2Vec3(float fYaw, float fPitch)
	{
		float fSinYaw = sinf( fYaw );
		float fCosYaw = cosf( fYaw );
		float fSinPitch = sinf( fPitch );

		return Vector3( -fSinYaw, fSinPitch, -fCosYaw );
	}

	INLINE Vector3 YawPitch2Vec3(float fYaw, float fPitch, Vector3& v)
	{
		float fSinYaw = sinf( fYaw );
		float fCosYaw = cosf( fYaw );
		float fSinPitch = sinf( fPitch );
		
		v.x = -fSinYaw;
		v.y = fSinPitch;
		v.z = -fCosYaw;
	}


	/** 
	 * ���Ƕȹ�һ��Ϊ0~FLOAT_PI*2
	 */
	INLINE void AngleNormalize(float& angle)
	{
		int n = int( angle / ( FLOAT_PI * 2 ) );
		if( angle >= 0 )
			angle -= n * FLOAT_PI * 2;
		else
			angle += ( -n + 1 ) * FLOAT_PI * 2;
	}

	/** 
	 * ����Pitch
	 */
	INLINE float CalcPitch(Vector3 dir)
	{
		Vec3Normalize(dir);
		Vector3 forward = dir;
		forward.y = 0.0f;
		Vec3Normalize(forward);
		float pitch = D3DXVec3Dot(&forward, &dir);

		if( pitch > 1.0f )
			pitch = 1.0f;
		if( pitch < -1.0f )
			pitch = -1.0f;

		pitch = acosf(pitch);
		if( dir.y < 0.0f )
		{
			pitch = -pitch;
		}

		return pitch;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �������Ԫ��



	/** ��4*4����õ�λ�ƺ���ת��Ԫ��
	*/
	ECORE_API void GetTranAndQuatFromMatrix4(const Matrix4& mat, Vector3& tran, Quaternion& quat);

	/** ��4*4����õ�����
	*/
	ECORE_API INLINE void GetScaleFromMatrix4( Vector3* pScale, const Matrix4* pMat );

	/** ����Ԫ���õ�ŷ����
	*/
	ECORE_API void GetEulerAngFromQuatO2I(const Quaternion&q, float* yaw, float* pitch, float* roll);
	ECORE_API void GetEulerAngFromQuatI2O(const Quaternion&q, float* yaw, float* pitch, float* roll);

	/** ��ŷ���Ǽ�����Ԫ��
	*/
	ECORE_API Quaternion RotateObjectToInertial(float yaw, float pitch, float roll);
	ECORE_API Quaternion RotateInertialToObject(float yaw, float pitch, float roll);

	/** ������������֮�����Ԫ��
	*/
	Quaternion GetRotationTo(const Vector3& from, const Vector3& to);

	/** ����һ��ZFarΪ����Զ��͸�Ӿ���
	*/
	void MatrixPerspectiveFovLH_INF(Matrix4 *pOut,float fov,float aspect, float znear);

	/** �޸�͸�Ӿ����Զ�zֵ�������ź�ƫ��
	*/
	void SetPrjZScaleOffset(Matrix4& matPrj,float zScale,float zOffset);

	/** ����View�ռ�zֵ��ֵ����zƫ��ֵ
	*/
	INLINE float CalZBias( float z, float zNear, float zFar, float nearBias, float farBias )
	{
		float x = ( z - zNear ) / ( zFar - zNear );
		if( x < 0.0f ) x = 0.0f;
		if( x > 1.0f ) x = 1.0f;
		return LinearLerp( nearBias, farBias, x );
	}

	/** ��Ԫ����һ��
	*/
	INLINE BOOL QuatNormalize(Quaternion& quat)
	{
		const FLOAT SquareSum = (FLOAT)(quat.x*quat.x+quat.y*quat.y+quat.z*quat.z+quat.w*quat.w);
		if( SquareSum >= FLOAT_EPSILON )
		{
			if( fabs(SquareSum-1.f) >= FLOAT_EPSILON )
			{
				const FLOAT Scale = 1.0f / FastSqrt(SquareSum);
				quat.x *= Scale; 
				quat.y *= Scale; 
				quat.z *= Scale;
				quat.w *= Scale;

				return TRUE;
			}

			return FALSE;
		}
		else 
		{	
			quat.x = 0.0f;
			quat.y = 0.0f;
			quat.z = 0.0f;
			quat.w = 1.0f;

			return FALSE;
		}
	}

	template< class T, class U > T Lerp( const T& A, const T& B, const U& Alpha )
	{
		return (T)(A + Alpha * (B-A));
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Line



	/** ����0--���߶κ���,1--�����߶�,2-���߶���
	*/
	INLINE int ProjectPointToLineSegment(const Vector3& pt,const Vector3& line1,const Vector3& line2,float& prj)
	{
		Vector3 line(line2.x-line1.x, line2.y-line1.y, line2.z-line1.z);
		Vector3 vec(pt.x-line1.x, pt.y-line1.y, pt.z-line1.z);
		float len=D3DXVec3Length(&line);
		if(len<FLOAT_EPSILON)
			return 1;
		prj=D3DXVec3Dot(&line,&vec)/len;
		if(prj<0)
			return 0;
		else if(prj > len)
			return 1;

		return 2;
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// λ����


	/** ��ָ��bit�ó�0
	*/
	INLINE DWORD ZeroBitFlag( DWORD flags, DWORD flag )
	{
		flag = ~flag;
		flags &= flag;
		return flags;
	}

    //////////////////////////////////////////////////////////////////////////
    // Cubic Hermite Curve
    //////////////////////////////////////////////////////////////////////////
    template< typename T, typename U > void AutoCalcTangent( const T& PrevP, const T& P, const T& NextP, const U& Tension, T& OutTan )
    {
        OutTan = 0.5f * (1.f - Tension) * ( (P - PrevP) + (NextP - P) );
    }

    // P - end points
    // T - tangent directions at end points
    // Alpha - distance along spline
    template< typename T, typename U > T CubicInterp( const T& P0, const T& T0, const T& P1, const T& T1, const U& A )
    {
        const FLOAT A2 = A  * A;
        const FLOAT A3 = A2 * A;

        return (T)(((2*A3)-(3*A2)+1) * P0) + ((A3-(2*A2)+A) * T0) + ((A3-A2) * T1) + (((-2*A3)+(3*A2)) * P1);
    }


	/*-----------------------------------------------------------------------------
	FFloat32
	-----------------------------------------------------------------------------*/

#define __INTEL_BYTE_ORDER__		1

	/**
	* 32 bit float components
	*/
	class FFloat32
	{
	public:
		union
		{
			struct
			{
#if __INTEL_BYTE_ORDER__
				DWORD	Mantissa : 23;
				DWORD	Exponent : 8;
				DWORD	Sign : 1;			
#else
				DWORD	Sign : 1;
				DWORD	Exponent : 8;
				DWORD	Mantissa : 23;			
#endif
			} Components;

			FLOAT	FloatValue;
		};

		FFloat32( FLOAT InValue=0.0f )
			:	FloatValue(InValue)
		{}
	};

	/*-----------------------------------------------------------------------------
	FFloat16
	-----------------------------------------------------------------------------*/

	/**
	* 16 bit float components and conversion
	*
	*
	* IEEE FLOAT 16
	* Represented by 10-bit mantissa M, 5-bit exponent E, and 1-bit sign S
	*
	* Specials:
	* 
	* E=0, M=0			== 0.0
	* E=0, M!=0			== Denormalized value (M / 2^10) * 2^-14
	* 0<E<31, M=any		== (1 + M / 2^10) * 2^(E-15)
	* E=31, M=0			== Infinity
	* E=31, M!=0		== NAN
	*
	*/
	class FFloat16
	{
	public:
		union
		{
			struct
			{
#if __INTEL_BYTE_ORDER__
				WORD	Mantissa : 10;
				WORD	Exponent : 5;
				WORD	Sign : 1;
#else
				WORD	Sign : 1;
				WORD	Exponent : 5;
				WORD	Mantissa : 10;			
#endif
			} Components;

			WORD	Encoded;
		};

		/** Constructor */
		FFloat16( WORD InEncoded=0 )
			:	Encoded(InEncoded)
		{
		}

		/** Constructor. Copy */
		FFloat16( const FFloat16& Copy )
		{
			Encoded = Copy.Encoded;
		}

		/** Constructor. Convert from Fp32 to Fp16. */
		explicit FFloat16( FLOAT FP32Value )
		{
			FFloat32 FP32(FP32Value);

			// Copy sign-bit
			Components.Sign = FP32.Components.Sign;

			// Check for zero or denormal.
			if ( FP32.Components.Exponent == 0 )			// Minimum exponent?
			{
				// Set to 0.
				Components.Exponent = 0;
				Components.Mantissa = 0;
			}
			// Check for INF or NaN.
			else if ( FP32.Components.Exponent == 255 )		// Maximum exponent? (2^8 - 1)
			{
				Components.Exponent = 31;
				Components.Mantissa = WORD(FP32.Components.Mantissa >> 13);
			}
			// Handle normal number.
			else
			{
				Components.Exponent = INT(FP32.Components.Exponent) - 127 + 15;
				Components.Mantissa = WORD(FP32.Components.Mantissa >> 13);
			}
		}	

		/** Convert from Fp16 to Fp32. */
		operator FLOAT() const
		{
			FFloat32	Result;

			Result.Components.Sign = Components.Sign;
			if (Components.Exponent == 0)
			{
				// Zero or denormal. Just clamp to zero...
				Result.Components.Exponent = 0;
				Result.Components.Mantissa = 0;
			}
			else if (Components.Exponent == 31)		// 2^5 - 1
			{
				// Infinity or NaN.
				Result.Components.Exponent = 255;
				Result.Components.Mantissa = DWORD(Components.Mantissa) << 13;
			}
			else
			{
				// Normal number.
				Result.Components.Exponent = INT(Components.Exponent) - 15 + 127; // Stored exponents are biased by half their range.
				Result.Components.Mantissa = DWORD(Components.Mantissa) << 13;
			}

			return Result.FloatValue;
		}
	};


	/*-----------------------------------------------------------------------------
	Vector3Half
	-----------------------------------------------------------------------------*/

	/**
	* A 3x1 of 16 bit floats.
	*/
	struct Vector3Half 
	{
		FFloat16	x,y,z;

		// Constructors.
		INLINE Vector3Half()
		{}
		INLINE Vector3Half(WORD inx,FLOAT iny,FLOAT inz)
			:x(inx),y(iny),z(inz)
		{}
		INLINE Vector3Half(FLOAT inx,FLOAT iny,FLOAT inz)
			:x(inx),y(iny),z(inz)
		{}
		INLINE Vector3Half(const Vector3& vec3)
			:x(vec3.x),y(vec3.y),z(vec3.z)
		{}
		operator Vector3() const
		{
			return Vector3((FLOAT)x,(FLOAT)y,(FLOAT)z);
		}
	};
}//endof namespace ECore
