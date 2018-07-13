#ifndef LARSYST_UTILITY_EXCEPTIONS_SEEN
#define LARSYST_UTILITY_EXCEPTIONS_SEEN

#include <sstream>
#include <stdexcept>
#include <string>

namespace larsyst {
struct larsyst_except : public std::exception {
  std::stringstream msgstrm;
  std::string msg;
  larsyst_except() : msgstrm(), msg() {}
  larsyst_except(larsyst_except const &other) : msgstrm(), msg() {
    msgstrm << other.msg;
    msg = other.msg;
  }
  const char *what() const noexcept { return msg.c_str(); }

  template <typename T> larsyst_except &operator<<(T const &obj) {
    msgstrm << obj;
    msg = msgstrm.str();
    return (*this);
  }
};

} // namespace larsyst

#define NEW_LARSYST_EXCEPT(EXCEPT_NAME)                                        \
  struct EXCEPT_NAME : public larsyst::larsyst_except {                        \
    EXCEPT_NAME() : larsyst::larsyst_except() {}                               \
    EXCEPT_NAME(EXCEPT_NAME const &other) : larsyst_except(other) {}           \
    template <typename T> EXCEPT_NAME &operator<<(T const &obj) {              \
      msgstrm << obj;                                                          \
      msg = msgstrm.str();                                                     \
      return (*this);                                                          \
    }                                                                          \
  }

NEW_LARSYST_EXCEPT(invalid_parameter_name);
NEW_LARSYST_EXCEPT(invalid_parameter_Id);
NEW_LARSYST_EXCEPT(invalid_parameter_value);
NEW_LARSYST_EXCEPT(incorrectly_configured);
NEW_LARSYST_EXCEPT(parameter_Id_not_handled);
NEW_LARSYST_EXCEPT(parameter_name_not_handled);
NEW_LARSYST_EXCEPT(systParamId_collision);

#endif
