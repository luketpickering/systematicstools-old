#ifndef SYSTTOOLS_UTILITY_ROOTUTILITY_SEEN
#define SYSTTOOLS_UTILITY_ROOTUTILITY_SEEN

#include "systematicstools/utility/exceptions.hh"

#include "TAxis.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"

#include <string>
#include <memory>

NEW_SYSTTOOLS_EXCEPT(invalid_tfile);
NEW_SYSTTOOLS_EXCEPT(invalid_hist_name);

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
  return ((bin_it == 0) || (bin_it == (ax->GetNbins() + 1)));
}

inline bool IsInHistogramRange(TAxis *ax, double v) {
  Int_t bin_it = ax->FindFixBin(v);
  return !IsFlowBin(ax, bin_it);
}

template <size_t n>
inline std::array<double, n + 1>
GetPolyFitCoeffs(std::vector<double> const &xvals,
                 std::vector<double> const &yvals) {

  size_t nd = std::min(xvals.size(), yvals.size());
  TGraph g(nd);
  for (size_t i = 0; i < nd; ++i) {
    g.SetPoint(i, xvals[i], yvals[i]);
  }

  static std::stringstream ss;
  static bool first = true;
  if (first) {
    std::stringstream x("");
    for (size_t i = 0; i < n + 1; ++i) {
      ss << '[' << i << "]" << (i ? "*" : "") << x.str();
      x << (i ? "*x" : "x");
      if (i < n) {
        ss << " + ";
      }
    }
    first = false;
  }

  static std::unique_ptr<TF1> f(
      new TF1("f", ss.str().c_str(), xvals.front(), xvals.back()));
  if (f->IsZombie()) {
    throw;
  }

  if (f->GetNpar() != (n + 1)) {
    throw;
  }

  g.Fit(f.get(), "Q0");

  std::array<double, n + 1> rtn;
  for (size_t i = 0; i < (n + 1); ++i) {
    rtn[i] = f->GetParameter(i);
  }

  return rtn;
}

#endif
