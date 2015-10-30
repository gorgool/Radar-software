#ifndef VECTOR3_H
#define VECTOR3_H

class tVECT3{
public:
  double x, y, z;
  tVECT3() :x(0), y(0), z(0){}
  tVECT3(const double X, const double Y, const double Z)
    :x(X), y(Y), z(Z){}

  double scal(const tVECT3 &v) const; // Скалярное произведение на вектор v
  double mod() const;                 // mod - Модуль вектора
  double mod2() const;                // mod2 - Квадрат модуля вектора
  double R() const;                   // Длина радиус-вектора
  double norm();                      // norm - Нормировка вектора.

  // mul - Векторное произведение двух векторов
  // перемножаются два входных вектора
  void mul(const tVECT3 &v1, const tVECT3 &v2);

  // turn - поворот вектора вокруг заданной оси
  void turn(const long   na    // Номер оси: 1 - x, 2 - y, 3 - z
    , const double ca    // Косинус и
    , const double sa    //  синус угла поворота
    );

  // Операторы ---------------------
  tVECT3 operator +(const tVECT3 &v)const;// Перегрузка оператора сложения
  tVECT3 operator -(const tVECT3 &v)const;// Перегрузка оператора вычитания
  tVECT3 operator -()const;               // Унарный - (обратное значение)
  tVECT3 operator *(const tVECT3 &v)const;// Векторное произведение
  tVECT3 operator *(const double c)const; // Умножения на число справа
  friend tVECT3 operator *(const double c, const tVECT3 &v); // Умножение на число слева
  double operator ^(const tVECT3 &v)const // Оператор ^ (скалярное произведение)
  {
    return x*v.x + y*v.y + z*v.z;
  }
  tVECT3 operator /(const double c)const; // Деление на число.

  friend double mod(const tVECT3 &v);   // Модуль вектора
  friend tVECT3 &norm(tVECT3 &v);       // Нормированный вектор
  friend double scal(const tVECT3 &u, const tVECT3 &v); // Скалярное произведение
  tVECT3 &operator +=(const tVECT3 &v); // Увеличение на вектор
  tVECT3 &operator -=(const tVECT3 &v); // Уменьшение на вектор
  tVECT3 &operator *=(const double d);  // Умножение на число
  tVECT3 &operator /=(const double d);  // Деление на число
}; // class tVECT3

#endif