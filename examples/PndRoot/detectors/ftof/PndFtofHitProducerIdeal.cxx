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
// -----                FairFtofProducerIdeal source file             -----
// -----                  Created by A. Sanchez              -----
// -------------------------------------------------------------------------

#include <cmath>

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "FairRootManager.h"
#include "PndFtofHitProducerIdeal.h"
#include "PndFtofHit.h"
#include "TGeoBBox.h"
#include "TGeoBBox.h"
//#include "PndFtofHitInfo.h"
#include "PndFtofPoint.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoVector.h"
#include "FairLogger.h"
#include "TVector3.h"

// -----   Default constructor   -------------------------------------------
PndFtofHitProducerIdeal::PndFtofHitProducerIdeal() : PndPersistencyTask("Ideal PndFtof Hit Producer"), fTimeOrderedDigi(kFALSE)
{
  fBranchName = "FtofPoint";
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Default constructor   -------------------------------------------
PndFtofHitProducerIdeal::PndFtofHitProducerIdeal(Double_t dt, Double_t dt2) : PndPersistencyTask("Ideal PndFtof Hit Producer"), fTimeOrderedDigi(kFALSE)
{
  fBranchName = "FtofPoint";
  fdt = dt;
  fdt2 = dt2;
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndFtofHitProducerIdeal::~PndFtofHitProducerIdeal() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndFtofHitProducerIdeal::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndFtofHitProducerIdeal::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject(fBranchName);

  if (!fPointArray) {
    std::cout << "-W- PndFtofHitProducerIdeal::Init: "
              << "No FtofPoint array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndFtofHit");
  ioman->Register("FtofHit", "Ftof", fHitArray, GetPersistency());

  LOG(info) << " PndFtofHitProducerIdeal: Intialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndFtofHitProducerIdeal::SetParContainers()
{
  // Get Base Container
  // FairRun* ana = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=ana->GetRuntimeDb(); //[R.K. 01/2017] unused variable?
  // fGeoPar = (PndGeoFtofPar*)(rtdb->getContainer("PndGeoFtofPar"));
}

// -----   Public method Exec   --------------------------------------------
void PndFtofHitProducerIdeal::Exec(Option_t *)
{
  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No HitArray");

  fHitArray->Clear();

  // Declare some variables
  PndFtofPoint *point = nullptr;

  Int_t detID = 0, // Detector ID
    trackID = 0;   // Track index

  Double_t time = 0.;
  // Double_t scitime = 0.; //[R.K. 01/2017] unused variable?
  // Double_t t2 = 0.; //[R.K. 01/2017] unused variable?
  Double_t t1 = 0.;
  // Double_t phdt2 = 0.,tdc12 =0.,tz=0.; //[R.K. 01/2017] unused variable?
  // Double_t phdt1 = 0.,tsig = 0.; //[R.K. 01/2017] unused variable?

  // Loop over FtofPoints
  Int_t nPoints = fPointArray->GetEntriesFast();

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndFtofPoint *)fPointArray->At(iPoint);
    if (fVerbose > 0)
      std::cout << " Ideal Hit Producer -Point-: " << point << std::endl;
    if (!point)
      continue;

    // Detector ID
    detID = point->GetVolumeID();

    // MCTrack ID
    trackID = point->GetTrackID();
    /* // original part commented out because not working properly
    FairGeoVector posCInL, posCOut, meanPos, meanPosL;
    Double_t ZLoc;Double_t Ztdc[3];
    Double_t DistZ[3];

    GetLocalHitPoints(point, posCInL,meanPos);

    TVector3 size = GetSensorDimensions(point->GetDetName().Data());

    if (meanPos.getZ()>0.)
    {
  tz = size.z()-meanPos.getZ();
  phdt1 = 1.492*(tz)/30;
  phdt2 = 1.492*(2*size.z()-tz)/30;
  }
  if(meanPos.getZ()<0.){
    tz = -size.z()-meanPos.getZ();
  phdt1 = 1.492*(2*size.z()+tz)/30;
  phdt2 = -1.492*(tz)/30;
 }
    if(meanPos.getZ()==0.)phdt1=phdt2=1.492*(size.z())/30;

    //   std::cout<<" z tdc "<<tz<<" z point "<<meanPos.getZ()<<std::endl;
    //       std::cout<<" t1 phot "<<phdt1<<" t2 phot "<<phdt2<<std::endl;

    tdc12 = (phdt1-phdt2);
    tsig = 0.08;
    smear(tdc12,tsig);


    if(phdt2>phdt1){
ZLoc = ((tdc12*30)/(2*1.492))+size.z();
DistZ[2]=size.z()-ZLoc;
//std::cout<<" t2>t1"<<std::endl;

    }

    if(phdt1>phdt2)
{ZLoc = ((tdc12*30)/(2*1.492))-size.z();
  DistZ[2]=-size.z()-ZLoc;
  //std::cout<<" t1>t2"<<std::endl;
}


    if(phdt1==phdt2)DistZ[2]=0.;

    DistZ[0]=DistZ[1]=0.;

    //std::cout<<" z loc "<<ZLoc<<" distz "<<DistZ[2]<<std::endl;

    TGeoHMatrix trans = GetTransformation(point->GetDetName().Data());
    trans.LocalToMaster(DistZ, Ztdc);


    //std::cout<<" z tdc "<<Ztdc[2]<<" z point "<<point->GetZin()<<std::endl;
    //        ____________/__
    // phdt2  |      @   / | phdt1, (phdt1-phdt2)c/2n=x
    //        |_________/__|
    //           L-x   / x

    //phdt1 phdt 2 time signals produced at each side of the
    //scintillator bar(scintillation process not treated
    // i introduce some aprox. for the TDC signal
    //to calculate the position in the bar by knowing tdc time.


    //refraction index polipropilrne 1.492 poliviniltoluene 1.58



    TVector3 dpos;

    dpos.SetXYZ(3.,0.25,fabs(Ztdc[2]-point->GetZin()));

    // coord. of the center of the slab in lab.c.s
    // z coord. determined by measuring t1-t2
    // at each side of the bar
    TVector3 position(posCInL.getX(),
    posCInL.getY(),
    Ztdc[2]);
    //point->Position(pos);
    */  // end of old part

    // Stefano's part
    TVector3 position(0, 0, 0), dpos(0, 0, 0), fPosHit(0, 0, 0), fDPosHit(0, 0, 0);
    TGeoNode *ftofNode = (TGeoNode *)gGeoManager->FindNode(point->GetX(), point->GetY(), point->GetZ());

    // retrieving size of the scintillator rod
    fDPosHit[0] = ((TGeoBBox *)ftofNode->GetVolume()->GetShape())->GetDX();
    fDPosHit[1] = ((TGeoBBox *)ftofNode->GetVolume()->GetShape())->GetDY();
    fDPosHit[2] = ((TGeoBBox *)ftofNode->GetVolume()->GetShape())->GetDZ();

    // retrieving center of the scintillator rod
    TGeoMatrix *ftofMat = (TGeoMatrix *)gGeoManager->GetCurrentMatrix();
    const Double_t *ftofPos = ftofMat->GetTranslation();
    fPosHit.SetXYZ(ftofPos[0], ftofPos[1], ftofPos[2]);

    // Filling values
    position.SetXYZ(ftofPos[0], gRandom->Gaus(point->GetY(), 10.), ftofPos[2]); // smearing of 10 cm of McPoint
    dpos.SetXYZ(fDPosHit[0], 10., fDPosHit[2]);
    time = gRandom->Gaus(point->GetTime(), 0.07); // 70 ps time resolution

    // Create new hit
    //      if (fTimeOrderedDigi){
    double timeBasedTime = time + FairRootManager::Instance()->GetEventTime();
    new ((*fHitArray)[iPoint]) PndFtofHit(trackID, detID, point->GetDetName(), timeBasedTime, t1, position, dpos, iPoint, point->GetEnergyLoss());

    //      } else {
    //		  new ((*fHitArray)[iPoint]) PndFtofHit(trackID, detID,
    //						   point->GetDetName(),time, t1,
    //						   position,dpos,iPoint,
    //						   point->GetEnergyLoss());
    //      }
    // std::cout << "Hit created for module: " << point->GetDetName() << std::endl;

  } // Loop over MCPoints

  // Event summary
  if (fVerbose > 0)
    LOG(info) << " PndFtofHitProducerIdeal: " << nPoints << " FtofPoints, " << nPoints << " Hits created.";
}
/* // useless
// -------------------------------------------------------------------------
void PndFtofHitProducerIdeal::smear(Double_t& time, Double_t& fdt)
{
/// smear a 3d vector

  Double_t t = time;
  //std::cout<<" time "<<time<<std::endl;
  Double_t sigt;

  sigt=gRandom->Gaus(0,fdt);
  t += sigt;
  time = t;
  return;
}

void PndFtofHitProducerIdeal::GetLocalHitPoints(PndFtofPoint* myPoint,
                 FairGeoVector& myHitIn,FairGeoVector& myInL)
{

  if (fVerbose > 1)
    std::cout << "GetLocalHitPoints" << std::endl;
  TGeoHMatrix trans = GetTransformation(myPoint->GetDetName().Data());

  Double_t posIn[3];
  Double_t posOut[3];
  Double_t posInLocal[3];
  Double_t posOutLocal[3];
  Double_t posCInLocal[3];
  Double_t posCOutLocal[3];
  Double_t posCIn[3];
  Double_t posCOut[3];

  posIn[0] = myPoint->GetXin();
  posIn[1] = myPoint->GetYin();
  posIn[2] = myPoint->GetZin();

  posOut[0] = myPoint->GetXout();
  posOut[1] = myPoint->GetYout();
  posOut[2] = myPoint->GetZout();

  //trans.MasterToLocal(posIn, posInLocal);

  if (fVerbose > 1){
    for (Int_t i = 0; i < 3; i++)
      std::cout << "posIn "<< i << ": " << posIn[i] << std::endl;
    //std::cout << "posInLOcal "<< i << ": " << posInLocal[i] << std::endl;
    trans.Print("");
  }
  trans.MasterToLocal(posIn, posInLocal);

  posCInLocal[0]=0.;
  posCInLocal[1]=0.;
  posCInLocal[2]=0.;

  trans.LocalToMaster(posCInLocal, posCIn);

  if (fVerbose > 1) {
    for (Int_t i = 0; i < 3; i++){
      std::cout << "posCInLocal "<< i << ": " << posCInLocal[i] << std::endl;
      std::cout << "posCIn "<< i << ": " << posCIn[i] << std::endl;
      std::cout << "posInLocal "<< i << ": " << posInLocal[i] << std::endl;
    }
  }

  myHitIn.setVector(posCIn);
  myInL.setVector(posInLocal);

}




TGeoHMatrix PndFtofHitProducerIdeal::GetTransformation(std::string detName) const
{
  gGeoManager->cd(detName.c_str());
  TGeoHMatrix* transMat = gGeoManager->GetCurrentMatrix();
  if (fVerbose > 1)
    transMat->Print("");
  return *transMat;
}

TVector3 PndFtofHitProducerIdeal::GetSensorDimensions(std::string detName) const
  {
    gGeoManager->cd(detName.c_str());
    TGeoVolume* actVolume = gGeoManager->GetCurrentVolume();
    TGeoBBox* actBox = (TGeoBBox*)(actVolume->GetShape());
    TVector3 result;
    result.SetX(actBox->GetDX());
    result.SetY(actBox->GetDY());
    result.SetZ(actBox->GetDZ());

    //result.Dump();

    return result;
  }
*/
ClassImp(PndFtofHitProducerIdeal)
