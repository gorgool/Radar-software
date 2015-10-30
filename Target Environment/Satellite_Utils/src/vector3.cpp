#define _USE_MATH_DEFINES
#include <math.h>
#include "../include/vector3.h"

// scal - ��������� ������������ �� ������ v
double tVECT3::scal(const tVECT3 &v)const
{
  return x*v.x + y*v.y + z*v.z;
}

// mod - ������ �������
double tVECT3::mod()const
{
  return sqrt(x*x + y*y + z*z);
}

// mod2 - ������� ������ �������
double tVECT3::mod2()const
{
  return x*x + y*y + z*z;
}

// ����� ������-�������
double tVECT3::R()const
{
  return sqrt(x*x + y*y + z*z);
}

// norm - ���������� �������. ��������� ���������� ������ �������
double tVECT3::norm()
{
  double d = mod();
  if (d>0)
  {
    x /= d; y /= d; z /= d;
  }
  return d;
}

// mul - ��������� ������������ ���� ��������
// ������������� ��� ������� �������
void tVECT3::mul
(const tVECT3 &v1
, const tVECT3 &v2)
{
  x = v1.y*v2.z - v1.z*v2.y;
  y = v1.z*v2.x - v1.x*v2.z;
  z = v1.x*v2.y - v1.y*v2.x;
} // tVECT3::mul

// turn - ������� ������� ������ �������� ���
void tVECT3::turn
(long   na    // ����� ���: 1 - x, 2 - y, 3 - z
, double ca    // ������� �
, double sa    //  ����� ���� ��������
)
{
  double d;
  switch (na)
  {
  case 1: { // x
            d = ca*y - sa*z;
            z = ca*z + sa*y;
            y = d;
            break;
  }
  case 2: { // y
            d = ca*z - sa*x;
            x = ca*x + sa*z;
            z = d;
            break;
  }
  case 3: { // z
            d = ca*x - sa*y;
            y = ca*y + sa*x;
            x = d;
            break;
  }
  } // switch
} // tVECT3::turn

// ��������� ---------------------

// ��������� ��������� ��������
tVECT3 tVECT3::operator +(const tVECT3 &v)const
{
  return tVECT3(x + v.x, y + v.y, z + v.z);
}

// ��������� ��������� ���������
tVECT3 tVECT3::operator -(const tVECT3 &v)const
{
  return tVECT3(x - v.x, y - v.y, z - v.z);
}

// ���������� �������� ��������� - (�������� ��������).
tVECT3 tVECT3::operator -()const
{
  return tVECT3(-x, -y, -z);
}

// ��������� ������������
tVECT3 tVECT3::operator *(const tVECT3 &v)const
{
  return tVECT3(
    y*v.z - z*v.y
    , z*v.x - x*v.z
    , x*v.y - y*v.x
    );
}

// ���������� ��������� ��������� �� ����� ������.
tVECT3 tVECT3::operator *(const double c)const
{
  return tVECT3(x*c, y*c, z*c);
}

// ���������� ��������� ��������� �� ����� �����.
tVECT3 operator *(const double c, const tVECT3 &v)
{
  return tVECT3(v.x*c, v.y*c, v.z*c);
}

// ���������� ��������� ������� �� �����.
tVECT3 tVECT3::operator /(const double c)const
{
  return tVECT3(x / c, y / c, z / c);
}

// ������ �������
double mod(const tVECT3 &v)
{
  return(sqrt(v.x*v.x + v.y*v.y + v.z*v.z));
}

// ������������� ������
tVECT3 &norm(tVECT3 &v)
{
  double d = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
  if (d > 0)
  {
    v.x /= d; v.y /= d; v.z /= d;
  }
  return v;
}

// ��������� ������������
double scal(const tVECT3 &u, const tVECT3 &v)
{
  return(u.x*v.x + u.y*v.y + u.z*v.z);
}

// ���������� �� ������
tVECT3 &tVECT3::operator +=(const tVECT3 &v)
{
  x += v.x; y += v.y; z += v.z; return *this;
}

// ���������� �� ������
tVECT3 &tVECT3::operator -=(const tVECT3 &v)
{
  x -= v.x; y -= v.y; z -= v.z; return *this;
}

// ��������� �� �����
tVECT3 &tVECT3::operator *=(const double d)
{
  x *= d; y *= d; z *= d; return *this;
}

// ������� �� �����
tVECT3 &tVECT3::operator /=(const double d)
{
  x /= d; y /= d; z /= d; return *this;
}