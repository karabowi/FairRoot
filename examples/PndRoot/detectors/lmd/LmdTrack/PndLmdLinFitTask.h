//****************************************************************************
//*                   This file is part of PandaRoot.                        *
//*                                                                          *
//*            PandaRoot is distributed under the terms of the               *
//*              GNU General Public License (GPL) version 3,                 *
//*                 copied verbatim in the file "LICENSE".                   *
//*                                                                          *
//*  Copyright (C) 2006 - 2024 FAIR GmbH and copyright holders of PandaRoot  *
//*     The copyright holders are listed in the file "COPYRIGHTHOLDERS".     *
//*               The authors are listed in the file "AUTHORS".              *
//****************************************************************************

//-----------------------------------------------------------
//
// Description:
//      3D Straight Line fitter
//
// Author List:
//      Mathias Michel
//      Anastasia Karavdina
//-----------------------------------------------------------

#ifndef PNDLMDLINFITTASK_H
#define PNDLMDLINFITTASK_H

#include "FairTask.h"

#include "TMatrixDSym.h"
#include "TString.h"

class PndGeoHandling;
class TGraph2DErrors;
class TClonesArray;
class TGeoManager;
class TVector3;
namespace ROOT {
namespace Math {
class Minimizer;
}
} // namespace ROOT

class PndLmdLinFitTask : public FairTask {
 public:
  // Constructors/Destructors ---------
  PndLmdLinFitTask();
  PndLmdLinFitTask(TString tTCandBranchName, TString tRecoBranchName, TString tOutputBranchName = "LMDPndTrack", TString tOutputFolder = "PndLmd");

  virtual ~PndLmdLinFitTask();

  virtual InitStatus Init();

  virtual void Exec(Option_t *opt);

  virtual void Finish();

  void SetRadLen(double x) { ftotRadLen = 1e-2 * x; } // rad.length X/X0 [%]

 protected:
  double FCN_MS(const double *vars);
  static double distance_MS(double x, double y, double z, double errx, double erry, const double *p, double *zpr);
  double ScatteredAngle(double radLen); // calculate uncertainty for kink angle
  double line3DfitMS(TGraph2DErrors *gr, const TVector3 &posSeed, const TVector3 &dirSeed, std::vector<double> &fitpar,
                     TMatrixDSym *covmatrix); // fit with kink angle
  double GetSigmaMS(int side)
  {
    if (side < 1)
      return fsigmaMSa;
    else
      return fsigmaMSb;
  }

  // Input Data------------
  TClonesArray *fTCandArray;
  TClonesArray *fRecoArray;

  TString fTCandBranchName;
  TString fRecoBranchName;
  TString fOutputBranchName;
  TString fOutputFolder;
  // Output Data----------
  TClonesArray *fTrackArray;

  double fPbeam;
  double fsigmaMSa; // single hit before CVD diamond
  double fsigmaMSb; // single hit after CVD diamond

  bool hitMergedfl[4];
  double fPDGCode;
  int fCharge;
  PndGeoHandling *fGeoH;

  ROOT::Math::Minimizer *fmin;
  TGraph2DErrors *fGraph2D;

  double ftotRadLen; // X/X0

  ClassDef(PndLmdLinFitTask, 2);
};

#endif
