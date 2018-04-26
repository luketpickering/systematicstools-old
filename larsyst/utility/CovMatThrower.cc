#include "CovMatThrower.hh"

#include "TDecompChol.h"

#include <iostream>

CovarianceThrower::CovarianceThrower(int NRows)
    : UncertMatrix(nullptr), LMatrix(nullptr), RVector(nullptr),
      CVector(nullptr), RNgine(nullptr), RNJesus(nullptr), NRows(NRows) {
  UncertMatrix = new TMatrixD(NRows, NRows);

  LMatrix = new TMatrixD(NRows, NRows);
  RVector = new TMatrixD(NRows, 1);
  CVector = new TMatrixD(NRows, 1);
}

void CovarianceThrower::SetupDecomp() {
  TDecompChol decomp(*UncertMatrix);
  if (!decomp.Decompose()) {
    std::cout << "[ERROR]: Failed to decompose uncertainty matrix."
              << std::endl;
    throw;
  }
  (*LMatrix) = decomp.GetU();
  (*LMatrix) = LMatrix->Transpose(*LMatrix);
}

CovarianceThrower::CovarianceThrower(TMatrixD &covmat,
                  std::unique_ptr<CLHEP::HepRandomEngine> &RNgine)
    : CovarianceThrower(covmat.GetNrows()) {
  for (int i = 0; i < NRows; ++i) {
    for (int j = 0; j < NRows; ++j) {
      (*UncertMatrix)[i][j] = covmat[i][j];
    }
  }

  RNJesus = std::make_unique<CLHEP::RandGaussQ>(*RNgine);
  SetupDecomp();
}

CovarianceThrower::CovarianceThrower(TMatrixDSym &covmat,
                  std::unique_ptr<CLHEP::HepRandomEngine> &RNgine)
    : CovarianceThrower(covmat.GetNrows()) {
  for (int i = 0; i < NRows; ++i) {
    for (int j = 0; j < NRows; ++j) {
      (*UncertMatrix)[i][j] = covmat[i][j];
    }
  }

  RNJesus = std::make_unique<CLHEP::RandGaussQ>(*RNgine);
  SetupDecomp();
}

CovarianceThrower::CovarianceThrower(TMatrixDSym &covmat, uint64_t Seed)
    : CovarianceThrower(covmat.GetNrows()) {
  for (int i = 0; i < NRows; ++i) {
    for (int j = 0; j < NRows; ++j) {
      (*UncertMatrix)[i][j] = covmat[i][j];
    }
  }

  RNgine = std::make_unique<CLHEP::MTwistEngine>(Seed);
  RNJesus = std::make_unique<CLHEP::RandGaussQ>(*RNgine);

  SetupDecomp();
}

CovarianceThrower::CovarianceThrower(TMatrixD &covmat, uint64_t Seed)
    : CovarianceThrower(covmat.GetNrows()) {
  for (int i = 0; i < NRows; ++i) {
    for (int j = 0; j < NRows; ++j) {
      (*UncertMatrix)[i][j] = covmat[i][j];
    }
  }

  RNgine = std::make_unique<CLHEP::MTwistEngine>(Seed);
  RNJesus = std::make_unique<CLHEP::RandGaussQ>(*RNgine);

  SetupDecomp();
}

TMatrixD const *CovarianceThrower::Throw() {
  for (int p_it = 0; p_it < NRows; ++p_it) {
    (*RVector)[p_it][0] = RNJesus->fire(0, 1);
  }
  (*CVector) = (*LMatrix) * (*RVector);

  return CVector;
}
