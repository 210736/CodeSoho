#pragma once

namespace ECore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Line


	struct Line
	{
		Vector3 origin;
		Vector3 to;

		bool IntersectsTri(Vector3& v1,Vector3& v2,Vector3& v3) const;
	};



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// AABBox

	class ECORE_API AABBox
	{
	public:
		Vector3 _min;
		Vector3 _max;

		AABBox() : _min( 0.0f, 0.0f, 0.0f ), _max( 1.0f, 1.0f, 1.0f )
		{
		}

		AABBox( const AABBox& rhs ) : _min( rhs._min ), _max( rhs._max )
		{
		}


		///////////////////////////////
		//


		/** 返回8个顶点坐标
		//
		//     6+------+7			
		//    / |     /|			
		//	 /  |    / |
		// 4+------+5  |    	
		//  |   |  |   |      	
		//	|   |  |   |
		//  |  2+--|---+3  	 y	z	
		//  |  /   |  /      | / 
		//  | /    | /       |/		
		//  0+-----+1        *---x	
		*/
		INLINE void GetCorners(Vector3 pt[8]) const
		{
			//
			//     6+------+7			
			//    / |     /|			
			//	 /  |    / |
			// 4+------+5  |    	
			//  |   |  |   |      	
			//	|   |  |   |
			//  |  2+--|---+3  	 y	z	
			//  |  /   |  /      | / 
			//  | /    | /       |/		
			//  0+-----+1        *---x	

			pt[0].x = _min.x; pt[0].y = _min.y; pt[0].z = _min.z;
			pt[1].x = _max.x; pt[1].y = _min.y; pt[1].z = _min.z;
			pt[2].x = _min.x; pt[2].y = _min.y; pt[2].z = _max.z;
			pt[3].x = _max.x; pt[3].y = _min.y; pt[3].z = _max.z;

			pt[4].x = _min.x; pt[4].y = _max.y; pt[4].z = _min.z;
			pt[5].x = _max.x; pt[5].y = _max.y; pt[5].z = _min.z;
			pt[6].x = _min.x; pt[6].y = _max.y; pt[6].z = _max.z;
			pt[7].x = _max.x; pt[7].y = _max.y; pt[7].z = _max.z;
		}


		/** 返回4条边
		*/
		void GetLines(Line lines[4]) const;
		/** 返回中心坐标
		*/
		INLINE Vector3 GetCenter() const
		{
			return Vector3( (_max.x+_min.x) * 0.5f, (_max.y+_min.y) * 0.5f, (_max.z+_min.z) * 0.5f );
		}
		INLINE void GetCenter(Vector3& out) const
		{
			out.x = ( _max.x + _min.x ) * 0.5f;
			out.y = ( _max.y + _min.y ) * 0.5f;
			out.z = ( _max.z + _min.z ) * 0.5f;
		}
		/** 从n个点计算AABBox
		*/
		void ComputeFromPoints(const Vector3* pts,int num);


		///////////////////////////////
		// 变换


		/** Transforms the box according to the matrix supplied.
		    @remarks
			    By calling this method you get the axis-aligned box which
			    surrounds the transformed version of this box. Therefore each
			    corner of the box is transformed by the matrix, then the
			    extents are mapped back onto the axes to produce another
			    AABB. Useful when you have a local AABB for an object which
			    is then transformed.
	    */
	    void Transform( const Matrix4& matrix );
		/** 快速变换
			@remarks 先变换中心点，然后根据尺寸重新生成AABB
		*/
		void TransformFast( const Matrix4& matrix,const Vector3& scale );
		void TransformFast( const Matrix4& matrix,const Vector3& scale, AABBox& out );
		/** 变换顶点
		//
		//     6+------+7			
		//    / |     /|			
		//	 /  |    / |
		// 4+------+5  |    	
		//  |   |  |   |      	
		//	|   |  |   |
		//  |  2+--|---+3  	 y	z	
		//  |  /   |  /      | / 
		//  | /    | /       |/		
		//  0+-----+1        *---x	
		*/
		void TransformCorners( const Matrix4& matrix,Vector3 pt[8]) const;


		///////////////////////////////
		// 相交测试


		/** 与另一个AABB相交测试
		*/
		INLINE bool Intersects(const AABBox& other) const
		{
			// Must be overlap along all three axes.
			// Try to find a separating axis.
			if(_min.x > other._max.x
				|| _max.x < other._min.x)
				return false;
			if(_min.y > other._max.y
				|| _max.y < other._min.y)
				return false;
			if(_min.z > other._max.z
				|| _max.z < other._min.z)
				return false;
			return true;
		}
		/** 与另一个AABB的顶面相交测试
		*/
		INLINE bool IntersectsTop(const AABBox& other) const
		{
			if(_min.x > other._max.x
				|| _max.x < other._min.x)
				return false;
			if(_min.z > other._max.z
				|| _max.z < other._min.z)
				return false;
			if(_min.y > other._max.y
				|| _max.y < other._max.y)
				return false;
			return true;
		}

		/** 与另一个AABB在XZ平面做相交测试
		*/
		INLINE bool IntersectsXZ(const AABBox& other) const
		{
			if(_min.x > other._max.x
				|| _max.x < other._min.x)
				return false;
			if(_min.z > other._max.z
				|| _max.z < other._min.z)
				return false;
			return true;
		}

		/** 判断一条线段是否与AABB相交
		*/
		bool IntersectsWithLine(Vector3& vPoint1, Vector3& vPoint2)  const;
		/** 判断一个三角形是否与AABB相交
		*/
		bool IntersectsTriangle(Vector3& v1,Vector3& v2,Vector3& v3) const;

		/** 与另一个静止的AABB做SweepTest
		*/
		bool SweepTest(const AABBox& staticBox,const Vector3& dir,float& t,BOOL* pTopPlane=NULL) const;

		/** 返回this是否完全包含rhs
		*/
		INLINE bool Contain( const AABBox& rhs ) const
		{
			return _max.x>=rhs._max.x
				&& _max.y>=rhs._max.y
				&& _max.z>=rhs._max.z
				&& _min.x<=rhs._min.x
				&& _min.y<=rhs._min.y
				&& _min.z<=rhs._min.z;
		}
		/** 返回this是否完全包含一个点
		*/
		INLINE bool Contain( const Vector3& pt ) const
		{
			return pt.x<=_max.x && pt.y<=_max.y && pt.z<=_max.z
				&& pt.x>=_min.x && pt.y>=_min.y && pt.z>=_min.z;
		}
		/** 返回this是否完全包含一个点
		*/
		INLINE bool Contain( const Vector2& pt ) const
		{
			return pt.x<=_max.x && pt.y<=_max.z
				&& pt.x>=_min.x && pt.y>=_min.z;
		}


		///////////////////////////////
		// 合并


		/** Merges the passed in box into the current box. The result is the
		    box which encompasses both.
	    */
	    void Merge( const AABBox& rhs );
		void Merge( const Vector3& v );


		///////////////////////////////
		// 其它功能


		/** 返回点到盒子的最近距离
		*/
		INLINE float DistToPointSq(const Vector3& point) const
		{
			Vector3 dist;

			if( point.x >= _min.x && point.x <= _max.x )
				dist.x = 0.f;
			else
				dist.x = EMin(fabs(_min.x-point.x),fabs(_max.x-point.x));

			if(point.y >= _min.y && point.y <= _max.y )
				dist.y = 0.f;
			else
				dist.y = EMin(fabs(_min.y-point.y),fabs(_max.y-point.y));

			if( point.z >= _min.z && point.z <= _max.z )
				dist.z = 0.f;
			else
				dist.z = EMin(fabs(_min.z-point.z),fabs(_max.z-point.z));

			return D3DXVec3LengthSq(&dist);
		}

		INLINE float DistToPoint(const Vector3& point) const
		{
			return FastSqrt(DistToPointSq(point));
		}

		/** 使用AABBox剪裁三角形
			@remarks
				生成的顶点存入newVertex中,外部调用时应事先给newVertex分配9个顶点的内存，
				因为一个三角形被6个面剪裁最多会生成9个顶点
			@return 剪裁过后面的顶点数目
		*/
		INLINE int ClipTriangle(const Vector3* tri, Vector3 *newVertex) const;

		/** 检查AABB是否正常
		*/
		bool Check() const;

	private:

		/**单个面剪裁，返回定点数目
		*/
		INLINE int ClipPolygonAgainstPlane( const int nPlaneIndex, const int vertexCount, const Vector3 *vertex, Vector3 *newVertex ) const;

		/**求1个点到AABBox指定面的有向距离
		*/
		INLINE float VecToAABBoxPlaneDist( const Vector3& vec, const int nPlaneIndex ) const;
		/**求2个点在AABBox指定面的法线方向的距离
		*/
		INLINE float AABBoxPlaneDist( const Vector3& vec1, const Vector3& vec2, const int nPlaneIndex ) const;
	};

	int AABBox::ClipTriangle(const Vector3* tri, Vector3 *newVertex) const
	{
		static Vector3	tempVertex[9];

		// Clip against all six planes
		int count = ClipPolygonAgainstPlane(0, 3, tri, tempVertex);
		if (count != 0)
		{
			count = ClipPolygonAgainstPlane(1, count, tempVertex, newVertex);
			if (count != 0)
			{
				count = ClipPolygonAgainstPlane(2, count, newVertex, tempVertex);
				if (count != 0)
				{
					count = ClipPolygonAgainstPlane(3, count, tempVertex, newVertex);
					if (count != 0)
					{
						count = ClipPolygonAgainstPlane(4, count, newVertex, tempVertex);
						if (count != 0)
						{
							count = ClipPolygonAgainstPlane(5, count, tempVertex, newVertex);
						}
					}
				}
			}
		}

		return (count);
	}


	int AABBox::ClipPolygonAgainstPlane( const int nPlaneIndex, const int vertexCount, const Vector3 *vertex, Vector3 *newVertex ) const
	{
		static bool negative[10];
		static int n, count, m;

		// Classify vertices
		count = 0;
		for( n = 0; n < vertexCount; n++ )
		{
			negative[n] = 
				(nPlaneIndex != 5 ? nPlaneIndex != 4 ? nPlaneIndex != 3 ? nPlaneIndex != 2 ? nPlaneIndex != 1 ? nPlaneIndex != 0 ? 0.0f :
				(vertex[n].x - _min.x) : (_max.x - vertex[n].x) : (vertex[n].z - _min.z) : (_max.z - vertex[n].z) : (vertex[n].y - _min.y): (_max.y - vertex[n].y)) < 0.0f;

			count += negative[n];
		}

		if( count == vertexCount )	// Discard this polygon if it's completely culled
			return 0;

		count = 0;
		for( n = 0; n < vertexCount; n++ )
		{
			// m is the index of the previous vertex
			m = (n != 0) ? n - 1 : vertexCount - 1;
			if( negative[n] )
			{
				if( !negative[m] )
				{
					// Current vertex is on negative side of plane, but previous vertex is on positive side.
					newVertex[count] = vertex[m] - (vertex[m] - vertex[n]) * 
						((nPlaneIndex != 5 ? nPlaneIndex != 4 ? nPlaneIndex != 3 ? nPlaneIndex != 2 ? nPlaneIndex != 1 ? nPlaneIndex != 0 ? 0.0f :
						(vertex[m].x - _min.x) : (_max.x - vertex[m].x) : (vertex[m].z - _min.z) : (_max.z - vertex[m].z) : (vertex[m].y - _min.y): (_max.y - vertex[m].y))
						/ 
						(nPlaneIndex != 5 ? nPlaneIndex != 4 ? nPlaneIndex != 3 ? nPlaneIndex != 2 ? nPlaneIndex != 1 ? nPlaneIndex != 0 ? 0.0f :
						(vertex[m].x - vertex[n].x) : (vertex[n].x - vertex[m].x) : (vertex[m].z - vertex[n].z) : (vertex[n].z - vertex[m].z) : (vertex[m].y - vertex[n].y) : (vertex[n].y - vertex[m].y)));
					++count;
				}
			}
			else
			{
				if( negative[m] )
				{
					// Current vertex is on positive side of plane, but previous vertex is on negative side.
					newVertex[count] = vertex[n] - (vertex[n] - vertex[m]) * 
						(
						(nPlaneIndex != 5 ? nPlaneIndex != 4 ? nPlaneIndex != 3 ? nPlaneIndex != 2 ? nPlaneIndex != 1 ? nPlaneIndex != 0 ? 0.0f :
						(vertex[n].x - _min.x) : (_max.x - vertex[n].x) : (vertex[n].z - _min.z) : (_max.z - vertex[n].z) : (vertex[n].y - _min.y) :	(_max.y - vertex[n].y))  
						/ 
						(nPlaneIndex != 5 ? nPlaneIndex != 4 ? nPlaneIndex != 3 ? nPlaneIndex != 2 ? nPlaneIndex != 1 ? nPlaneIndex != 0 ? 0.0f :
						(vertex[n].x - vertex[m].x) : (vertex[m].x - vertex[n].x) : (vertex[n].z - vertex[m].z) : (vertex[m].z - vertex[n].z) :	(vertex[n].y - vertex[m].y) : (vertex[m].y - vertex[n].y)));
					++count;
				}

				newVertex[count] = vertex[n];	// Include current vertex
				++count;
			}
		}

		// Return number of vertices in clipped polygon
		return count;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Plane


	enum Halfspace
	{
		NEGATIVE = -1,
		ON_PLANE = 0,
		POSITIVE = 1
	};


	class ECORE_API Plane
	{
	public:
		Vector3 normal;
		float	dist;

		Plane():normal(0,1,0),dist(0)
		{}
		Plane(const Vector3& point,const Vector3& _normal)
		{
			normal=_normal;
			dist=-D3DXVec3Dot(&_normal,&point);
		}
		Plane(const Vector3& p0,const Vector3& p1,const Vector3& p2)
		{
			Redefine(p0,p1,p2);
		}

		INLINE void Redefine(const Vector3& p0,const Vector3& p1,const Vector3& p2)
		{
			Vector3 edge1 = p1 - p0;
			Vector3 edge2 = p2 - p0;
			D3DXVec3Cross(&normal,&edge1,&edge2);
			D3DXVec3Normalize(&normal,&normal);
			dist=-D3DXVec3Dot(&normal,&p0);
		}

		INLINE void Redefine(const Vector3& _normal,const Vector3& _point)
		{
			normal=_normal;
			dist=-D3DXVec3Dot(&normal,&_point);
			D3DXVec3Normalize(&normal,&normal);
		}

		// casting
		INLINE operator float* ()
		{
			return (float*)this;
		}

		INLINE operator const float* () const
		{
			return (float*)this;
		}

		INLINE void Normalize()
		{
			float mag=FastSqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			normal/=mag;
			dist/=mag;
		}

		INLINE float DistToPoint(const Vector3& pos) const
		{
			return D3DXVec3Dot(&normal,&pos)+dist;
		}

		INLINE Halfspace ClassifyPoint(const Vector3& pos) const
		{
			float d=DistToPoint(pos);
			if (d < 0)
				return NEGATIVE;
			if (d > 0)
				return POSITIVE;
			return ON_PLANE;
		}

		INLINE BOOL IsPositive( const AABBox& box ) const
		{
			Vector3 verts[8];
			box.GetCorners( verts );

			for( int i=0; i<8; ++i )
			{
				if( DistToPoint( verts[i] ) )
					return TRUE;
			}

			return FALSE;
		}

		INLINE void Transform(const Matrix4& mat)
		{
			Matrix4 tmpMatrix;
			D3DXMatrixInverse(&tmpMatrix, NULL, &mat);
			D3DXMatrixTranspose(&tmpMatrix, &tmpMatrix);

			D3DXPlaneTransform((LPD3DXPLANE)this,(LPD3DXPLANE)this,&tmpMatrix);
		}

		/** 已知平面上一点的XZ坐标，求Y坐标
		*/
		INLINE float GetHeight(float x,float z)
		{
			return (-1.0f*dist-normal.x*x-normal.z*z)/normal.y;
		}
	};



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// OBB



	class ECORE_API OBB
	{
	public:
		Vector3 m_center;
		Vector3 m_extent;
		Matrix4 m_rotate;
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Triangle



	class ECORE_API Triangle
	{
	public:
		Vector3 a;
		Vector3 b;
		Vector3 c;

		Triangle(){}
		~Triangle(){}

		void BuildAABBox(AABBox& box);
	};



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// OBBox



	class ECORE_API OBBox
	{
	public:
		Matrix4 trans;
		Vector3 size;
	};



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Sphere


	class ECORE_API Sphere
	{
	public:
		Vector3	centre;
		float	radius;

		Sphere():centre(0,0,0),radius(1)
		{}

		/** Returns whether or not this sphere interects another sphere. */
		INLINE bool Intersect(const Sphere& s) const
		{
			return Vec3Dist(centre,s.centre) <=	(s.radius + radius);
		}
	};



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// QuadFace


	struct ECORE_API QuadFace
	{
		Vector3	pt[4];
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Rectf


	struct ECORE_API Rectf
	{
		float top, left, bottom, right;

		INLINE Rectf(const RECT& rc)
		{
			left	= (FLOAT)rc.left;
			top		= (FLOAT)rc.top;
			right	= (FLOAT)rc.right;
			bottom  = (FLOAT)rc.bottom;
		}

		INLINE FLOAT Width() const  { return right - left; }
		INLINE FLOAT Height() const { return bottom - top; }

		INLINE VOID operator=(CONST RECT& rc)
		{
			left	= (FLOAT)rc.left;
			top		= (FLOAT)rc.top;
			right	= (FLOAT)rc.right;
			bottom  = (FLOAT)rc.bottom;
		}
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MathConst


	class ECORE_API MathConst
	{
	public:
		MathConst();
		~MathConst();

	public:
		static Vector3			AxisX;
		static Vector3			AxisY;
		static Vector3			AxisZ;
		static Vector3			NegAxisX;
		static Vector3			NegAxisY;
		static Vector3			NegAxisZ;
		static Vector2			Zero2;
		static Vector3			Zero3;
		static Vector4			Zero4;
		static Vector4			Zero3One1;
		static Vector3			Max3;
		static Vector3			One3;
		static Matrix4			IdentityMat;
		static Matrix3x4		IdentityMat3x4;
		static Matrix4			RH2LHMat;//右手系坐标转换到左手系坐标
		static Matrix4			LH2RHMat;//左手系坐标转换到右手系坐标
		static AABBox			SmallBox;
		static AABBox			BigBox;
		static Quaternion		IdentityQuat;
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Rect


	struct ECORE_API Rect : public tagRECT
	{
		Rect() { left = 0; top = 0; right = 0; bottom = 0; }
		Rect(int _left,int _top,int _right,int _bottom)
		{
			SetLTRB( _left, _top, _right, _bottom );
		}
		INLINE VOID SetLTRB( int _left,int _top,int _right,int _bottom )
		{
			left	= _left;
			top		= _top;
			right	= _right;
			bottom	= _bottom;
		}
		INLINE VOID SetLR( int _left,int _right )
		{
			left	= _left;
			right	= _right;
		}
		INLINE VOID SetTB( int _top,int _bottom )
		{
			top		= _top;
			bottom	= _bottom;
		}
		INLINE int Width() const
		{
			return right-left;
		}
		INLINE int Height() const
		{
			return bottom-top;
		}
		INLINE BOOL Intersect(const Rect& other) const 
		{
			if( left > other.right
				|| right < other.left
				|| top > other.bottom
				|| bottom < other.top )
				return FALSE;

			return TRUE;
		}
		INLINE void Intersect(const Rect& other,Rect& out) const
		{
			out.left	= EMax( left, other.left );
			out.right	= EMin( right, other.right );
			out.top		= EMax( top, other.top );
			out.bottom	= EMin( bottom, other.bottom );
		}
		INLINE BOOL Contain(int x,int y) const
		{
			return x>=left
				&& x<right
				&& y>=top
				&& y<bottom;
		}
		INLINE BOOL operator==(const Rect& other) const
		{
			return left==other.left
				&& right==other.right
				&& top==other.top
				&& bottom==other.bottom;
		}
		INLINE BOOL operator!=(const Rect& other) const
		{
			return left!=other.left
				|| right!=other.right
				|| top!=other.top
				|| bottom!=other.bottom;
		}	
		INLINE int Area() const
		{
			return ( right - left ) * ( bottom - top );
		}
		INLINE void operator=(const RECT& rhs)
		{
			left = rhs.left;
			right = rhs.right;
			top = rhs.top;
			bottom = rhs.bottom;
		}
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Point


	struct ECORE_API Point
	{
	public:
		LONG  x;
		LONG  y;
		Point(): x( 0 ), y( 0 ){}
		Point(LONG _x,LONG _y) : x( _x ), y( _y )
		{
		}
		Point( const tagPOINT& other ) : x( other.x ), y( other.y )
		{
		}
		Point( const Point& other ) : x( other.x ), y( other.y )
		{
		}
		~Point(){}

		INLINE bool operator==(const tagPOINT& other) const
		{
			return x==other.x
				&& y==other.y;
		}

		INLINE bool operator!=(const tagPOINT& other) const
		{
			return x!=other.x
				|| y!=other.y;
		}

		INLINE Point operator+(const tagPOINT& other) const
		{
			return Point( x + other.x, y + other.y );
		}

		INLINE const Point& operator=(const tagPOINT& other)
		{
			x = other.x;
			y = other.y;
			return *this;
		}

		INLINE void operator+=(const tagPOINT& other)
		{
			x += other.x;
			y += other.y;
		}

		INLINE bool operator==(const Point& other) const
		{
			return x==other.x
				&& y==other.y;
		}

		INLINE bool operator!=(const Point& other) const
		{
			return x!=other.x
				|| y!=other.y;
		}

		INLINE Point operator+(const Point& other) const
		{
			return Point( x + other.x, y + other.y );
		}

		INLINE const Point& operator=(const Point& other)
		{
			x = other.x;
			y = other.y;
			return *this;
		}

		INLINE void operator+=(const Point& other)
		{
			x += other.x;
			y += other.y;
		}
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 全局函数


	/** Sphere / box intersection test. 
	*/
	bool Intersect(const Sphere& sphere, const AABBox& box);
	/** Plane / box intersection test. 
	*/
	bool Intersect(const Plane& plane, const AABBox& box);
	/** Sphere / plane intersection test. 
	@remarks NB just do a plane.getDistance(sphere.getCenter()) for more detail!
	*/
	bool Intersect(const Sphere& sphere, const Plane& plane);

	/**	计算一个三角形的中心点	
	*/
	ECORE_API void TriangleCentroid(const Vector3& p1,const Vector3& p2,const Vector3& p3,Vector3& out);
	/**	计算一个三角形的面积	
	*/
	ECORE_API float TriangleArea(const Vector3& p1,const Vector3& p2,const Vector3& p3);
	/** 计算一个三角形的2D面积
	*/
	INLINE FLOAT TriangleArea2D(const Vector3& a, const Vector3& b, const Vector3& c)
	{
		return fabsf( (b.x - a.x) * (c.z - a.z) - (c.x - a.x) * (b.z - a.z) ) * 0.5f;
	}

	/** 生成一个平面
	*/
	ECORE_API void BuildPlane(const Vector3& p1,const Vector3& p2,const Vector3& p3,Plane& out);

	/** 计算两个Point的距离 */
	INLINE float PointDistSqF( const Point& p1, const Point& p2 )
	{
		return float( (p1.x-p2.x) * (p1.x-p2.x) + (p1.y-p2.y) * (p1.y-p2.y) );
	}

	/** 计算两个Point的距离 */
	INLINE int PointDistSq( const Point& p1, const Point& p2 )
	{
		return (p1.x-p2.x) * (p1.x-p2.x) + (p1.y-p2.y) * (p1.y-p2.y);
	}

	/** 计算点到线段的2D距离 */
	INLINE FLOAT DistancePtSegSq2D( const Vector3& v, const Vector3& p, const Vector3& q )
	{
		FLOAT pqx = q.x - p.x;
		FLOAT pqz = q.z - p.z;
		FLOAT dx = v.x - p.x;
		FLOAT dz = v.z - p.z;
		FLOAT d = pqx*pqx + pqz*pqz;
		FLOAT t = pqx*dx + pqz*dz;
		if (d > 0)
			t /= d;
		if (t < 0)
			t = 0;
		else if (t > 1)
			t = 1;

		dx = p.x + t*pqx - v.x;
		dz = p.z + t*pqz - v.z;

		return dx*dx + dz*dz;
	}

	/** 计算点到线段的2D距离，并返回线段上的最近点 */
	INLINE FLOAT DistancePtSegSq2D( const Vector3& v, const Vector3& p, const Vector3& q, OUT Vector3& c )
	{
		FLOAT pqx = q.x - p.x;
		FLOAT pqz = q.z - p.z;
		FLOAT pqy = q.y - p.y;
		FLOAT dx = v.x - p.x;
		FLOAT dz = v.z - p.z;
		FLOAT d = pqx*pqx + pqz*pqz;
		FLOAT t = pqx*dx + pqz*dz;
		if (d > 0)
			t /= d;
		if (t < 0)
			t = 0;
		else if (t > 1)
			t = 1;

		c.x = p.x + t*pqx;
		c.z = p.z + t*pqz;
		c.y = p.y + t*pqy; // y和pqy是不是不用算？

		dx = c.x - v.x;
		dz = c.z - v.z;

		return dx*dx + dz*dz;
	}

	/** 计算点到线段的距离 */
	INLINE FLOAT DistancePtSegSq(const Vector3& pt, const Vector3& p, const Vector3& q)
	{
		FLOAT pqx = q.x - p.x;
		FLOAT pqy = q.y - p.y;
		FLOAT pqz = q.z - p.z;
		FLOAT dx = pt.x - p.x;
		FLOAT dy = pt.y - p.y;
		FLOAT dz = pt.z - p.z;
		FLOAT d = pqx*pqx + pqy*pqy + pqz*pqz;
		FLOAT t = pqx*dx + pqy*dy + pqz*dz;
		if (d > 0)
			t /= d;
		if (t < 0)
			t = 0;
		else if (t > 1)
			t = 1;

		dx = p[0] + t*pqx - pt[0];
		dy = p[1] + t*pqy - pt[1];
		dz = p[2] + t*pqz - pt[2];

		return dx*dx + dy*dy + dz*dz;
	}

	/** 判断点是否在线段的左边 */
	INLINE BOOL PtOnSegLeft2D( const Vector3& v, const Vector3& p, const Vector3& q )
	{
		return (q.x - p.x) * (v.z - p.z) -
			   (v.x - p.x) * (q.z - p.z) > 0.f;
	}

	/** 判断点是否在线段的右边 */
	INLINE BOOL PtOnSegRight2D( const Vector3& v, const Vector3& p, const Vector3& q )
	{
		return (q.x - p.x) * (v.z - p.z) -
			   (v.x - p.x) * (q.z - p.z) < 0.f;
	}

	/** 判断点在线段的哪一边 */
	INLINE INT PtOnSeg2D( const Vector3& v, const Vector3& p, const Vector3& q )
	{
		FLOAT c = (q.x - p.x) * (v.z - p.z) -
				  (v.x - p.x) * (q.z - p.z);
		if( c > 0.f )
			return 0;	// Left
		else if( c < 0.f )
			return 1;	// Right
		else
			return 2;	// On
	}

	/** 线段相交测试 */
	INLINE BOOL IntersectSeg2D( const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d )
	{
		return PtOnSeg2D( c, a, b ) != PtOnSeg2D( d, a, b )
			&& PtOnSeg2D( a, c, d ) != PtOnSeg2D( b, c, d );
	}

	/** 点到三角形的距离 */
	INLINE FLOAT DistancePtTri(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
	{
		Vector3 v0, v1, v2;
		v0 = c - a;
		v1 = b - a;
		v2 = p - a;

		const FLOAT dot00 = Vec3Dot2(v0, v0);
		const FLOAT dot01 = Vec3Dot2(v0, v1);
		const FLOAT dot02 = Vec3Dot2(v0, v2);
		const FLOAT dot11 = Vec3Dot2(v1, v1);
		const FLOAT dot12 = Vec3Dot2(v1, v2);

		// Compute barycentric coordinates
		FLOAT invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
		FLOAT u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		FLOAT v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		// If point lies inside the triangle, return interpolated y-coord.
		static const FLOAT EPS = 1e-4f;
		if (u >= -EPS && v >= -EPS && (u+v) <= 1+EPS)
		{
			FLOAT y = a.y + v0.y*u + v1.y*v;
			return fabsf(y-p.y);
		}
		return FLT_MAX;
	}

	/** 获取三角形高度 */
	INLINE FLOAT GetTriHeight(FLOAT x, FLOAT z, const Vector3& a, const Vector3& b, const Vector3& c)
	{
		Vector3 v0, v1, v2;
		v0 = c - a;
		v1 = b - a;
		v2.x = x - a.x;
		v2.z = z - a.z;

		const FLOAT dot00 = Vec3Dot2(v0, v0);
		const FLOAT dot01 = Vec3Dot2(v0, v1);
		const FLOAT dot02 = Vec3Dot2(v0, v2);
		const FLOAT dot11 = Vec3Dot2(v1, v1);
		const FLOAT dot12 = Vec3Dot2(v1, v2);

		// Compute barycentric coordinates
		FLOAT invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
		FLOAT u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		FLOAT v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		// If point lies inside the triangle, return interpolated y-coord.
		static const FLOAT EPS = 1e-4f;
		if (u >= -EPS && v >= -EPS && (u+v) <= 1+EPS)
		{
			return a.y + v0.y*u + v1.y*v;
		}
		return FLT_MAX;
	}

	/** 点是否在三角形内 */
	INLINE BOOL PtInTri2D(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
	{
		Vector3 v0, v1, v2;
		v0.x = c.x - a.x;
		v0.z = c.z - a.z;
		v1.x = b.x - a.x;
		v1.z = b.z - a.z;
		v2.x = p.x - a.x;
		v2.z = p.z - a.z;

		const FLOAT dot00 = Vec3Dot2(v0, v0);
		const FLOAT dot01 = Vec3Dot2(v0, v1);
		const FLOAT dot02 = Vec3Dot2(v0, v2);
		const FLOAT dot11 = Vec3Dot2(v1, v1);
		const FLOAT dot12 = Vec3Dot2(v1, v2);

		// Compute barycentric coordinates
		FLOAT invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
		FLOAT u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		FLOAT v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		// If point lies inside the triangle, return interpolated y-coord.
		static const FLOAT EPS = 1e-4f;
		return (u >= -EPS && v >= -EPS && (u+v) <= 1+EPS);
	}

}//endof namespace ECore
