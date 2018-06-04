#ifndef LARSYST_UTILITY_STRINGPARSERS_SEEN
#define LARSYST_UTILITY_STRINGPARSERS_SEEN

#include <algorithm>
#include <cctype>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

// #define LARSYST_UTILITY_STRINGPARSERS_DEBUG 1

namespace larsyst {

template <typename T> inline T str2T(std::string const &str) {
  std::istringstream stream(str);
  T d;
  stream >> d;

  if (stream.fail()) {
    std::cerr << "[WARN]: Failed to parse string: " << str
              << " as requested type." << std::endl;
    return T();
  }

  return d;
}

template <> inline bool str2T<bool>(std::string const &str) {
  if ((str == "true") || (str == "True") || (str == "TRUE") || (str == "1")) {
    return true;
  }

  if ((str == "false") || (str == "False") || (str == "FALSE") ||
      (str == "0")) {
    return false;
  }

  std::istringstream stream(str);
  bool d;
  stream >> d;

  if (stream.fail()) {
    std::cerr << "[WARN]: Failed to parse string: " << str
              << " as requested type." << std::endl;
    return false;
  }

  return d;
}
template <typename T>
inline void AppendVect(std::vector<T> &target, std::vector<T> const &toApp) {
  for (size_t i = 0; i < toApp.size(); ++i) {
    target.push_back(toApp[i]);
  }
}

// trim from start (in place)
static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  [](int ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](int ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

template <typename T>
inline std::vector<T>
ParseToVect(std::string const &inp, std::string const &delim,
            bool PushEmpty = false, bool trimInput = true) {
  std::string inpCpy = inp;
  if (trimInput) {
    trim(inpCpy);
  }
  size_t nextOccurence = 0;
  size_t prevOccurence = 0;
  std::vector<T> outV;
  bool AtEnd = false;
  while (!AtEnd) {
    nextOccurence = inpCpy.find(delim, prevOccurence);
    if (nextOccurence == std::string::npos) {
      if (prevOccurence == inpCpy.length()) {
        break;
      }
      AtEnd = true;
    }
    if (PushEmpty || (nextOccurence != prevOccurence)) {
      outV.push_back(str2T<T>(
          inpCpy.substr(prevOccurence, (nextOccurence - prevOccurence))));
    }
    prevOccurence = nextOccurence + delim.size();
  }
  return outV;
}

// Converts "5_10:1" into a vector containing: 5,6,7,8,9,10
inline std::vector<double> BuildDoubleList(std::string const &str) {
#ifdef LARSYST_UTILITY_STRINGPARSERS_DEBUG
  std::cout << "------ Translating " << str << " into..." << std::endl;
#endif
  std::vector<std::string> steps = ParseToVect<std::string>(str, ":");
  if (steps.size() != 2) {
    return ParseToVect<double>(str, ",");
  }
  double step = str2T<double>(steps[1]);

  std::vector<double> range = ParseToVect<double>(steps[0], "_");
  if (steps.size() != 2) {
    std::cout
        << "[ERROR]: When attempting to parse bin range descriptor: \" " << str
        << "\", couldn't determine range. Expect form: <bin1low>_<binXUp>:step"
        << std::endl;
    exit(1);
  }

  int nsteps = (range[1] - range[0]) / step;

#ifdef LARSYST_UTILITY_STRINGPARSERS_DEBUG
  std::cout << "------ return [" << std::flush;
#endif
  std::vector<double> rtn;
  for (int step_it = 0; step_it <= nsteps; ++step_it) {
    rtn.push_back(range[0] + step * step_it);
#ifdef LARSYST_UTILITY_STRINGPARSERS_DEBUG
    std::cout << rtn.back() << ", " << std::flush;
#endif
  }
#ifdef LARSYST_UTILITY_STRINGPARSERS_DEBUG
  std::cout << "]" << std::endl;
#endif
  return rtn;
}

// Converts "1,5_10:1,15" into a vector containing: 1,5,6,7,8,9,10,15
inline std::vector<double> ParseDoubleListDescriptor(std::string const &str) {
#ifdef LARSYST_UTILITY_STRINGPARSERS_DEBUG
  std::cout << "--- Translating " << str << " into... " << std::endl;
#endif

  std::vector<std::string> splitDescriptors =
      ParseToVect<std::string>(str, ",");

  std::vector<double> list;
  for (size_t vbd_it = 0; vbd_it < splitDescriptors.size(); ++vbd_it) {
    AppendVect(list, BuildDoubleList(splitDescriptors[vbd_it]));
  }

  for (std::vector<double>::iterator i = list.begin(); i < list.end();) {
    std::vector<double>::iterator c = i;
    std::advance(i, 1);
    if ((*i) == (*c)) {
      i = list.erase(i);
    }
  }
#ifdef LARSYST_UTILITY_STRINGPARSERS_DEBUG
  std::cout << "--- return [" << std::flush;
#endif
  for (std::vector<double>::iterator i = list.begin(); i < list.end();
       std::advance(i, 1)) {
#ifdef LARSYST_UTILITY_STRINGPARSERS_DEBUG
    std::cout << (*i) << ", " << std::flush;
#endif
  }
#ifdef LARSYST_UTILITY_STRINGPARSERS_DEBUG
  std::cout << "]." << std::endl;
#endif
  return list;
}
} // namespace larsyst
#endif
