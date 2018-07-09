#ifndef LARSYST_UTILITY_ROOTUTILITY_SEEN
#define LARSYST_UTILITY_ROOTUTILITY_SEEN

#include "larsyst/utility/exceptions.hh"

#include "TFile.h"
#include "TAxis.h"

#include <string>

NEW_LARSYST_EXCEPT(invalid_tfile);
NEW_LARSYST_EXCEPT(invalid_hist_name);

inline TFile *CheckOpenFile(std::string const &fname, char const *opts = "") {
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

inline bool IsFlowBin(TAxis *ax, Int_t bin_it) {
  return ((bin_it == 0) || (bin_it == (ax->GetNbins()+1)));
}

inline bool IsInHistogramRange(TAxis *ax, double v) {
  Int_t bin_it = ax->FindFixBin(v);
  return !IsFlowBin(ax,bin_it);
}

#endif
