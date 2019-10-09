#include "stdafx.h"
#include ".\geomdefine.h"
#include "Ray.h"
#include "E:/M_Server/D3d/Include/d3dx9mesh.h"
namespace ECore
{


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// AABBox
	
	
	void AABBox::Transform( const Matrix4& matrix )
	{
		Vector3 corners1[8];
		GetCorners(corners1);

		// Transform
		Vector3 corners2[8];
		D3DXVec3TransformCoordArray( corners2, sizeof(Vector3), corners1, sizeof(Vector3), &matrix, 8 );

		// Check extents
		_min.x = FLOAT_MAX; _min.y = FLOAT_MAX; _min.z = FLOAT_MAX;
		_max.x = -FLOAT_MAX; _max.y = -FLOAT_MAX; _max.z = -FLOAT_MAX;
		for( int i=0; i<8; ++i )
		{
			Vector3& temp = corners2[i];

			if( temp.x > _max.x )
				_max.x = temp.x;
			if( temp.y > _max.y )
				_max.y = temp.y;
			if( temp.z > _max.z )
				_max.z = temp.z;
			if( temp.x < _min.x )
				_min.x = temp.x;
			if( temp.y < _min.y )
				_min.y = temp.y;
			if( temp.z < _min.z )
				_min.z = temp.z;
		}
	}

	void AABBox::TransformCorners( const Matrix4& matrix,Vector3 pt[8] ) const
	{
		GetCorners(pt);
		D3DXVec3TransformCoordArray( pt, sizeof(Vector3), pt, sizeof(Vector3), &matrix, 8 );
	}

	void AABBox::TransformFast( const Matrix4& matrix,const Vector3& scale )
	{
		Vector3 center = GetCenter();
		D3DXVec3TransformCoord(&center,&center,&matrix);
		Vector3 halfSize(_max.x-_min.x,_max.y-_min.y,_max.z-_min.z);
		halfSize *= 0.5f;
		halfSize.x *= scale.x;
		halfSize.y *= scale.y;
		halfSize.z *= scale.z;
		_min.x = center.x - halfSize.x;
		_min.y = center.x - halfSize.y;
		_min.z = center.x - halfSize.z;
		_max.x = center.x + halfSize.x;
		_max.y = center.y + halfSize.y;
		_max.z = center.z + halfSize.z;
	}

	void AABBox::TransformFast( const Matrix4& matrix,const Vector3& scale, AABBox& out )
	{
		Vector3 center = GetCenter();
		D3DXVec3TransformCoord(&center,&center,&matrix);
		Vector3 halfSize(_max.x-_min.x,_max.y-_min.y,_max.z-_min.z);
		halfSize *= 0.5f;
		halfSize.x *= scale.x;
		halfSize.y *= scale.y;
		halfSize.z *= scale.z;
		out._min.x = center.x - halfSize.x;
		out._min.y = center.x - halfSize.y;
		out._min.z = center.x - halfSize.z;
		out._max.x = center.x + halfSize.x;
		out._max.y = center.y + halfSize.y;
		out._max.z = center.z + halfSize.z;
	}

	void AABBox::Merge( const AABBox& rhs )
	{
		if(rhs._max.x > _max.x)
			_max.x=rhs._max.x;
		if(rhs._max.y > _max.y)
			_max.y=rhs._max.y;
		if(rhs._max.z > _max.z)
			_max.z=rhs._max.z;
		//--
		if(rhs._min.x < _min.x)
			_min.x=rhs._min.x;
		if(rhs._min.y < _min.y)
			_min.y=rhs._min.y;
		if(rhs._min.z < _min.z)
			_min.z=rhs._min.z;
	}

	void AABBox::Merge( const Vector3& v )
	{
		if( v.x > _max.x )
			_max.x = v.x;
		if( v.y > _max.y )
			_max.y = v.y;
		if( v.z > _max.z )
			_max.z = v.z;
		if( v.x < _min.x )
			_min.x = v.x;
		if( v.y < _min.y )
			_min.y = v.y;
		if( v.z < _min.z )
			_min.z = v.z;
	}

	bool AABBox::IntersectsWithLine(Vector3& vPoint1, Vector3& vPoint2) const
	{
		Vector3 vLineMiddle=(vPoint1+vPoint2)/2.0f;
		Vector3 vLineVect=vPoint2-vPoint1;
		Vec3Normalize(vLineVect);
        float fHalfLength=Vec3Dist(vPoint1,vPoint2)/2.0f;
		
		const Vector3 e = (_max - _min) * 0.5f;
		const Vector3 t = (_min + e) - vLineMiddle;
		float r;

		if ((fabs(t.x) > e.x + fHalfLength * fabs(vLineVect.x)) || 
			(fabs(t.y) > e.y + fHalfLength * fabs(vLineVect.y)) ||
			(fabs(t.z) > e.z + fHalfLength * fabs(vLineVect.z)) )
				return false;

		r = e.y * fabs(vLineVect.z) + e.z * fabs(vLineVect.y);

		if (fabs(t.y * vLineVect.z - t.z * vLineVect.y) > r)
				return false;

		r = e.x * fabs(vLineVect.z) + e.z * fabs(vLineVect.x);
		if (fabs(t.z * vLineVect.x - t.x * vLineVect.z) > r)
				return false;

		r = e.x * fabs(vLineVect.y) + e.y * fabs(vLineVect.x);
		if (fabs(t.x * vLineVect.y - t.y * vLineVect.x) > r)
				return false;

		return true;
	}

	bool AABBox::IntersectsTriangle( Vector3& v1,Vector3& v2,Vector3& v3 ) const
	{
		//box完全包含三角形
		if(Contain(v1)
			&& Contain(v2)
			&& Contain(v3))
			return true;

		//线段与box相交
		if(IntersectsWithLine(v1,v2)
			|| IntersectsWithLine(v2,v3)
			|| IntersectsWithLine(v1,v3))
			return true;

		//box的线段与三角形相交
		Line lines[4];
		GetLines(lines);
		for(int i=0;i<4;i++)
		{
			if(lines[i].IntersectsTri(v1,v2,v3))
				return true;
		}

		return false;
	}

	bool AABBox::SweepTest(const AABBox& staticBox,const Vector3& d,float& t,BOOL* pTopPlane) const
	{
		float tEnter = 0.0f;
		float tLeave = 1.0f;

		BOOL bDown = FALSE;

		if( pTopPlane )
			*pTopPlane = FALSE;

		// 检查x轴
		float xEnter = 0.0f;
		if( d.x == 0.0f )
		{
			// x轴上重叠部分为空？
			if( staticBox._min.x >= _max.x ||
				staticBox._max.x <= _min.x )
				return false;
		}
		else
		{
			// 只除一次
			float oneOverD = 1.0f / d.x;

			// 计算开始接触和脱离接触的时间
			float xLeave;
			xEnter = (staticBox._min.x - _max.x) * oneOverD;
			xLeave = (staticBox._max.x - _min.x) * oneOverD;

			// 检查顺序
			if( xEnter > xLeave )
				std::swap(xEnter, xLeave);

			// 更新区间
			if( xEnter > tEnter ) tEnter = xEnter;
			if( xLeave < tLeave ) tLeave = xLeave;

			// 是否导致空重叠区？
			if( tEnter > tLeave )
				return false;
		}

		// 检查y轴
		float yEnter = 0.0f;
		if( d.y == 0.0f )
		{
			// y轴上重叠部分为空？
			if( staticBox._min.y >= _max.y ||
				staticBox._max.y <= _min.y )
				return false;
		}
		else
		{
			// 只除一次
			float oneOverD = 1.0f / d.y;

			// 计算开始接触和脱离接触的时间
			float yLeave;
			yEnter = (staticBox._min.y - _max.y) * oneOverD;
			yLeave = (staticBox._max.y - _min.y) * oneOverD;

			// 检查顺序
			if( yEnter > yLeave )
			{
				bDown = TRUE;
				std::swap(yEnter, yLeave);
			}

			// 更新区间
			if( yEnter > tEnter ) tEnter = yEnter;
			if( yLeave < tLeave ) tLeave = yLeave;

			// 是否导致空重叠区？
			if( tEnter > tLeave )
				return false;
		}

		// 检查z轴
		float zEnter = 0.0f;
		if( d.z == 0.0f )
		{
			// z轴上重叠部分为空？
			if( staticBox._min.z >= _max.z ||
				staticBox._max.z <= _min.z )
				return false;
		}
		else
		{
			// 只除一次
			float oneOverD = 1.0f / d.z;

			// 计算开始接触和脱离接触的时间
			float zLeave;
			zEnter = (staticBox._min.z - _max.z) * oneOverD;
			zLeave = (staticBox._max.z - _min.z) * oneOverD;

			// 检查顺序
			if( zEnter > zLeave )
				std::swap(zEnter, zLeave);

			// 更新区间
			if( zEnter > tEnter ) tEnter = zEnter;
			if( zLeave < tLeave ) tLeave = zLeave;

			// 是否导致空重叠区？
			if( tEnter > tLeave )
				return false;
		}

		if( pTopPlane
			&& bDown				// 从上往下碰撞
			&& yEnter > xEnter		// y轴接触时间比x轴晚
			&& yEnter > zEnter )	// y轴接触时间比z轴晚
		{
			*pTopPlane = TRUE;
		}

		// 有相交发生，返回交点的参数值
		t = tEnter;
		return true;
	}

	void AABBox::GetLines( Line lines[4] ) const
	{
		Vector3 corners[8];
		GetCorners(corners);

		/** 0-7
		*/
		lines[0].origin=corners[0];
		lines[0].to=corners[7];

		/** 1-6
		*/
		lines[1].origin=corners[1];
		lines[1].to=corners[6];

		/** 2-4
		*/
		lines[2].origin=corners[2];
		lines[2].to=corners[4];

		/** 3-5
		*/
		lines[3].origin=corners[3];
		lines[3].to=corners[5];
	}


	void AABBox::ComputeFromPoints( const Vector3* pts,int num )
	{
		_min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);
		_max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);

		//--
		for( int i=0; i<num; ++i )
		{
			const Vector3& pt = pts[i];
			if( pt.x > _max.x )
				_max.x = pt.x;
			if( pt.y > _max.y )
				_max.y = pt.y;
			if( pt.z > _max.z )
				_max.z = pt.z;
			if( pt.x < _min.x )
				_min.x = pt.x;
			if( pt.y < _min.y )
				_min.y = pt.y;
			if( pt.z < _min.z )
				_min.z = pt.z;
		}
	}

	bool AABBox::Check() const
	{
		if(!MathConst::BigBox.Contain(*this))
			return false;

		return _min.x<=_max.x	&& _min.y<=_max.y	&& _min.z<=_max.z;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MathConst



	Vector3		MathConst::AxisX;
	Vector3		MathConst::AxisY;
	Vector3		MathConst::AxisZ;
	Vector3		MathConst::NegAxisX;
	Vector3		MathConst::NegAxisY;
	Vector3		MathConst::NegAxisZ;
	Vector2		MathConst::Zero2;
	Vector3		MathConst::Zero3;
	Vector4		MathConst::Zero4;
	Vector4		MathConst::Zero3One1;
	Vector3		MathConst::Max3;
	Vector3		MathConst::One3;
	Matrix4		MathConst::IdentityMat;
	Matrix3x4	MathConst::IdentityMat3x4;
	Matrix4		MathConst::RH2LHMat;
	Matrix4		MathConst::LH2RHMat;
	AABBox		MathConst::SmallBox;
	AABBox		MathConst::BigBox;
	Quaternion	MathConst::IdentityQuat;

	MathConst g_mathConst;//目的是调用构造函数
	MathConst::MathConst()
	{
		AxisX=Vector3(1,0,0);
		AxisY=Vector3(0,1,0);
		AxisZ=Vector3(0,0,1);
		NegAxisX=Vector3(-1,0,0);
		NegAxisY=Vector3(0,-1,0);
		NegAxisZ=Vector3(0,0,-1);
		Zero2=Vector2(0,0);
		Zero3=Vector3(0,0,0);
		Zero4=Vector4(0,0,0,0);
		Zero3One1=Vector4(0,0,0,1);
		One3=Vector3(1,1,1);
		Max3=Vector3(1000000,1000000,1000000);
		D3DXMatrixIdentity(&IdentityMat);
		IdentityMat3x4.SetMatrixTranspose(IdentityMat);
		D3DXMatrixRotationX(&RH2LHMat,-FLOAT_PI*0.5f);
		D3DXMatrixRotationX(&LH2RHMat,FLOAT_PI*0.5f);
		SmallBox._max=Vector3(1,1,1);
		SmallBox._min=Vector3(-1,-1,-1);
		BigBox._max=Vector3(1000000,1000000,1000000);
		BigBox._min=Vector3(-1000000,-1000000,-1000000);
		D3DXQuaternionIdentity(&IdentityQuat);
	}

	MathConst::~MathConst()
	{}




	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Triangle



	void Triangle::BuildAABBox( AABBox& box )
	{
		box._min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);
		box._max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);

		if(a.x<box._min.x)box._min.x=a.x;
		if(a.x>box._max.x)box._max.x=a.x;
		if(a.y<box._min.y)box._min.y=a.y;
		if(a.y>box._max.y)box._max.y=a.y;
		if(a.z<box._min.z)box._min.z=a.z;
		if(a.z>box._max.z)box._max.z=a.z;

		if(b.x<box._min.x)box._min.x=b.x;
		if(b.x>box._max.x)box._max.x=b.x;
		if(b.y<box._min.y)box._min.y=b.y;
		if(b.y>box._max.y)box._max.y=b.y;
		if(b.z<box._min.z)box._min.z=b.z;
		if(b.z>box._max.z)box._max.z=b.z;

		if(c.x<box._min.x)box._min.x=c.x;
		if(c.x>box._max.x)box._max.x=c.x;
		if(c.y<box._min.y)box._min.y=c.y;
		if(c.y>box._max.y)box._max.y=c.y;
		if(c.z<box._min.z)box._min.z=c.z;
		if(c.z>box._max.z)box._max.z=c.z;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Line


	bool Line::IntersectsTri( Vector3& v1,Vector3& v2,Vector3& v3 ) const
	{
		Vector3 dir=to-origin;
		float dist;
		float length=D3DXVec3Length(&dir);
		dir/=length;
		if(D3DXIntersectTri(&v1,&v2,&v3,&origin,&dir,NULL,NULL,&dist))
		{
			if(dist<length)
				return true;
		}

		return false;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 全局函数



	bool Intersect(const Sphere& sphere, const AABBox& box)
	{
		// Use splitting planes
		const Vector3& center = sphere.centre;
		float radius = sphere.radius;
		const Vector3& min = box._min;
		const Vector3& max = box._max;

		// just test facing planes, early fail if sphere is totally outside
		if (center.x < min.x && 
			min.x - center.x > radius)
		{
			return false;
		}
		if (center.x > max.x && 
			center.x  - max.x > radius)
		{
			return false;
		}

		if (center.y < min.y && 
			min.y - center.y > radius)
		{
			return false;
		}
		if (center.y > max.y && 
			center.y  - max.y > radius)
		{
			return false;
		}

		if (center.z < min.z && 
			min.z - center.z > radius)
		{
			return false;
		}
		if (center.z > max.z && 
			center.z  - max.z > radius)
		{
			return false;
		}

		// Must intersect
		return true;
	}

	bool Intersect(const Plane& plane, const AABBox& box)
	{
		// Get corners of the box
		Vector3 pCorners[8];
		box.GetCorners(pCorners);

		// Test which side of the plane the corners are
		// Intersection occurs when at least one corner is on the 
		// opposite side to another
		Halfspace lastSide = plane.ClassifyPoint(pCorners[0]);
		for (int corner = 1; corner < 8; ++corner)
		{
			if (plane.ClassifyPoint(pCorners[corner]) != lastSide)
			{
				return true;
			}
		}

		return false;
	}

	bool Intersect(const Sphere& sphere, const Plane& plane)
	{
		return (fabs(D3DXVec3Dot(&plane.normal,&sphere.centre)) <= sphere.radius );
	}

	void TriangleCentroid(const Vector3& p1,const Vector3& p2,const Vector3& p3,Vector3& out)
	{
		out=(p1+p2+p3)*(1.0f/3.0f);
	}

	float TriangleArea(const Vector3& p1,const Vector3& p2,const Vector3& p3)
	{
		Vector3 edge1=p3-p1;
		Vector3 edge2=p2-p1;
		Vector3 cross;
		D3DXVec3Cross(&cross,&edge1,&edge2);

		//叉积的一半可以算出三角形的面积,see Eric Lengyel's Mathematics for 3D game programmer & CG
		return 0.5f * D3DXVec3Length(&cross);
	}

	void BuildPlane(const Vector3& p1,const Vector3& p2,const Vector3& p3,Plane& out)
	{
		Vector3 edge1=p3-p1;
		Vector3 edge2=p2-p1;
		D3DXVec3Cross(&out.normal,&edge1,&edge2);
		D3DXVec3Normalize(&out.normal,&out.normal);

		out.dist=-D3DXVec3Dot(&p1,&out.normal);
	}

}//endof namespace ECore