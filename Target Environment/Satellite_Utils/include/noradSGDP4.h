#ifndef NORAD_SGDP4_H
#define NORAD_SGDP4_H

#include "norad_base.h"

class NoradSGDP4 : public NoradBase
{
public:
	NoradSGDP4(const KEO &keo,IMode imode);
	virtual ~NoradSGDP4() { }
	virtual bool getPosition(double tsince,tVECT3 &v0,tVECT3 &v1,CSType cs) const=0;

protected:
	friend struct D_SGDP;
	/// \brief Необходимые орбитальные параметры, вычисляемые один раз
	double m_betao2;
	double m_betao;
	double m_s4;
	double m_qoms24;
	double m_tsi;
	double m_eta;
	double m_eeta;
	double m_coef;
	double m_coef1;
	double m_c1;
	double m_c3;
	double m_c4;
	double m_a3ovk2;
	double m_xmdot;
	double m_omgdot;
	double m_xnodot;
	double m_xnodcf;
	double m_t2cof;
	double m_cosio;
	double m_sinio;
	mutable double m_xlcof;
	mutable double m_aycof;
	mutable double m_x3thm1;
	mutable double m_x1mth2;
	mutable double m_x7thm1;

	/// \brief Данные для расчета
	struct CalcData {
		double xmp;
		double xnode;    ///< Модифицированная долгота восходящего узла
		double omega;    ///< Модифицированный аргумент перигея
		double tempa;
		double tempe;
		double templ;
		double a;
		double e;         ///< Модифицированный эксцентриситет
		double xinc;      ///< Модифицированное наклонение
		double xl;
		double tsince;    ///< Время относительно эпохи привязки (минут)
		double sinio;     ///< Изменяемый синус наклонения
		double cosio;     ///< Изменяемый косинус наклонения
		CSType cs;        ///< Тип СК, в которой представляются результаты
	};

	/// \brief Инициализация данных перед расчетом
	void _initCalcData(double tsince,CalcData &cd,CSType cs) const;

	/// \brief Основной расчет положения
	bool _get_position(const CalcData &cd,tVECT3 &v0,tVECT3 &v1) const;
};
//------------------------------------------------------------------------------
#endif // NORAD_SGDP4_H
