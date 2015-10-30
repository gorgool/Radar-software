#ifndef UTILS_H
#define UTILS_H

#include <cinttypes>

struct KEO;

const static int EPOCH_DAYS_OFFSET = 18994;

/// \brief Пространство имен для служебных пересчетов
namespace Utils
{
	/// \brief Процедура пересчета даты из представления <номер суток (от эпохи),
	/// доля суток> в  представление <год, месяц, день, час, минута, секунда>
	/// \param[in] epoch Эпоха
	/// \param[in] ns Номер суток
	/// \param[in] ts Доля суток
	/// \param[out] ng Год
	/// \param[out] nm Месяц
	/// \param[out] nd День
	/// \param[out] nh Час
	/// \param[out] nmin Минута
	/// \param[out] nc Секунда
	void DurData(int16_t epoch,int16_t ns,double ts,int16_t &ng,int16_t &nm,
	             int16_t &nd,int16_t &nh,int16_t &nmin,double &nc);

	/// \brief Процедура вычисления числа дней от начала года
	/// \param[in] ng Год
	/// \param[in] nm Месяц
	/// \return Количество дней от начала года до конца месяца
	int WychKd(int16_t ng,int16_t nm);

	/// \brief Юлианская дата, соответствующая дате григорианского календаря
	/// \details (по алгоритму из Fliegel & Van Flandern, Comm. of the ACM,
	/// Vol. 11, No. 10, October 1968, p. 657)
	/// \param[in] year Год (4 цифры года)
	/// \param[in] month Номер месяца в году (1-12)
	/// \param[in] day Номер дня в месяце (1-31)
	/// \param[in] hour Время в часах
	/// \return Юлианскую дату
	double Julian_date(int year,int month,int day,double hour);

	/// \brief Вычисление среднего гринвичского звездного времени для Юлианской даты
	/// \param[in] epoch Дата привязки в виде, представленном в каталоге
	/// \param[out] days50 Output, days from Jan 1st 1950 00:00:00 UTC
	/// \return The return value is the Angle, in radians, measuring eastward
	/// from the Vernal Equinox to the prime meridian. This Angle is also
	/// referred to as "ThetaG" (Theta GMST).
	///
	/// \details References:
	///    The 1992 Astronomical Almanac, page B6.
	///    Explanatory Supplement to the Astronomical Almanac, page 50.
	///    Orbital Coordinate Systems, Part III, Dr. T.S. Kelso, Satellite Times,
	///       Nov/Dec 1995
	double toGmst(double epoch,double &days50);
}
//------------------------------------------------------------------------------
#endif // UTILS_H
