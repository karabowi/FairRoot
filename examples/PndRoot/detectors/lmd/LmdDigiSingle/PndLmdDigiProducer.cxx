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

/*
 * PndLmdDigiProducer.cxx
 *
 *  Created on: Aug 10, 2009
 *      Author: huagen
 */

// the implementation of digiproducer

#include "PndLmdDigiProducer.h"
#include "PndLmdDigiPara.h"
#include "PndLmdDigiStrips.h"
#include "PndLmdMCPoint.h"
#include "PndLmdCalStrip.h"
#include "PndLmdStrips.h"
#include "PndLmdGeoHandling.h"

#include "TGeoManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "PndStringVector.h"
#include "FairParamList.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TVector3.h"

#include <iostream>
#include <vector>
using namespace std;

PndLmdDigiProducer::PndLmdDigiProducer() : FairTask("Pnd LMD Digi Producer")
{
  fBranchName = "PndLmdMCPoint";
}

PndLmdDigiProducer::PndLmdDigiProducer(Int_t k, SensorSegment segment) : FairTask("Pnd LMD Digi Producer")
{
  fBranchName = "PndLmdMCPoint";
  //	fDigiParTrap = digipar;
  fSegment = segment;
}

PndLmdDigiProducer::PndLmdDigiProducer(Double_t k, SensorSide side) : FairTask("Pnd LMD Digi Producer")
{
  fBranchName = "PndLmdMCPoint";
  //  fDigiParTrap = digipar;
  fSide = side;
}

PndLmdDigiProducer ::~PndLmdDigiProducer()
{
  delete fGeoH;
}

void PndLmdDigiProducer::SetParContainers()
{
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fDigiParTrap = (PndLmdDigiPara *)(rtdb->getContainer("LmdStripDigiParaTrap"));
  // if(!fDigiParTrap) cout<<"The digi parameters has been set to container!"<<endl;
  // fDigiParTrap->print();
}

InitStatus PndLmdDigiProducer::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// init method for the digiproducer, input/output array, digipar file etc.
InitStatus PndLmdDigiProducer::Init()
{
  FairRun *ana = FairRun::Instance();
  FairRootManager *ioman = FairRootManager::Instance();

  fGeoH = new PndLmdGeoHandling(gGeoManager);

  if (!ioman) {
    std::cout << "-E- PndLmdDigiProducer::Init :"
              << " The FariRootManager is not initialized!" << std::endl;
    return kFATAL;
  }

  // input array initialization
  fPointArray = (TClonesArray *)ioman->GetObject(fBranchName);
  if (!fPointArray) {
    std::cout << "-E- PndLmdDigiProducer::Init :"
              << "No PndLmdMCPoint Array" << std::endl;
    return kERROR;
  }

  // creat and register output array
  fStripArray = new TClonesArray("PndLmdDigiStrips");
  ioman->Register("LMDStripDigis", "LMD", fStripArray, kTRUE);

  if (!fDigiParTrap) {
    LOG(error) << " PndLmdDigiProducer::DigiParTrap Container doesn't exist!";
    return kERROR;
  }
  // print the digipar parameters for check
  fDigiParTrap->print();

  fStripCalTrapLeft = new PndLmdCalStrip(fDigiParTrap, kLEFT);
  cout << "the digi paras for left segment!" << endl;
  fStripCalTrapLeft->print();

  fStripCalTrapRight = new PndLmdCalStrip(fDigiParTrap, kRIGHT);
  cout << "the digi paras for right segment!" << endl;
  fStripCalTrapRight->print();

  std::cout << "the PndLmdDigiProcuder initialized successfully!" << std::endl;

  return kSUCCESS;
}

void PndLmdDigiProducer::Exec(Option_t *)
{
  // reset the output array
  fStripArray->Clear();

  // define variables
  PndLmdMCPoint *point = nullptr;

  Int_t nPoint = fPointArray->GetEntriesFast();
  if (fVerbose > 0) {
    std::cout << "the Nr of points is" << nPoints << std::endl;
  }

  // loop all points in an event
  Int_t iStrip = 0;
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndLmdMCPoint *)fPointArray->At(iPoint);
    if (!point) {
      std::cout << "No point !" << std::endl;
      continue;
    }

    fCurrentStripCalLeft = fStripCalTrapLeft;
    fCurrentStripCalRight = fStripCalTrapRight;
    fCurrentDigiPar = fDigiParTrap;

    // transform the point from global coordinate to local coordinate
    TVector3 posInL = fGeoH->MasterToLocalId(point->GetEntryPoint(), point->GetDetName());
    TVector3 posOutL = fGeoH->MasterToLocalId(point->GetExitPoint(), point->GetDetName());
    // std::cout<<"the point in global coordinate is "<<point->GetEntryPoint().X()<<","<<point->GetEntryPoint().Y()
    //<<","<<point->GetEntryPoint().Z()<<std::endl;
    std::cout << "the Entry point in local coordinate is " << posInL.X() << "," << posInL.Y() << "," << posInL.Z() << std::endl;
    std::cout << "the Exit point in local coordinate is " << posOutL.X() << "," << posOutL.Y() << "," << posOutL.Z() << std::endl;

    Double_t eLoss = point->GetEnergyLoss();
    std::cout << "the eLoss of this point is: " << eLoss << endl;

    TVector3 pos, pos1 = posInL + posOutL;
    pos.SetXYZ(pos1.X() / 2., pos1.Y() / 2., pos1.Z() / 2.);
    // for Left segment
    if (pos.X() > 0) {
      //	std::cout<<"Hits on Left segment"<<std::endl;

      // calculate the cluster strips fired
      std::vector<PndLmdStrips> leftStrips = fCurrentStripCalLeft->DigiStripFromPoint(posInL.X(), posInL.Y(), posInL.Z(), posOutL.X(), posOutL.Y(), posOutL.Z(), eLoss);
      if (leftStrips.size() != 0) {
        for (std::vector<PndLmdStrips>::const_iterator kit = leftStrips.begin(); kit != leftStrips.end(); ++kit) { // register the strips fired
          AddDigi(iStrip, iPoint, kLMDHitsStrip, point->GetDetName(), fCurrentStripCalLeft->CalFeFromStrip(kit->GetIndex()),
                  fCurrentStripCalLeft->CalChannelFromStrip(kit->GetIndex()), kit->GetCharge());
          Int_t chan1 =
            (fCurrentStripCalLeft->CalFeFromStrip(kit->GetIndex())) * (fCurrentDigiPar->GetNrFeChannels()) + (fCurrentStripCalLeft->CalChannelFromStrip(kit->GetIndex()));

          cout << "the iPoint is: " << iPoint << endl;
          cout << "the iStrip is :" << iStrip - 1 << endl;
          cout << "the channel is :" << chan1 << endl;
          cout << "the information of digis: " << point->GetDetName() << "," << kit->GetCharge() << endl;
          cout << endl;
        }
      } else
        std::cout << "the left segment is empty" << std::endl;

    } else { // std::cout<<"Hits on Right segment"<<std::endl;
      std::vector<PndLmdStrips> rightStrips = fCurrentStripCalRight->DigiStripFromPoint(posInL.X(), posInL.Y(), posInL.Z(), posOutL.X(), posOutL.Y(), posOutL.Z(), eLoss);
      //      cout<<"Call the calStrip class to get digi successfully"<<endl;

      if (rightStrips.size() != 0) {
        for (std::vector<PndLmdStrips>::const_iterator kit = rightStrips.begin(); kit != rightStrips.end(); ++kit) {
          // register the strips fired
          AddDigi(iStrip, iPoint, kLMDHitsStrip, point->GetDetName(), fCurrentStripCalRight->CalFeFromStrip(kit->GetIndex()) + fCurrentDigiPar->GetNrLeftFe(),
                  fCurrentStripCalRight->CalChannelFromStrip(kit->GetIndex()), kit->GetCharge());
          Int_t chan2 = (fCurrentStripCalRight->CalFeFromStrip(kit->GetIndex()) + fCurrentDigiPar->GetNrLeftFe()) * (fCurrentDigiPar->GetNrFeChannels()) +
                        (fCurrentStripCalLeft->CalChannelFromStrip(kit->GetIndex()));

          /*
                              new((*fStripArray)[iStrip]) PndLmdDigiStrips(iPoint,kLMDHitsStrip,point->GetDetName(),
                                  fCurrentStripCalRight->CalFeFromStrip(kit->GetIndex())+fCurrentDigiPar->GetNrLeftFe(),
                                  fCurrentStripCalRight->CalChannelFromStrip(kit->GetIndex()),
                                  kit->GetCharge());
                                    iStrip++;
          */
          //    cout<<endl;
          cout << "the iPoint is: " << iPoint << endl;
          cout << "the iStrip is :" << iStrip - 1 << endl;
          cout << "the channel is :" << chan2 << endl;
          cout << "the information of digis: " << point->GetDetName() << "," << kit->GetCharge() << endl;
          cout << endl;
        }
      } else
        std::cout << "the right segment is empty" << std::endl;

    } // judge the hits on left segment or right segment, then calculate strips

  } // loop all points for one event

  //   if(fVerbose>0)
  LOG(info) << " PndLmdDigiProducer: " << nPoints << " PndLmdMCPoint, " << iStrip << " PndLmdDigis" << std::endl;

} // public method for Exec

void PndLmdDigiProducer::AddDigi(Int_t &iStrip, Int_t iPoint, Int_t detID, TString detName, Int_t fe, Int_t channel, Double_t charge)
{
  Bool_t found = kFALSE;
  PndLmdDigiStrips *aDigi = nullptr;
  for (Int_t skr = 0; skr < iStrip && found == kFALSE; skr++) {
    aDigi = (PndLmdDigiStrips *)fStripArray->At(skr);
    if (aDigi->GetDetID() == detID, aDigi->GetDetName() == detName, aDigi->GetFe() == fe, aDigi->GetChannel() == channel) {
      //	aDigi->AddCharge(charge);
      //	aDigi->AddIndex(iPoint);
      found = kTRUE;
    }
  }

  if (found == kFALSE) {
    new ((*fStripArray)[iStrip]) PndLmdDigiStrips(iPoint, detID, detName, fe, channel, charge);
    iStrip++;
  }
}

ClassImp(PndLmdDigiProducer);
