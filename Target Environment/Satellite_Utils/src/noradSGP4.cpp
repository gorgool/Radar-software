#include "../include/noradSGP4.h"
#include "../include/catalog_tle.h"

//------------------------------------------------------------------------------
NoradSGP4::NoradSGP4(const KEO &keo,IMode imode)
  : NoradSGDP4(keo,imode)
{
	// конструктор

	m_c5=0.0;
	m_omgcof=0.0;
	m_xmcof=0.0;

	double etasq=m_eta * m_eta;

	if(_imode==SGDP4_NEAR_NORM){
		m_c5=2.0*m_coef1*_keo.semi_major_r*m_betao2*(1.0+2.75*(etasq+m_eeta)+
		                                             m_eeta*etasq);
		if(keo.eccentricity>TleConst::ECC_ALL){
			m_c3=m_coef*m_tsi*m_a3ovk2*_keo.mean_motion_r*TleConst::AE*
			    m_sinio/_keo.eccentricity;
		}

		m_omgcof=_keo.b_star*m_c3*cos(_keo.arg_perigee);
	}

	if(keo.eccentricity>TleConst::ECC_ALL){
		m_xmcof=-TleConst::TOTHRD*m_coef*_keo.b_star*TleConst::AE/m_eeta;
	}

	m_sinmo=sin(_keo.mean_anomaly);
	m_delmo=pow(1.0+m_eta*cos(_keo.mean_anomaly),3.0);

	double c1sq=m_c1*m_c1;
	m_d2=4.0*keo.semi_major_r*m_tsi*c1sq;
	double temp0=m_d2*m_tsi*m_c1/3.0;
	m_d3=(17.0*keo.semi_major_r+m_s4)*temp0;
	m_d4=0.5*temp0*keo.semi_major_r*m_tsi*(221.0*keo.semi_major_r+31.0*m_s4)*m_c1;
	m_t3cof=m_d2+2.0*c1sq;
	m_t4cof=0.25*(3.0*m_d3+m_c1*(12.0*m_d2+10.0*c1sq));
	m_t5cof=0.2*(3.0*m_d4+12.0*m_c1*m_d3+6.0*m_d2*m_d2+15.0*c1sq*(2.0*m_d2+c1sq));
}
//------------------------------------------------------------------------------
bool NoradSGP4::getPosition(double tsince,tVECT3 &v0,tVECT3 &v1,CSType cs) const
{
	// получение положения и скорости на заданное время

	CalcData cd;
	_initCalcData(tsince,cd,cs);

	double temp0;
	double delm;

	switch(_imode){
	case SGDP4_NEAR_SIMP:
		cd.tempa=1.0-tsince*m_c1;
		cd.tempe=_keo.b_star*tsince*m_c4;
		cd.templ=tsince*tsince*m_t2cof;
		break;

	case SGDP4_NEAR_NORM:
		delm=m_xmcof*(pow(1.0+m_eta*cos(cd.xmp),3.0)-m_delmo);
		temp0=tsince*m_omgcof+delm;
		cd.xmp+=temp0;
		cd.omega-=temp0;
		cd.tempa=1.0-(tsince*(m_c1+tsince*(m_d2+tsince*(m_d3+tsince*m_d4))));
		cd.tempe=_keo.b_star*(m_c4*tsince+m_c5*(sin(cd.xmp)-m_sinmo));
		cd.templ=tsince*tsince*(m_t2cof+tsince*(m_t3cof+tsince*(m_t4cof+tsince*m_t5cof)));
		break;

	default:
		return false;
	}

	cd.a=_keo.semi_major_r*cd.tempa*cd.tempa;
	cd.e=_keo.eccentricity-cd.tempe;
	cd.xl=cd.xmp+cd.omega+cd.xnode+_keo.mean_motion_r*cd.templ;

	return _get_position(cd,v0,v1);
}
//------------------------------------------------------------------------------
