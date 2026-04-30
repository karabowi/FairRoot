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

////////////////////////////////////////////////////////////////////////////
// PndOtSingleStraw header file
//
// Copied from PndFtsSingleStraw
//
// authors: Radoslaw Karabowicz, GSI, 2024
////////////////////////////////////////////////////////////////////////////

#ifndef __PNDOTSINGLESTRAW__
#define __PNDOTSINGLESTRAW__
// from ROOT
#include <TH2.h>
#include <TMatrix.h>
#include <TMatrixD.h>
#include <TNamed.h>
#include <TVector3.h>
// standard
#include <vector>

// ClassImp(TMatrixDBase)
// class TMatrixD;;

class TH2F;

class PndOtSingleStraw : public TNamed {

  // --------------------------------------------------------------------
 public:
  PndOtSingleStraw();
  virtual ~PndOtSingleStraw(){};
  ClassDef(PndOtSingleStraw, 1);

  // Get methods

  Double_t GetCDist(Int_t k) { return CDist[k]; };
  Double_t GetCNele(Int_t k) { return CNele[k]; };
  Double_t GetCNeleT(Int_t k) { return CNeleT[k]; };
  Double_t GetTeleTime(Int_t k) { return TeleTime[k]; };
  Double_t GetPulse(Int_t k) { return Pulse[k]; };
  Double_t GetPulseT(Int_t k) { return PulseT[k]; };
  Double_t GetWi() { return Wi * 1.e-09; }; // in GeV
  Double_t GetGasGain() { return GasGain; };
  Double_t GetXmax() { return Xmax; };
  Double_t GetDx() { return Dx; };
  Double_t GetEmed() { return Emed; };
  Double_t GetEmin() { return Emin; };
  Double_t GetNcl() { return Ncl; };
  Double_t GetCsi() { return Csi; };
  Double_t GetDelta() { return Delta; };
  Double_t GetEmax() { return Emax; };
  Double_t GetIAr() { return IAr; };
  Int_t GetNNClus() { return NNClus; };
  Int_t GetNchann() { return Nchann; };
  Double_t GetPulseMax() { return PulseMax; };
  Double_t GetGamma() { return gamma; };
  Double_t GetBeta() { return beta; };
  Double_t GetpSTP() { return pSTP; };
  Double_t GetPulseTime() { return PulseTime; };
  Double_t GetbPolya() { return bPolya; };
  Double_t GetSigUrb() { return SigUrb; };
  Double_t GetNUrban() { return NUrban; };
  Double_t GetEup() { return Eup; };
  Double_t GetAvUrb() { return AvUrb; };

  // coordinates
  Double_t GetXin() { return Xin; };
  Double_t GetYin() { return Yin; };
  Double_t GetZin() { return Zin; };
  Double_t GetXout() { return Xout; };
  Double_t GetYout() { return Yout; };
  Double_t GetZout() { return Zout; };
  Double_t GetRpath() { return Rpath; };

  TVector3 GetWDist(Int_t k) { return WDist[k]; };

  // put methods
  void PutTrackXYZ(Double_t v1, Double_t v2, Double_t v3, Double_t v4, Double_t v5, Double_t v6)
  {
    Xin = v1;
    Yin = v2;
    Zin = v3;
    Xout = v4;
    Yout = v5;
    Zout = v6;
  };
  void PutRpath(Double_t value) { Rpath = value; };

  void PutPolya(Double_t par) { bPolya = par; };

  void PutRadius(Double_t value) { Radius = value; };
  void PutPress(Double_t value) { pSTP = value; };

  // ----------------------------------------------------------------------------
  // calls  at each track for MC applications

  // define  the wire
  void PutWireXYZ(Double_t w1, Double_t w2, Double_t w3, Double_t w4, Double_t w5, Double_t w6);
  // straw time
  Double_t PartToTime(Double_t Mass, Double_t Momentum, Double_t InOut[]);
  // ADC signal corresponding to the energy loss of StrawCharge
  Double_t PartToADC();

  // fast reconstructed distance in cm
  Double_t FastRec(Double_t TrueDcm, Int_t Flag);

  //  ADC signal charge fast simulation
  Double_t FastPartToADC();

  // ----------------------------------------------------------------------------
  // standard calls

  // once to set constants

  void TConst(Double_t Radius, Double_t pSTP, Double_t ArP, Double_t CO2P);

  // to define the track
  void TInit(Double_t Mass, Double_t Momentum, Double_t InOut[]); // for each track

  //   other   possible calls

  Double_t StrawCharge(); // total discharge electron calculation

  Int_t StrawSignal(Int_t nsteps); // oscilloscope signal generation (ns)
                                   // Pulse[PulseT] in ns
                                   // return number of primary electrons
  Int_t StrawTime();               // output time of the straw (ns) &
                                   // PulseTime[Int_t] in ns
  // Int_t StrawTot();                     // Time over threshold time of the straw (ns)  not implemented

  Double_t TimnsToDiscm(Double_t time); // from time (ns) to  radius in cm

  //-----------------------------------------------------------------------------
  // utility methods

  void TDirCos();                           // track director cosines
  Double_t TrueDist(Double_t Point[]);      // true distance wire-track
  Double_t DiffLong(Double_t distcm);       // longituinal diffusion in microns
  Double_t DiffTran(Double_t distcm);       // transverse diffusion in microns
  void Polya(Double_t bpar);                // Polya cumulative calculation
  Double_t PolyaSamp();                     // sampling from Polya distribution
  Double_t RRise(Double_t gamma);           // relativistic rise calculation
  Int_t Cluster();                          // cluster generation
  Int_t Eject();                            // primary  electron generation
  TVector3 WDistCalc(Double_t d);           // distance electron-wire
  Double_t Signal(Double_t t, Double_t t0); // signal functional form
  Double_t STEloss();                       // Landau energy loss
  Double_t STUrban();                       // Urban energy loss
  Int_t TimeEle();                          // arrivals times of all the electrons
  Double_t DistEle(Double_t tns);           // distance of all the electrons

  //------------------------------------------------------------------------------

 private:
  //-----------------------------------------------------------------

  // distance, number of electrons (with delta rays),
  // distance from wire of the cluster
  std::vector<Double_t> CDist, CDistC, CNele, CNeleT, TeleTime, AmplSig;
  std::vector<Double_t> Pulse, PulseT;
  std::vector<TVector3> WDist;

  // set constants
  // masses in GeV, energies in GeV, cgs system

  // Input for the medium

  //  Double_t PClus[20], CO2Clus[20], CumClus[21], CH4Clus[20];
  Double_t CumClus[21], CH4Clus[20]; // shadows deleted

  Double_t Wi;
  Double_t ArPerc, CO2Perc, CH4Perc;    // volume percentages
  Double_t ArWPerc, CO2WPerc, CH4WPerc; // weight percentages
  Double_t pSTP;                        // pressure (STP reference)
  Double_t Radius;                      // straw radius
  Double_t AAr;                         // Argon
  Double_t ZAr;                         // Argon
  Double_t RhoAr;                       // g/cm3  (1.78 mg/cm3)
  Double_t NclAr;                       // clusters/cm

  Double_t EmedAr;
  Double_t EminAr;
  Double_t EmpAr;
  Double_t CsiAr;
  Double_t IAr;     // ionization potential  (188 eV)
  Double_t WiAr;    // energy to reate an ion pair in Argon
  Double_t Ncl;     // mean number of cluster in the mixture
  Double_t Ecl;     // electron per cluster
  Double_t Lcl;     // mean free path between clusters
  Double_t Ntote;   // mean total number of eletrons
  Double_t GasGain; // gain of the gas
  Double_t Cutoff;  // limit the number of primry electrons
  // CO2
  Double_t EmedCO2;
  Double_t EminCO2;
  Double_t EmpCO2;
  Double_t CsiCO2;
  Double_t ACO2;   // CO2 (39.948)
  Double_t ZCO2;   // CO2 (18)
  Double_t RhoCO2; // g/cm3  CO2 (1.98 mg/cm3)
  Double_t ICO2;   // ionization potential (GeV) (188 eV)
  Double_t WiCO2;  // energy to create an ion pair
  Double_t NclCO2; // clusters/cm

  // Methane CH4 ------------------------------------------------
  Double_t EmedCH4;
  Double_t EmpCH4;
  Double_t CsiCH4;
  Double_t EminCH4;
  Double_t ACH4;   // CO2 (39.948)
  Double_t ZCH4;   // CO2 (18)
  Double_t RhoCH4; // g/cm3  CO2 (0.71 mg/cm3)
  Double_t ICH4;   // ionization potential (GeV) (188 eV)
  Double_t WiCH4;  // energy to create an ion pair
  Double_t NclCH4; // clusters/cm

  Double_t RhoMixCO2;
  Double_t RhoMixCH4;
  //----------------------------------------------------------------------
  // Input for the particle  (Gev, energy loses in Kev

  Double_t PZeta;  // charge
  Double_t piMass; // particle mass (GeV)
  Double_t PMass;  // incident particle mass
  Double_t PMom;   // particle momentum (GeV)
  Double_t Dx;     //  distance travelled in gas (cm)
  Double_t eMass;  // electron mass (GeV) (0.511 MeV)
  Double_t prMass; // proton mass
  Double_t Delta;  // polarization Sternheimer parameter
  Int_t CNumb;     // current number of clusters

  // ------------------------------------------------------
  // quantities for each track
  // calculated in TInit

  Double_t PEn; // particle energy GeV
  Double_t beta;
  Double_t gamma;
  Double_t Emed; // GeV
  Double_t Emin; // GeV/cm  (2.7 keV)
  Double_t Csi;
  Double_t Emax;
  Double_t Emp; // most probable energy
  Int_t NNClus; // number of clusters

  // ---------------------------------------------------------------------
  // mathematical and statistical  parameters

  Double_t PolyaCum[100], Xs[100];
  Double_t Xin, Yin, Zin, Xout, Yout, Zout, Rpath;
  Int_t NPolya;
  Double_t Xmax;
  Double_t bPolya;
  Double_t Calpha, Cbeta, Cgamma;
  Double_t NUrban; // total number of collision in the Urban model
  Double_t SigUrb; // std dev of the Urban distribution (routine STUrban)
  Double_t Eup;    // upper limit  of the Urban distribution (routine STUrban)
  Double_t AvUrb;  // average of the Urban distribution (routine STUrban)

  // --------------------------------------------------------------
  // for the straws

  Double_t Wx1, Wy1, Wz1, Wx2, Wy2, Wz2; // wire coordinates
  Double_t Wp, Wq, Wr;                   // director cosine of the wire
  Double_t PulseMax;
  Double_t PulseTime;
  Double_t Thresh1; // first threshold
  Double_t Thresh2;
  Int_t Nchann; // number of channels for the straw signal

  // ----------------------------------------------------------------------
  // dummy

  Double_t Out1;
  Int_t Out2, Out3;
};

#endif
