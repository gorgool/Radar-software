#ifndef NORAD_SGP4_H
#define NORAD_SGP4_H

#include "noradSGDP4.h"

class NoradSGP4 : public NoradSGDP4
{
public:
	NoradSGP4(const KEO &keo,IMode imode);
	~NoradSGP4() { }

	
	/* 
    Эта процедура формирует положение и скорость спутника на орбите
	  для заданного числа минут, прошедшего после эпохи TLE, используя NORAD
	  Simplified General Perturbation 4 модель, модель для близких к Земле
	  спутников
  */
	virtual bool getPosition(double tsince,tVECT3 &v0,tVECT3 &v1,CSType cs) const;

protected:
	/// \brief Необходимые орбитальные параметры, вычисляемые один раз
	double m_c5;
	double m_omgcof;
	double m_xmcof;
	double m_delmo;
	double m_sinmo;
	double m_d2;
	double m_d3;
	double m_d4;
	double m_t3cof;
	double m_t4cof;
	double m_t5cof;
};
//------------------------------------------------------------------------------
#endif // NORAD_SGP4_H
