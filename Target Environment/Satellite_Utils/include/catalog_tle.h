#ifndef CATALOG_TLE_H
#define CATALOG_TLE_H
#include <memory>
#include "norad_base.h"
#include "utils.h"
#include <cinttypes>
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <list>

//------------------------------------------------------------------------------
/// \brief Параметры кеплеровых элементов орбиты
struct KEO
{
	// прочитанные элементы
	std::string name;     ///< Наименование КО
	double jd;            ///< Юлианский день привязки
	double inclination;   ///< Наклоненение орбиты (рад)
	double eccentricity;  ///< Эксцентриситет
	double raan;          ///< Долгота восходящего узла (рад)
	double arg_perigee;   ///< Аргумент перицентра (рад)
	double b_star;        ///< Коэффициент торможения B*
	/// \brief Первая производная от среднего движения (ускорение), деленная на два
	double drag;
	double mean_motion;   ///< Частота обращения (оборотов/день)
	double mean_anomaly;  ///< Средняя аномалия (рад)

	// восстановленные элементы
	double mean_motion_r; ///< Частота обращения (рад/мин)
	double semi_major_r;  ///< Большая полуось (а.е.)
	double semi_minor_r;  ///< Малая полуось (а.е.)
	double perigee_r;     ///< Перигей (км)
	double apogee_r;      ///< Апогей (км)

	double rcs;           ///< ЭПР объекта из каталога SSR (м²)

	double omega;         ///< Угловая частота для моделирования ЭПР (рад/с)

	std::size_t number;      ///< Номер КО по каталогу NORAD
	std::size_t errors;      ///< Маска признаков ошибок при прогнозе

	NoradBase *norad;     ///< Экземпляр для выполнения прогноза

	double a0,a1,xno,betao2,betao,cos_i,period;
};

#define GR(Angle) (Angle) * M_PI/ 180.
#define RG(Angle) (Angle) * 180./ M_PI

// Пространство имен для констант, используемых в моделях SGP4/SDP4
namespace TleConst
{
	const double PI=3.141592653589793;                ///< π
	const double TWOPI=2.0*PI;                        ///< 2π
	/// \brief Перевод из градусов в радианы
	const double RADS_PER_DEG=PI/180.0;

	/// \brief For all drag terms in GSFC case
	const double ECC_ALL=1.0E-4;
	/// \brief Too low for computing further drops
	const double ECC_EPS=1.0E-6;
	/// \brief Exit point for serious decaying of orbits
	const double ECC_LIMIT_LOW=-1.0E-3;
	const double ECC_LIMIT_HIGH=(1.0-ECC_EPS);        ///< Очень близко к 1

	/// \brief Minimum divisor allowed for (...)/(1+cos(inclination))
	const double EPS_COSIO=1.5E-12;
	/// \brief Минимально-допустимое значение синуса для SDP4
	const double SIN_EPS=1.0E-12;
	const double NR_EPS=1.0E-12;                      ///< Minimum for double

	const double TOTHRD=2.0/3.0;                      ///< 2/3

	/// \brief Гравитационная постоянная Земли (км³/с²)
	const double GM=398601.2;
	const double GEOSYNC_ALT=42241.892;               ///< (км)
	const double EARTH_DIA=12800.0;                   ///< (км)
	const double DAY_SIDERAL=(23*3600)+(56*60)+4.09;  ///< Звездные сутки (с)
	const double DAY_24HR=(24*3600);                  ///< Секунд в сутках (с)

	const double AE=1.0;
	/// \brief Астрономическая единица (км) (IAU 76)
	const double AU=149597870.0;
	/// \brief Радиус Солнца (км) (IAU 76)
	const double SR=696000.0;

	/// \brief Экваториальный радиус Земли (км) (WGS '72)
	const double XKMPER_WGS72=6378.135;
	/// \brief Полярное сжатие (WGS '72)
	const double F=1.0/298.26;
	/// \brief Гравитационная постоянная Земли (км³/с²) (WGS '72)
	const double GE=398600.8;
	/// \brief Второй гармонический коэффициент J2 (WGS '72)
	const double J2=1.0826158E-3;
	/// \brief Третий гармонический коэффициент J3 (WGS '72)
	const double J3=-2.53881E-6;
	/// \brief Четвертый гармонический коэффициент J4 (WGS '72)
	const double J4=-1.65597E-6;
	const double CK2=J2/2.0;
	const double CK4=-3.0*J4/8.0;
	const double XJ3=J3;
	const double QO=AE+120.0/XKMPER_WGS72;
	const double S=AE+78.0/XKMPER_WGS72;
	/// \brief Часов в день (солнечный)
	const double HR_PER_DAY=24.0;
	/// \brief Минут в день (солнечный)
	const double MIN_PER_DAY=1440.0;
	/// \brief Секунд в день (солнечный)
	const double SEC_PER_DAY=86400.0;
	/// \brief Вращение Земли за один звездный день
	const double OMEGA_E=1.00273790934;
	/// \brief sqrt(ge) ER³/min²
	//const double XKE=sqrt(3600.0*GE/(XKMPER_WGS72*XKMPER_WGS72*XKMPER_WGS72));
	const double XKE=7.43669161331734132e-2;
	//const double QOMS2T=pow((QO-S),4);                ///< (QO - S)⁴ ER⁴
	const double QOMS2T=1.880279159015270643865E-9;

	const double J1900=(2451545.5-36525.-1.);
	/// \brief Юлианская дата на Dec 31.5 1899 = Dec 31 1899 12h UTC
	const double EPOCH_JAN0_12H_1900=2415020.0;
	/// \brief Юлианская дата на Jan  1.0 1900 = Jan  1 1900 00h UTC
	const double EPOCH_JAN1_00H_1900=2415020.5;
	/// \brief Юлианская дата на Jan  1.5 2000 = Jan  1 2000 12h UTC
	const double EPOCH_JAN1_12H_2000=2451545.0;

	const double THGR70=1.7321343856509374;
	const double C1=1.72027916940703639E-2;
	const double C1P2P=C1+TWOPI;
	const double FK5R=5.07551419432269442E-15;

	/// \brief Перевод астрономических единиц в метры
	const double radiusAe=1000.0*XKMPER_WGS72/AE;
	/// \brief Перевод AE/мин в м/с
	const double speedAe=radiusAe*MIN_PER_DAY/SEC_PER_DAY;
	/// \brief Коэффициент пересчета модуля радиус-вектора до ОН в геоцентрической
	/// инерциальной в угловую скорость вращения Земли
	const double ROT_SPEED=TWOPI*OMEGA_E/SEC_PER_DAY;
}
//------------------------------------------------------------------------------
/// \brief Класс работы с каталогом TLE
class CatalogTLE
{
	friend class RkoClient;
	friend class ClientProcess;
	friend class Select;
	friend class NoradSGP4;
	friend class NoradSDP4;

public:
  CatalogTLE()
  {
    // юлианская дата начала системной эпохи
    _jd0 = Utils::Julian_date(1957, 12, 31, 0.0) + EPOCH_DAYS_OFFSET;
  }
  ~CatalogTLE() {}
  // Разбор файла каталога
  bool parse_keo(const std::string& filename);

  std::list<std::shared_ptr<KEO>>& get_satellite_list() { return _keo_el; }

  void clear_satellite_list() { _keo_el.clear(); }

private:
  // Имя файла с каталогом TLE
	std::string _file_name;      

	// Юлианская дата Эпохи #mmctime_t
	double _jd0;

	// Список всех кеплеровых элементов орбит
	std::list<std::shared_ptr<KEO>> _keo_el;
};

#endif // CATALOG_TLE_H
