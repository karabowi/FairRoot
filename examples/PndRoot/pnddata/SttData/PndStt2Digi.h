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
 

#ifndef PNDSTT2DIGI_H
#define PNDSTT2DIGI_H

#include "FairTimeStamp.h"

#include "TObject.h"
#include "TString.h"
#include "PndDetectorList.h"
#include <iostream>
#include <vector>


class PndStt2Digi : public FairTimeStamp {
  
  //friend std::ostream &operator<<(std::ostream &out, PndStt2Digi &digi)
  //{
    //out << "PndStt2 Digi in sensor: " << digi.GetTubeID()
        //<< " drift time: " << digi.DriftTime()
        //<< " amplitude: " << digi.GetEloss() 
        //<< " timestamp: " << digi.GetTimeStamp() << endl;
    //return out;
  //}

 public:
  PndStt2Digi();
  PndStt2Digi(Int_t mcindex, Int_t detID, Int_t tubeID, Double_t eloss, Double_t driftTime, Double_t hittime, Double_t dist2wire, Double_t timestamp = -1);
  /**<constructor
   * \param index  position of PndSttMCPoint in TClonesArray
   * \param detID  detector ID (from/for PndPoint/Hit)
   * \param tubeID ShortID of tube
   * \param eloss  energy loss
   */
  virtual ~PndStt2Digi(){};

  Int_t GetMcIndex()      const { return fMcIndex; }
  Int_t GetDetID()        const { return fDetID; }
  Int_t GetTubeID()       const { return fTubeID; }
  Double_t GetEloss()     const { return fEloss; }
  Double_t GetDriftTime() const { return fDriftTime; }
  Double_t GetHitTime()   const { return fHitTime; }
  Double_t GetDist2Wire() const { return fDist2Wire; }

  //virtual void Print() { std::cout << *this; }

 protected:
  Int_t fMcIndex;            // index of the MC point
  Int_t fDetID;              // branch ID of mc point array
  Int_t fTubeID;             // Geometry ID for tube 
  Double_t fEloss;           // collected E_loss
  Double_t fDriftTime;       // Drift time 
  Double_t fHitTime;         // Hit time = t_flight + t_drift + t_signal  
  Double_t fDist2Wire;       // Distance to wire

  ClassDef(PndStt2Digi, 1);
};

#endif
