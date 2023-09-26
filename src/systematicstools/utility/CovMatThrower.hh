#pragma once

#include "CLHEP/Random/MTwistEngine.h"
#include "CLHEP/Random/RandGaussQ.h"

#include "TMatrixDSym.h"
#include "TMatrixD.h"

#include <memory>

class CovarianceThrower {
  TMatrixD *UncertMatrix;
  TMatrixD *LMatrix;
  TMatrixD *RVector;

  TMatrixD *CVector;

  std::unique_ptr<CLHEP::HepRandomEngine> RNgine;
  std::unique_ptr<CLHEP::RandGaussQ> RNJesus;

  int NRows;

  CovarianceThrower(int NRows);

public:
  void SetupDecomp();

  CovarianceThrower(TMatrixD &covmat, uint64_t Seed = 0);
  CovarianceThrower(TMatrixDSym &covmat, uint64_t Seed = 0);
  CovarianceThrower(TMatrixD &covmat,
                    std::unique_ptr<CLHEP::HepRandomEngine> &);
  CovarianceThrower(TMatrixDSym &covmat,
                    std::unique_ptr<CLHEP::HepRandomEngine> &);

  TMatrixD const *Throw();

  ~CovarianceThrower() {
    delete UncertMatrix;
    delete LMatrix;
    delete RVector;
    delete CVector;
  }
};
