#pragma once

#include "systematicstools/utility/exceptions.hh"

#include "TAxis.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#include <limits>
#include <memory>
#include <string>

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

inline bool IsFlowBin(TAxis const *ax, Int_t bin_it) {
  return ((bin_it <= 0) || (bin_it >= (ax->GetNbins() + 1)));
}

inline bool IsInHistogramRange(TAxis const *ax, double v) {
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

static Int_t const kBinOutsideRange = std::numeric_limits<Int_t>::max();

template <size_t N> struct THType {};
template <> struct THType<1> {
  typedef TH1 type;
  static size_t GetNbins(type const *H, bool inc_flow = false) {
    return inc_flow ? (H->GetNbinsX() + 2) : H->GetNbinsX();
  }
  static size_t GetNbins(std::unique_ptr<type> const &H,
                         bool inc_flow = false) {
    return GetNbins(H.get(), inc_flow);
  }
  static bool IsFlowBin(type const *H, Int_t bin) {
    return ::IsFlowBin(H->GetXaxis(), bin);
  }
  static bool IsFlowBin(std::unique_ptr<type> const &H, Int_t bin) {
    return IsFlowBin(H.get(), bin);
  }
  static Int_t GetBin(type const *H, std::array<double, 1> const &vals) {
    Int_t bin = H->GetXaxis()->FindFixBin(vals[0]);
    if (IsFlowBin(H, bin)) {
      return kBinOutsideRange;
    }
    return bin;
  }
};
template <> struct THType<2> {
  typedef TH2 type;
  static size_t GetNbins(type const *H, bool inc_flow = false) {
    return inc_flow ? ((H->GetNbinsX() + 2) * (H->GetNbinsY() + 2))
                    : (H->GetNbinsX() * H->GetNbinsY());
  }
  static size_t GetNbins(std::unique_ptr<type> const &H,
                         bool inc_flow = false) {
    return GetNbins(H.get(), inc_flow);
  }
  static bool IsFlowBin(type const *H, Int_t bin) {
    Int_t xbin, ybin, zbin;
    H->GetBinXYZ(bin, xbin, ybin, zbin);
    return ::IsFlowBin(H->GetXaxis(), xbin) || ::IsFlowBin(H->GetYaxis(), ybin);
  }
  static bool IsFlowBin(std::unique_ptr<type> const &H, Int_t bin) {
    return IsFlowBin(H.get(), bin);
  }
  static Int_t GetBin(type const *H, std::array<double, 2> const &vals) {
    Int_t xbin = H->GetXaxis()->FindFixBin(vals[0]);
    Int_t ybin = H->GetYaxis()->FindFixBin(vals[1]);
    Int_t gbin = H->GetBin(xbin, ybin);
    if (IsFlowBin(H, gbin)) {
      return kBinOutsideRange;
    }
    return gbin;
  }
};
template <> struct THType<3> {
  typedef TH3 type;
  static size_t GetNbins(type const *H, bool inc_flow = false) {
    return inc_flow ? ((H->GetNbinsX() + 2) * (H->GetNbinsY() + 2) *
                       (H->GetNbinsZ() + 2))
                    : (H->GetNbinsX() * H->GetNbinsY() * H->GetNbinsZ());
  }
  static size_t GetNbins(std::unique_ptr<type> const &H,
                         bool inc_flow = false) {
    return GetNbins(H.get(), inc_flow);
  }
  static bool IsFlowBin(type const *H, Int_t bin) {
    Int_t xbin, ybin, zbin;
    H->GetBinXYZ(bin, xbin, ybin, zbin);
    return ::IsFlowBin(H->GetXaxis(), xbin) ||
           ::IsFlowBin(H->GetYaxis(), ybin) || ::IsFlowBin(H->GetZaxis(), zbin);
  }
  static bool IsFlowBin(std::unique_ptr<type> const &H, Int_t bin) {
    return IsFlowBin(H.get(), bin);
  }
  static Int_t GetBin(type const *H, std::array<double, 3> const &vals) {
    Int_t xbin = H->GetXaxis()->FindFixBin(vals[0]);
    Int_t ybin = H->GetYaxis()->FindFixBin(vals[1]);
    Int_t zbin = H->GetYaxis()->FindFixBin(vals[2]);
    Int_t gbin = H->GetBin(xbin, ybin, zbin);
    if (IsFlowBin(H, gbin)) {
      return kBinOutsideRange;
    }
    return gbin;
  }
};