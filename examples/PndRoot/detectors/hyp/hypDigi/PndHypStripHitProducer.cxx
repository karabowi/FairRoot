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

// -------------------------------------------------------------------------
// -----                PndHypStripHitProducer            -----
// -------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TArrayD.h"
#include "TVector2.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "PndHypStripHitProducer.h"
#include "PndHypHit.h"
#include "PndHypPoint.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
//#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "FairLogger.h"
//#include "StringVector.h"
#include "PndHypCalcStrip.h"
#include "PndHypDigiStrip.h"
//#include "PndHypDigiStrip.h"
// enum SensorSide { SensorSide::kTOP, SensorSide::kBOTTOM };
// -----   Default constructor   -------------------------------------------
PndHypStripHitProducer::PndHypStripHitProducer() : FairTask("Hyp Strip Hit Producer")
{
  fBranchName = "HypPoint";

  /*ftopPitch = 0.;
   fbotPitch = 0.;
   forient = 0.;
   fskew = 0.;
   fTopAnchor = TVector2(0,0);
   fBotAnchor = TVector2(0,0);
   fnrTopFE = 0;
   fnrBotFE = 0;
   fnrFECh = 0;
   fthreshold = 0.;
   fnoise = 0.;*/

  fOverrideParams = false;
  // stripHits = 0;
  //  fHitArray  = new TClonesArray("PndHypHit");
  //	fStripArray	= new TClonesArray("PndHypStripHit");
}
// -------------------------------------------------------------------------

PndHypStripHitProducer::PndHypStripHitProducer(Double_t topPitch, Double_t botPitch, Double_t ori, Double_t skew, TVector2 topAnchor, TVector2 botAnchor, Int_t nrTopFE,
                                               Int_t nrBotFE, Int_t nrFECh, Double_t threshold, Double_t noise, TString, TString feType)
  : // sensorType //[R.K.03/2017] unused variable(s)
    FairTask("Hyp Strip Hit Producer")
{
  fBranchName = "HypPoint";
  // ftopPitch = topPitch;
  //   fbotPitch = botPitch;
  //   forient = ori;
  //   fskew = skew;
  //   fTopAnchor = topAnchor;
  //   fBotAnchor = botAnchor;
  //   fnrTopFE = nrTopFE;
  //   fnrBotFE = nrBotFE;
  //   fnrFECh = nrFECh;
  //   fthreshold = threshold;
  //   fnoise = noise;

  fOverrideParams = true;
  SetParamSet(topPitch, botPitch, ori, skew, topAnchor, botAnchor, nrTopFE, nrBotFE, nrFECh, threshold, noise, "Rect", feType);
  // stripHits = 0;
  std::cout << "Hyp Strip Hit Producer initiated" << std::endl;
}
// -------------------------------------------------------------------------

void PndHypStripHitProducer::SetParamSet(Double_t topPitch, Double_t botPitch, Double_t ori, Double_t skew, TVector2 topAnchor, TVector2 botAnchor, Int_t nrTopFE, Int_t nrBotFE,
                                         Int_t nrFECh, Double_t threshold, Double_t noise, TString sensorType, TString feType)
{
  FairRunAna *ana = FairRunAna::Instance();
  // FairRootManager* ioman = FairRootManager::Instance(); //[R.K. 01/2017] unused variable
  if (fDigiPar == nullptr)
    SetParContainers();
  if (fOverrideParams) {
    if (sensorType.Contains("Rect"))
      fCurrentDigiPar = fDigiPar;
    else if (sensorType.Contains("Trap"))
      fCurrentDigiPar = fDigiPar;
    fCurrentDigiPar->SetTopPitch(topPitch);
    fCurrentDigiPar->SetBotPitch(botPitch);
    fCurrentDigiPar->SetSkew(skew);
    fCurrentDigiPar->SetOrient(ori);
    fCurrentDigiPar->SetTopAnchor(topAnchor);
    fCurrentDigiPar->SetBotAnchor(botAnchor);
    fCurrentDigiPar->SetNrFECh(nrFECh);
    fCurrentDigiPar->SetNrTopFE(nrTopFE);
    fCurrentDigiPar->SetNrBotFE(nrBotFE);
    fCurrentDigiPar->SetThreshold(threshold);
    fCurrentDigiPar->SetNoise(noise);
    fCurrentDigiPar->SetSensType(sensorType);
    fCurrentDigiPar->SetFeType(feType);
    fCurrentDigiPar->setChanged();
    fCurrentDigiPar->setInputVersion(ana->GetRunId(), 1);
  }
}

// -----   Destructor   ----------------------------------------------------
PndHypStripHitProducer::~PndHypStripHitProducer()
{
  delete fGeoH;
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndHypStripHitProducer::SetParContainers()
{
  // Get Base Container

  FairRunAna *ana = FairRunAna::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  // fGeoPar = (PndGeoHypPar*)(rtdb->getContainer("PndGeoHypPar"));
  fDigiPar = (PndHypStripDigiPar *)(rtdb->getContainer("PndHypStripDigiPar"));
  // fGeoMappingPar = (PndHypGeoMappingPar*)(rtdb->getContainer("PndHypGeoMappingPar"));
  std::cout << "fdigi par init" << std::endl;
}

InitStatus PndHypStripHitProducer::ReInit()
{
  /*
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb=ana->GetRuntimeDb();
   //fGeoPar = (PndGeoHypPar*)(rtdb->getContainer("PndGeoHypPar"));
   fDigiPar=(PndHypStripDigiPar*)(rtdb->getContainer("PndHypStripDigiPar"));
   //fGeoMappingPar = (PndHypGeoMappingPar*)(rtdb->getContainer("PndHypGeoMappingPar"));
   std::cout << "fdigi par 2 init" << std::endl;
   */
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndHypStripHitProducer::Init()
{
  // FairRunAna* ana = FairRunAna::Instance(); //[R.K. 01/2017] unused variable
  FairRootManager *ioman = FairRootManager::Instance();

  fGeoH = new PndHypGeoHandling(gGeoManager);

  if (!ioman) {
    std::cout << "-E- PndHypStripHitProducer::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fPointArray = (TClonesArray *)ioman->GetObject(fBranchName);
  if (!fPointArray) {
    std::cout << "-W- PndHypStripHitProducer::Init: "
              << "No HypPoint array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  // fHitArray = new TClonesArray("PndHypHit");
  // ioman->Register("HypHit", "Hyp", fHitArray, kTRUE);

  // Create and register output array
  fStripArray = new TClonesArray("PndHypDigiStrip");
  ioman->Register("HypStripDigis", "Hyp", fStripArray, kTRUE);

  // Create and register parameter array
  //  fStripArray = new TClonesArray("PndHypDigiPar");
  //  ioman->Register("HypDigiParam", "Hyp", fDigiPar, kTRUE);

  //  SetParContainers();

  LOG(info) << " PndHypStripHitProducer: Initialisation successfull";
  // std::cout << "-I- Test: " << fGeoMappingPar->detIdPairArray->GetEntries() << std::endl;

  if (!fDigiPar) {
    LOG(error) << " MvdStripHitProducer: DigiPar Container does not exist!";
    return kERROR;
  }
  // if (fOverrideParams)
  //        {
  // 	 fDigiPar->SetTopPitch(ftopPitch);
  // 	 fDigiPar->SetBotPitch(fbotPitch);
  // 	 fDigiPar->SetSkew(fskew);
  // 	 fDigiPar->SetOrient(forient);
  // 	 fDigiPar->SetTopAnchor(fTopAnchor);
  // 	 fDigiPar->SetBotAnchor(fBotAnchor);
  // 	 fDigiPar->SetNrFECh(fnrFECh);
  // 	 fDigiPar->SetNrTopFE(fnrTopFE);
  // 	 fDigiPar->SetNrBotFE(fnrBotFE);
  // 	 fDigiPar->SetThreshold(fthreshold);
  // 	 fDigiPar->SetNoise(fnoise);
  // 	 fDigiPar->setChanged();
  // 	 fDigiPar->setInputVersion(ana->GetRunId(),1);

  //        }

  fDigiPar->print();

  fStripCalcTop = new PndHypCalcStrip(fDigiPar, SensorSide::kTOP);
  fStripCalcBot = new PndHypCalcStrip(fDigiPar, SensorSide::kBOTTOM);
  fStripCalcTop->SetVerboseLevel(fVerbose);
  fStripCalcBot->SetVerboseLevel(fVerbose);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndHypStripHitProducer::Exec(Option_t *)
{
  // Reset output array
  // if ( ! fHitArray )
  // Fatal("Exec", "No HitArray");

  // fHitArray->Delete();

  fStripArray->Delete();

  //  fFeStripArray->Delete();

  // Declare some variables
  PndHypPoint *point = nullptr;

  Int_t detID = 0; // Detector ID
  // trackID = 0;     // Track index //[R.K.03/2017] unused variable

  // Loop over PndHypPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  if (fVerbose > 0) {
    std::cout << " Nr of Points: " << nPoints << std::endl;
  }

  Int_t iStrip = 0;
  // Int_t iFeStrip = 0;
  Int_t nSiL = -1;

  /*PndHypCalcStrip StripCalcTop(fDigiPar->topPitch, fDigiPar->orient,
   fDigiPar->topNrFE * fDigiPar->feChannels , fDigiPar->feChannels,
   fDigiPar->topAnchor,
   fDigiPar->threshold, fDigiPar->noise);

   PndHypCalcStrip StripCalcBot(fDigiPar->botPitch, fDigiPar->orient + fDigiPar->skew,
   fDigiPar->botNrFE * fDigiPar->feChannels, fDigiPar->feChannels,
   fDigiPar->botAnchor,
   fDigiPar->threshold, fDigiPar->noise);

   PndHypCalcStrip StripCalcTop(ftopPitch, forient,
   fnrTopFE * fnrFECh , fnrFECh,
   fTopAnchor,
   fthreshold, fnoise);

   PndHypCalcStrip StripCalcBot(fbotPitch, forient + fskew,
   fnrBotFE * fnrFECh, fnrFECh,
   fBotAnchor,
   fthreshold, fnoise);
   */

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndHypPoint *)fPointArray->At(iPoint);
    // if (!((point->GetDetName()).Contains("Strip"))) continue;
    // if ((point->GetDetName()).Contains("Disk")) continue;
    //      if ((point->GetDetName()).Contains("SideChips")) continue;

    // if (fVerbose > 1){
    // std::cout<<"***** Strip Hit ******"<<std::endl;
    // std::cout<<" DetName : "<<point->GetDetName()<<std::endl;
    //}
    if (!point) {
      std::cout << "No Point!" << std::endl;
      continue;
    }
    if (fVerbose > 1) {
      std::cout << "****Global Point: " << std::endl;
      point->Print("");
    }
    // FairGeoVector posInL, posOutL, meanPos, meanPosL;
    // GetLocalHitPoints(point, posInL, posOutL);
    TVector3 posIn, posOut;
    point->PositionIn(posIn);
    point->PositionOut(posOut);

    TVector3 posInL = fGeoH->MasterToLocalId(posIn, point->GetDetName());
    TVector3 posOutL = fGeoH->MasterToLocalId(posOut, point->GetDetName());

    if (fVerbose > 1) {
      std::cout << "Local Hits: " << std::endl;
    }

    if (fVerbose > 1) {

      std::cout << "Energy: " << point->GetEnergyLoss() << std::endl;
    }

    // TVector3 size = GetSensorDimensions(point->GetDetName().Data());
    TVector3 size = GetSensorDimensions(fGeoH->GetPath(point->GetDetName()).Data());

    if (fVerbose > 2)
      std::cout << "Sensor Size : x=" << size.x() * 2. << " , y=" << size.y() * 2. << " , z =" << size.z() * 2. << std::endl;

    TVector2 anchor(-size.x(), size.y());

    // Top Side
    if (fVerbose > 0)
      std::cout << "Top Side: " << std::endl;
    // PndHypCalcStrip StripCalc(ftopPitch, forient, fnrTopFE * fnrFECh ,
    // 				 //FairGeoVector(-size.x(),-size.y(),0.),
    // 				 TVector2(0.,0.),
    // 				 fthreshold, fnoise);

    // StripCalcTop.SetVerboseLevel(fVerbose);
    Double_t sttop = ((size.x() * 2) / fDigiPar->GetTopPitch());

    Int_t rfetop = Int_t(sttop + 0.5);
    Int_t rtfe = Int_t((rfetop / 128) + 0.5);

    if (rfetop >= rtfe * 128)
      fStripCalcTop->SetreStrip(rtfe * 128);
    else
      fStripCalcTop->SetreStrip(rfetop);
    std::cout << " tof fe " << Int_t(rfetop / 128) << " total strips " << rfetop << std::endl;

    nSiL = point->GetVolumeID();

    fStripCalcTop->SetAnchor(anchor);
    // Calculate a cluster of Strips fired
    std::vector<PndHypStrip> topStrips = fStripCalcTop->GetStrips(posInL.X(), posInL.Y(), posInL.Z(), posOutL.X(), posOutL.Y(), posOutL.Z(), point->GetEnergyLoss(), nSiL);

    if (topStrips.size() != 0) {
      // stripHits += topStrips.size();
      if (fVerbose > 0)
        std::cout << "SensorStrips: top " << topStrips.size() << std::endl;

      // trackID = point->GetTrackID(); //[R.K.03/2017] unused variable
      detID = point->GetVolumeID();

      //      for(uint i = 0; i < myStrips.size(); i++)
      // iStrip = 0;
      // Int_t sp;SensorSide si; //[R.K. 01/2017] unused variable

      for (std::vector<PndHypStrip>::const_iterator it = topStrips.begin(); it != topStrips.end(); it++)

      {

        new ((*fStripArray)[iStrip])
          PndHypDigiStrip(iPoint, detID, point->GetDetName(), fStripCalcTop->CalcFEfromStrip(it->GetIndex()), fStripCalcTop->CalcChannelfromStrip(it->GetIndex()), it->GetCharge());
        // dynamic_cast<PndHypDigiStrip*>(fStripArray->operator[](iStrip))->SetMCID(trackID);

        if (fVerbose > 1)
          std::cout << *it << " " << topStrips[iStrip] << std::endl;
        iStrip++;
      }
    } else if (fVerbose > 2)
      std::cout << "Top side empty" << std::endl;

    // Bottom Side
    // MVD x-y local coordinates,TVector2(0.,size.y()*2.)
    // HYP x-z,y-z local coor. TVector2(0.,size.z()*2.)
    if (fVerbose > 0)
      std::cout << "Bottom Side: " << std::endl;
    //  StripCalc = PndHypCalcStrip(fbotPitch, forient + fskew, fnrBotFE * fnrFECh ,
    // 				   //FairGeoVector(-size.x(),-size.y(),0.),
    // 				   TVector2(0.,size.y()*2.),
    // 				   fthreshold, fnoise);

    fStripCalcBot->SetVerboseLevel(fVerbose);
    Double_t stbot;
    stbot = ((size.y() * 2) / fDigiPar->GetBotPitch());

    Int_t rfebot = Int_t(stbot);
    Int_t rbfe = Int_t(rfebot / 128);

    if (rfebot >= rbfe * 128)
      fStripCalcBot->SetreStrip(rbfe * 128);
    else
      fStripCalcBot->SetreStrip(rfebot);

    std::cout << " bof fe " << Int_t(rfebot / 128) << " total strips " << rfebot << std::endl;
    fStripCalcBot->SetAnchor(anchor);

    // Calculate a cluster of Strips fired

    std::vector<PndHypStrip> botStrips = fStripCalcBot->GetStrips(posInL.X(), posInL.Y(), posInL.Z(), posOutL.X(), posOutL.Y(), posOutL.Z(), point->GetEnergyLoss(), nSiL);

    if (botStrips.size() != 0) {
      // std::cout << "Deposited charge below threshold" << std::endl;
      // continue;
      //}
      // stripHits += botStrips.size();
      if (fVerbose > 1)
        std::cout << "SensorStrips: bot " << botStrips.size() << std::endl;

      // trackID = point->GetTrackID(); //[R.K.03/2017] unused variable
      detID = point->GetVolumeID();
      // uint iStrip = 0;
      // Int_t sp;SensorSide si; //[R.K. 01/2017] unused variable

      for (std::vector<PndHypStrip>::const_iterator it = botStrips.begin(); it != botStrips.end(); ++it) {
        new ((*fStripArray)[iStrip]) PndHypDigiStrip(iPoint, detID, point->GetDetName(), fStripCalcBot->CalcFEfromStrip(it->GetIndex()) + rtfe,
                                                     // StripCalcBot.CalcFEfromStrip(it->GetIndex()) + fnrTopFE,
                                                     fStripCalcBot->CalcChannelfromStrip(it->GetIndex()), it->GetCharge());
        //    dynamic_cast<PndHypDigiStrip*>(fStripArray->operator[](iStrip))->SetMCID(trackID);

        if (fVerbose > 1)
          std::cout << *it << std::endl;

        iStrip++;
      }

    } else if (fVerbose > 2)
      std::cout << "Bottom side empty" << std::endl;
    // TODO read Parameters from Database
  } // Loop over MCPoints

  // Event summary
  std::cout << "-I- PndHypStripHitProducer: " << nPoints << " HypPoints, "
            << " Hits created."
            << " " << iStrip << std::endl;
}

// void PndHypStripHitProducer::GetLocalHitPoints(PndHypPoint* myPoint, FairGeoVector& myHitIn, FairGeoVector& myHitOut)
// {

//   if (fVerbose > 1)
//     std::cout << "GetLocalHitPoints" << std::endl;
//   //TGeoHMatrix trans = GetTransformation(myPoint->GetDetName().Data());
//   TGeoHMatrix trans = GetTransformation(fGeoH->GetPath(myPoint->GetDetName()).Data());

//   double posIn[3];
//   double posOut[3];
//   double posInLocal[3];
//   double posOutLocal[3];

//   posIn[0] = myPoint->GetXin();
//   posIn[1] = myPoint->GetYin();
//   posIn[2] = myPoint->GetZin();

//   posOut[0] = myPoint->GetXout();
//   posOut[1] = myPoint->GetYout();
//   posOut[2] = myPoint->GetZout();

//   if (fVerbose > 1){
//     for (int i = 0; i < 3; i++)
//       std::cout << "posIn "<< i << ": " << posIn[i] << std::endl;

//     trans.Print("");
//   }

//   trans.MasterToLocal(posIn, posInLocal);
//   trans.MasterToLocal(posOut, posOutLocal);

//   if (fVerbose > 1) {
//     for (int i = 0; i < 3; i++){
//       std::cout << "posInLocal "<< i << ": " << posInLocal[i] << std::endl;
//       std::cout << "posOutLocal "<< i << ": " << posOutLocal[i] << std::endl;
//     }
//   }

//   //posIn/OutLocal have the center of the coordinate system in the center of the shape
//   //typically sensors have their coordinate system centered at the lower left corner

//   TVector3 offset = GetSensorDimensions(fGeoH->GetPath(myPoint->GetDetName()).Data());
//   //TVector3 offset = GetSensorDimensions(myPoint->GetDetName().Data());

//   posInLocal[0] += offset.x();
//   posInLocal[1] += offset.y();
//   //posInLocal[2] += offset.z();

//   posOutLocal[0] += offset.x();
//   posOutLocal[1] += offset.y();
//   //posOutLocal[2] += offset.z();

//   myHitIn.setVector(posInLocal);
//   myHitOut.setVector(posOutLocal);

// }

// TGeoHMatrix PndHypStripHitProducer::GetTransformation(std::string detName) const
// {
//   gGeoManager->cd(detName.c_str());
//   TGeoHMatrix* transMat = gGeoManager->GetCurrentMatrix();
//   if (fVerbose > 1)
//     transMat->Print("");
//   return *transMat;
// }

TVector3 PndHypStripHitProducer::GetSensorDimensions(std::string detName) const
{
  gGeoManager->cd(detName.c_str());
  TGeoVolume *actVolume = gGeoManager->GetCurrentVolume();
  TGeoBBox *actBox = (TGeoBBox *)(actVolume->GetShape());
  TVector3 result;
  result.SetX(actBox->GetDX());
  result.SetY(actBox->GetDY());
  result.SetZ(actBox->GetDZ());

  //   //result.Dump();

  return result;
}
// -------------------------------------------------------------------------

ClassImp(PndHypStripHitProducer)
