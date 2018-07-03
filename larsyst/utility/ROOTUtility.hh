#ifndef LARSYST_UTILITY_ROOTUTILITY_SEEN
#define LARSYST_UTILITY_ROOTUTILITY_SEEN

#include "larsyst/utility/exceptions.hh"

#include "TFile.h"

#include <string>

#define NEW_EXCEPT(EXCEPT_NAME)                                                \
  struct EXCEPT_NAME : public larsyst::larsyst_except {                        \
    EXCEPT_NAME() : larsyst::larsyst_except() {}                               \
    EXCEPT_NAME(EXCEPT_NAME const &other) : larsyst::larsyst_except(other) {}  \
    template <typename T> EXCEPT_NAME &operator<<(T const &obj) {              \
      msgstrm << obj;                                                          \
      msg = msgstrm.str();                                                     \
      return (*this);                                                          \
    }                                                                          \
  }

NEW_EXCEPT(invalid_tfile);
NEW_EXCEPT(invalid_hist_name);

#undef NEW_EXCEPT

inline TFile *CheckOpenFile(std::string const &fname, char const *opts) {
  TFile *inpF = new TFile(fname.c_str(), opts);
  if (!inpF || !inpF->IsOpen()) {
    throw invalid_tfile() << "[ERROR]: Couldn't open input file: " << fname;
  }
  return inpF;
}

template <class TH>
inline TH *GetHistogram(TFile *f, std::string const &fhname) {
  TH *inpH = dynamic_cast<TH *>(f->Get(fhname.c_str()));

  if (!inpH) {
    throw invalid_hist_name() << "[ERROR]: Couldn't get TH: " << fhname
                              << " from input file: " << f->GetName();
  }

  inpH = static_cast<TH *>(inpH->Clone());
  inpH->SetDirectory(nullptr);
  return inpH;
}

template <class TH>
inline TH *GetHistogram(std::string const &fname, std::string const &hname) {
  TDirectory *ogDir = gDirectory;

  TFile *inpF = CheckOpenFile(fname);

  TH *h = GetHistogram<TH>(inpF, hname);

  inpF->Close();
  delete inpF;

  if (ogDir) {
    ogDir->cd();
  }

  return h;
}

#endif
