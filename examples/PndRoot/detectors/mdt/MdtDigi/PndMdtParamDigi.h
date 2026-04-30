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

//==================================================================
// description
// parameterized digitizaiton model from garfield simulation
// author, Jifeng Hu, hu@to.infn.it, Torino University
//

#ifndef PNDMDTPARAMDIGI_H
#define PNDMDTPARAMDIGI_H 1

#include "TVector3.h"
#include "TGraphErrors.h"
#include <vector>
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TRandom.h"
#include <math.h>
#include <map>

// value and its error
typedef std::pair<Double_t, Double_t> ValueErrorType;

namespace CLHEP {
class RandGeneral;
}

class PndMdtParamDigi : public TNamed {

 public:
  /** Default constructor **/
  PndMdtParamDigi();

  /** Destructor **/
  ~PndMdtParamDigi();
  void SetVerbose(Int_t v) { fVerbose = v; }
  // call Init before invoking the other interfaces.
  Bool_t Init();
  // signal on Anode wire
  //==========================================================================
  // Int_t particleType: electron, muon, pion, kaon+-, proton, indiced as 0, 1, 2, 3, 4
  // TVector3 iniP     : initial 3-momentum of this charged particle
  // TVector3 iniPos   : initial position when entering into a tube
  // TVector3 fnlPos   : final position when exiting a tube
  //==========================================================================
  // by default, strip lenght 100 cm
  // this inteface should be invoked first when parameters change
  PndMdtParamDigi &SetParams(Int_t ptlType, TVector3 iniP, TVector3 iniPos, TVector3 finalPos, Double_t stripLen = 100.);

  void UseNoise(Bool_t swith) { fUseNoise = swith; }
  void UseDetailedSim(Bool_t swith = kTRUE) { fDetailedSim = swith; }
  void UsePlot(Bool_t swith = kTRUE) { fUsePlot = swith; }
  void UseGaussianAmp(Bool_t swith) { fGaussianAmp = swith; }
  void SetOptimization(Int_t val) { fNumofTruncation = val; }
  void SetNoiseWidth(Double_t anode, Double_t strip)
  {
    fNoiseSigmaAnode = anode;
    fNoiseSigmaStrip = strip;
  }
  //===============================================================
  // readout interfaces
  // only fired information is provided
  // for wire, <-1, fired time>, for strips, <strip number from 0, fired time>
  std::vector<std::pair<Int_t, Double_t>> GetFiredInfo();
  // both anode and strip signals
  void Compute(Bool_t useConvolution = kTRUE);
  // access data of induced current
  const std::vector<Double_t> &GetWireSignal() const { return fSignalDataAnode; }
  const std::map<Int_t, std::vector<Double_t>> &GetStripSignals() const { return fSignalDataStripM; }
  //===============================================================

  // plot a charged track and its produced signal
  void Draw();

 private:
  // anode signal only
  void GetSignal(Bool_t useConvolution = kTRUE);
  // get threshold-crossing time and amplitude
  // if tube is fired, return kTRUE;
  // in case of kTRUE, time and amplitude will be set.
  Bool_t Digitize(Double_t &time, Double_t &amp);                // anode
  Bool_t Digitize(Int_t stripNo, Double_t &time, Double_t &amp); // strip
  //
  void GetRawSignalbySimAvalanche(Double_t fNoiseLevel = 1.);
  void GetRawSignalbyWeightingAvalanche(Double_t fNoiseLevel = 1.);
  void AddNoise(Double_t fNoiseLevel = 1., Int_t isAnode = 1);
  void ApplyTransferFunction(Double_t *fSignalData, Int_t nSize = fSamplingSize);

  // by default, sampling rate is set to 100 MHz, 200 sampling points
  static const Int_t fSamplingSize = 20;
  Int_t fSamplingRate;
  Int_t fSamplingInterval;
  std::vector<Double_t> fSignalDataAnode;
  std::map<Int_t, std::vector<Double_t>> fSignalDataStripM; // strip no, and its corresponding signal
  TF1 *fTrF;                                                // transfer function
  Bool_t fUseNoise;
  Bool_t fGaussianAmp;       // amplication model
  Double_t fNoiseSigmaAnode; // 0.01 mu Ampere
  Double_t fNoiseSigmaStrip; // 0.01 mu Ampere
  Int_t fNumofTruncation;

 private:
  // drift time of primary ionized electrons to wire surface
  inline Double_t GetElectronDriftTime(TVector2 iniPos)
  {
    const Double_t p0 = 5.35513e+03;
    const Double_t p1 = 3.92749e+02;
    const Double_t p2 = -5.41433e+03;
    const Double_t p3 = -3.31810e+03;
    Double_t x = iniPos.Mod() - cWIRERADIUS;
    Double_t x1 = log(1. + x);
    return (x * (p0 + p1 * pow(x, 0.25)) + x1 * (p2 + p3 * x1));
  }
  // drift time of secondary ions from wire surface to a given point
  inline Double_t GetShiftTime(TVector2 fWireSurfacePos, TVector2 fProductionPos)
  {
    const Double_t p0 = 1.81940e-8; // unit: micro sencond
    const Double_t p1 = 1.80248;
    const Double_t p2 = 3.68652e+2;
    const Double_t p3 = 1.67353e+3;
    // const Double_t mean = 0; //[R.K. 01/2017] unused variable?
    // const Double_t sigma = 2.73477e-3;//micro second //[R.K. 01/2017] unused variable?
    Double_t dr = (fProductionPos - fWireSurfacePos).Mod();
    return (((p3 * dr + p2) * dr + p1) * dr + p0 /*+ gRandom->Gaus(mean, sigma)*/) * 1.e3; // nano sencod
  }

 private:
  Int_t fParticleType;        // electron 0, muon 1, pion 2,
  TVector3 fParticleMomentum; // momentum of the incident charged particle
  TVector3 fInitPostion;
  TVector3 fExitPosition;
  Bool_t fParamsChanged;
  Double_t fStripLength;
  Double_t fRestMass[5];

  void GetMPVofPrimaryIonization(Int_t particleType, const TVector3 &momentum, ValueErrorType &val) const;
  Int_t GetAmplicationFactor(Int_t particleType, Double_t momentum) const;
  // sampling position of secondary ion/electron for a given center
  Bool_t fDetailedSim;
  void SamplingPosition(TVector2 fDirection, TVector2 &fIonProductionPos);
  // mean free path function as velocity
  TGraphErrors *gFreePath[5]; //

  inline Double_t GetMeanFreePath(Int_t ptlType, Double_t mom) const
  {
    Double_t mMass = fRestMass[ptlType];
    // Double_t mEnergy = sqrt(mom*mom + mMass*mMass);
    Double_t mBeta = mom / mMass;
    Double_t mPath = gFreePath[ptlType]->Eval(mBeta);
    return gRandom->Exp(mPath);
  }

  // avalanche size function
  Bool_t fUseAvaHist;
  TH1F *hAvaSize;
  // TH1F* hAnodeI;
  // TH1F* hCathodeIx;
  // TH1F* hCathodeIy;
  //
  ////parameters for avalanche
  static const Int_t NRAD = 100; // radial bins, not changeable
  static const Int_t NPHI = 100; // phi bins, not changeable
  // geometry constant
  Double_t cWIRERADIUS; // unit: cm
  Double_t cCELLSIZE;   // unit: cm
  Double_t cSTRIPWIDTH; // unit: cm
  Double_t cMAXRADIUS;  // not changeable
  Double_t cMAPFACTOR;  // not changeable
  Double_t cUNITLOGDR;  //= TMath::Log(1. + cMAPFACTOR*(cMAXRADIUS - cWIRERADIUS))/NRAD;
  Double_t cUNITPHI;    //= TMath::Pi()*2./NPHI;
  TH1F *hAnodeI1d[NRAD];
  TH1F *hCathodeI2d[NRAD][NPHI];
  Int_t fNr[NRAD];
  Int_t fNrNphi[NRAD * NPHI];

  // cluster initialization
  struct ClusInfo {
    explicit ClusInfo(TVector3 _v = TVector3()) : fPosition(_v) {}
    TVector3 fPosition;
    Int_t fNumofPrimaryIonization;
    Int_t fNumofIonsofThisCluster;
    Double_t fStartTime;
  };

  // histograms
  Bool_t fUsePlot;
  TH1F *hIndex;
  TH2F *h2dIndex;
  TH2F *hTrack;
  TH1F *hIonNum;
  TH2F *hAvaPos;
  TH1F *hAvaNum;
  TH1F *hAva1D;
  TH2F *hAva2D;
  //
  TH1F *hExp1;
  TH1F *hExp2;
  TH1F *hGen;
  TH1F *hLan;
  Int_t fVerbose;
  //
 public:
  struct AvaBinType {
    AvaBinType(Int_t _i = 0, Double_t _v = 0.) : Index(_i), Probabilty(_v) {}
    Int_t Index;
    Double_t Probabilty;
  }; // index, density
 private:
  std::vector<AvaBinType> fProbFunc1D;
  std::vector<AvaBinType> fProbFunc2D;

  //
  Double_t SPEEDOFLIGHT;
  CLHEP::RandGeneral *fRandAva;

  ClassDef(PndMdtParamDigi, 1);
};

#endif
