#pragma once

namespace ECore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 宏定义


	//! Integer representation of a floating-point value.
	#define IR(x)					((DWORD&)(x))


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 常量定义


	const float FLOAT_MAX		= 3.402823466e+38f;
	const float FLOAT_EPSILON	= 1e-6f;
	const float FLOAT_PI		= 3.141592654f;
	const float WORLD_MAX		= 1e+6f;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 角度与弧度换算


	INLINE float _DegToRad(float a) { return a*0.01745329252f;};
	INLINE float _RadToDeg(float a) { return a*57.29577951f;};



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 基本类型



	typedef D3DXVECTOR2			Vector2;
	typedef D3DXVECTOR3			Vector3;
	typedef D3DXVECTOR4			Vector4;
	typedef D3DXVECTOR2_16F		Vector2_16F;
	typedef D3DXVECTOR4_16F		Vector4_16F;
	typedef D3DXQUATERNION		Quaternion;
	typedef D3DXMATRIX			Matrix4;


	//--------------------------
	// 4D Vector (16 bit)
	//--------------------------
	struct Vector3_16F
	{
	public:
		INLINE void ToVector3(Vector3& out) { D3DXFloat16To32Array(&out.x, &x, 3); }

	public:
		D3DXFLOAT16 x, y, z;
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Matrix2



	struct Matrix2
	{
		float  _11, _12;
		float  _21, _22;
	};



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Matrix3x4


	struct Matrix3x4
	{
		float  m[3][4];

		INLINE void GetMatrix(Matrix4& mat) const
		{
			mat.m[0][0] = m[0][0];
			mat.m[1][0] = m[0][1];
			mat.m[2][0] = m[0][2];
			mat.m[3][0] = m[0][3];

			mat.m[0][1] = m[1][0];
			mat.m[1][1] = m[1][1];
			mat.m[2][1] = m[1][2];
			mat.m[3][1] = m[1][3];

			mat.m[0][2] = m[2][0];
			mat.m[1][2] = m[2][1];
			mat.m[2][2] = m[2][2];
			mat.m[3][2] = m[2][3];

			mat.m[0][3] = 0;
			mat.m[1][3] = 0;
			mat.m[2][3] = 0;
			mat.m[3][3] = 1;
		}

		INLINE void SetMatrix(const Matrix4& mat)
		{
			m[0][0] = mat.m[0][0];
			m[0][1] = mat.m[0][1];
			m[0][2] = mat.m[0][2];
			m[0][3] = mat.m[0][3];

			m[1][0] = mat.m[1][0];
			m[1][1] = mat.m[1][1];
			m[1][2] = mat.m[1][2];
			m[1][3] = mat.m[1][3];

			m[2][0] = mat.m[2][0];
			m[2][1] = mat.m[2][1];
			m[2][2] = mat.m[2][2];
			m[2][3] = mat.m[2][3];
		}

		INLINE void SetMatrixTranspose(const Matrix4& mat)
		{
			m[0][0] = mat.m[0][0];
			m[0][1] = mat.m[1][0];
			m[0][2] = mat.m[2][0];
			m[0][3] = mat.m[3][0];

			m[1][0] = mat.m[0][1];
			m[1][1] = mat.m[1][1];
			m[1][2] = mat.m[2][1];
			m[1][3] = mat.m[3][1];

			m[2][0] = mat.m[0][2];
			m[2][1] = mat.m[1][2];
			m[2][2] = mat.m[2][2];
			m[2][3] = mat.m[3][2];
		}
	};


}//namespace ECore