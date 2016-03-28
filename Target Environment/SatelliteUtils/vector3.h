#ifndef VECTOR3_H
#define VECTOR3_H

class tVECT3{
public:
  double x, y, z;
  tVECT3() :x(0), y(0), z(0){}
  tVECT3(const double X, const double Y, const double Z)
    :x(X), y(Y), z(Z){}

  double scal(const tVECT3 &v) const; // ��������� ������������ �� ������ v
  double mod() const;                 // mod - ������ �������
  double mod2() const;                // mod2 - ������� ������ �������
  double R() const;                   // ����� ������-�������
  double norm();                      // norm - ���������� �������.

  // mul - ��������� ������������ ���� ��������
  // ������������� ��� ������� �������
  void mul(const tVECT3 &v1, const tVECT3 &v2);

  // turn - ������� ������� ������ �������� ���
  void turn(const long   na    // ����� ���: 1 - x, 2 - y, 3 - z
    , const double ca    // ������� �
    , const double sa    //  ����� ���� ��������
    );

  // ��������� ---------------------
  tVECT3 operator +(const tVECT3 &v)const;// ���������� ��������� ��������
  tVECT3 operator -(const tVECT3 &v)const;// ���������� ��������� ���������
  tVECT3 operator -()const;               // ������� - (�������� ��������)
  tVECT3 operator *(const tVECT3 &v)const;// ��������� ������������
  tVECT3 operator *(const double c)const; // ��������� �� ����� ������
  friend tVECT3 operator *(const double c, const tVECT3 &v); // ��������� �� ����� �����
  double operator ^(const tVECT3 &v)const // �������� ^ (��������� ������������)
  {
    return x*v.x + y*v.y + z*v.z;
  }
  tVECT3 operator /(const double c)const; // ������� �� �����.

  friend double mod(const tVECT3 &v);   // ������ �������
  friend tVECT3 &norm(tVECT3 &v);       // ������������� ������
  friend double scal(const tVECT3 &u, const tVECT3 &v); // ��������� ������������
  tVECT3 &operator +=(const tVECT3 &v); // ���������� �� ������
  tVECT3 &operator -=(const tVECT3 &v); // ���������� �� ������
  tVECT3 &operator *=(const double d);  // ��������� �� �����
  tVECT3 &operator /=(const double d);  // ������� �� �����
}; // class tVECT3

#endif