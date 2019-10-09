#pragma once

namespace ECore
{
	/**	\class Transform
		\brief �任����,���Լ���һϵ�б任
	*/
	class ECORE_API Transform
	{
	public:
		Transform(void)
		{
			LoadIdentity();
		}
		Transform::Transform(const Matrix4& mat) : m_mat(mat)
		{}

		/**	��Transform�óɵ�λ����,��û���κα任		*/
		INLINE void LoadIdentity()
		{
			D3DXMatrixIdentity(&m_mat);
		}
		/**	����һ��λ��		*/
		INLINE void Translate(const Vector3& offset)
		{
			m_mat._41 += offset.x; 
			m_mat._42 += offset.y; 
			m_mat._43 += offset.z;
		}
		/**	������ת		*/
		INLINE void Rotate(float yaw,float pitch,float roll)
		{
			Matrix4 mat;
			D3DXMatrixRotationYawPitchRoll(&mat,yaw,pitch,roll);

			D3DXMatrixMultiply(&m_mat,&m_mat,&mat);//(Out = M1 * M2).
		}
		/**	������ĳ������ת
			\param axis����ĸx,y,z֮һ,rad����ת�Ƕ�,�Ƕ�Ϊ������Ϊ��ʱ�뷽����ת
		*/
		INLINE void RotateAxis(char axis,float rad)
		{
			Matrix4 mat;
			switch(axis)
			{
			case 'X':
			case 'x':
				D3DXMatrixRotationX(&mat,rad);
				break;
			case 'Y':
			case 'y':
				D3DXMatrixRotationY(&mat,rad);
				break;
			case 'Z':
			case 'z':
				D3DXMatrixRotationZ(&mat,rad);
				break;
			default:
				ASSERT(0 && "axis must be x,y or z");
			}

			D3DXMatrixMultiply(&m_mat,&m_mat,&mat);//(Out = M1 * M2).
		}
		/**	��������������ת		*/
		INLINE void RotateAxis(const Vector3& axis,float rad)
		{
			Matrix4 mat;
			D3DXMatrixRotationAxis(&mat,&axis,rad);

			D3DXMatrixMultiply(&m_mat,&m_mat,&mat);//(Out = M1 * M2).
		}
		/** �����Ԫ����ת */
		INLINE void RotateQuat(const Quaternion& q)
		{
			Matrix4 mat;
			D3DXMatrixRotationQuaternion(&mat,&q);

			D3DXMatrixMultiply(&m_mat,&m_mat,&mat);//(Out = M1 * M2).
		}
		/**	��������		*/
		INLINE void Scale(float x,float y,float z)
		{
			m_mat._11 *= x;
			m_mat._22 *= y;
			m_mat._33 *= z;
		}
		/** ����	*/
		INLINE void Mirror(float x,float y,float z)
		{
			m_mat._11 *= x;
			m_mat._21 *= x;
			m_mat._31 *= x;

			m_mat._12 *= y;
			m_mat._22 *= y;
			m_mat._32 *= y;

			m_mat._13 *= z;
			m_mat._23 *= z;
			m_mat._33 *= z;
		}

		/** �ڵ�ǰ�ı任�м���һ���任		*/
		INLINE void CombineTrans(const Matrix4* pMatrix)
		{
			ASSERT(pMatrix!=NULL);
			D3DXMatrixMultiply(&m_mat,&m_mat,pMatrix);
		}
		/**	ʹ��һ�������滻��ǰ�ľ���		*/
		INLINE void LoadMatrix(const Matrix4* pMatrix)
		{
			ASSERT(pMatrix!=NULL);
			m_mat=*pMatrix;
		}
		
		INLINE void TransPos(Vector3& v)
		{
			D3DXVec3TransformCoord(&v,&v,&m_mat);
		}
		INLINE void TransDir(Vector3& v)
		{
			D3DXVec3TransformNormal(&v,&v,&m_mat);
		}
		INLINE const Matrix4* GetMatrix() const	{	return &m_mat;}
	private:
		Matrix4	m_mat;
	};
}//namespace ECore