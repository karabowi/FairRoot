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
 
#ifndef PNDSTT2DIGIPAR_H
#define PNDSTT2DIGIPAR_H

#include "FairParGenericSet.h"
#include <iostream>

#include "TObjArray.h"
#include "TArrayD.h"

//typedef std::string TStr; 
//typedef vector<TStr> TvStr;

using namespace std;


class PndStt2DigiPar : public FairParGenericSet {
 public:
  PndStt2DigiPar(const char *name = "PndStt2DigiPar", const char *title = "Stt Digitization Parameters", const char *context = "TestDefaultContext");
  ~PndStt2DigiPar(void) {};
  void clear(void) {};
  void putParams(FairParamList *l);
  Bool_t getParams(FairParamList *l);
  
  Text_t* GetFcnTime2RadMean() { return fFcnTime2RadMean; }
  Text_t* GetFcnTime2RadSigL() { return fFcnTime2RadSigL; }
  Text_t* GetFcnTime2RadSigH() { return fFcnTime2RadSigH; }
  Text_t* GetFcnRad2TimeMean() { return fFcnRad2TimeMean; }
  Text_t* GetFcnRad2TimeSigL() { return fFcnRad2TimeSigL; }
  Text_t* GetFcnRad2TimeSigH() { return fFcnRad2TimeSigH; }
  
  Double_t GetResFactTime()    { return fResFactTime; }
  Double_t GetResFactRad()     { return fResFactRad; }
  Double_t GetResIsoIdeal()    { return fResIsoIdeal; }
  Double_t GetSignalSpeed()    { return fSignalSpeed; }
  Double_t GetReadoutZ()       { return fReadoutZ; }
  
  Text_t  fFcnTime2RadMean[255];  // parameterization mean for       t_drift -> r_iso   as <formula>,<parameter list>, e.g. "pol3(0),1.0,2.3,-0.2,0.002"
  Text_t  fFcnTime2RadSigL[255];  // parameterization sigma low for  t_drift -> r_iso   as <formula>,<parameter list>
  Text_t  fFcnTime2RadSigH[255];  // parameterization sigma high for t_drift -> r_iso   as <formula>,<parameter list>
  Text_t  fFcnRad2TimeMean[255];  // parameterization mean for       r_dist  -> t_drift as <formula>,<parameter list>
  Text_t  fFcnRad2TimeSigL[255];  // parameterization sigma low for  r_dist  -> t_drift as <formula>,<parameter list>
  Text_t  fFcnRad2TimeSigH[255];  // parameterization sigma high for r_dist  -> t_drift as <formula>,<parameter list>
  
  Double_t fResFactTime;          // multiplicator for time resolution
  Double_t fResFactRad;           // multiplicator for isochrone radius resolution
  Double_t fResIsoIdeal;          // resolution of ideal hit iso radius (just smears true radius) [cm]
  Double_t fSignalSpeed;          // speed of signal along wire
  Double_t fReadoutZ;             // z position of readout plane
  
  ClassDef(PndStt2DigiPar, 1)
};

#endif
