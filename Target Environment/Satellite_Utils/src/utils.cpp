#include "../include/utils.h"
#include "../include/catalog_tle.h"

//------------------------------------------------------------------------------
void Utils::DurData(int16_t epoch,int16_t ns,double ts,int16_t &ng,int16_t &nm,
                    int16_t &nd,int16_t &nh,int16_t &nmin,double &nc)
{
	// процедура пересчета даты из представления <номер суток (от эпохи),
	// доля суток> в представление <год, месяц, день, час, минута, секунда>

	ng=epoch;
	while(1){
		int16_t tmp=(ng%4==0) ? 366 : 365;
		if(ns>tmp){
			ng++;
			ns-=tmp;
		}else{
			break;
		}
	}

	int16_t month(1);
	while(1){
		if(ns<=WychKd(ng,month)){
			nm=month;
			if(month==1){
				nd=ns;
			}else{
				nd=ns-WychKd(ng,month-1);
			}
			break;
		}else{
			month++;
		}
	}

	nc=ts*24.0* 3600.0;
	nh=static_cast<uint16_t>(nc/3600.0);
	nc-=nh*3600.0;
	nmin= static_cast<uint16_t>(nc/60.0);
	nc-=nmin*60.0;
}
//------------------------------------------------------------------------------
int Utils::WychKd(int16_t ng,int16_t nm)
{
	// процедура вычисления числа дней от начала года

	int16_t dmounth[12] = {
	  31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
	};
	int16_t kd=dmounth[nm-1];
	if(ng%4==0 && nm>1 && ng%1000&& ng%400)
		kd++;
	return kd;
}
//------------------------------------------------------------------------------
double Utils::Julian_date(int year,int month,int day,double hour)
{
	// юлианская дата, соответствующая дате григорианского календаря

	int j=(month-14)/12;
	int jd12h=day-32075+1461*(year+4800+j)/4+367*(month-2-j*12)/12-
	    3*((year+4900+j)/100)/4;
	return jd12h-0.5+hour/24.0;
}
//------------------------------------------------------------------------------
#define THETAG 2
//------------------------------------------------------------------------------
double Utils::toGmst(double epoch,double &days50)
{
	// вычисление среднего гринвичского звездного времени для Юлианской даты

	// извлекаем год и день в году
	int32_t jy=(int32_t)((epoch+2.0e-7)*0.001); // Extract the year
	double d=epoch-jy*1.0e3;                    // And then the day of year

	// Assume " 8" is 1980, or more sensibly 2008 ?
	if(jy<50){
		jy+=100;
	}

	// Fix for leap years ?
	int32_t n=jy<70 ? (jy-72)/4 : (jy-69)/4;

	days50=(jy-70)*365.0+7305.0+n+d;

#if THETAG == 0

	// Original report #3 code
	double theta=days50*6.3003880987+1.72944494;

#elif THETAG == 1

	// Method from project pluto code/
	// Reference:  The 1992 Astronomical Almanac, page B6
	double jd=d+TleConst::J1900+jy*365.+((jy-1)/4);
	const double UT=fmod(jd+0.5,1.0);
	const double TU=(jd-TleConst::EPOCH_JAN1_12H_2000-UT)/36525.0;

	double GMST=24110.54841+TU*(8640184.812866+TU*(0.093104-TU*6.2e-06));

	GMST=fmod(GMST+TleConst::SEC_PER_DAY*TleConst::OMEGA_E*UT,TleConst::SEC_PER_DAY);

	if(GMST<0.0){
		GMST+=TleConst::SEC_PER_DAY;  // "wrap" negative modulo value
	}

	double theta=TleConst::TWOPI*(GMST/TleConst::SEC_PER_DAY);

#elif THETAG == 2

	// Method from SGP4SUB.F code
	double ts70=days50-7305.0;
	int32_t ids70=(int32_t)(ts70+1.0e-8);
	double ds70=ids70;

	double trfac=ts70-ds70;

	// CALCULATE GREENWICH LOCATION AT EPOCH
	double theta=TleConst::THGR70+TleConst::C1*ds70+TleConst::C1P2P*trfac+
	    ts70*ts70*TleConst::FK5R;

#else

#error "Unknown method for theta-G calculation"

#endif

	theta=fmod(theta,TleConst::TWOPI);
	if(theta<0.0){
		theta+=TleConst::TWOPI;
	}

	return theta;
}
//------------------------------------------------------------------------------
