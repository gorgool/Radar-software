#define _USE_MATH_DEFINES
#include <string>
#include <fstream>
#include <iostream>
#include "catalog_tle.h"
#include "utils.h"
#include "noradCircl.h"
#include "noradSDP4.h"
#include "noradSGP4.h"

// Максимальное количество объектов в TLE-каталоге
#define TLE_MAX_COUNT 20000
// Максимальное количество объектов в TLE-каталоге для SGP4 модели
#define SGP4_MAX_COUNT 16000
// Максимальное количество объектов в TLE-каталоге для SDP4 модели
#define SDP4_MAX_COUNT 4000

using namespace std;

bool CatalogTLE::parse_keo(const string& filename)
{
  setlocale(LC_ALL, "C");

  ifstream file(filename);

  if (!file.good())
  {
    cout << "Error opening file : " << filename << endl;
    return false;
  }

  while (true)
  {
    string s1, s2, name;
    // Reading 2 strings (non comments)
    while (true)
    {
      string temp;

      if (getline(file, temp))
      {

        if (temp[0] != '#')
        {
          if (temp[0] == '1')
          {
            if (s1.empty())
              s1 = temp;
            else
            {
              cout << "Error parsing file : " << filename << endl;
              return false;
            }
          }

          else if (temp[0] == '2')
          {
            if (s2.empty())
              s2 = temp;
            else
            {
              cout << "Error parsing file : " << filename << endl;
              return false;
            }
          }

          else name = temp;

          // if two string and name are non empty exit from cycle
          if (!s1.empty() && !s2.empty())
            break;
        }
      }
      else
        return true;
    }

    std::shared_ptr<KEO> entry(new KEO);

    entry->name = name.empty() ? "Noname" : name;
    entry->number = stoi(s1.substr(2, 5));
    double epoch = stod(s1.substr(18, 7));
    uint16_t e_year = static_cast<uint16_t>(epoch / 1000);
    double e_day = epoch - e_year * 1000;
    uint16_t day_num = static_cast<uint16_t>(floor(e_day));
    e_year += e_year >= 57 ? 1900 : 2000;
    // дата и время по Григорианскому календарю
    int16_t g_year, g_month, g_day, g_hour, g_minute;
    double g_second;
    // пересчет даты из представления <номер суток (от эпохи), доля суток>
    // в представление <год, месяц, день, час, минута, секунда>
    Utils::DurData(e_year, day_num, e_day - day_num, g_year, g_month, g_day, g_hour, g_minute, g_second);
    entry->jd = Utils::Julian_date(g_year, g_month, g_day, g_hour + g_minute / 60.0 + g_second / 3600.0);

    entry->drag = stod(s1.substr(34, 10));
    entry->b_star = stod(s1.substr(53, 6)) * pow(10, stod(s1.substr(59, 2)));

    entry->inclination = stod(s2.substr(8, 8));
    entry->raan = stod(s2.substr(17, 8));
    entry->eccentricity = stod("0." + s2.substr(26, 7));
    entry->arg_perigee = GR(stod(s2.substr(34, 8)));
    entry->mean_anomaly = GR(stod(s2.substr(43, 8)));
    entry->mean_motion = stod(s2.substr(52, 11));


    // частота обращения (радиан/минуту)
    double xno = entry->mean_motion * TleConst::TWOPI / TleConst::MIN_PER_DAY;

    // восстановление среднего движения и большой полуоси по элементам орбиты
    double a1 = pow(TleConst::XKE / xno, TleConst::TOTHRD);
    double betao2 = 1.0 - entry->eccentricity * entry->eccentricity;
    double betao = sqrt(betao2);
    double cos_i = cos(entry->inclination);
    double temp0 = (1.5 * TleConst::CK2 * (3.0 * cos_i * cos_i - 1.0) / (betao * betao2));
    double del1 = temp0 / (a1 * a1);
    double a0 = a1 * (1.0 - del1 * ((1.0 / 3.0) + del1 * (1.0 + 134.0 / 81.0 * del1)));
    double del0 = temp0 / (a0*a0);

    // частота обращения (рад/мин)
    entry->mean_motion_r = xno / (1.0 + del0);
    // большая полуось (а.е.)
    entry->semi_major_r = a0 / (1.0 - del0);
    // малая полуось (а.е.) - не используется
    entry->semi_minor_r = entry->semi_major_r*betao;
    // перигей (км)
    entry->perigee_r = TleConst::XKMPER_WGS72 * (entry->semi_major_r * (1.0 - entry->eccentricity) - TleConst::AE);
    // апогей (км)
    entry->apogee_r = TleConst::XKMPER_WGS72 * (entry->semi_major_r * (1.0 + entry->eccentricity) - TleConst::AE);
    // период обращения (минут)
    double period = TleConst::TWOPI / entry->mean_motion_r;

    entry->a0 = a0;
    entry->a1 = a1;
    entry->xno = xno;
    entry->betao2 = betao2;
    entry->betao = betao;
    entry->cos_i = cos_i;
    entry->period = period;

    // результат инициализации
    NoradBase::IMode imode = NoradBase::SGDP4_NOT_INIT;
    if (entry->eccentricity <= 0.0)
    {
      // специальный режим для идеальной круговой орбиты
      imode = NoradBase::SGDP4_ZERO_ECC;
    }
    else
    {
      imode = NoradBase::SGDP4_NOT_INIT;
    }

    if (imode == NoradBase::SGDP4_ZERO_ECC)
    {
      entry->norad = new NoradCircl(*entry, imode);
    }
    else
    {
      if (period >= 225.0)
      {
        // модель SDP4 - период длиннее 225 минут
        imode = NoradBase::SGDP4_DEEP_NORM;
        double epoch = (e_year - 1900) * 1.0e3 + e_day;
        entry->norad = new NoradSDP4(*entry, imode, epoch);
      }
      else
      {
        if (entry->perigee_r < 220.0)
        {
          // ближний космос, упрощенные уравнения
          imode = NoradBase::SGDP4_NEAR_SIMP;
        }
        else
        {
          // ближний космос, нормальные уравнения
          imode = NoradBase::SGDP4_NEAR_NORM;
        }
        entry->norad = new NoradSGP4(*entry, imode);
      }
    }

    _keo_el.push_back(entry);
  }
}