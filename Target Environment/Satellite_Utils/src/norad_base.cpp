#include "../include/norad_base.h"
#include "../include/catalog_tle.h"

//------------------------------------------------------------------------------
double fmod2p(const double arg)
{
	// приведение к интервалу [0; 2π)

	double modu=fmod(arg,TleConst::TWOPI);

	if(modu<0.0){
		modu+=TleConst::TWOPI;
	}

	return modu;
}
//------------------------------------------------------------------------------
/// \brief ArcTangent of sin(x) / cos(x)
/// \details The advantage of this function over arctan()
/// is that it returns the correct quadrant of the Angle
double acTan(const double sinx,const double cosx)
{
	if(cosx==0.0){
		return (sinx>0.0) ? (TleConst::PI/2.0) : (3.0*TleConst::PI/2.0);
	}else{
		return (cosx>0.0) ? (atan(sinx/cosx)) : (TleConst::PI+atan(sinx/cosx));
	}
}
//------------------------------------------------------------------------------
NoradBase::NoradBase(const KEO &keo,IMode imode)
  : _keo(keo),_imode(imode)
{
	// конструктор

}
//------------------------------------------------------------------------------
double NoradBase::thetaJD(double jd)
{
	// угол поворота вокруг оси Z геоцентрической инерциальной СК (ECI)
	// на заданную юлианскую дату

	double ut=fmod(jd+0.5,1.0);
	double t_cen=(jd-ut-2451545.0)/36525.0;
	double gmst=24110.54841+t_cen*(8640184.812866+t_cen*(0.093104-t_cen*6.2E-6));
	gmst=fmod(gmst+TleConst::SEC_PER_DAY*TleConst::OMEGA_E*ut,TleConst::SEC_PER_DAY);
	if(gmst<0.0){
		gmst+=TleConst::SEC_PER_DAY;
	}
	return TleConst::TWOPI*gmst/TleConst::SEC_PER_DAY;
}
//------------------------------------------------------------------------------
bool NoradBase::_kep2xyz(const kep_t &kep,tVECT3 &v0,tVECT3 &v1) const
{
	// получение ECI координат и скоростей в геоцентрической инерциальной СК

	// ориентация векторов
	const double sinT=sin(kep.theta);
	const double cosT=cos(kep.theta);
	const double sinI=sin(kep.eqinc);
	const double cosI=cos(kep.eqinc);
	const double sinS=sin(kep.ascn);
	const double cosS=cos(kep.ascn);

	const double xmx=-sinS*cosI;
	const double xmy=cosS*cosI;

	// положение
	const double ux=xmx*sinT+cosS*cosT;
	const double uy=xmy*sinT+sinS*cosT;
	const double uz=sinI*sinT;

	v0.x=kep.radius*ux;
	v0.y=kep.radius*uy;
	v0.z=kep.radius*uz;

	// скорость
	const double vx=xmx*cosT-cosS*sinT;
	const double vy=xmy*cosT-sinS*sinT;
	const double vz=sinI*cosT;

	v1.x=kep.rfdotk*vx+kep.rdotk*ux;
	v1.y=kep.rfdotk*vy+kep.rdotk*uy;
	v1.z=kep.rfdotk*vz+kep.rdotk*uz;

	if(kep.cs==cs_ECI){
		return true;
	}

	// перевод в ПГСК

	// из скорости удаляется скорость вращения Земли в инерциальной СК
	// угловая скорость
	const double Rx_UTy=sqrt(v0.x*v0.x+v0.y*v0.y)*TleConst::ROT_SPEED;
	const double theta=atan2(v0.y,v0.x);

	v1.x=v1.x+Rx_UTy*sin(theta);
	v1.y=v1.y-Rx_UTy*cos(theta);
	v1.z=v1.z;

	double rot=thetaJD(_keo.jd+kep.tsinse/TleConst::MIN_PER_DAY);
	v0.turn(3,cos(rot),sin(-rot));
	v1.turn(3,cos(rot),sin(-rot));
	return true;
}
//------------------------------------------------------------------------------
