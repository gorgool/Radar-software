#include "../include/noradCircl.h"
#include "../include/catalog_tle.h"

//------------------------------------------------------------------------------
NoradCircl::NoradCircl(const KEO &keo,IMode imode)
  : NoradBase(keo,imode)
{
	// конструктор

}
//------------------------------------------------------------------------------
bool NoradCircl::getPosition(double tsince,tVECT3 &v0,tVECT3 &v1,CSType cs) const
{
	// получение положения и скорости на заданное время
	kep_t kep;
	kep.smjaxs=kep.radius=_keo.semi_major_r*TleConst::radiusAe;
	kep.theta=fmod(TleConst::PI+_keo.mean_motion_r*tsince,TleConst::TWOPI)-
	    TleConst::PI;
	kep.eqinc=_keo.inclination;
	kep.ascn=_keo.raan;

	kep.argp=0.0;
	kep.ecc=0.0;

	// m/sec
	kep.rfdotk=_keo.semi_major_r*_keo.mean_motion_r*TleConst::speedAe;
	kep.cs=cs;
	kep.tsinse=tsince;
	return _kep2xyz(kep,v0,v1);
}
//------------------------------------------------------------------------------
