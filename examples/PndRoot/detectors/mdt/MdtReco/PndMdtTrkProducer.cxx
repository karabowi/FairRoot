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
// -----                PndMdtTrkProducer source file                  -----
// -----                  Created 16/06/08  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMdtTrkProducer.h"
#include "PndMdtMuonFilter.h"
#include "PndMdtGeoConstructorFast.h"
#include "PndMdtTrk.h"
#include "PndMdtHit.h"
#include "PndTrack.h"
#include "PndDetectorList.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTrackParH.h"
#include "FairLogger.h"

#include "TVector3.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMdtTrkProducer::PndMdtTrkProducer() : PndPersistencyTask(" MDT Tracklet Producer")
{
  Reset();
  SetPersistency(kTRUE);
  fRec_method = 0; // default, not use lhetrack as seed
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMdtTrkProducer::~PndMdtTrkProducer() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndMdtTrkProducer::Init()
{
  cout << "-I- PndMdtTrkProducer::Init: "
       << "INITIALIZATION *********************" << endl;

  // FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndMdtTrkProducer::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  fHitArray = (TClonesArray *)ioman->GetObject("MdtHit");
  if (!fHitArray) {
    cout << "-W- PndMdtTrkProducer::Init: "
         << "No MdtHit array!" << endl;
    return kERROR;
  }

  if (fRec_method == 1) {
    fLheGenTrack = (TClonesArray *)ioman->GetObject("LheGenTrack");
    if (!fLheGenTrack) {
      cout << "-W- PndMdtTrkProducer::Init: "
           << "No LheGenTrack array!" << endl;
      // return kERROR;
    }
  }

  // Create and register output array
  fTrkArray = new TClonesArray("PndMdtTrk");

  SetGeometry();

  ioman->Register("MdtTrk", "Mdt", fTrkArray, GetPersistency());

  LOG(info) << " PndMdtTrkProducer: Intialization successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

//______________________________________________________
void PndMdtTrkProducer::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run)
    Fatal("PndMdtTrkProducer:: SetParContainers", "No analysis run");

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    Fatal("PndMdtTrkProducer:: SetParContainers", "No runtime database");

  // Get Mdt Reconstruction parameter container
  // fRecoPar = (PndMdtRecoPar*) db->getContainer("PndMdtRecoPar");
}

//______________________________________________________
void PndMdtTrkProducer::SetGeometry()
{
  // Resetting geoemtry parameters
  for (Int_t mm = 0; mm < 3; mm++)
    for (Int_t ll = 0; ll < 20; ll++) {
      mdtLayerPos[mm][ll] = -1;
      mdtIronThickness[mm][ll] = -1;
    }
  mdtModule1MaxZ = -1;

  // Thichness of barrel iron layers [cm]
  mdtIronThickness[0][0] = 6.;
  for (Int_t ll = 1; ll < 12; ll++)
    mdtIronThickness[0][ll] = 3;
  mdtIronThickness[0][12] = 6.;
  // Thichness of endcap+mf iron layers [cm]
  for (Int_t ll = 0; ll < 10; ll++)
    mdtIronThickness[1][ll] = 6;

  Short_t version = 0;
  Text_t buffer[50];
  Text_t lbuffer[255];

  // Finding geometry version
  if (gGeoManager->FindVolumeFast("MdtBarrelLayer00"))
    version = 1; // fast
  if (gGeoManager->FindVolumeFast("MdtBarrelOct0"))
    version = 2; // full
  if (version == 0) {
    cout << "PndMdtTrkProducer::SetGeometry : Warning - No MDT Barrel Geometry" << endl;
  } else {
    for (Int_t ll = 0; ll < 15; ll++) {
      mdtLayerPos[0][ll] = -1;
      Int_t sec = 0, box = 0;
      sprintf(buffer, "MDT%is%il%ib%iw%i", 1, sec, ll, box, 0);
      TGeoVolume *v = gGeoManager->FindVolumeFast(buffer);
      if (v) {
        if (version == 1) {
          sec = 0;
          sprintf(buffer, "MDT%is%il%ib%iw%i", 1, sec, ll, box, 0);
          if (ll < 10)
            sprintf(lbuffer, "cave_1/Mdt_1/MdtBarrel_1/MdtBarrelLayer0%i_1/%s_%i", ll, buffer, 8 * ll);
          else
            sprintf(lbuffer, "cave_1/Mdt_1/MdtBarrel_1/MdtBarrelLayer%i_1/%s_%i", ll, buffer, 8 * ll);
        }
        if (version == 2) {
          sec = 2;
          box = 6;
          sprintf(buffer, "MDT%is%il%ib%iw%i", 1, sec, ll, box, 0);
          if (ll < 10)
            sprintf(lbuffer, "cave_1/MdtBarrel_0/MdtBarrelOct%i_%i/MdtBarrelOct%iLayer0%i_0/BP1%i%i%i0_%i/BA1%i%i%i0_%i/%s_0", sec, sec, sec, ll, sec, ll, box, box, sec, ll, box,
                    box, buffer);
          else
            sprintf(lbuffer, "cave_1/MdtBarrel_0/MdtBarrelOct%i_%i/MdtBarrelOct%iLayer%i_0/BP1%i%i%i0_%i/BA1%i%i%i0_%i/%s_0", sec, sec, sec, ll, sec, ll, box, box, sec, ll, box,
                    box, buffer);
        }

        gGeoManager->cd(lbuffer);
        Double_t local[3] = {0., 0., 0.};
        Double_t master[3];
        gGeoManager->LocalToMaster(local, master);
        mdtLayerPos[0][ll] = master[1];

        // Finding the maximum point of Z
        if (ll == 0) {
          const Double_t *origin = ((TGeoBBox *)gGeoManager->GetCurrentVolume()->GetShape())->GetOrigin();
          if (version == 1)
            local[1] = ((TGeoBBox *)gGeoManager->GetCurrentVolume()->GetShape())->GetDY() + origin[1];
          if (version == 2)
            local[2] = ((TGeoBBox *)gGeoManager->GetCurrentVolume()->GetShape())->GetDZ() + origin[2];
          gGeoManager->LocalToMaster(local, master);
          mdtModule1MaxZ = master[2];
          if (fVerbose > 1)
            cout << "mdtModule1MaxZ = " << mdtModule1MaxZ << ";" << endl;
        }

        if (fVerbose > 1)
          cout << "mdtLayerPos[0][" << ll << "] = " << mdtLayerPos[0][ll] << ";" << endl;
      } // end of recognized volume
    }   // end of layer loop
  }     // end of barrel

  Int_t ec_laymax = 0;
  version = 1; // standard fast
  // Finding geometry version
  if (gGeoManager->FindVolumeFast("BP20000"))
    version = 2; // full
  for (Int_t ll = 0; ll < 10; ll++) {
    mdtLayerPos[1][ll] = -1;
    sprintf(buffer, "MDT%is%il%ib%iw%i", 2, 0, ll, 0, 0);
    TGeoVolume *v = gGeoManager->FindVolumeFast(buffer);
    if (v) {
      if (version == 1) {
        sprintf(lbuffer, "cave_1/Mdt_1/MdtEndcap_1/MdtEndcapLayer0%i_1/%s_%i", ll, buffer, 200 + 8 * ll);
      }
      if (version == 2) {
        sprintf(lbuffer, "cave_1/MdtEndcap_0/MdtEndcapLayer0%i_0/BP20%i00_0/BA20%i00_0/%s_0", ll, ll, ll, buffer);
      }

      gGeoManager->cd(lbuffer);
      Double_t local[3] = {0., 0., 0.};
      Double_t master[3];
      gGeoManager->LocalToMaster(local, master);
      mdtLayerPos[1][ll] = master[2];
      if (fVerbose > 1)
        cout << "mdtLayerPos[1][" << ll << "] = " << mdtLayerPos[1][ll] << ";" << endl;
      ec_laymax++;
    } // end of recognized volume
  }   // end of layer loop

  // Muon Filter
  version = 1; // standard fast
  // Finding geometry version
  if (gGeoManager->FindVolumeFast("MF"))
    version = 2; // Dubna
  for (Int_t ll = 0; ll < 10; ll++) {
    mdtLayerPos[1][ll + ec_laymax] = -1;
    sprintf(buffer, "MDT%is%il%ib%iw%i", 3, 0, ll, 0, 0);
    TGeoVolume *v = gGeoManager->FindVolumeFast(buffer);
    if (v) {
      if (version == 1) {
        sprintf(lbuffer, "cave_1/Mdt_1/MdtMuonFilter_1/MdtMuonFilterLayer0%i_1/%s_%i", ll, buffer, 300 + 8 * ll);
      }
      if (version == 2) {
        sprintf(lbuffer, "cave_1/MF_0/MdtMFLayer0%i_0/BP30%i00_0/BA30%i00_0/%s_0", ll, ll, ll, buffer);
      }

      gGeoManager->cd(lbuffer);
      Double_t local[3] = {0., 0., 0.};
      Double_t master[3];
      gGeoManager->LocalToMaster(local, master);
      mdtLayerPos[1][ll + ec_laymax] = master[2];
      if (fVerbose > 1)
        cout << "mdtLayerPos[1][" << ll + ec_laymax << "] = " << mdtLayerPos[1][ll + ec_laymax] << ";" << endl;
    } // end of recognized volume
  }   // end of layer loop

  // Forward
  version = 1;
  // Finding geometry version
  // if (gGeoManager->FindVolumeFast("Forward"))    version = 2; // Dubna not existing right now
  for (Int_t ll = 0; ll < 17; ll++) {
    mdtLayerPos[2][ll] = -1;
    sprintf(buffer, "MDT%is%il%ib%iw%i", 4, 0, ll, 0, 0);
    TGeoVolume *v = gGeoManager->FindVolumeFast(buffer);
    if (v) {
      if (version == 1) {
        sprintf(lbuffer, "cave_1/Mdt_1/MdtForward_1/%s_%i", buffer, ll);
      }
      if (version == 2) {
        if (ll < 10)
          sprintf(lbuffer, "cave_1/Forward_0/MdtForwardLayer0%i_0/BP40%i00_%i/BA40%i00_0/%s_0", ll, ll, ll, ll, buffer);
        else
          sprintf(lbuffer, "cave_1/Forward_0/MdtForwardLayer%i_0/BP40%i00_%i/BA40%i00_0/%s_0", ll, ll, ll, ll, buffer);
      }

      gGeoManager->cd(lbuffer);
      Double_t local[3] = {0., 0., 0.};
      Double_t master[3];
      gGeoManager->LocalToMaster(local, master);
      mdtLayerPos[2][ll] = master[2];
      if (fVerbose > 1)
        cout << "mdtLayerPos[2][" << ll << "] = " << mdtLayerPos[2][ll] << ";" << endl;
    } // end of recognized volume
  }   // end of layer loop
}

// -----   Public method Exec   --------------------------------------------
void PndMdtTrkProducer::Exec(Option_t *)
{
  // Reset output array
  fTrkArray->Delete();
  if (!MdtMapping())
    return; // exit if the event contains no Mdt hits
  if (fRec_method == 1)
    AlgorithmWithLheGenTrack();

  TVector3 direction(1., 0., 0.); // direction of hits in previous 2 layers, lyt
  Float_t deltaAngle = -1.;       // lyt

  if (mapMdtBarrel.size() > 0) {
    vector<Int_t> vecMdt0 = mapMdtBarrel[0];
    TVector3 oldPos(0., 0., 0.);
    TVector3 newPos(0., 0., 0.);
    for (size_t iMap = 0; iMap < vecMdt0.size(); iMap++) // loop over hits in layer0
    {
      Int_t layerCount = 1, maxLayer = 0;
      Float_t layerDist = 0.;
      Float_t ironDist = 0.;

      // check whether this hit is available in mapHitDirection
      map<Int_t, TVector3>::const_iterator hit_direction_iter;
      map<Int_t, Float_t>::const_iterator hit_distance_iter;

      hit_direction_iter = mapHitDirection.find(vecMdt0[iMap]);
      hit_distance_iter = mapHitDistance.find(vecMdt0[iMap]);
      if (fRec_method == 1 && hit_direction_iter == mapHitDirection.end()) {
        // cout<<"this hit is not correlated to any track..."<<endl;
        continue;
      }
      direction = (*hit_direction_iter).second;
      Float_t dist_layer0_lhetrack = (*hit_distance_iter).second;

      PndMdtTrk *mdtTrk = new PndMdtTrk();
      // mdtTrk->SetHitIndex(0, vecMdt0[iMap]);
      mdtTrk->SetModule(1);

      PndMdtHit *mdtHit0 = (PndMdtHit *)fHitArray->At(vecMdt0[iMap]);
      mdtHit0->Position(oldPos);
      mdtTrk->SetHitIndex(0, vecMdt0[iMap]);
      mdtTrk->SetHitDist(0, dist_layer0_lhetrack); // set to be the distance of hit with lhetrack  lyt
      mdtTrk->SetHitDeltaAngle(0, -1.);            // lyt
      mdtTrk->SetLayerDist(0, 0);
      mdtTrk->SetHitMult(0, 1);

      map<Int_t, vector<Int_t>>::const_iterator layer_iter;
      for (layer_iter = mapMdtBarrel.begin(); layer_iter != mapMdtBarrel.end(); ++layer_iter) // layer loop
      {
        if (((*layer_iter).first) == 0)
          continue; // skip first layer
        if (((*layer_iter).first - maxLayer) > 1)
          break;
        layerDist = mdtLayerPos[0][(*layer_iter).first] - mdtLayerPos[0][maxLayer];
        vector<Int_t> vecMdt = (*layer_iter).second;
        Float_t corrDist = -1;
        Int_t corrId = -1;
        TVector3 corrPos(0., 0., 0.);
        Int_t layerMult = 0;
        for (size_t hit_iter = 0; hit_iter < vecMdt.size(); ++hit_iter) {
          PndMdtHit *mdtHit = (PndMdtHit *)fHitArray->At(vecMdt[hit_iter]);
          mdtHit->Position(newPos);
          Float_t hitDist = (oldPos - newPos).Mag2();
          deltaAngle = direction.Angle(newPos - oldPos); // lyt 13042010
          direction = newPos - oldPos;                   // lyt
          if ((corrDist < 0.) || (corrDist > hitDist))   // find closes hit
          {
            corrDist = hitDist;
            corrId = vecMdt[hit_iter];
            corrPos = newPos;
          }
          if ((hitDist > 0.) && ((TMath::Sqrt(hitDist) / layerDist) < 2.5))
            layerMult++;
        }

        if ((corrDist > 0.) && ((TMath::Sqrt(corrDist) / layerDist) < 2.5)) // if there in one correlated hit closer than 2.5 layer distance
        {
          ironDist = ironDist + mdtIronThickness[0][(*layer_iter).first - 1] * TMath::Sqrt(corrDist) / layerDist;
          mdtTrk->SetHitIndex(layerCount, corrId);
          mdtTrk->SetHitDist(layerCount, corrDist);
          mdtTrk->SetHitDeltaAngle(layerCount, deltaAngle); // lyt
          mdtTrk->SetLayerDist(layerCount, layerDist);
          mdtTrk->SetHitMult(layerCount, layerMult);
          maxLayer = (*layer_iter).first;
          layerCount++;
          oldPos = corrPos; // reset position for next mdt layer
        } else
          break;
      } // end of layer loop

      // Loop over Endcap for hybrid tracklets
      if (mapMdtEndcap.size() > 0) {
        map<Int_t, vector<Int_t>>::const_iterator layer2_iter;
        for (layer2_iter = mapMdtEndcap.begin(); layer2_iter != mapMdtEndcap.end(); ++layer2_iter) // layer loop
        {
          if (((*layer2_iter).first) == 0)
            continue; // skip first layer
          if (((*layer2_iter).first) == 1) {
            layerDist = mdtLayerPos[1][(*layer2_iter).first] - mdtModule1MaxZ;
          } else {
            if (((*layer2_iter).first - maxLayer) > 1)
              break;
            layerDist = mdtLayerPos[1][(*layer2_iter).first] - mdtLayerPos[1][maxLayer];
          }
          vector<Int_t> vecMdt = (*layer2_iter).second;
          Float_t corrDist = -1;
          Int_t corrId = -1;
          TVector3 corrPos(0., 0., 0.);
          Int_t layerMult = 0;
          for (size_t hit_iter = 0; hit_iter < vecMdt.size(); ++hit_iter) {
            PndMdtHit *mdtHit = (PndMdtHit *)fHitArray->At(vecMdt[hit_iter]);
            mdtHit->Position(newPos);
            Float_t hitDist = (oldPos - newPos).Mag2();
            deltaAngle = direction.Angle(newPos - oldPos); // lyt
            direction = newPos - oldPos;                   // lyt
            if ((corrDist < 0.) || (corrDist > hitDist))   // find closes hit
            {
              corrDist = hitDist;
              corrId = vecMdt[hit_iter];
              corrPos = newPos;
            }
            if ((hitDist > 0.) && ((TMath::Sqrt(hitDist) / layerDist) < 2.5))
              layerMult++;
          }

          if ((corrDist > 0.) && ((TMath::Sqrt(corrDist) / layerDist) < 2.5)) // if there in one correlated hit closer than 2.5 layer distance
          {
            ironDist = ironDist + mdtIronThickness[1][(*layer_iter).first - 1] * TMath::Sqrt(corrDist) / layerDist;
            mdtTrk->SetModule(-1);
            mdtTrk->SetHitIndex(layerCount, corrId);
            mdtTrk->SetHitDist(layerCount, corrDist);
            mdtTrk->SetHitDeltaAngle(layerCount, deltaAngle); // lyt
            mdtTrk->SetLayerDist(layerCount, layerDist);
            mdtTrk->SetHitMult(layerCount, layerMult);
            layerCount++;
            maxLayer = (*layer2_iter).first;
            oldPos = corrPos; // reset position for next mdt layer
          } else
            break;
        } // end of layer loop
      }
      mdtTrk->SetIronDist(ironDist);
      mdtTrk->SetMaxLayer(maxLayer);
      mdtTrk->SetLayerCount(layerCount);
      AddTrk(mdtTrk); // storing the PndMdtTrk object
      delete (mdtTrk);
    } // end of layer0 loop
  }

  if (mapMdtEndcap.size() > 0) {
    vector<Int_t> vecMdt0 = mapMdtEndcap[0];
    TVector3 oldPos(0., 0., 0.);
    TVector3 newPos(0., 0., 0.);

    for (size_t iMap = 0; iMap < vecMdt0.size(); iMap++) // loop over hits in layer0
    {
      Int_t layerCount = 1, maxLayer = 0;
      Float_t layerDist = 0., ironDist = 0.;

      // check whether this hit is available in mapHitDirection
      map<Int_t, TVector3>::const_iterator hit_direction_iter;
      map<Int_t, Float_t>::const_iterator hit_distance_iter;

      hit_direction_iter = mapHitDirection.find(vecMdt0[iMap]);
      hit_distance_iter = mapHitDistance.find(vecMdt0[iMap]);
      if (fRec_method == 1 && hit_direction_iter == mapHitDirection.end()) {
        // cout<<"this hit is not correlated to any track..."<<endl;
        continue;
      }
      direction = (*hit_direction_iter).second;
      Float_t dist_layer0_lhetrack = (*hit_distance_iter).second;

      PndMdtTrk *mdtTrk = new PndMdtTrk();
      mdtTrk->SetHitIndex(0, vecMdt0[iMap]);
      mdtTrk->SetModule(2);

      PndMdtHit *mdtHit0 = (PndMdtHit *)fHitArray->At(vecMdt0[iMap]);
      mdtHit0->Position(oldPos);
      mdtTrk->SetHitIndex(0, vecMdt0[iMap]);
      mdtTrk->SetHitDist(0, dist_layer0_lhetrack); // set to be the distance of hit with lhetrack  lyt
      mdtTrk->SetHitDeltaAngle(0, -1.);            // lyt
      mdtTrk->SetLayerDist(0, 0);
      mdtTrk->SetHitMult(0, 1);

      map<Int_t, vector<Int_t>>::const_iterator layer_iter;
      for (layer_iter = mapMdtEndcap.begin(); layer_iter != mapMdtEndcap.end(); ++layer_iter) // layer loop
      {
        if (((*layer_iter).first) == 0)
          continue; // skip first layer
        if (((*layer_iter).first - maxLayer) > 1)
          break;
        layerDist = mdtLayerPos[1][(*layer_iter).first] - mdtLayerPos[1][maxLayer];
        vector<Int_t> vecMdt = (*layer_iter).second;
        Float_t corrDist = -1;
        Int_t corrId = -1;
        TVector3 corrPos(0., 0., 0.);
        Int_t layerMult = 0;
        for (size_t hit_iter = 0; hit_iter < vecMdt.size(); ++hit_iter) {
          PndMdtHit *mdtHit = (PndMdtHit *)fHitArray->At(vecMdt[hit_iter]);
          mdtHit->Position(newPos);
          Float_t hitDist = (oldPos - newPos).Mag2();
          deltaAngle = direction.Angle(newPos - oldPos); // lyt
          direction = newPos - oldPos;                   // lyt
          if ((corrDist < 0.) || (corrDist > hitDist))   // find closes hit
          {
            corrDist = hitDist;
            corrId = vecMdt[hit_iter];
            corrPos = newPos;
          }
          if ((hitDist > 0.) && ((TMath::Sqrt(hitDist) / layerDist) < 2.5))
            layerMult++;
        }

        if ((corrDist > 0.) && ((TMath::Sqrt(corrDist) / layerDist) < 2.5)) // if there in one correlated hit closer than 2.5 later distance
        {
          ironDist = ironDist + mdtIronThickness[1][(*layer_iter).first - 1] * TMath::Sqrt(corrDist) / layerDist;
          mdtTrk->SetHitIndex(layerCount, corrId);
          mdtTrk->SetHitDist(layerCount, corrDist);
          mdtTrk->SetHitDeltaAngle(layerCount, deltaAngle); // lyt
          mdtTrk->SetLayerDist(layerCount, layerDist);
          mdtTrk->SetHitMult(layerCount, layerMult);
          layerCount++;
          maxLayer = (*layer_iter).first;
          oldPos = corrPos; // reset position for next mdt layer
        } else
          break;

      } // end of layer loop
      mdtTrk->SetIronDist(ironDist);
      mdtTrk->SetMaxLayer(maxLayer);
      mdtTrk->SetLayerCount(layerCount);
      AddTrk(mdtTrk); // storing the PndMdtTrk object
      delete (mdtTrk);
    } // end of layer0 loop
  }   // end of endcap block

  if (mapMdtForward.size() > 0) {
    vector<Int_t> vecMdt0 = mapMdtForward[0];
    TVector3 oldPos(0., 0., 0.);
    TVector3 newPos(0., 0., 0.);

    for (size_t iMap = 0; iMap < vecMdt0.size(); iMap++) // loop over hits in layer0
    {
      Int_t layerCount = 1, maxLayer = 0;
      Float_t layerDist = 0;

      // check whether this hit is available in mapHitDirection
      map<Int_t, TVector3>::const_iterator hit_direction_iter;
      map<Int_t, Float_t>::const_iterator hit_distance_iter;

      hit_direction_iter = mapHitDirection.find(vecMdt0[iMap]);
      hit_distance_iter = mapHitDistance.find(vecMdt0[iMap]);
      if (fRec_method == 1 && hit_direction_iter == mapHitDirection.end()) {
        // cout<<"this hit is not correlated to any track..."<<endl;
        continue;
      }
      direction = (*hit_direction_iter).second;
      Float_t dist_layer0_lhetrack = (*hit_distance_iter).second;

      PndMdtTrk *mdtTrk = new PndMdtTrk();
      mdtTrk->SetHitIndex(0, vecMdt0[iMap]);
      mdtTrk->SetModule(4);

      PndMdtHit *mdtHit0 = (PndMdtHit *)fHitArray->At(vecMdt0[iMap]);
      mdtHit0->Position(oldPos);
      mdtTrk->SetHitIndex(0, vecMdt0[iMap]);
      mdtTrk->SetHitDist(0, dist_layer0_lhetrack); // set to be the distance of hit with lhetrack  lyt
      mdtTrk->SetHitDeltaAngle(0, -1.);            // lyt
      mdtTrk->SetLayerDist(0, 0);
      mdtTrk->SetHitMult(0, 1);

      map<Int_t, vector<Int_t>>::const_iterator layer_iter;
      for (layer_iter = mapMdtForward.begin(); layer_iter != mapMdtForward.end(); ++layer_iter) // layer loop
      {
        if (((*layer_iter).first) == 0)
          continue; // skip first layer
        if (((*layer_iter).first - maxLayer) > 1)
          break;
        layerDist = mdtLayerPos[2][(*layer_iter).first] - mdtLayerPos[2][maxLayer];
        vector<Int_t> vecMdt = (*layer_iter).second;
        Float_t corrDist = -1;
        Int_t corrId = -1;
        TVector3 corrPos(0., 0., 0.);
        Int_t layerMult = 0;
        for (size_t hit_iter = 0; hit_iter < vecMdt.size(); ++hit_iter) {
          PndMdtHit *mdtHit = (PndMdtHit *)fHitArray->At(vecMdt[hit_iter]);
          mdtHit->Position(newPos);
          Float_t hitDist = (oldPos - newPos).Mag2();
          deltaAngle = direction.Angle(newPos - oldPos); // lyt
          direction = newPos - oldPos;                   // lyt
          if ((corrDist < 0.) || (corrDist > hitDist))   // find closes hit
          {
            corrDist = hitDist;
            corrId = vecMdt[hit_iter];
            corrPos = newPos;
          }
          if ((hitDist > 0.) && ((TMath::Sqrt(hitDist) / layerDist) < 2.5))
            layerMult++;
        }

        if ((corrDist > 0.) && ((TMath::Sqrt(corrDist) / layerDist) < 2.5)) // if there in one correlated hit closer than 2.5 later distance
        {

          mdtTrk->SetHitIndex(layerCount, corrId);
          mdtTrk->SetHitDist(layerCount, corrDist);
          mdtTrk->SetHitDeltaAngle(layerCount, deltaAngle); // lyt
          mdtTrk->SetLayerDist(layerCount, layerDist);
          mdtTrk->SetHitMult(layerCount, layerMult);
          layerCount++;
          maxLayer = (*layer_iter).first;
          oldPos = corrPos; // reset position for next mdt layer
        }

      } // end of layer loop

      mdtTrk->SetMaxLayer(maxLayer);
      mdtTrk->SetLayerCount(layerCount);
      AddTrk(mdtTrk); // storing the PndMdtTrk object
      delete (mdtTrk);
    } // end of layer0 loop
  }   // end of Forward block
}

// -----   Private method MdtMapping   --------------------------------------------
Bool_t PndMdtTrkProducer::MdtMapping()
{
  Int_t nHits = fHitArray->GetEntriesFast();
  if (nHits == 0)
    return kFALSE;

  Reset();
  PndMdtHit *mdtHit = nullptr;
  for (Int_t iHit = 0; iHit < nHits; iHit++) {
    mdtHit = (PndMdtHit *)fHitArray->At(iHit);
    Int_t mdtLayer = -1; // mdtModule = -1,  //[R.K. 01/2017] unused variable

    switch (mdtHit->GetModule()) {
    case 1:
      // mdtModule = 1; //[R.K. 01/2017] unused variable
      mdtLayer = mdtHit->GetLayerID();
      mapMdtBarrel[mdtLayer].push_back(iHit);
      break;

    case 2:
      // mdtModule = 2; //[R.K. 01/2017] unused variable
      mdtLayer = mdtHit->GetLayerID();
      mapMdtEndcap[mdtLayer].push_back(iHit);
      break;

    case 3:
      // mdtModule = 2; //[R.K. 01/2017] unused variable
      mdtLayer = mdtHit->GetLayerID() + 5;
      mapMdtEndcap[mdtLayer].push_back(iHit);
      break;

    case 4:
      // mdtModule = 3; //[R.K. 01/2017] unused variable
      mdtLayer = mdtHit->GetLayerID();
      mapMdtForward[mdtLayer].push_back(iHit);
      break;

    default: cout << "-E- PndMdtTrkProducer::Init: Wrong MDT Module" << endl; return kFALSE;
    }
  }

  return kTRUE;
}

// -----   Private method AddTrk   --------------------------------------------
PndMdtTrk *PndMdtTrkProducer::AddTrk(PndMdtTrk *track)
{
  // Creates a new hit in the TClonesArray.

  //  Float_t chi2=0;
  //   for (Int_t ll=1; ll<10; ll++)
  //     {
  //       if (track->GetHitBit(ll)==0) continue;
  //       if (ll==1)
  // 	chi2 = chi2 + (track->GetHit(ll)/1.5)*(track->GetHitAngle(ll)/1.5); // manual correction for the first layer
  //       else
  // 	chi2 = chi2 + track->GetHitAngle(ll)*track->GetHitAngle(ll);
  //     }
  //   track->SetChi2(chi2);

  TClonesArray &trkRef = *fTrkArray;
  Int_t size = trkRef.GetEntriesFast();
  return new (trkRef[size]) PndMdtTrk(*track);
}

// -----   Public method Reset   --------------------------------------------
void PndMdtTrkProducer::Reset()
{
  // reset maps
  mapMdtBarrel.clear();
  mapMdtEndcap.clear();
  mapMdtForward.clear();
  mapHitDirection.clear();
}

void PndMdtTrkProducer::AlgorithmWithLheGenTrack() // lyt April 15th, 2010
{
  if (!fLheGenTrack) {
    return;
  }
  Int_t nTracks = fLheGenTrack->GetEntriesFast();
  for (Int_t i = 0; i < nTracks; i++) {
    PndTrack *track = (PndTrack *)fLheGenTrack->At(i);
    Int_t ierr = 0;
    FairTrackParP par = track->GetParamLast();
    if ((par.GetMomentum().Mag() < 0.1) || (par.GetMomentum().Mag() > 15.))
      continue;
    FairTrackParH *helix = new FairTrackParH(&par, ierr);

    // loop mdthit, to find distance
    PndMdtHit *mdtHit = nullptr;
    Int_t mdtEntries = fHitArray->GetEntriesFast();
    Int_t mdtIndex = -1; // mdtMod = 0, , mdtLayer = 0 //[R.K. 01/2017] unused variable
    // Float_t mdtGLength = -1000; //[R.K. 03/2017] unused variable?
    Float_t mdtQuality = 1000000;

    // Float_t chi2 = 0; //[R.K. 01/2017] unused variable?
    TVector3 vertex(0., 0., 0.);
    TVector3 mdtPos(0., 0., 0.);
    TVector3 momentum(0., 0., 0.);
    TVector3 momentum_keep(0., 0., 0.);

    for (Int_t mm = 0; mm < mdtEntries; mm++) {
      mdtHit = (PndMdtHit *)fHitArray->At(mm);
      if (mdtHit->GetLayerID() != 0)
        continue; // only deal with the first layer
      if (mdtHit->GetModule() > 2)
        continue;
      mdtHit->Position(mdtPos);
      if (1) // fGeanePro
      {
        FairGeanePro *fProMdt = new FairGeanePro();
        fProMdt->SetPoint(mdtPos);
        fProMdt->PropagateToPCA(1, 1);
        vertex.SetXYZ(-10000, -10000, -10000); // reset vertex
        FairTrackParH *fRes = new FairTrackParH();
        Bool_t rc = fProMdt->Propagate(helix, fRes, -13 * helix->GetQ()); //-13*pidCand->GetCharge());
        if (!rc)
          continue;

        vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
        momentum.SetXYZ(fRes->GetPx(), fRes->GetPy(), fRes->GetPz()); // set momentum, to be used as the direction to extrapolated to next layer.
                                                                      // mdtGLength = fProMdt->GetLengthAtPCA(); //[R.K. 03/2017] unused variable?
      }

      Float_t dist;
      if (mdtHit->GetModule() == 1) {
        dist = (mdtPos - vertex).Mag2();
      } else {
        dist = (vertex.X() - mdtPos.X()) * (vertex.X() - mdtPos.X()) + (vertex.Y() - mdtPos.Y()) * (vertex.Y() - mdtPos.Y());
      }

      if (mdtQuality > dist) {
        mdtIndex = mm;
        mdtQuality = dist;
        momentum_keep = momentum;
        // mdtMod = mdtHit->GetModule(); //[R.K. 01/2017] unused variable
        // mdtLayer = 1; //[R.K. 01/2017] unused variable
      }
    } // found one closest hit to the track;

    Float_t mdtCorrCut = 900; // temporary. need study, and better to set outside.

    if (mdtQuality < mdtCorrCut) {
      mapHitDirection[mdtIndex] = momentum_keep;
      mapHitDistance[mdtIndex] = mdtQuality;
    }
  }
}

ClassImp(PndMdtTrkProducer)
