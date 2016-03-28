#ifndef NORAD_BASE_H
#define NORAD_BASE_H

#include "vector3.h"
#include <cstring>

struct KEO;

/// \brief Приведение к интервалу [0; 2π)
/// \param[in] arg Приводимый угол (рад)
double fmod2p(const double arg);
/// \brief ArcTangent of sin(x) / cos(x)
/// \details The advantage of this function over arctan()
/// is that it returns the correct quadrant of the Angle
double acTan(const double sinx,const double cosx);

/// \brief Возведение в четвертую степень
inline double pow4(double a)
{
	a*=a;
	return a*a;
}

/// \brief Базовый класс моделей NORAD SGP4/SDP4 и круговых
class NoradBase
{
public:
	/// \brief Результаты инициализации
	enum IMode {
		SGDP4_NOT_INIT,   ///< инициализация не завершена
		SGDP4_ZERO_ECC,   ///< идеальная круговая орбита
		SGDP4_NEAR_SIMP,  ///< ближний космос, упрощенные уравнения
		SGDP4_NEAR_NORM,  ///< ближний космос, нормальные уравнения
		SGDP4_DEEP_NORM,  ///< глубокий космос, нормальные уравнения
		SGDP4_DEEP_RESN,  ///< глубокий космос, резонансный
		SGDP4_DEEP_SYNC,  ///< глубокий космос, синхронный (стационар)
		SGDP4_MAX_COUNT   ///< общее количество возможных результатов
	};

	/// \brief Типы координатных систем, в которых представляются результаты
	enum CSType {
		cs_ECI,           ///< Геоцентрическая инерциальная СК
		cs_GSC            ///< Подвижная Гринвичская СК
	};

	/// \brief Конструктор
	/// \param[in] keo Кеплеровы элементы орбиты
	/// \param[in] imode Результат инициализации
	NoradBase(const KEO &keo,IMode imode);

	/// \brief Деструктор
	virtual ~NoradBase() { }

	/// \brief Результат инициализации
	IMode imode() const
	{
		return _imode;
	}

	virtual bool getPosition(double tsince,tVECT3 &v0,tVECT3 &v1,CSType cs) const=0;

	/// \brief Маски ошибок выполнения прогноза
	enum ErrorsMasks {
		EM_NoError=0x00000000,     ///< нет ошибки
		EM_DataError=0x00000001,   ///< ошибка в данных спутника
		EM_AltError=0x00000002,    ///< ошибка в прогнозе положения
		EM_EccError=0x00000004,    ///< ошибка в значении эксцентриситета
		EM_RevError=0x00000008,    ///< ошибка в значении частоты обращения
		EM_InclError=0x00000010,   ///< ошибка в значении наклонения орбиты
		EM_SmallAWarn=0x00000020,  ///< выдано предупреждение о падении
		EM_SmallEWarn=0x00000040,  ///< выдано предупреждение о разрушении орбиты
		EM_BigEWarn=0x00000080,    ///< выдано предупреждение о большом квадрате e
		EM_IntegrWarn=0x00000100,  ///< выдано предупреждение о пределе интегрирования
		EM_SGDP4_Error=0x80000000  ///< есть какая-либо ошибка
	};

	/// \brief Угол поворота вокруг оси Z геоцентрической инерциальной СК (ECI)
	/// на заданную юлианскую дату
	static double thetaJD(double jd);

protected:
	/// \brief Кеплеровы элементы орбиты
	const KEO &_keo;

	/// \brief Результат инициализации
	IMode _imode;

	/// \brief Кеплеровы элементы орбиты для расчета положения и скорости
	typedef struct kep_s
	{
		double theta;     ///< Angle "theta" from equatorial plane (rad) = U
		double ascn;      ///< Right ascension (rad)
		double eqinc;     ///< Equatorial inclination (rad)
		double radius;    ///< Radius (km)
		double rdotk;
		double rfdotk;

		// Following are without short-term perturbations but used to
		// speed searchs.

		double argp;      ///< Argument of perigee at 'tsince' (rad)
		double smjaxs;    ///< Semi-major axis at 'tsince' (km)
		double ecc;       ///< Eccentricity at 'tsince'

		double tsinse;    ///< Время относительно эпохи привязки (минут)
		CSType cs;        ///< Тип СК, в которой представляются результаты

	} kep_t;

	/// \brief Получение координат и скоростей в геоцентрической инерциальной СК
	/// \param[in] kep Спрогнозированные кеплеровы элементы орбиты
	/// \param[out] v0 Положение в геоцентрической инерциальной СК (м)
	/// \param[out] v1 Скорость в геоцентрической инерциальной СК (м/с)
	/// \return True в случае успешного получения координат
	bool _kep2xyz(const kep_t &kep,tVECT3 &v0,tVECT3 &v1) const;
};
//------------------------------------------------------------------------------
#endif // NORAD_BASE_H
