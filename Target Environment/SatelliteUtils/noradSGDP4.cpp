#include "noradSGDP4.h"
#include "catalog_tle.h"

//------------------------------------------------------------------------------
NoradSGDP4::NoradSGDP4(const KEO &keo,IMode imode)
  : NoradBase(keo,imode)
{
	// конструктор

	// выполняется инициализация всех независимых от времени переменных, требуемых
	// для расчета положения и скорости

	m_sinio=sin(_keo.inclination);
	m_cosio=cos(_keo.inclination);

	double theta2=m_cosio*m_cosio;
	m_x3thm1=3.0*theta2-1.0;
	double eosq=_keo.eccentricity*_keo.eccentricity;

	m_betao2=1.0-eosq;
	m_betao=sqrt(m_betao2);

	m_s4=TleConst::S;
	m_qoms24=TleConst::QOMS2T;

	if(keo.perigee_r<156.0){
		// для перигея ниже 156 км изменяем значения S и QOMS2T
		m_s4=keo.perigee_r-78.0;

		if(keo.perigee_r<=98.0){
			m_s4=20.0;
		}

		m_qoms24=pow4((120.0-m_s4)*(TleConst::AE/TleConst::XKMPER_WGS72));
		m_s4=m_s4/TleConst::XKMPER_WGS72+TleConst::AE;
	}

	const double pinvsq=1.0/(_keo.semi_major_r*_keo.semi_major_r*m_betao2*m_betao2);

	m_tsi=1.0/(_keo.semi_major_r-m_s4);
	m_eta=_keo.semi_major_r*_keo.eccentricity*m_tsi;
	m_eeta=_keo.eccentricity*m_eta;

	const double etasq=m_eta*m_eta;
	const double psisq=fabs(1.0-etasq);

	m_coef=m_qoms24*pow4(m_tsi);
	m_coef1=m_coef/pow(psisq,3.5);

	const double c2=m_coef1*_keo.mean_motion_r*
	    (_keo.semi_major_r*(1.0+1.5*etasq+m_eeta*(4.0+etasq))+
	     0.75*TleConst::CK2*m_tsi/psisq*m_x3thm1*(8.0+3.0*etasq*(8.0+etasq)));

	m_c1=_keo.b_star*c2;
	m_a3ovk2=-TleConst::XJ3/TleConst::CK2*pow(TleConst::AE,3.0);

	m_c3=0;

	m_x1mth2=1.0-theta2;
	m_c4=2.0*_keo.mean_motion_r*m_coef1*_keo.semi_major_r*m_betao2*
	    (m_eta*(2.0+0.5*etasq)+_keo.eccentricity*(0.5+2.0*etasq)-
	     2.0*TleConst::CK2*m_tsi/(_keo.semi_major_r*psisq)*
	     (-3.0*m_x3thm1*(1.0-2.0*m_eeta+etasq*(1.5-0.5*m_eeta))+
	      0.75*m_x1mth2*(2.0*etasq-m_eeta*(1.0+etasq))*cos(2.0*_keo.arg_perigee)));

	m_x7thm1=7.0*theta2-1.0;

	const double theta4=theta2*theta2;
	const double temp1=3.0*TleConst::CK2*pinvsq*_keo.mean_motion_r;
	const double temp2=temp1*TleConst::CK2*pinvsq;
	const double temp3=1.25*TleConst::CK4*pinvsq*pinvsq*_keo.mean_motion_r;

	m_xmdot=_keo.mean_motion_r+0.5*temp1*m_betao*m_x3thm1+
	    0.0625*temp2*m_betao*(13.0-78.0*theta2+137.0*theta4);

	const double x1m5th=1.0-5.0*theta2;

	m_omgdot=-0.5*temp1*x1m5th+0.0625*temp2*(7.0-114.0*theta2+395.0*theta4)+
	    temp3*(3.0-36.0*theta2+49.0*theta4);

	const double xhdot1=-temp1*m_cosio;

	m_xnodot=xhdot1+(0.5*temp2*(4.0-19.0*theta2)+2.0*temp3*(3.0-7.0*theta2))*m_cosio;
	m_xnodcf=3.5*m_betao2*xhdot1*m_c1;
	m_t2cof=1.5*m_c1;

	// проверяем возможность деления на 0 для X/(1+m_cosio) при вычислении m_xlcof
	double temp0=1.0+m_cosio;

	if(fabs(temp0)<TleConst::EPS_COSIO){
		temp0=temp0>=0.0 ? fabs(TleConst::EPS_COSIO) : -fabs(TleConst::EPS_COSIO);
	}

	m_xlcof=0.125*m_a3ovk2*m_sinio*(3.0+5.0*m_cosio)/temp0;
	m_aycof=0.25*m_a3ovk2*m_sinio;
}
//------------------------------------------------------------------------------
void NoradSGDP4::_initCalcData(double tsince,CalcData &cd,CSType cs) const
{
	// инициализация данных перед расчетом

	cd.xmp=_keo.mean_anomaly+m_xmdot*tsince;
	cd.xnode=_keo.raan+tsince*(m_xnodot+tsince*m_xnodcf);
	cd.omega=_keo.arg_perigee+m_omgdot*tsince;
	cd.tsince=tsince;
	cd.cs=cs;
	cd.xinc=_keo.inclination;
	cd.sinio=m_sinio;
	cd.cosio=m_cosio;
}
//------------------------------------------------------------------------------
bool NoradSGDP4::_get_position(const CalcData &cd,tVECT3 &v0,tVECT3 &v1) const
{
	// основной расчет положения

	if(cd.a<1.0){
		if((_keo.errors & EM_SmallAWarn)==0){
			(*(std::size_t *)&_keo.errors)|=EM_SmallAWarn;
		}
		return false;
	}

	if(cd.e<TleConst::ECC_LIMIT_LOW){
		// идет разрушение орбиты
		if((_keo.errors & EM_SmallEWarn)==0){
			(*(std::size_t *)&_keo.errors)|=EM_SmallEWarn;
		}
		return false;
	}

	double e=cd.e;
	if(e<TleConst::ECC_EPS){ e=TleConst::ECC_EPS; }
	else if(e>TleConst::ECC_LIMIT_HIGH){ e=TleConst::ECC_LIMIT_HIGH; }

	double beta2=1.0-e*e;

	// Long period periodics
	const double sinOMG=sin(cd.omega);
	const double cosOMG=cos(cd.omega);

	double temp0=1.0/(cd.a*beta2);
	const double axn=e*cosOMG;
	const double ayn=e*sinOMG+temp0*m_aycof;
	const double xlt=cd.xl+temp0*m_xlcof*axn;

	const double elsq=axn*axn+ayn*ayn;
	if(elsq>=1.0){
		if((_keo.errors & EM_BigEWarn)==0){
			(*(std::size_t *)&_keo.errors)|=EM_BigEWarn;
		}
		return false;
	}

	kep_t kep;

	// Sensibility check for N-R correction
	kep.ecc=sqrt(elsq);

	// Solve Kepler's equation using Newton-Raphson root solving. Here 'capu' is
	// almost the "Mean anomaly", initialise the "Eccentric Anomaly" term 'epw'.
	// The fmod() saves reduction of Angle to +/-2pi in SINCOS() and prevents
	// convergence problems.

	// Later modified to support 2nd order NR method which saves roughly 1
	// iteration for only a couple of arithmetic operations.

	double epw=fmod(xlt-cd.xnode,TleConst::TWOPI);
	const double capu=epw;

	double maxnr = kep.ecc;

	const std::size_t maxi(10);  // количество итераций

	double sinEPW,cosEPW;
	double ecosE,esinE;

	for(std::size_t indx=0;indx<maxi;++indx){
		sinEPW=sin(epw);
		cosEPW=cos(epw);

		ecosE=axn*cosEPW+ayn*sinEPW;
		esinE=axn*sinEPW-ayn*cosEPW;

		double f=capu-epw+esinE;
		if(fabs(f)<TleConst::NR_EPS){
			break;
		}

		double df=1.0-ecosE;

		// 1st order Newton-Raphson correction
		double nr=f/df;

		if(indx==0 && fabs(nr)>1.25*maxnr){
			nr=nr>= 0 ? fabs(maxnr) : -fabs(maxnr);
		}else{
			// 2nd order Newton-Raphson correction/
			nr=f/(df+0.5*esinE*nr);
		}

		epw+=nr;  // Newton-Raphson correction of -F/DF
	}

	// Short period preliminary quantities
	temp0=1.0-elsq;
	const double betal=sqrt(temp0);
	const double pl=cd.a*temp0;
	const double r=cd.a*(1.0-ecosE);
	const double invR=1.0/r;
	double temp2=cd.a*invR;
	const double temp3=1.0/(1.0+betal);
	const double cosu=temp2*(cosEPW-axn+ayn*esinE*temp3);
	const double sinu=temp2*(sinEPW-ayn-axn*esinE*temp3);
	const double u=atan2(sinu,cosu);
	const double sin2u=2.0*sinu*cosu;
	const double cos2u=2.0*cosu*cosu-1.0;
	temp0=1.0/pl;
	const double temp1=TleConst::CK2*temp0;
	temp2=temp1*temp0;

	// Update for short term periodics to position terms
	const double rk=r*(1.0-1.5*temp2*betal*m_x3thm1)+0.5*temp1*m_x1mth2*cos2u;
	const double uk=u-0.25*temp2*m_x7thm1*sin2u;
	const double xnodek=cd.xnode+1.5*temp2*cd.cosio*sin2u;
	const double xinck=cd.xinc+1.5*temp2*cd.cosio*cd.sinio*cos2u;

	if(rk<1.0){
		if((_keo.errors & EM_SmallAWarn)==0){
			(*(std::size_t *)&_keo.errors)|=EM_SmallAWarn;
		}
		return false;
	}

	kep.radius=rk*TleConst::radiusAe;    // в метрах
	kep.theta=uk;
	kep.eqinc=xinck;
	kep.ascn=xnodek;
	kep.argp=cd.omega;
	kep.smjaxs=cd.a*TleConst::radiusAe;

	temp0=sqrt(cd.a);
	temp2=TleConst::XKE/(cd.a*temp0);

	// в метрах/секунду
	kep.rdotk=(TleConst::XKE*temp0*esinE*invR-temp2*temp1*m_x1mth2*sin2u)*
	    TleConst::speedAe;
	kep.rfdotk=(TleConst::XKE*sqrt(pl)*invR+temp2*temp1*(m_x1mth2*cos2u+1.5*m_x3thm1))*
	    TleConst::speedAe;
	kep.cs=cd.cs;
	kep.tsinse=cd.tsince;

#if USE_DUNDEE_DEBUG
	npos.xmp=cd.xmp; npos.xnode=cd.xnode; npos.omega=cd.omega;
	npos.tempa=cd.tempa; npos.tempe=cd.tempe; npos.templ=cd.templ;
	npos.tsince=cd.tsince;
	npos.xl=cd.xl; npos.sinio=cd.sinio; npos.cosio=cd.cosio;
	npos.x3thm1=m_x3thm1; npos.x1mth2=m_x1mth2; npos.x7thm1=m_x7thm1;
	npos.xlcof=m_xlcof; npos.aycof=m_aycof;
	npos.beta2=beta2; npos.sinOMG=sinOMG; npos.cosOMG=cosOMG;
	npos.temp0=temp0; npos.axn=axn; npos.ayn=ayn; npos.xlt=xlt;
	npos.elsq=elsq; npos.epw=epw; npos.capu=capu; npos.maxnr=maxnr;
	npos.ecosE=ecosE; npos.esinE=esinE; npos.sinEPW=sinEPW; npos.cosEPW=cosEPW;
	npos.betal=betal; npos.pl=pl; npos.r=r; npos.invR=invR; npos.temp2=temp2;
	npos.temp3=temp3; npos.cosu=cosu; npos.sinu=sinu; npos.u=u;
	npos.sin2u=sin2u; npos.cos2u=cos2u; npos.temp1=TleConst::speedAe;

	nkep.theta=kep.theta;
	nkep.ascn=kep.ascn;
	nkep.eqinc=kep.eqinc;
	nkep.radius=kep.radius;
	nkep.rdotk=kep.rdotk;
	nkep.rfdotk=kep.rfdotk;
	nkep.argp=kep.argp;
	nkep.smjaxs=kep.smjaxs;
	nkep.ecc=kep.ecc;
#endif // USE_DUNDEE_DEBUG

	return _kep2xyz(kep,v0,v1);
}
//------------------------------------------------------------------------------
