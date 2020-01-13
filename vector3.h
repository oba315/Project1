#ifndef VECTOR_3_H
#define VECTOR_3_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>


//----------------------------------------------------------------------------

class Vector3
{
public:
	double	x, y, z;

	// コンストラクタ
	Vector3() { x = y = z = 0;}
	Vector3(double a, double b, double c ) { x=a; y=b; z=c;}
	/// 代入
	Vector3& operator = ( const Vector3& b )
	{
		x = b.x;
		y = b.y;
		z = b.z;
		return *this;
	}

	/// 正負反転
	Vector3 operator - ()
	{
		return Vector3( -x, -y, -z );
	}

	/// 和
	Vector3 operator + ( const Vector3& b )
	{
		return Vector3( x + b.x, y + b.y, z + b.z );
	}
	/// 差
	Vector3 operator - ( const Vector3& b )
	{
		return Vector3( x - b.x, y - b.y, z - b.z );
	}
	/// ベクトル x 数値
	Vector3 operator * ( double t )
	{
		return Vector3( x * t, y * t, z * t );
	}
	///  数値 x ベクトル
	friend inline Vector3 operator * ( double t, Vector3& b )
	{
		return b * t;
	}
	/// 商
	Vector3 operator / ( const double t ) 
	{
		return Vector3( x / t, y / t, z / t );
	}

	// 自身に対する和差積商
	/// 和
	Vector3& operator += ( Vector3& b ) 
	{
		x += b.x;
		y += b.y;
		z += b.z;
		return *this;
	}
	/// 差
	Vector3& operator -= ( Vector3& b ) 
	{
		x -= b.x;
		y -= b.y;
		z -= b.z;
		return *this;
	}
	/// 積
	Vector3& operator *= ( double t ) 
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}
	/// 商
	Vector3& operator /= ( double t )
	{
		x /= t;
		y /= t;
		z /= t;
		return *this;
	}

	/// 内積
	double dot( const Vector3& b )
	{
		return x * b.x + y * b.y + z * b.z;
	}

	/// 外積
	Vector3 cross( const Vector3& b )
	{
		return Vector3( y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x );
	}

	/// 長さ
	double lengthSquared() 
	{
		return this->dot(*this);
	}
	double length() 
	{
		return (double)sqrt( (double)this->lengthSquared() );
	}

	/// 単位ベクトル化
	Vector3& normalize()
	{
		(*this) /= this->length();
		return *this;
	}

	// 比較
	/// 同じか
	bool operator == ( Vector3& b )
	{
		return( b.x==x && b.y==y && b.z == z );
	}

	/// 異なるか
	bool operator != ( Vector3& b )
	{
		return !( b == *this );
	}

	// print
	void print() {
		std::cout << "[" << x << " " << y << " " << z << "]";
	}
};

#endif