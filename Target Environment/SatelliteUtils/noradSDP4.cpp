#include "noradSDP4.h"
#include "catalog_tle.h"
#include "utils.h"

static const double zes=0.01675;
static const double zel=0.05490;
static const double zns=1.19459e-05;
static const double znl=1.5835218e-04;
static const double thdt=4.37526908801129966e-3;

NoradSDP4 *nsdp0=(NoradSDP4 *)malloc(sizeof(NoradSDP4));
NoradSDP4 *nsdp1=(NoradSDP4 *)malloc(sizeof(NoradSDP4));

//------------------------------------------------------------------------------
NoradSDP4::NoradSDP4(const KEO &keo, IMode imode, double epoch)
  : NoradSGDP4(keo,imode),m_ilsd(_keo.inclination>=0.2 ? 1 : 0)
{
	// конструктор

	double sinomo=sin(_keo.arg_perigee);
	double cosomo=cos(_keo.arg_perigee);
	double sinq=sin(_keo.raan);
	double cosq=cos(_keo.raan);

	if(fabs(m_sinio)<=TleConst::SIN_EPS){
		m_sinio=m_sinio>=0.0 ? fabs(TleConst::SIN_EPS) : -fabs(TleConst::SIN_EPS);
	}

	double eqsq=_keo.eccentricity*_keo.eccentricity;

	// Deep space initialization
	double days50;
	dp_thgr=Utils::toGmst(epoch,days50);

	double eq=_keo.eccentricity;
	double aqnv=1.0/_keo.semi_major_r;
	double xmao=_keo.mean_anomaly;
	double xpidot=m_omgdot+m_xnodot;

	// Initialize lunar solar terms
	double day=days50+18261.5;
	double xnodce=4.5236020-9.2422029E-4*day;
	double temp0=fmod(xnodce,TleConst::TWOPI);
	double dpi_stem=sin(temp0);
	double dpi_ctem=cos(temp0);
	double dpi_zcosil=0.91375164-0.03568096*dpi_ctem;
	double dpi_zsinil=sqrt(1.0-dpi_zcosil*dpi_zcosil);
	double dpi_zsinhl=0.089683511*dpi_stem/dpi_zsinil;
	double dpi_zcoshl=sqrt(1.0-dpi_zsinhl*dpi_zsinhl);
	double dpi_c=4.7199672+0.22997150*day;
	double dpi_gam=5.8351514+0.0019443680*day;

	dp_zmol=fmod2p(dpi_c-dpi_gam);

	double dpi_zx=0.39785416*dpi_stem/dpi_zsinil;
	double dpi_zy=dpi_zcoshl*dpi_ctem+0.91744867*dpi_zsinhl*dpi_stem;
	dpi_zx=atan2(dpi_zx,dpi_zy);
	dpi_zx=fmod(dpi_zx+dpi_gam-xnodce,TleConst::TWOPI);

	double dpi_zcosgl=cos(dpi_zx);
	double dpi_zsingl=sin(dpi_zx);

	dp_zmos=6.2565837+0.017201977*day;
	dp_zmos=fmod2p(dp_zmos);

	// do solar terms
	const double zsings=-0.98088458;
	const double zcosgs=0.1945905;
	const double zcosis=0.91744867;
	const double zsinis=0.39785416;
	const double c1ss=2.9864797e-06;

	double zcosg=zcosgs;
	double zsing=zsings;
	double zcosi=zcosis;
	double zsini=zsinis;
	double zcosh=cosq;
	double zsinh=sinq;
	double cc=c1ss;
	double zn=zns;
	double ze=zes;
	double xnoi=1.0/_keo.mean_motion_r;

	double se=0.0;
	double si=0.0;
	double sl=0.0;
	double sgh=0.0;
	double shdq=0.0;

	// Apply the solar and lunar terms on the first pass, then re-apply the
	// solar terms again on the second pass.

	for(int pass=1;pass<=2;pass++){
		// Do solar terms
		double a1=zcosg*zcosh+zsing*zcosi*zsinh;
		double a3=-zsing*zcosh+zcosg*zcosi*zsinh;
		double a7=-zcosg*zsinh+zsing*zcosi*zcosh;
		double a8=zsing*zsini;
		double a9=zsing*zsinh+zcosg*zcosi*zcosh;
		double a10=zcosg*zsini;

		double a2=m_cosio*a7+m_sinio*a8;
		double a4=m_cosio*a9+m_sinio*a10;
		double a5=-m_sinio*a7+m_cosio*a8;
		double a6=-m_sinio*a9+m_cosio*a10;
		double x1=a1*cosomo+a2*sinomo;
		double x2=a3*cosomo+a4*sinomo;
		double x3=-a1*sinomo+a2*cosomo;
		double x4=-a3*sinomo+a4*cosomo;
		double x5=a5*sinomo;
		double x6=a6*sinomo;
		double x7=a5*cosomo;
		double x8=a6*cosomo;
		double z31=12.0*x1*x1-3.0*x3*x3;
		double z32=24.0*x1*x2-6.0*x3*x4;
		double z33=12.0*x2*x2-3.0*x4*x4;
		double z1=3.0*(a1*a1+a2*a2)+z31*eqsq;
		double z2=6.0*(a1*a3+a2*a4)+z32*eqsq;
		double z3=3.0*(a3*a3+a4*a4)+z33*eqsq;
		double z11=-6.0*a1*a5+eqsq*(-24.0*x1*x7-6.0*x3*x5);
		double z12=-6.0*(a1*a6+a3*a5)+eqsq*(-24.0*(x2*x7+x1*x8)-6.0*(x3*x6+x4*x5));
		double z13=-6.0*a3*a6+eqsq*(-24.0*x2*x8-6.0*x4*x6);
		double z21=6.0*a2*a5+eqsq*(24.0*x1*x5-6.0*x3*x7);
		double z22=6.0*(a4*a5+a2*a6)+eqsq*(24.0*(x2*x5+x1*x6)-6.0*(x4*x7+x3*x8));
		double z23=6.0*a4*a6+eqsq*(24.0*x2*x6-6.0*x4*x8);
		z1=z1+z1+m_betao2*z31;
		z2=z2+z2+m_betao2*z32;
		z3=z3+z3+m_betao2*z33;
		double s3=cc*xnoi;
		double s2=-0.5*s3/m_betao;
		double s4=s3*m_betao;
		double s1=-15.0*eq*s4;
		double s5=x1*x3+x2*x4;
		double s6=x2*x3+x1*x4;
		double s7=x2*x4-x1*x3;
		se=s1*zn*s5;
		si=s2*zn*(z11+z13);
		sl=-zn*s3*(z1+z3-14.0-6.0*eqsq);
		sgh=s4*zn*(z31+z33-6.0);

		if(_keo.inclination>=5.2359877E-2){
			double sh=-zn*s2*(z21+z23);
			shdq=sh/m_sinio;
		}

		dp_ee2=2.0*s1*s6;
		dp_e3=2.0*s1*s7;
		dp_xi2=2.0*s2*z12;
		dp_xi3=2.0*s2*(z13-z11);
		dp_xl2=-2.0*s3*z2;
		dp_xl3=-2.0*s3*(z3-z1);
		dp_xl4=-2.0*s3*(-21.0-9.0*eqsq)*ze;
		dp_xgh2=2.0*s4*z32;
		dp_xgh3=2.0*s4*(z33-z31);
		dp_xgh4=-18.0*s4*ze;
		dp_xh2=-2.0*s2*z22;
		dp_xh3=-2.0*s2*(z23-z21);

		if(pass==1){
			// Do lunar terms
			dp_sse=se;
			dp_ssi=si;
			dp_ssl=sl;
			dp_ssh=shdq;
			dp_ssg=sgh-m_cosio*dp_ssh;
			dp_se2=dp_ee2;
			dp_si2=dp_xi2;
			dp_sl2=dp_xl2;
			dp_sgh2=dp_xgh2;
			dp_sh2=dp_xh2;
			dp_se3=dp_e3;
			dp_si3=dp_xi3;
			dp_sl3=dp_xl3;
			dp_sgh3=dp_xgh3;
			dp_sh3=dp_xh3;
			dp_sl4=dp_xl4;
			dp_sgh4=dp_xgh4;
			zcosg=dpi_zcosgl;
			zsing=dpi_zsingl;
			zcosi=dpi_zcosil;
			zsini=dpi_zsinil;
			zcosh=dpi_zcoshl*cosq+dpi_zsinhl*sinq;
			zsinh=sinq*dpi_zcoshl-cosq*dpi_zsinhl;
			zn=znl;

			const double c1l=4.7968065e-07;

			cc=c1l;
			ze=zel;
		}
		memcpy(pass==1 ? nsdp0 : nsdp1,this,sizeof(NoradSDP4));
	}

	dp_sse+=se;
	dp_ssi+=si;
	dp_ssl+=sl;
	dp_ssg+=sgh-m_cosio*shdq;
	dp_ssh+=shdq;

	// Geopotential resonance initialization
	dp_iresfl=0;
	dp_isynfl=0;

	double g310;
	double f220;
	double bfact=0.0;

	// Determine if orbit is 24- or 12-hour resonant.
	// Mean motion is given in radians per minute.
	if((_keo.mean_motion_r>0.0034906585)&&(_keo.mean_motion_r<0.0052359877)){
		// Orbit is within the Clarke Belt (period is 24-hour resonant).
		// Synchronous resonance terms initialization
		dp_iresfl=1;
		dp_isynfl=1;

		double g200=1.0+eqsq*(-2.5+0.8125*eqsq);

		g310=1.0+2.0*eqsq;

		double g300=1.0+eqsq*(-6.0+6.60937*eqsq);

		f220=0.75*(1.0+m_cosio)*(1.0+m_cosio);

		double f311=0.9375*m_sinio*m_sinio*(1.0+3*m_cosio)-0.75*(1.0+m_cosio);
		double f330=1.0+m_cosio;

		const double q22=1.7891679e-06;
		const double q31=2.1460748e-06;
		const double q33=2.2123015e-07;

		f330=1.875*f330*f330*f330;
		dp_del1=3.0*_keo.mean_motion_r*_keo.mean_motion_r*aqnv*aqnv;
		dp_del2=2.0*dp_del1*f220*g200*q22;
		dp_del3=3.0*dp_del1*f330*g300*q33*aqnv;
		dp_del1=dp_del1*f311*g310*q31*aqnv;
		dp_xlamo=xmao+_keo.raan+_keo.arg_perigee-dp_thgr;
		bfact=m_xmdot+xpidot-thdt;
		bfact=bfact+dp_ssl+dp_ssg+dp_ssh;

	}else if(((_keo.mean_motion_r>=8.26E-03)&&(_keo.mean_motion_r<=9.24E-03))&&(eq>=0.5)){
		// Period is 12-hour resonant
		dp_iresfl=1;

		double eoc=eq*eqsq;
		double g201=-0.306-(eq-0.64)*0.440;

		double g211;
		double g322;
		double g410;
		double g422;
		double g520;

		if(eq<=0.65){
			g211=3.616-13.247*eq+16.290*eqsq;
			g310=-19.302+117.390*eq-228.419*eqsq+156.591*eoc;
			g322=-18.9068+109.7927*eq-214.6334*eqsq+146.5816*eoc;
			g410=-41.122+242.694*eq-471.094*eqsq+313.953*eoc;
			g422=-146.407+841.880*eq-1629.014*eqsq+1083.435*eoc;
			g520=-532.114+3017.977*eq-5740.0*eqsq+3708.276*eoc;

		}else{
			g211=-72.099+331.819*eq-508.738*eqsq+266.724*eoc;
			g310=-346.844+1582.851*eq-2415.925*eqsq+1246.113*eoc;
			g322=-342.585+1554.908*eq-2366.899*eqsq+1215.972*eoc;
			g410=-1052.797+4758.686*eq-7193.992*eqsq+3651.957*eoc;
			g422=-3581.69+16178.11*eq-24462.77*eqsq+12422.52*eoc;

			if(eq<=0.715){
				g520=1464.74-4664.75*eq+3763.64*eqsq;
			}else{
				g520=-5149.66+29936.92*eq-54087.36*eqsq+31324.56*eoc;
			}
		}

		double g533;
		double g521;
		double g532;

		if(eq<0.7){
			g533=-919.2277+4988.61*eq-9064.77*eqsq+5542.21*eoc;
			g521=-822.71072+4568.6173*eq-8491.4146*eqsq+5337.524*eoc;
			g532=-853.666+4690.25*eq-8624.77*eqsq+5341.4*eoc;
		}else{
			g533=-37995.78+161616.52*eq-229838.2*eqsq+109377.94*eoc;
			g521=-51752.104+218913.95*eq-309468.16*eqsq+146349.42*eoc;
			g532=-40023.88+170470.89*eq-242699.48*eqsq+115605.82*eoc;
		}

		double sini2=m_sinio*m_sinio;
		double theta2=m_cosio*m_cosio;

		f220=0.75*(1.0+2.0*m_cosio+theta2);

		const double root22=1.7891679e-06;
		const double root32=3.7393792e-07;
		const double root44=7.3636953e-09;
		const double root52=1.1428639e-07;
		const double root54=2.1765803e-09;

		double f221=1.5*sini2;
		double f321=1.875*m_sinio*(1.0-2.0*m_cosio-3.0*theta2);
		double f322=-1.875*m_sinio*(1.0+2.0*m_cosio-3.0*theta2);
		double f441=35.0*sini2*f220;
		double f442=39.3750*sini2*sini2;
		double f522=9.84375*m_sinio*
		    (sini2*(1.0-2.0*m_cosio-5.0*theta2)+
		     0.33333333*(-2.0+4.0*m_cosio+6.0*theta2));
		double f523=m_sinio*(4.92187512*sini2*(-2.0-4.0*m_cosio+10.0*theta2)+
		                     6.56250012*(1.0+2.0*m_cosio-3.0*theta2));
		double f542=29.53125*m_sinio*
		    (2.0-8.0*m_cosio+theta2*(-12.0+8.0*m_cosio+10.0*theta2));
		double f543=29.53125*m_sinio*
		    (-2.0-8.0*m_cosio+theta2*(12.0+8.0*m_cosio-10.0*theta2));
		double xno2=_keo.mean_motion_r*_keo.mean_motion_r;
		double ainv2=aqnv*aqnv;
		double temp1=3.0*xno2*ainv2;
		double temp=temp1*root22;

		dp_d2201=temp*f220*g201;
		dp_d2211=temp*f221*g211;
		temp1=temp1*aqnv;
		temp=temp1*root32;
		dp_d3210=temp*f321*g310;
		dp_d3222=temp*f322*g322;
		temp1=temp1*aqnv;
		temp=2.0*temp1*root44;
		dp_d4410=temp*f441*g410;
		dp_d4422=temp*f442*g422;
		temp1=temp1*aqnv;
		temp=temp1*root52;
		dp_d5220=temp*f522*g520;
		dp_d5232=temp*f523*g532;
		temp=2.0*temp1*root54;
		dp_d5421=temp*f542*g521;
		dp_d5433=temp*f543*g533;
		dp_xlamo=xmao+_keo.raan+_keo.raan-dp_thgr-dp_thgr;
		bfact=m_xmdot+m_xnodot+m_xnodot-thdt-thdt;
		bfact=bfact+dp_ssl+dp_ssh+dp_ssh;
	}

	if(dp_iresfl==0){
		dp_xfact=0.0;
		dp_xli=0.0;
		dp_xni=0.0;
		dp_atime=0.0;
		dp_xnddt0=0.0;
		dp_xndot0=0.0;
		dp_xldot0=0.0;
		dp_xnddt=0.0;
		dp_xndot=0.0;
		dp_xldot=0.0;
		_imode=SGDP4_DEEP_NORM;

	}else{
		// Initialize integrator
		dp_xfact=bfact-_keo.mean_motion_r;
		dp_xli=dp_xlamo;
		dp_xni=_keo.mean_motion_r;
		dp_atime=0.0;

		_dot_terms_calculated();

		// Save the "dot" terms for integrator re-start
		dp_xnddt0=dp_xnddt;
		dp_xndot0=dp_xndot;
		dp_xldot0=dp_xldot;

		if(dp_isynfl!=0){
			_imode=SGDP4_DEEP_SYNC;
		}else{
			_imode=SGDP4_DEEP_RESN;
		}
	}
}
//------------------------------------------------------------------------------
NoradSDP4::~NoradSDP4()
{
	// деструктор

}
//------------------------------------------------------------------------------
void NoradSDP4::_dot_terms_calculated() const
{
	// расчет параметров скорости

	// DOT TERMS CALCULATED
	if(dp_isynfl!=0){
		const double fasx2=0.13130908;
		const double fasx4=2.8843198;
		const double fasx6=0.37448087;

		dp_xndot=dp_del1*sin(dp_xli-fasx2)+dp_del2*sin((dp_xli-fasx4)*2.0)+
		    dp_del3*sin((dp_xli-fasx6)*3.0);

		dp_xnddt=dp_del1*cos(dp_xli-fasx2)+dp_del2 *cos((dp_xli-fasx4)*2.0)*2.0+
		    dp_del3*cos((dp_xli-fasx6)*3.0)*3.0;

	}else{
		const double g22=5.7686396;
		const double g32=0.95240898;
		const double g44=1.8014998;
		const double g52=1.0508330;
		const double g54=4.4108898;

		double xomi=_keo.arg_perigee+m_omgdot*dp_atime;
		double x2omi=xomi+xomi;
		double x2li=dp_xli+dp_xli;

		dp_xndot=dp_d2201*sin(x2omi+dp_xli-g22)+dp_d2211*sin(dp_xli-g22)+
		    dp_d3210*sin(xomi+dp_xli-g32)+dp_d3222*sin(-xomi+dp_xli-g32)+
		    dp_d5220*sin(xomi+dp_xli-g52)+dp_d5232*sin(-xomi+dp_xli-g52)+
		    dp_d4410*sin(x2omi+x2li-g44)+dp_d4422*sin(x2li-g44)+
		    dp_d5421*sin(xomi+x2li-g54)+dp_d5433*sin(-xomi+x2li-g54);

		dp_xnddt=dp_d2201*cos(x2omi+dp_xli-g22)+dp_d2211*cos(dp_xli-g22)+
		    dp_d3210*cos(xomi+dp_xli-g32)+dp_d3222*cos(-xomi+dp_xli-g32)+
		    dp_d5220*cos(xomi+dp_xli-g52)+dp_d5232*cos(-xomi+dp_xli-g52)+
		    (dp_d4410*cos(x2omi+x2li-g44)+dp_d4422*cos(x2li-g44)+
		     dp_d5421*cos(xomi+x2li-g54)+dp_d5433*cos(-xomi+x2li-g54))*2.0;
	}

	dp_xldot=dp_xni+dp_xfact;
	dp_xnddt*=dp_xldot;
}
//------------------------------------------------------------------------------
bool NoradSDP4::_secular(DeepCalcData &dcd) const
{
	// расчет вековых эффектов для глубокого космоса

	dcd.cd.xmp+=dp_ssl*dcd.cd.tsince;
	dcd.cd.omega+=dp_ssg*dcd.cd.tsince;
	dcd.cd.xnode+=dp_ssh*dcd.cd.tsince;
	dcd.ecc+=dp_sse*dcd.cd.tsince;
	dcd.cd.xinc+=dp_ssi*dcd.cd.tsince;

	if(dp_iresfl==0){
		return true;
	}

	// A minor increase in some efficiency can be had by restarting if
	// the new time is closer to epoch than to the old integrated
	// time. This also forces a re-start on a change in sign (i.e. going
	// through zero time) as then we have |tsince - atime| > |tsince|
	// as well. Second test is for stepping back towards zero, forcing a restart
	// if close enough rather than integrating to zero.

	const double STEP(720.0);
	const double MAX_INTEGRATE(STEP*10000);

	// Most accurate (OK, most _consistant_) method. Restart if need to
	// integrate 'backwards' significantly from current point.
	if(fabs(dcd.cd.tsince)<STEP || (dp_atime>0.0 && dcd.cd.tsince<dp_atime-1.0) ||
	   (dp_atime<0.0 && dcd.cd.tsince>dp_atime+1.0)){
		// Epoch restart if we are at, or have crossed, tsince==0
		dp_atime=0.0;
		dp_xni=_keo.mean_motion_r;
		dp_xli=dp_xlamo;
		// Restore the old "dot" terms
		dp_xnddt=dp_xnddt0;
		dp_xndot=dp_xndot0;
		dp_xldot=dp_xldot0;
	}

	double ft=dcd.cd.tsince-dp_atime;

	if(fabs(ft)>MAX_INTEGRATE){
		if((_keo.errors & EM_IntegrWarn)==0){
			(*(std::size_t *)&_keo.errors)|=EM_IntegrWarn;
		}
		return false;
	}

	if(fabs(ft)>=STEP){
		// Do integration if required. Find the step direction to
		// make 'atime' catch up with 'tsince'
		double delt=dcd.cd.tsince>=dp_atime ? STEP : -STEP;

		do {
			// INTEGRATOR (using the last "dot" terms)
			dp_xli+=delt*(dp_xldot+delt*0.5*dp_xndot);
			dp_xni+=delt*(dp_xndot+delt*0.5*dp_xnddt);
			dp_atime+=delt;

			_dot_terms_calculated();

			// Are we close enough now?
			ft=dcd.cd.tsince-dp_atime;
		} while(fabs(ft)>=STEP);
	}

	double xl=dp_xli+ft*(dp_xldot+ft*0.5*dp_xndot);
	dcd.xn=dp_xni+ft*(dp_xndot+ft*0.5*dp_xnddt);

	double temp0=-dcd.cd.xnode+dp_thgr+dcd.cd.tsince*thdt;

	if(dp_isynfl==0){
		dcd.cd.xmp=xl+temp0+temp0;
	}else{
		dcd.cd.xmp=xl-dcd.cd.omega+temp0;
	}
	return true;
}
//------------------------------------------------------------------------------
void NoradSDP4::_compute_LunarSolar(DeepCalcData &dcd) const
{
	// подготовка параметров Луны и Солнца

	// Update Solar terms
	double zm=dp_zmos+zns*dcd.cd.tsince;
	double zf=zm+zes*2.0*sin(zm);
	double sinzf=sin(zf);
	double coszf=cos(zf);
	double f2=sinzf*0.5*sinzf-0.25;
	double f3=sinzf*(-0.5)*coszf;
	const double ses=dp_se2*f2+dp_se3*f3;
	const double sis=dp_si2*f2+dp_si3*f3;
	const double sls=dp_sl2*f2+dp_sl3*f3+dp_sl4*sinzf;

	const double sghs=dp_sgh2*f2+dp_sgh3*f3+dp_sgh4*sinzf;
	const double shs=dp_sh2*f2+dp_sh3*f3;

	// Update Lunar terms
	zm=dp_zmol+znl*dcd.cd.tsince;
	zf=zm+zel*2.0*sin(zm);
	sinzf=sin(zf);
	coszf=cos(zf);
	f2=sinzf*0.5*sinzf-0.25;
	f3=sinzf*(-0.5)*coszf;
	const double sel=dp_ee2*f2+dp_e3*f3;
	const double sil=dp_xi2*f2+dp_xi3*f3;
	const double sll=dp_xl2*f2+dp_xl3*f3+dp_xl4*sinzf;

	const double sghl=dp_xgh2*f2+dp_xgh3*f3+dp_xgh4*sinzf;
	const double shl=dp_xh2*f2+dp_xh3*f3;

	// Save computed values to calling structure
	dcd.pgh=sghs+sghl;
	dcd.ph=shs+shl;
	dcd.pe=ses+sel;
	dcd.pinc=sis+sil;
	dcd.pl=sls+sll;
}
//------------------------------------------------------------------------------
void NoradSDP4::_deep_periodics(DeepCalcData &dcd) const
{
	// расчет влияния периодичности Луны и Солнца

	_compute_LunarSolar(dcd);

	dcd.cd.xinc+=dcd.pinc;
	dcd.cd.e+=dcd.pe;

	// Spacetrack report #3 has sin/cos from before perturbations added to xinc
	// (oldxinc), but apparently report # 6 has then from after they are added
	const double sinis=sin(dcd.cd.xinc);
	const double cosis=cos(dcd.cd.xinc);

	if(m_ilsd!=0){
		// APPLY PERIODICS DIRECTLY
		const double tmp_ph=dcd.ph/sinis;

		dcd.cd.omega+=dcd.pgh-cosis*tmp_ph;
		dcd.cd.xnode+=tmp_ph;
		dcd.xmam+=dcd.pl;

	}else{
		// APPLY PERIODICS WITH LYDDANE MODIFICATION

		const double oldxnode=dcd.cd.xnode;

		const double sinok=sin(dcd.cd.xnode);
		const double cosok=cos(dcd.cd.xnode);
		double alfdp=sinis*sinok;
		double betdp=sinis*cosok;
		const double dalf=dcd.ph*cosok+dcd.pinc*cosis*sinok;
		const double dbet=-dcd.ph*sinok+dcd.pinc*cosis*cosok;
		alfdp+=dalf;
		betdp+=dbet;
		double xls=dcd.xmam+dcd.cd.omega+cosis*dcd.cd.xnode;
		const double dls=dcd.pl+dcd.pgh-dcd.pinc*dcd.cd.xnode*sinis;
		xls+=dls;
		dcd.cd.xnode=atan2(alfdp,betdp);

		// Get perturbed xnodes in to same quadrant as original
		const double temp((oldxnode-dcd.cd.xnode)/TleConst::TWOPI);
		int ishift=static_cast<int>(temp>0 ? temp+0.5 : temp-0.5);
		dcd.cd.xnode+=TleConst::TWOPI*ishift;

		dcd.xmam+=dcd.pl;
		dcd.cd.omega=xls-dcd.xmam-cosis*dcd.cd.xnode;
	}
}
//------------------------------------------------------------------------------
bool NoradSDP4::getPosition(double tsince,tVECT3 &v0,tVECT3 &v1,CSType cs) const
{
	// получение положения и скорости на заданное время

	DeepCalcData dcd;
	_initCalcData(tsince,dcd.cd,cs);

	dcd.cd.tempa=1.0-tsince * m_c1;
	dcd.cd.tempe=_keo.b_star*tsince*m_c4;
	dcd.cd.templ=tsince*tsince*m_t2cof;
	dcd.xn=_keo.mean_motion_r;
	dcd.ecc=_keo.eccentricity;

	if(!_secular(dcd)){
		return false;
	}

	dcd.cd.a=pow(TleConst::XKE/dcd.xn,TleConst::TOTHRD)*dcd.cd.tempa*dcd.cd.tempa;
	dcd.cd.e=dcd.ecc-dcd.cd.tempe;
	dcd.xmam=dcd.cd.xmp+_keo.mean_motion_r*dcd.cd.templ;

	_deep_periodics(dcd);

	if(dcd.cd.xinc<0.0){
		dcd.cd.xinc=-dcd.cd.xinc;
		dcd.cd.xnode+=TleConst::PI;
		dcd.cd.omega-=TleConst::PI;
	}

	dcd.cd.xl=dcd.xmam+dcd.cd.omega+dcd.cd.xnode;

	// Re-compute the perturbed values
	dcd.cd.sinio=sin(dcd.cd.xinc);
	dcd.cd.cosio=cos(dcd.cd.xinc);

	double theta2=dcd.cd.cosio*dcd.cd.cosio;

	m_x3thm1=3.0*theta2-1.0;
	m_x1mth2=1.0-theta2;
	m_x7thm1=7.0*theta2-1.0;

	// Check for possible divide-by-zero for X/(1+cosIO) when calculating xlcof
	double temp0=1.0+dcd.cd.cosio;

	if(fabs(temp0)<TleConst::EPS_COSIO){
		temp0=temp0>=0.0 ? fabs(TleConst::EPS_COSIO) : -fabs(TleConst::EPS_COSIO);
	}

	m_xlcof=0.125*m_a3ovk2*dcd.cd.sinio*(3.0+5.0*dcd.cd.cosio)/temp0;
	m_aycof=0.25*m_a3ovk2*dcd.cd.sinio;

	return _get_position(dcd.cd,v0,v1);
}
//------------------------------------------------------------------------------
