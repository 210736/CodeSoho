#pragma once

namespace ECore
{
	class ECORE_API Color4f;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Color4ub


	/** four unsigned char component color
	*/
	class ECORE_API Color4ub
	{
	public:
		BYTE R,G,B,A;

		Color4ub()
			: A(255), R(0), G(0), B(0)
		{}
		Color4ub(unsigned char A,unsigned char R,unsigned char G,unsigned char B):A(A),R(R),G(G),B(B)
		{}
		Color4ub(DWORD color)
		{
			A=(BYTE)(color>>24);
			R=(BYTE)((color>>16)&0x000000FF);
			G=(BYTE)((color>>8)&0x000000FF);
			B=(BYTE)((color)&0x000000FF);
		}
		INLINE Color4ub(const Color4f& c4f);

		// casting
		INLINE operator DWORD () const
		{	return (A << 24) | (R << 16) | (G << 8) | B;	}
	};



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Color4f


	/** four float component color
	*/
	class ECORE_API Color4f
	{
	public:
		float R,G,B,A;

		Color4f()
			: A(1.0f) ,R(0.0f) ,G(0.0f) ,B(0.0f)
		{}
		Color4f(float a,float r,float g,float b)
			: A(a), R(r), G(g), B(b)
		{}
		Color4f(DWORD color)
		{
			A=(color>>24)/255.0f;
			R=((color>>16)&0x000000FF)/255.0f;
			G=((color>>8)&0x000000FF)/255.0f;
			B=((color)&0x000000FF)/255.0f;
		}
		Color4f(const Vector4& vec):R(vec.x),G(vec.y),B(vec.z),A(vec.w)
		{}
		Color4f(const Color4ub& color)
		{
			A=color.A/255.0f;
			R=color.R/255.0f;
			G=color.G/255.0f;
			B=color.B/255.0f;
		}

		INLINE bool operator == (const Color4f& other) const
		{	return other.R==R && other.G==G && other.B==B && other.A==A; }

		INLINE bool operator != (const Color4f& other) const
		{	return other.R!=R || other.G!=G || other.B!=B || other.A!=A; }

		INLINE const Color4f& operator * (float r)
		{
			R*=r;	G*=r;	B*=r;	A*=r;
			return *this;
		}

		INLINE const Color4f& operator / (float r)
		{
			R/=r;	G/=r;	B/=r;	A/=r;
			return *this;
		}

		INLINE const Color4f& operator *= (const Color4f& other)
		{
			R*=other.R;	G*=other.G;	B*=other.B;	A*=other.A;
			return *this;
		}

		INLINE const Color4f& operator *= (float r)
		{
			R*=r;	G*=r;	B*=r;	A*=r;
			return *this;
		}

		INLINE const Color4f& operator + (const Color4f& c)
		{
			R+=c.R;
			G+=c.G;
			B+=c.B;
			A+=c.A;
			return *this;
		}

		INLINE const Color4f& operator += (const Color4f& c)
		{
			R+=c.R;
			G+=c.G;
			B+=c.B;
			A+=c.A;
			return *this;
		}

		INLINE const Color4f& operator /= (float f)
		{
			R/=f;
			G/=f;
			B/=f;
			A/=f;
			return *this;
		}
	};

	INLINE Color4f operator * (const Color4f& c1,const Color4f& c2)
	{
		Color4f ret;
		ret.R=c1.R*c2.R;
		ret.G=c1.G*c2.G;
		ret.B=c1.B*c2.B;
		ret.A=c1.A*c2.A;
		return ret;
	}

	INLINE Color4ub::Color4ub(const Color4f& c4f)
	{
		A = (BYTE)(c4f.A*255);
		R = (BYTE)(c4f.R*255);
		G = (BYTE)(c4f.G*255);
		B = (BYTE)(c4f.B*255);
	}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ColorConst


    class ECORE_API ColorConst
    {
    public:
        ColorConst();
        ~ColorConst();

    public:
        static const DWORD BLACK    = 0xFF000000;
        static const DWORD WHITE    = 0xFFFFFFFF;
        static const DWORD RED      = 0xFFFF0000;
        static const DWORD GREEN    = 0xFF00FF00;
        static const DWORD BLUE     = 0xFF0000FF;
        static const DWORD GRAY     = 0xFF808080;
        static const DWORD YELLOW   = 0xFFFFFF00;
        static const DWORD ORANGE   = 0xFFFF8000;
    };


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Lerp


	INLINE void LinearLerpColor4f( const Color4f& c1, const Color4f& c2, float x, Color4f& oc )
	{
		oc.A=(c1.A*(1-x)+c2.A*x);
		oc.R=(c1.R*(1-x)+c2.R*x);
		oc.G=(c1.G*(1-x)+c2.G*x);
		oc.B=(c1.B*(1-x)+c2.B*x);
	}

	INLINE void LinearLearpColor4ub( const Color4ub& c1, const Color4ub& c2, float x, Color4ub& oc )
	{
		oc.A=BYTE(c1.A*(1-x)+c2.A*x);
		oc.R=BYTE(c1.R*(1-x)+c2.R*x);
		oc.G=BYTE(c1.G*(1-x)+c2.G*x);
		oc.B=BYTE(c1.B*(1-x)+c2.B*x);
	}

}//namespace ECore
