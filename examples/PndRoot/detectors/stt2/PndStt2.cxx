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

#include "PndStt2.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoVolume.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"

#include "PndDetectorList.h"
#include "PndStack.h"

//#include "PndGeoHandling.h"
//#include "PndStt2GeoPar.h"
//#include "PndStt2MapCreator.h"
//#include "PndStt2Geo.h"

#include "PndStt2GeoHandler.h"
#include "PndStt2Point.h"
#include "PndGeoStt.h"
#include "PndGeoSttPar.h"

#include <TClonesArray.h>
#include <TGeoBBox.h>
#include <TGeoCompositeShape.h>
#include <TGeoManager.h>
#include <TGeoMaterial.h>
#include <TGeoMedium.h>
#include <TGeoTube.h>
#include <TVirtualMC.h>
#include <iostream>

using std::cout;
using std::endl;

// ------------------------------------------------------------------------

PndStt2::PndStt2() : FairDetector("PndStt2", kTRUE), 
  //fTrackID(-1), fTubeID(-1), fPosIn(), fPosOut(), fMomIn(), fMomOut(), fTime(-1.), fLength(-1.), fELoss(-1), fGeoH(PndGeoHandling::Instance()), fSttGeoPar(0)
  fTrackID(-1), fTubeID(-1), fPosIn(), fPosOut(), fMomIn(), fMomOut(), fTime(-1.), fLength(-1.), fELoss(-1), fSttGeoPar(0)
{
    //  fPndStt2PointCollection = new TClonesArray("PndStt2Point");
}

// ------------------------------------------------------------------------

PndStt2::PndStt2(const char* name, Bool_t active) : FairDetector(name, active), 
  fTrackID(-1), fTubeID(-1), fPosIn(), fPosOut(), fMomIn(), fMomOut(), fTime(-1.), fLength(-1.), fELoss(-1), fSttGeoPar(0)
{
    //  fPndStt2PointCollection = new TClonesArray("PndStt2Point");
}

// ------------------------------------------------------------------------

PndStt2::PndStt2(const PndStt2& right) : FairDetector(right),
  fTrackID(-1), fTubeID(-1), fPosIn(), fPosOut(), fMomIn(), fMomOut(), fTime(-1.), fLength(-1.), fELoss(-1), fSttGeoPar(0)
{
    //  fPndStt2PointCollection = new TClonesArray("PndStt2Point");
}

// ------------------------------------------------------------------------

PndStt2::~PndStt2()
{
    //    if (fPndStt2PointCollection) {
    //        fPndStt2PointCollection->Delete();
    //  delete fPndStt2PointCollection;
    //    }
}

// ------------------------------------------------------------------------

void PndStt2::Initialize()
{
  /**
   * WORKAROUND needed for Geant4 in MT mode
   * Call AddSensitiveVolume for sensitive volumes in order to fill
   * thread-local FairModule::svList.
   */
  //DefineSensitiveVolumes();

  FairDetector::Initialize();
  //FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
  //PndStt2GeoPar* par = (PndStt2GeoPar*)(rtdb->getContainer("PndStt2GeoPar"));

  //fListOfSensitives.push_back("stt01gas"); 
  //fGeoH->CreateUniqueSensorId("", fListOfSensitives);
}

// ------------------------------------------------------------------------

Bool_t PndStt2::ProcessHits(FairVolume* vol)
{
    /** This method is called from the MC stepping */

    // Set parameters at entrance of volume. Reset ELoss.
    if (gMC->IsTrackEntering()) {
        fELoss = 0.;
        fTime = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
        //fTubeID = fGeoH->GetShortID(gMC->CurrentVolPath());
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Create PndStt2Point at exit of active volume
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {
      
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        //fTubeID = fGeoH->GetShortID(gMC->CurrentVolPath());
        fTubeID = fSttGeoH->GetTubeIdFromStr(gMC->CurrentVolPath());
        gMC->TrackPosition(fPosOut);
        gMC->TrackMomentum(fMomOut);
        
        if (fELoss == 0.) return kFALSE;
        
        AddHit(fTrackID,
               static_cast<int>(PndDetectorId::kSTT2), //FairRootManager::Instance()->GetBranchId("MCTrack"), 
               fTubeID,
               TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
               TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
               TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
               TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
               fTime,
               fLength,
               fELoss);

        // Increment number of PndStt2 det points in TParticle
        PndStack* stack = (PndStack*)gMC->GetStack();
        stack->AddPoint(PndDetectorId::kSTT2);
    }

    return kTRUE;
}

// ------------------------------------------------------------------------

void PndStt2::EndOfEvent()
{

    //LOG(info) << "PndStt2: " << fPndStt2PointCollection->GetEntriesFast() << " points registered in this event";
  fPointVector->clear();


}

// ------------------------------------------------------------------------

void PndStt2::Register()
{

    /** This will create a branch in the output tree called
      PndStt2Point, setting the last parameter to kFALSE means:
      this collection will not be written to the file, it will exist
      only during the simulation.
  */
  FairRootManager::Instance()->RegisterAny("STT2Point", fPointVector, kTRUE);
}

// ------------------------------------------------------------------------

TClonesArray* PndStt2::GetCollection(Int_t iColl) const
{
    return NULL;
}

// ------------------------------------------------------------------------

void PndStt2::Reset() 
{ 
    fPointVector->clear();
    ResetParameters();
}

// ------------------------------------------------------------------------

void PndStt2::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if (!fileName.EndsWith(".geo")) {
    LOG(error) << "STT2: only .geo files supported.";
    return;
  }
  
  //int fGeoType = 1;
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
  //PndGeoSttPar *par = (PndGeoSttPar *)(rtdb->getContainer("PndGeoSttPar"));
  
  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  
  //PndStt2Geo *Geo = new PndStt2Geo();
  PndGeoStt *Geo = new PndGeoStt();
  Geo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(Geo);
  Bool_t rc = geoFace->readSet(Geo);
  if (rc) 
    Geo->create(geoLoad->getGeoBuilder());
    
  TList *volList = Geo->getListOfVolumes();  
  ProcessNodes(volList);

  //fSttGeoH = new PndStt2GeoHandler; // only needed for function GetTubeIdFromStr
  fSttGeoH = PndStt2GeoHandler::Instance(); // only needed for function GetTubeIdFromStr
  
  // iterate volumes to determine max tube id
  TListIter iter(volList);
  FairGeoNode *node = nullptr;
  int maxTubeId = 0;
  while ((node = (FairGeoNode *)iter.Next())) {
    TString nodename = node->GetName();
    if (!nodename.Contains("stt01tube")) continue;
    int tubeid = fSttGeoH->GetTubeIdFromStr(nodename);
    if (tubeid>=maxTubeId) maxTubeId = tubeid;
    //cout << tubeid<<" "<<std::flush;
    //tubecounter++;
  }
  cout <<endl;
  
  // set STT geo parameters
  //fSttGeoPar = (PndStt2GeoPar *)(rtdb->getContainer("PndStt2GeoPar"));
  fSttGeoPar = (PndGeoSttPar *)(rtdb->getContainer("PndGeoSttPar"));
  fileName = fileName(fileName.Last('/')+1, fileName.Length());
  //infoName.ReplaceAll(".geo","_geoinfo.root");
  fSttGeoPar->SetMaxTubeId(maxTubeId);
  fSttGeoPar->SetGeoFileName(fileName);
  fSttGeoPar->SetGeometryType(3);
  fSttGeoPar->setChanged();
  fSttGeoPar->setInputVersion(fRun->GetRunId(), 1);
  
  LOG(info) << "PndStt2::ConstructGeometry : STT total number of tubes: " << maxTubeId;
}

void PndStt2::InitOnWorker()
{
  fSttGeoH = PndStt2GeoHandler::Instance(); // only needed for function GetTubeIdFromStr
  
  LOG(info) << "PndStt2::InitOnWorker : STT total number of tubes: ";
}


// ------------------------------------------------------------------------

void PndStt2::AddHit(Int_t    trackID,
                     Int_t    detID,
                     Int_t    tubeID,
                     TVector3 posIn,
                     TVector3 posOut,
                     TVector3 momIn,
                     TVector3 momOut,
                     Double_t tof,
                     Double_t length,
                     Double_t eLoss)
{
    //    LOG(info) << "S [" << FairRootManager::Instance()->GetInstanceId() << "] <" << trackID << "> @ " << posIn.X() << ", " << posIn.Y() << ", " << posIn.Z();

    fPointVector->push_back(PndStt2Point(trackID, detID, tubeID, posIn, posOut, momIn, momOut, tof, length, eLoss));
}

// ------------------------------------------------------------------------

FairModule* PndStt2::CloneModule() const {
    PndStt2* newStt2 = new PndStt2(*this);
    newStt2->InitOnWorker();
    return newStt2;
}

// ------------------------------------------------------------------------


void PndStt2::DefineSensitiveVolumes()
{
    TObjArray* volumes = gGeoManager->GetListOfVolumes();
    TIter next(volumes);
    TGeoVolume* volume;
    while ((volume = static_cast<TGeoVolume*>(next()))) {
        if (IsSensitive(volume->GetName())) {
            LOG(debug2) << "Sensitive Volume " << volume->GetName();
            AddSensitiveVolume(volume);
        }
    }
}

// ------------------------------------------------------------------------

Bool_t PndStt2::IsSensitive(const std::string& name)
{
    if (name.find("stt01gas") != std::string::npos) {
        return kTRUE;
    }
    return kFALSE;
}
// ------------------------------------------------------------------------
// -------------------------------------------------------------------------
bool PndStt2::CheckIfSensitive(std::string name)
{
  return IsSensitive(name);
}

// ------------------------------------------------------------------------
void PndStt2::FinishRun()
{
  //cout <<"## STT Geo Parameters ##"<<endl<<"-------------------------------"<<endl;
  //cout <<"## TubeInRad()   " << fSttGeoPar->GetTubeInRad()   << endl;
  //cout <<"## TubeOutRad()  " << fSttGeoPar->GetTubeOutRad()  << endl;
  //cout <<"## GeoFileName() " << fSttGeoPar->GetGeoFileName() << endl;
  //cout <<"## GeoInfoName() " << fSttGeoPar->GetGeoInfoName() << endl;
  
  
  
  //fSttGeoPar->SetGeoFileName(GetGeometryFileName());
  //fSttGeoPar->setChanged();
  //fSttGeoPar->setInputVersion(fRun->GetRunId(), 1);
  
  //PndStt2GeoHandler geoh(fSttGeoPar);
}

// ------------------------------------------------------------------------

ClassImp(PndStt2);
