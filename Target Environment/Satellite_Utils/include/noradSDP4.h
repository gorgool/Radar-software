#ifndef NORAD_SDP4_H
#define NORAD_SDP4_H

#include "noradSGDP4.h"

class NoradSDP4 : public NoradSGDP4
{
public: 

	NoradSDP4(const KEO &keo,IMode imode,double epoch);
	~NoradSDP4();

	/*
    Эта процедура формирует положение и скорость спутника на орбите
	  для заданного числа минут, прошедшего после эпохи TLE, используя NORAD
	  Simplified Deep Perturbation 4 модель, модель для далеких от Земли
	  спутников
  */
	virtual bool getPosition(double tsince,tVECT3 &v0,tVECT3 &v1,CSType cs) const;

protected:
	friend struct D_SDP;
	/// \brief Расчетные параметры
	struct DeepCalcData {
		CalcData cd;
		double xn;
		double ecc;
		double xmam;           ///< Модифицированная средняя аномалия
		// параметры Луны и Солнца
		double pgh;
		double ph;
		double pe;
		double pinc;
		double pl;
	};

	/// \brief Расчет параметров скорости
	void _dot_terms_calculated() const;

	/// \brief Расчет вековых эффектов для глубокого космоса
	/// \param[out] dcd Формируемая структура расчетных параметров
	bool _secular(DeepCalcData &dcd) const;

	/// \brief Подготовка параметров Луны и Солнца
	/// \param[out] dcd Формируемая структура расчетных параметров
	/// \details Do the Lunar-Solar terms for the SGDP4_dpper() function (normally
	/// only every 1/2 hour needed. Seperate function so initialisng could save the
	/// epoch terms to zero them. Not sure if this is a good thing (some believe
	/// it the way the equations were intended) as the two-line elements may
	/// be computed to give the right answer with out this (which I would hope
	/// as it would make predictions consistant with the 'official' model code)
	void _compute_LunarSolar(DeepCalcData &dcd) const;

	/// \brief Расчет влияния периодичности Луны и Солнца
	/// \param[out] dcd Формируемая структура расчетных параметров
	void _deep_periodics(DeepCalcData &dcd) const;

	/// \brief Необходимые орбитальные параметры, вычисляемые один раз
	double dp_e3;
	double dp_ee2;
	double dp_se2;
	double dp_se3;
	double dp_sgh2;
	double dp_sgh3;
	double dp_sgh4;
	double dp_sh2;
	double dp_sh3;
	double dp_si2;
	double dp_si3;
	double dp_sl2;
	double dp_sl3;
	double dp_sl4;
	double dp_xgh2;
	double dp_xgh3;
	double dp_xgh4;
	double dp_xh2;
	double dp_xh3;
	double dp_xi2;
	double dp_xi3;
	double dp_xl2;
	double dp_xl3;
	double dp_xl4;
	double dp_zmol;
	double dp_zmos;

	double dp_d2201;
	double dp_d2211;
	double dp_d3210;
	double dp_d3222;
	double dp_d4410;
	double dp_d4422;
	double dp_d5220;
	double dp_d5232;
	double dp_d5421;
	double dp_d5433;
	double dp_del1;
	double dp_del2;
	double dp_del3;
	double dp_sse;
	double dp_ssg;
	double dp_ssh;
	double dp_ssi;
	double dp_ssl;
	double dp_thgr;
	double dp_xfact;
	double dp_xlamo;
	double dp_xnddt0;
	double dp_xndot0;
	double dp_xldot0;
	mutable double dp_atime;
	mutable double dp_xli;
	mutable double dp_xni;
	// параметры скорости
	mutable double dp_xnddt;
	mutable double dp_xndot;
	mutable double dp_xldot;

	int dp_iresfl;
	int dp_isynfl;

	const int m_ilsd;
};
extern NoradSDP4 *nsdp0;
extern NoradSDP4 *nsdp1;
//------------------------------------------------------------------------------
#endif // NORAD_SDP4_H
