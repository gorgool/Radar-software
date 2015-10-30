#include <vector>
#include <string>
#include <sstream>

namespace Utils
{
  template<typename T, char ... delims>
  std::vector<T> split_string(const std::string& input)
  {
    static char _delims[] = { delims ... };
    struct tokens : std::ctype<char>
    {
      tokens() : std::ctype<char>(get_table()) {}

      static std::ctype_base::mask const* get_table()
      {
        typedef std::ctype<char> cctype;
        static const cctype::mask *const_rc = cctype::classic_table();

        static cctype::mask rc[cctype::table_size];
        memcpy(rc, const_rc, cctype::table_size * sizeof(cctype::mask));

        for (char del : _delims)
        {
          rc[static_cast<std::size_t>(del)] = std::ctype_base::space;
        }
        return &rc[0];
      }
    };

    std::stringstream ss(input);
    ss.imbue(std::locale(std::locale(), new tokens()));
    return std::vector<T>(std::istream_iterator<T>{ ss }, std::istream_iterator<T>());
  }
}
