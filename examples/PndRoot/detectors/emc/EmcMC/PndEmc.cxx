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

/////////////////////////////////////////////////////////////
//
//  PndEmc
//
//  Filler of PndEmcPoint
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include "PndEmc.h"

#include "PndEmcPoint.h"
#include "PndEmcReader.h"
#include "PndEmcStructure.h"
#include "PndEmcGeoPar.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairGeoMedia.h"
//#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "FairParIo.h"
#include "FairRun.h"

#include "TObjArray.h"
#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoArb8.h"
#include "TGeoVoxelFinder.h"
#include "TGeoMatrix.h"
#include "TGeoCompositeShape.h"
#include "PndDetectorList.h"
#include "FairGeoMedium.h"
#include "PndStack.h"
#include "TString.h"
//#include "TGeant3.h"
#include <TFile.h>

#include <iostream>
#include "math.h"

using std::cout;
using std::endl;

template <typename T>
int getsign(const T &a)
{
  return (a > 0 ? 1 : a < 0 ? -1 : 0);
}

// -----   Default constructor   -------------------------------------------
PndEmc::PndEmc()
  : fTrackID(0), fVolumeID(0), fEventID(-1), fPos(0, 0, 0, 0), fMom(0, 0, 0, 0), fTime(0), fLength(0), fELoss(0), fPosIndex(0), fEmcCollection(), bIsFastFsc(kFALSE),
    fStoreData(kTRUE), fwendcap(kFALSE), bwendcap(kFALSE), fgeoName2(""), fgeoName3(""), fgeoName4(""), MapperVersion(0)
{
  fEmcCollection = new TClonesArray("PndEmcPoint");
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------

PndEmc::PndEmc(const char *name, Bool_t active, Bool_t fast, Bool_t storepnts)
  : FairDetector(name, active), fTrackID(0), fVolumeID(0), fEventID(-1), fPos(0, 0, 0, 0), fMom(0, 0, 0, 0), fTime(0), fLength(0), fELoss(0), fPosIndex(0), fEmcCollection(),
    bIsFastFsc(fast), fStoreData(storepnts), fwendcap(kFALSE), bwendcap(kFALSE), fgeoName2(""), fgeoName3(""), fgeoName4(""), MapperVersion(0)
{
  fEmcCollection = new TClonesArray("PndEmcPoint");
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndEmc::~PndEmc()
{
  if (fEmcCollection) {
    fEmcCollection->Delete();
    delete fEmcCollection;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Intialize   ---------------------------------------
void PndEmc::Initialize()
{
  // Init function

  FairDetector::Initialize();
  FairRun *sim = FairRun::Instance();
  FairRuntimeDb *rtdb = sim->GetRuntimeDb();
  PndEmcGeoPar *par = (PndEmcGeoPar *)(rtdb->getContainer("PndEmcGeoPar"));
  par->setChanged();
  par->setInputVersion(sim->GetRunId(), 1);
}
// -------------------------------------------------------------------------
void PndEmc::BeginEvent()
{
  // Begin of the event
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t PndEmc::ProcessHits(FairVolume *)
{ // vol //[R.K.03/2017] unused variable(s)

  TString nam = gMC->CurrentVolName();

  //  if(gMC->IsTrackEntering()||gMC->IsTrackExiting()){
  //	  printf("\n###################\n");
  //	  if(gMC->IsTrackEntering()){
  //		  printf("track is entering volume %s\n",nam.Data());
  //	  }
  //	  if(gMC->IsTrackExiting()){
  //		  printf("track is exiting volume %s\n",nam.Data());
  //	  }
  //	  if(gMC->IsTrackInside()){
  //		  printf("track is inside volume %s\n",nam.Data());
  //	  }
  //	  if (gMC->IsNewTrack()){
  //		  printf("track is a new track %s\n", nam.Data());
  //	  }
  //	  TLorentzVector trackmomentum;
  //	  TLorentzVector trackposition;
  //	  gMC->TrackPosition(trackposition);
  //	  gMC->TrackMomentum(trackmomentum);
  //	  printf("Track is at x: %e, y: %e, z: %e, t: %e\n",trackposition.X(),trackposition.Y(),trackposition.Z(),trackposition.T());
  //	  printf("Momentum is: px: %e, py: %e, pz: %e, E: %e\n",trackmomentum.Px(),trackmomentum.Py(),trackmomentum.Pz(),trackmomentum.E());
  //	  printf("Steplength is: %e\n",gMC->TrackStep());
  //	  printf("Deposited Energy: %e\n",gMC->Edep());
  //	  printf("Particle Type: %i\n", gMC->TrackPid());
  //	  printf("TrackID: %i\n", gMC->GetStack()->GetCurrentTrackNumber());
  //	  printf("MotherID: %i\n", gMC->GetStack()->GetCurrentParentTrackNumber());
  //
  //	 //  CurrentBoundaryNormal is not implemented.
  //	 // Double_t x,y,z;
  //	 // if(gMC->CurrentBoundaryNormal(x,y,z)){
  //	//	  printf("track is at boundary with normal x: %e, y: %e, z: %e\n",x,y,z);
  //	//	  printf("track is at an angle of %e to normal\n",trackposition.Angle(TVector3(x,y,z))*TMath::RadToDeg());
  //	//  }
  //
  //	  printf("current Volume form gGeoManager is: %s\n",gGeoManager->GetCurrentVolume()->GetName());
  //	  printf("step from gGeoManager is: %e\n",gGeoManager->GetStep());
  //	  printf("###################\n");
  //  }

  if (gMC->Edep() <= 0) {
    // skip all the points which have no energy loss (i.e. Entering)
    // problem for MC truth!
    // ((Idea: Check if particle was produced inside crystal or outside))
    // ANY particle ENTERING and not being a NEW TRACK the crystal produces a hit
    if (gMC->IsNewTrack())
      return kTRUE;
    if (!gMC->IsTrackEntering() && !gMC->IsTrackExiting())
      return kTRUE;
  }

  // ---------------------------------------------------------------------------------
  // Getting parameters for the ROOT file with geometry for Forward End-Cap.
  // Each of the subvolume name for FwEndCap geometry in the ROOT file contains "Vol".
  Int_t copyNoCrys, copyNoBox, copyNoSub, copyNoQuar, idCrys, idBox, idSub, idQuar, copyNo, id, nMod, nRow, nCrys;
  copyNoCrys = copyNoBox = copyNoSub = copyNoQuar = idCrys = idBox = idSub = idQuar = copyNo = id = nMod = nRow = nCrys = -1;

  if (nam.Contains("Vol")) {

    TString namCrys = gMC->CurrentVolOffName(0); // Crystal name
    TString namBox = gMC->CurrentVolOffName(1);  // Box name
    TString namSub = gMC->CurrentVolOffName(2);  // Subunit/HalfSubunit name
    TString namQuar = gMC->CurrentVolOffName(3); // Quarter name

    if (namSub.Contains("SubunitVolFwEndCap")) { // including both Subunits and HalfSubunits

      // Return the current volumeOff upward in the geometrical tree (copy number of the volume)
      gMC->CurrentVolOffID(0, copyNoCrys);
      gMC->CurrentVolOffID(1, copyNoBox);
      gMC->CurrentVolOffID(2, copyNoSub);

      Int_t SubunitRow = -100;                              // Subunit/HalfSubunit row number
      Int_t SubunitCol = -100;                              // Subunit/HalfSubunit column number
      Int_t CrystalCol = -100;                              // FW EndCap crystal column number
      Int_t CrystalRow = -100;                              // FW EndCap crystal row number
      Int_t RestOfHalfSubunitRowNo[6] = {5, 6, 7, 8, 9, 9}; // row number of 6 peripheral half Subunits
      Int_t RestOfHalfSubunitColNo[6] = {8, 7, 6, 5, 3, 2}; // column number of 6 peripheral half Subunits

      // determination of SubunitRow and SubunitCol:
      if (copyNoSub >= 1 && copyNoSub <= 10) { // the middle row of 10 HalfSubunits
        SubunitRow = 0;
        SubunitCol = (int)pow(-1., 1 + (copyNoSub - 1) / 5) * (4 + (copyNoSub - 1) % 5);
      } else if (copyNoSub > 11 && copyNoSub < 24) { // the upper and lower pipe-region row of 14 HalfSubunits
        SubunitRow = (int)pow(-1., (copyNoSub - 11) / 7) * 2;
        SubunitCol = ((copyNoSub - 11) % 7) - 3;
      } else if ((copyNoSub >= 25 && copyNoSub <= 27) || copyNoSub == 57 || copyNoSub == 58) { // the outermost left column of 5 HalfSubunits
        if (copyNoSub >= 25 && copyNoSub <= 27)
          SubunitRow = copyNoSub - 25;
        else
          SubunitRow = copyNoSub - 59;
        SubunitCol = -9;
      } else if (copyNoSub >= 40 && copyNoSub <= 44) { // the outermost right column of 5 HalfSubunits
        SubunitRow = 42 - copyNoSub;
        SubunitCol = 9;
      } else if (copyNoSub == 28 || copyNoSub == 39 || copyNoSub == 45 || copyNoSub == 56) { // 4 single peripheral half Subunits
        if (copyNoSub <= 39)
          SubunitRow = RestOfHalfSubunitRowNo[0];
        else
          SubunitRow = -RestOfHalfSubunitRowNo[0];
        SubunitCol = (int)pow(-1., copyNoSub - 1) * RestOfHalfSubunitColNo[0];
      } else if (copyNoSub == 29 || copyNoSub == 38 || copyNoSub == 46 || copyNoSub == 55) { // 4 single peripheral half Subunits
        if (copyNoSub <= 38)
          SubunitRow = RestOfHalfSubunitRowNo[1];
        else
          SubunitRow = -RestOfHalfSubunitRowNo[1];
        SubunitCol = (int)pow(-1., copyNoSub) * RestOfHalfSubunitColNo[1];
      } else if (copyNoSub == 30 || copyNoSub == 37 || copyNoSub == 47 || copyNoSub == 54) { // 4 single peripheral half Subunits
        if (copyNoSub <= 37)
          SubunitRow = RestOfHalfSubunitRowNo[2];
        else
          SubunitRow = -RestOfHalfSubunitRowNo[2];
        SubunitCol = (int)pow(-1., copyNoSub - 1) * RestOfHalfSubunitColNo[2];
      } else if (copyNoSub == 31 || copyNoSub == 36 || copyNoSub == 48 || copyNoSub == 53) { // 4 single peripheral half Subunits
        if (copyNoSub <= 36)
          SubunitRow = RestOfHalfSubunitRowNo[3];
        else
          SubunitRow = -RestOfHalfSubunitRowNo[3];
        SubunitCol = (int)pow(-1., copyNoSub) * RestOfHalfSubunitColNo[3];
      } else if (copyNoSub == 32 || copyNoSub == 35 || copyNoSub == 49 || copyNoSub == 52) { // 4 single peripheral half Subunits
        if (copyNoSub <= 35)
          SubunitRow = RestOfHalfSubunitRowNo[4];
        else
          SubunitRow = -RestOfHalfSubunitRowNo[4];
        SubunitCol = (int)pow(-1., copyNoSub - 1) * RestOfHalfSubunitColNo[4];
      } else if (copyNoSub == 33 || copyNoSub == 34 || copyNoSub == 50 || copyNoSub == 51) { // 4 single peripheral half Subunits
        if (copyNoSub <= 34)
          SubunitRow = RestOfHalfSubunitRowNo[5];
        else
          SubunitRow = -RestOfHalfSubunitRowNo[5];
        SubunitCol = (int)pow(-1., copyNoSub) * RestOfHalfSubunitColNo[5];
      } else if (copyNoSub >= 61 && copyNoSub <= 74) { // the middle column of full Subunits
        SubunitRow = (int)pow(-1., (copyNoSub - 61) / 7) * ((copyNoSub - 61) % 7 + 3);
        SubunitCol = 0;
      } else if (copyNoSub % 100 >= 1 && copyNoSub % 100 <= 5) { // rest of the full Subunits
        if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
          SubunitRow = 1;
          SubunitCol = (int)pow(-1., 1 + copyNoSub / 100) * (3 + copyNoSub % 100);
        } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
          SubunitRow = -1;
          SubunitCol = (int)pow(-1., copyNoSub / 100) * (3 + copyNoSub % 100);
        }
      } else if (copyNoSub % 100 >= 6 && copyNoSub % 100 <= 11) { // rest of the full Subunits
        if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
          SubunitRow = 2;
          SubunitCol = (int)pow(-1., 1 + copyNoSub / 100) * (2 + copyNoSub % 100 - 5);
        } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
          SubunitRow = -2;
          SubunitCol = (int)pow(-1., copyNoSub / 100) * (2 + copyNoSub % 100 - 5);
        }
      } else if (copyNoSub % 100 >= 12 && copyNoSub % 100 <= 19) { // rest of the full Subunits
        if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
          SubunitRow = 3;
          SubunitCol = (int)pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 11);
        } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
          SubunitRow = -3;
          SubunitCol = (int)pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 11);
        }
      } else if (copyNoSub % 100 >= 20 && copyNoSub % 100 <= 27) { // rest of the full Subunits
        if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
          SubunitRow = 4;
          SubunitCol = (int)pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 19);
        } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
          SubunitRow = -4;
          SubunitCol = (int)pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 19);
        }
      } else if (copyNoSub % 100 >= 28 && copyNoSub % 100 <= 34) { // rest of the full Subunits
        if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
          SubunitRow = 5;
          SubunitCol = (int)pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 27);
        } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
          SubunitRow = -5;
          SubunitCol = (int)pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 27);
        }
      } else if (copyNoSub % 100 >= 35 && copyNoSub % 100 <= 40) { // rest of the full Subunits
        if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
          SubunitRow = 6;
          SubunitCol = (int)pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 34);
        } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
          SubunitRow = -6;
          SubunitCol = (int)pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 34);
        }
      } else if (copyNoSub % 100 >= 41 && copyNoSub % 100 <= 45) { // rest of the full Subunits
        if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
          SubunitRow = 7;
          SubunitCol = (int)pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 40);
        } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
          SubunitRow = -7;
          SubunitCol = (int)pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 40);
        }
      } else if (copyNoSub % 100 >= 46 && copyNoSub % 100 <= 49) { // rest of the full Subunits
        if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
          SubunitRow = 8;
          SubunitCol = (int)pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 45);
        } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
          SubunitRow = -8;
          SubunitCol = (int)pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 45);
        }
      } else if (copyNoSub % 100 == 50) { // rest of the full Subunits
        if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
          SubunitRow = 9;
          SubunitCol = (int)pow(-1., 1 + copyNoSub / 100);
        } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
          SubunitRow = -9;
          SubunitCol = (int)pow(-1., copyNoSub / 100);
        }
      }

      Int_t flag = 1; // this flag introducing is not necessary in this class but is also harmless
      if (std::abs(SubunitRow) <= 1 && std::abs(SubunitCol) <= 3)
        flag = 0; // empty copyNoSub in the beam-pipe area
      else if (std::abs(SubunitCol) == 9 && std::abs(SubunitRow) >= 3)
        flag = 0; // empty copyNoSub in the residual area
      else if (std::abs(SubunitCol) == 8 && std::abs(SubunitRow) >= 6)
        flag = 0; //  -||-
      else if (std::abs(SubunitCol) == 7 && std::abs(SubunitRow) >= 7)
        flag = 0; //  -||-
      else if (std::abs(SubunitCol) == 6 && std::abs(SubunitRow) >= 8)
        flag = 0; //  -||-
      else if (std::abs(SubunitCol) >= 4 && std::abs(SubunitRow) == 9)
        flag = 0; //  -||-

      // determination of CrystalRow and CrystalCol:
      if (flag && copyNoSub >= 61) { // determination of CrystalRow and CrystalCol for the 214 full Subunits
        if (copyNoBox == 1 || copyNoBox == 4) {
          if (copyNoCrys == 1 || copyNoCrys == 3)
            CrystalRow = SubunitRow * 4 + (3 - getsign(SubunitRow)) / 2;
          else // here necessarily: copyNoCrys==2 || copyNoCrys==4
            CrystalRow = SubunitRow * 4 + (1 - getsign(SubunitRow)) / 2;
        } else if (copyNoBox == 2 || copyNoBox == 3) {
          if (copyNoCrys == 1 || copyNoCrys == 3)
            CrystalRow = SubunitRow * 4 - (1 + getsign(SubunitRow)) / 2;
          else // here necessarily: copyNoCrys==2 || copyNoCrys==4
            CrystalRow = SubunitRow * 4 - (3 + getsign(SubunitRow)) / 2;
        }

        if (copyNoSub >= 61 && copyNoSub <= 74) { // determination of CrystalCol for the 14 middle column full Subunits
          if (copyNoBox == 1 || copyNoBox == 3) {
            if (copyNoCrys == 1 || copyNoCrys == 4)
              CrystalCol = 2;
            else // here necessarily: copyNoCrys==3 || copyNoCrys==2
              CrystalCol = 1;
          } else if (copyNoBox == 4 || copyNoBox == 2) {
            if (copyNoCrys == 1 || copyNoCrys == 4)
              CrystalCol = -1;
            else // here necessarily: copyNoCrys==3 || copyNoCrys==2
              CrystalCol = -2;
          }
        } else { // determination of CrystalCol for the other 4*50=200 full Subunits
          if (copyNoBox == 1 || copyNoBox == 3) {
            if (copyNoCrys == 1 || copyNoCrys == 4)
              CrystalCol = SubunitCol * 4 + (3 + getsign(SubunitCol)) / 2;
            else // here necessarily: copyNoCrys==3 || copyNoCrys==2
              CrystalCol = SubunitCol * 4 + (1 + getsign(SubunitCol)) / 2;
          } else if (copyNoBox == 4 || copyNoBox == 2) {
            if (copyNoCrys == 1 || copyNoCrys == 4)
              CrystalCol = SubunitCol * 4 - (1 - getsign(SubunitCol)) / 2;
            else // here necessarily: copyNoCrys==3 || copyNoCrys==2
              CrystalCol = SubunitCol * 4 - (3 - getsign(SubunitCol)) / 2;
          }
        }
      }

      else if (flag && copyNoSub <= 10) { // determination of CrystalRow and CrystalCol for the 10 middle-row half Subunits
        if (copyNoCrys == 1 || copyNoCrys == 3)
          CrystalRow = 1;
        else // here necessarily: copyNoCrys==2 || copyNoCrys==4
          CrystalRow = -1;

        if (copyNoBox == 1) {
          if (copyNoCrys == 1 || copyNoCrys == 4)
            CrystalCol = SubunitCol * 4 + (3 + getsign(SubunitCol)) / 2;
          else // here necessarily: copyNoCrys==3 || copyNoCrys==2
            CrystalCol = SubunitCol * 4 + (1 + getsign(SubunitCol)) / 2;
        } else { // here necessarily: copyNoBox==2
          if (copyNoCrys == 1 || copyNoCrys == 4)
            CrystalCol = SubunitCol * 4 - (1 - getsign(SubunitCol)) / 2;
          else // here necessarily: copyNoCrys==3 || copyNoCrys==2
            CrystalCol = SubunitCol * 4 - (3 - getsign(SubunitCol)) / 2;
        }
      } else if (flag && copyNoSub > 11 && copyNoSub < 24) { // determination of CrystalRow and CrystalCol for the 10 upper and lower pipe-region row of half subunits
        if (copyNoCrys == 1 || copyNoCrys == 3)
          CrystalRow = (1 + 17 * getsign(SubunitRow)) / 2;
        else // here necessarily: copyNoCrys==2 || copyNoCrys==4
          CrystalRow = (17 * getsign(SubunitRow) - 1) / 2;

        if (SubunitCol == 0 && copyNoBox == 1) {
          if (copyNoCrys == 1 || copyNoCrys == 4)
            CrystalCol = 2;
          else
            CrystalCol = 1;
        } else if (SubunitCol == 0) { // here necessarily: copyNoBox==2
          if (copyNoCrys == 1 || copyNoCrys == 4)
            CrystalCol = -1;
          else
            CrystalCol = -2;
        } else if (copyNoBox == 1) {
          if (copyNoCrys == 1 || copyNoCrys == 4)
            CrystalCol = SubunitCol * 4 + (3 + getsign(SubunitCol)) / 2;
          else // here necessarily: copyNoCrys==3 || copyNoCrys==2
            CrystalCol = SubunitCol * 4 + (1 + getsign(SubunitCol)) / 2;
        } else { // here necessarily: copyNoBox==2
          if (copyNoCrys == 1 || copyNoCrys == 4)
            CrystalCol = SubunitCol * 4 - (1 - getsign(SubunitCol)) / 2;
          else // here necessarily: copyNoCrys==3 || copyNoCrys==2
            CrystalCol = SubunitCol * 4 - (3 - getsign(SubunitCol)) / 2;
        }
      } else if (flag && ((copyNoSub >= 25 && copyNoSub <= 27) || copyNoSub == 57 || copyNoSub == 58 ||
                          (copyNoSub >= 40 && copyNoSub <= 44))) { // determination of CrystalRow and CrystalCol for the 2 outermost left and right columns of half Subunits
        if (copyNoBox == 1 && (copyNoCrys == 1 || copyNoCrys == 4))
          if (copyNoSub == 25 || copyNoSub == 42)
            CrystalRow = 2;
          else
            CrystalRow = SubunitRow * 4 + (3 + getsign(SubunitRow)) / 2;
        else if (copyNoBox == 1) // here necessarily: copyNoCrys==3 || copyNoCrys==2
          if (copyNoSub == 25 || copyNoSub == 42)
            CrystalRow = 1;
          else
            CrystalRow = SubunitRow * 4 + (1 + getsign(SubunitRow)) / 2;

        else if (copyNoBox == 2 && (copyNoCrys == 1 || copyNoCrys == 4))
          if (copyNoSub == 25 || copyNoSub == 42)
            CrystalRow = -1;
          else
            CrystalRow = SubunitRow * 4 - (1 - getsign(SubunitRow)) / 2;
        else if (copyNoBox == 2) // here necessarily: copyNoCrys==3 || copyNoCrys==2 //FIXME Check Logic. use braces!
          if (copyNoSub == 25 || copyNoSub == 42)
            CrystalRow = -2;
          else
            CrystalRow = SubunitRow * 4 - (3 - getsign(SubunitRow)) / 2;

        if (copyNoCrys == 1 || copyNoCrys == 3)
          if (!(copyNoSub >= 40 && copyNoSub <= 44))
            CrystalCol = -36;
          else
            CrystalCol = 35;
        else // here necessarily: copyNoCrys==4 || copyNoCrys==2
          if (!(copyNoSub >= 40 && copyNoSub <= 44))
          CrystalCol = -35;
        else
          CrystalCol = 36;
      } else if (flag && (copyNoSub == 28 || copyNoSub == 29 || copyNoSub == 38 || copyNoSub == 39 || copyNoSub == 45 || copyNoSub == 46 || copyNoSub == 55 ||
                          copyNoSub == 56)) { // determination of CrystalRow and CrystalCol for the 8 single peripheral half Subunits
        if (copyNoBox == 1 && (copyNoCrys == 1 || copyNoCrys == 4))
          CrystalRow = SubunitRow * 4 + (3 - getsign(SubunitRow)) / 2;
        else if (copyNoBox == 1) // here necessarily: copyNoCrys==3 || copyNoCrys==2
          CrystalRow = SubunitRow * 4 + (1 - getsign(SubunitRow)) / 2;
        else if (copyNoCrys == 1 || copyNoCrys == 4) // here necessarily: copyNoBox==2
          CrystalRow = SubunitRow * 4 - (1 + getsign(SubunitRow)) / 2;
        else // here necessarily: copyNoBox==2 && (copyNoCrys==3 || copyNoCrys==2)
          CrystalRow = SubunitRow * 4 - (3 + getsign(SubunitRow)) / 2;

        if (copyNoCrys == 4 || copyNoCrys == 2)
          CrystalCol = SubunitCol * 4 + (1 - getsign(SubunitCol)) / 2;
        else // here necessarily: copyNoCrys==3 || copyNoCrys==1
          CrystalCol = SubunitCol * 4 - (1 + getsign(SubunitCol)) / 2;
      } else if (flag && ((copyNoSub >= 30 && copyNoSub <= 37) ||
                          (copyNoSub >= 47 && copyNoSub <= 54))) { // determination of CrystalRow and CrystalCol for the 16 single peripheral half Subunits
        if (copyNoCrys == 1 || copyNoCrys == 3)
          CrystalRow = SubunitRow * 4 + (1 - 3 * getsign(SubunitRow)) / 2;
        else // here necessarily: copyNoCrys==4 || copyNoCrys==2
          CrystalRow = SubunitRow * 4 - (1 + 3 * getsign(SubunitRow)) / 2;

        if (copyNoBox == 1 && (copyNoCrys == 1 || copyNoCrys == 4))
          CrystalCol = SubunitCol * 4 + (3 + getsign(SubunitCol)) / 2;
        else if (copyNoBox == 1) // here necessarily: copyNoCrys==3 || copyNoCrys==2
          CrystalCol = SubunitCol * 4 + (1 + getsign(SubunitCol)) / 2;
        else if (copyNoCrys == 1 || copyNoCrys == 4) // here necessarily: copyNoBox==2
          CrystalCol = SubunitCol * 4 - (1 - getsign(SubunitCol)) / 2;
        else // here necessarily: copyNoBox==2 && (copyNoCrys==3 || copyNoCrys==2)
          CrystalCol = SubunitCol * 4 - (3 - getsign(SubunitCol)) / 2;
      }

      if (CrystalRow == -100 || CrystalCol == -100)
        std::cout << "No assignment for CrystalRow and CrystalCol\n";

      nMod = 3;
      copyNo = 0;
      nRow = CrystalRow + 37;  // now nRow represents the FW EndCap crystal row number (-37<=CrystalRow<=37, CrystalRow!=0)----> 0<=nRow<=74, nRow!=37
      nCrys = CrystalCol + 36; // now nCrys represents the FW EndCap crystal column number (-36<=CrystalCol<=36, CrystalCol!=0)----->0<=nCrys<=72, nCrys!=36

      // Text_t buffer[40];
      // sprintf(buffer,"emc0%dr%dc%dcp%d",nMod, CrystalCol, CrystalRow, copyNoQuar);
      // copyNo = copyNoQuar;
    }

    /*
    //used for the old version:
    if (namCrys.Contains("CrystalVol_block"))
    {
     gMC->CurrentVolOffID(0,copyNoCrys);
     nMod=3;
     copyNo = copyNoCrys;
     nRow = 999;
     nCrys = 999;
    }
    */

    else if (namQuar.Contains("Quarter4Vol")) {
      // ----- NEW Backward EndCap - with the FwEndCap geometry ----

      // Return the current volume off upward in the geometrical tree
      // ID and copy number
      idCrys = gMC->CurrentVolOffID(0, copyNoCrys);
      idBox = gMC->CurrentVolOffID(1, copyNoBox);
      idSub = gMC->CurrentVolOffID(2, copyNoSub) - 1;
      idQuar = gMC->CurrentVolOffID(3, copyNoQuar);

      Int_t col = 0;    //, k1=0; //[R.K. 01/2017] unused variable?
      Int_t subrow = 4; // 4 crystals in each subvolume
      Int_t next = 0;   // starts (from the middle) next column, represents rows
      copyNoSub -= 1;   // When geometry is created, copyNoSub starts from 1-13
                        // and the loop below starts from 0-12

      //  Now, 26.02.2009, 3 crystals in the middle's subunit are added =>
      // => number of Subunits for BwEncCap & straight geometry is the same
      if ((copyNoSub >= 0) && (copyNoSub <= 3)) {
        next = copyNoSub;
        col = 0;
      } else if ((copyNoSub >= 4) && (copyNoSub <= 7)) {
        next = (copyNoSub - 4);
        col = 1;
      } else if ((copyNoSub >= 8) && (copyNoSub <= 10)) {
        next = (copyNoSub - 8);
        col = 2;
      } else if ((copyNoSub >= 11) && (copyNoSub <= 12)) {
        next = (copyNoSub - 11);
        col = 3;
      }

      Int_t flag4 = 1;
      // 26.02.2009
      // "next" means "row", "col" means "col"

      if (next > 1 && col > 2)
        flag4 = 0; // corner's subunit + one below
      if (next > 2 && col > 1)
        flag4 = 0; // + one to the left

      if (flag4 != 0) {
        if ((copyNoBox == 0) || (copyNoBox == 3)) {
          if (copyNoCrys == 1 || copyNoCrys == 3) {
            nCrys = next * 4 + 3;
          } else if (copyNoCrys == 0 || copyNoCrys == 2) {
            nCrys = next * 4 + 4;
          }
        } else if ((copyNoBox == 1) || (copyNoBox == 2)) {
          if (copyNoCrys == 0 || copyNoCrys == 2) {
            nCrys = next * 4 + 2;
          } else if (copyNoCrys == 1 || copyNoCrys == 3) {
            nCrys = next * 4 + 1;
          }
        }
        if ((copyNoBox == 0) || (copyNoBox == 2)) {
          if (copyNoCrys == 0 || copyNoCrys == 3) {
            nRow = subrow * col + 4;
          } else if (copyNoCrys == 1 || copyNoCrys == 2) {
            nRow = subrow * col + 3;
          }
        } else if ((copyNoBox == 1) || (copyNoBox == 3)) {
          if (copyNoCrys == 0 || copyNoCrys == 3) {
            nRow = subrow * col + 2;
          } else if (copyNoCrys == 1 || copyNoCrys == 2) {
            nRow = subrow * col + 1;
          }
        }
      }
      nMod = 4;
      copyNo = copyNoQuar;
    } else if (namQuar.Contains("QuarterNewVol")) {
      // ----- NEW Backward EndCap - with the FwEndCap geometry ----

      nMod = 4;

      // Return the current volume off upward in the geometrical tree
      // ID and copy number
      idCrys = gMC->CurrentVolOffID(0, copyNoCrys);
      idBox = gMC->CurrentVolOffID(1, copyNoBox);
      idSub = gMC->CurrentVolOffID(2, copyNoSub) - 1;
      idQuar = gMC->CurrentVolOffID(3, copyNoQuar);

      Int_t col = 0;
      Int_t next = 0; // starts (from the middle) next column, represents rows
      copyNoSub -= 1; // When geometry is created, copyNoSub starts from 1-13
                      // and the loop below starts from 0-12

      //  Now, 26.02.2009, 3 crystals in the middle's subunit are added =>
      // => number of Subunits for BwEncCap & straight geometry is the same
      if ((copyNoSub >= 0) && (copyNoSub <= 2)) {
        next = copyNoSub + 1;
        col = 0;
      } else if ((copyNoSub >= 3) && (copyNoSub <= 6)) {
        next = (copyNoSub - 3);
        col = 1;
      } else if ((copyNoSub >= 7) && (copyNoSub <= 10)) {
        next = (copyNoSub - 7);
        col = 2;
      } else if ((copyNoSub >= 11) && (copyNoSub <= 13)) {
        next = (copyNoSub - 11);
        col = 3;
      }

      Int_t flag4 = 1;
      // 26.02.2009
      // "next" means "row", "col" means "col"

      if (next == 3 && col == 3)
        flag4 = 0; // corner's subunit + one below
      if (next == 0 && col == 0)
        flag4 = 0; // + one to the left

      if (flag4 != 0) {
        if ((copyNoBox == 0) || (copyNoBox == 3)) {
          if (copyNoCrys == 1 || copyNoCrys == 3) {
            nCrys = next * 4 + 3;
          } else if (copyNoCrys == 0 || copyNoCrys == 2) {
            nCrys = next * 4 + 4;
          }
        } else if ((copyNoBox == 1) || (copyNoBox == 2)) {
          if (copyNoCrys == 0 || copyNoCrys == 2) {
            nCrys = next * 4 + 2;
          } else if (copyNoCrys == 1 || copyNoCrys == 3) {
            nCrys = next * 4 + 1;
          }
        }
        if ((copyNoBox == 0) || (copyNoBox == 2)) {
          if (copyNoCrys == 0 || copyNoCrys == 3) {
            nRow = col * 4 + 4;
          } else if (copyNoCrys == 1 || copyNoCrys == 2) {
            nRow = col * 4 + 3;
          }
        } else if ((copyNoBox == 1) || (copyNoBox == 3)) {
          if (copyNoCrys == 0 || copyNoCrys == 3) {
            nRow = col * 4 + 2;
          } else if (copyNoCrys == 1 || copyNoCrys == 2) {
            nRow = col * 4 + 1;
          }
        }
      }
      nMod = 4;
      copyNo = copyNoQuar;
    }

  } // if (nam.Contains("Vol"))

  // ----- Backward EndCap - 2017 version ----
  if (nam.Contains("PWOCrystal") && TString(gMC->CurrentVolOffName(2)).Contains("BWECquarter")) {
    nMod = 4;
    idCrys = gMC->CurrentVolOffID(0, nCrys);
    idSub = gMC->CurrentVolOffID(1, nRow);
    idQuar = gMC->CurrentVolOffID(2, copyNo);
    // printf("nMod=%d  nRow=%d  copyNo=%d  nCrys=%d\n",nMod,nRow,copyNo,nCrys);
  }

  // ---------------------------------------------------------------------------------

  fTrackID = gMC->GetStack()->GetCurrentTrackNumber(); // trk ID
  fEventID = gMC->CurrentEvent();
  fELoss = gMC->Edep();
  fLength = gMC->TrackLength();
  fTime = gMC->TrackTime();
  gMC->TrackPosition(fPos); // cm
  gMC->TrackMomentum(fMom); // GeV
  // Int_t CrystalID; //[R.K.03/2017] unused variable
  if (fTrackID < 0) {
    LOG(error) << "PndEMC::ProcessHits: TrackID < 0 " << fTrackID << " eventID " << fEventID << " volumeName " << nam;
  }
  if (nam.Contains("CrystalType6")) {
    nMod = 7;
    copyNo = 1;
    TString namCrystal = gMC->CurrentVolOffName(1); // Crystal name
    //        TString namRow    = gMC->CurrentVolOffName(2); // Row name
    //        namRow.Remove(0,3);
    //        if(namRow.IsDigit()){
    //            nRow=namRow.Atoi();
    //        } else {
    //            nMod=-1;
    //        }
    // CrystalID= //[R.K.03/2017] unused variable
    gMC->CurrentVolOffID(1, nCrys);
    nRow = (nCrys - 1) / 5 + 1;
    nCrys = (nCrys - 1) % 5;
    //        printf("Crystal has name %s, ID %i, and copy number %i\n",namCrystal.Data(),CrystalID,nCrys);
    if (namCrystal.Contains("CrystalType6a")) {
      nCrys = nCrys * 2 + 1;
    } else if (namCrystal.Contains("CrystalType6b")) {
      nCrys = nCrys * 2 + 2;
    } else {
      nMod = -1;
    }
  }

  if (nam.BeginsWith("Crystal-")) { // added for barrel root file
    int iAlveole, iCrystal;
    char sign, type;
    sscanf(nam, "Crystal-%d%c-%c%d", &iAlveole, &sign, &type, &iCrystal);

    // nMod
    if (sign == 'p')
      nMod = 1;
    else if (sign == 'm')
      nMod = 2;
    else
      std::cout << "Error!!!" << std::endl;

    // nRow
    nRow = (iAlveole - 1) * 4 + iCrystal;

    // copyNo
    gMC->CurrentVolOffID(3, copyNo);

    // nCrys
    Int_t iPhi;
    gMC->CurrentVolOffID(0, iPhi);
    if (nMod == 1) {
      if (type == 'L')
        nCrys = (iPhi - 1) * 2 + 1;
      if (type == 'R')
        nCrys = (iPhi - 1) * 2 + 2;
    } else if (nMod == 2) {
      if (type == 'R')
        nCrys = (iPhi - 1) * 2 + 1;
      if (type == 'L')
        nCrys = (iPhi - 1) * 2 + 2;
    } else {
      std::cout << "Error!!!" << std::endl;
    }
  }

  if (nam.BeginsWith("emc")) {

    sscanf(nam, "emc%dr%dc%d", &nMod, &nRow, &nCrys);

    // Crys 1-5000; copyNo 1-20; nRow 1-100, nMod 1-6
    if ((nMod == 1) || (nMod == 2))
      id = gMC->CurrentVolOffID(2, copyNo);
    if ((nMod == 3) || (nMod == 4) || (nMod == 6))
      id = gMC->CurrentVolOffID(1, copyNo);
    // 1 -because the pad stays inside flayer4 (Emc4), so only "1" as inheritance.
    // In barrel part one pad stays into Emc1 which stays inside Emc12 (and after Emc12 is
    // copied and rotated -> the inheritance level is "2"
  } else if (bIsFastFsc) {
    nMod = 5;
    nRow = abs((Int_t)(fPos.X() / 11.)) + 1;
    nCrys = abs((Int_t)(fPos.Y() / 11.)) + 1;
    if ((fPos.X() < 0.) && (fPos.Y() > 0.))
      copyNo = 1;
    if ((fPos.X() < 0.) && (fPos.Y() < 0.))
      copyNo = 2;
    if ((fPos.X() > 0.) && (fPos.Y() < 0.))
      copyNo = 3;
    if ((fPos.X() > 0.) && (fPos.Y() > 0.))
      copyNo = 4;
  } else if (!bIsFastFsc) {

    // Int_t ModCopy=0; //[R.K.02/2017] Unused variable
    Int_t SupModCopy = 0;
    Int_t LocCopy = 0;
    Int_t nSupCol = 0;
    Int_t nSupRow = 0;
    Int_t nModCol = 0;
    Int_t nModRow = 0;
    if (nam.Contains("FscSciVolume")) {
      gMC->CurrentVolOffID(4, SupModCopy); // upto FscSuperModuleVolume

      nSupCol = SupModCopy / 100;
      nSupRow = SupModCopy % 100;

      gMC->CurrentVolOffID(2, LocCopy); // upto FscModuleVolume
      nModCol = LocCopy % 2;
      nModRow = LocCopy / 2;

      nCrys = (nSupCol - 1) * 2 + nModCol + 1;
      nRow = (nSupRow - 1) * 2 + nModRow + 1;
      nMod = 5;
      copyNo = 1;
      //                  cout<<"SupModCopy="<<SupModCopy<<endl;
      //                  cout<<"LocCopy="<<LocCopy<<endl;
      //                  cout<<"nRow="<<nRow<<endl;
      //                  cout<<"nCrys="<<nCrys<<endl;
      //

      //		  TGeant3* gMC3 = (TGeant3*) gMC;
      //		  gMC3->Gpcxyz(); //a simple test
    } else if (nam.Contains("FscFiberVolume")) {
      nMod = 10;                           // for fibers different module
      gMC->CurrentVolOffID(4, SupModCopy); // upto FscSuperModuleVolume

      nSupCol = SupModCopy / 100;
      nSupRow = SupModCopy % 100;

      gMC->CurrentVolOffID(2, LocCopy); // upto FscModuleVolume
      nModCol = LocCopy % 2;
      nModRow = LocCopy / 2;

      nCrys = (nSupCol - 1) * 2 + nModCol + 1;
      nRow = (nSupRow - 1) * 2 + nModRow + 1;

      // Int_t fiberID = //[R.K.03/2017] unused variable
      gMC->CurrentVolOffID(1, copyNo); // copyNo - number of FIber
      //                  cout<<"Fiber nRow="<<nRow<<endl;
      //                  cout<<"Fiber nCrys="<<nCrys<<endl;
      //                  cout<<"Fiber copyNo="<<copyNo<<endl;

    } else {

      nam = gMC->CurrentVolOffName(2);
      sscanf(nam, "emc%dr%dc%d", &nMod, &nRow, &nCrys);
      if (nMod == 5)
        id = gMC->CurrentVolOffID(3, copyNo);
    }
  }

  fVolumeID = nMod * 100000000 + nRow * 1000000 + copyNo * 10000 + nCrys;

  if (fVolumeID < 0) {
    std::cout << "Negative element index in EMC, name=" << nam << std::endl;
    return kTRUE;
  }

  TVector3 pos(fPos.X(), fPos.Y(), fPos.Z());

  AddHit(fTrackID, fVolumeID, fEventID, TVector3(fPos.X(), fPos.Y(), fPos.Z()), TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, fELoss, nMod, nRow, nCrys, copyNo,
         gMC->IsTrackEntering() && !gMC->IsNewTrack(), gMC->IsTrackExiting());

  // Increment number of emc points for TParticle
  // if (gMC->IsTrackEntering())
  {
    PndStack *stack = (PndStack *)gMC->GetStack();
    stack->AddPoint(PndDetectorId::kEMC);
  }

  //   cout << "PndEmc: fTrackID= " << fTrackID
  //     << " fVolumeID= " << fVolumeID
  //    // << " fELoss= " << fELoss
  //    // << " fTime= " << fTime
  //     << " nMod= "<<nMod
  //     << " nRow= "<<nRow
  //     << " nCrys="<<nCrys
  //     << " copyNo="<<copyNo
  //     << endl;

  ResetParameters();

  return kTRUE;
}
// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndEmc::EndOfEvent()
{
  if (fVerboseLevel)
    Print();
  Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndEmc::Register()
{

  FairRootManager::Instance()->Register("EmcPoint", "Emc", fEmcCollection, fStoreData);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray *PndEmc::GetCollection(Int_t iColl) const
{
  if (iColl == 0)
    return fEmcCollection;

  return nullptr;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndEmc::Print() const
{
  Int_t nHits = fEmcCollection->GetEntriesFast();
  // cout << "-I- PndEmc: " << nHits << " points registered in this event."
  //<< endl;

  if (fVerboseLevel > 1)
    for (Int_t i = 0; i < nHits; i++)
      (*fEmcCollection)[i]->Print();
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void PndEmc::Reset()
{
  fEmcCollection->Delete();

  fPosIndex = 0;
}
// ----------------------------------------------------------------------------

// ------ Public method to enable/disable the storage of points ------------

void PndEmc::SetStorageOfData(Bool_t val)
{
  fStoreData = val;
  return;
}

// -------------------------------------------------------------------------
// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void PndEmc::CopyClones(TClonesArray *cl1, TClonesArray *cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();
  // cout << "-I- PndEmc: " << nEntries << " entries to add." << endl;
  TClonesArray &clref = *cl2;
  PndEmcPoint *oldpoint = nullptr;
  for (Int_t i = 0; i < nEntries; i++) {
    oldpoint = (PndEmcPoint *)cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndEmcPoint(*oldpoint);
    fPosIndex++;
  }
  //  cout << " -I- PndEmc: " << cl2->GetEntriesFast() << " merged entries."
  //   << endl;
}
// general function for chosing the combination of EMC geometry
void PndEmc::SetGeometryVersion(const Int_t GeoNumber)
{

  MapperVersion = 1;

  switch (GeoNumber) {

  // TODO: Make  emc_mechanics_and_module5_fsc.root default after resolution of overlap with Forward MDT
  case 1: // new added for barrel EMC
    SetGeometryFileNameQuadruple("emc_module12_2018v1.root", "emc_module3_2012_new.root", "emc_module4_2017.root", "emc_module5_fsc.root");
    MapperVersion = 1; // new MapperVersion for barrel EMC
    break;

  case 2:
    SetGeometryFileNameTriple("emc_module12.dat", "emc_module3_2012_new.root", "emc_module4_2017.root");
    MapperVersion = 9;
    break;

  case 3:
    SetGeometryFileNameQuadruple("emc_module12.dat", "emc_module3new.root", "emc_module4_2017.root", "emc_module5_fsc.root");
    MapperVersion = 3;
    break;

  case 4:
    SetGeometryFileNameTriple("emc_module12.dat", "emc_module3new.root", "emc_module4_2017.root");
    MapperVersion = 3;
    break;

  case 5:
    SetGeometryFileName("emc_module3.dat");
    MapperVersion = 2;
    break;

  case 6:
    SetGeometryFileName("emc_module3new.root");
    MapperVersion = 3;
    break;

  case 7:
    SetGeometryFileName("emc_module3_2012_new.root");
    MapperVersion = 4;
    break;

  case 8:
    SetGeometryFileName("emc_module4.dat");
    MapperVersion = 2;
    break;

  case 9:
    SetGeometryFileName("emc_module4_StraightGeo26.root");
    MapperVersion = 9;
    break;

  case 10:
    SetGeometryFileName("emc_module4_StraightGeo26_Al.root");
    MapperVersion = 9;
    break;

  case 11:
    SetGeometryFileName("emc_module4_2017.root");
    MapperVersion = 9;
    break;

  case 112: // old (2008) version of backward emc geometry
    SetGeometryFileName("emc_module4_StraightGeo24.4.root");
    MapperVersion = 8;
    break;

  case 12:
    SetGeometryFileName("emc_module4_StraightGeo24.4_Al2.root");
    MapperVersion = 9;
    break;

  case 13:
    SetGeometryFileName("emc_module4_FwEndCapGeo.root");
    MapperVersion = 9;
    break;

  case 14:
    SetGeometryFileName("emc_module4_FwEndCapGeo_Al.root");
    MapperVersion = 9;
    break;

  case 15:
    SetGeometryFileName("emc_module5_fsc.root");
    MapperVersion = 9;
    break;

  case 16:
    SetGeometryFileName("emc_module_5x5.dat");
    MapperVersion = 6;
    break;

  case 17:
    SetGeometryFileName("emc_proto60.root");
    MapperVersion = 7;
    break;

  case 18:
    SetGeometryFileName("emc_mechanics_and_module5_fsc.root");
    MapperVersion = 9;
    break;

  case 19:
    SetGeometryFileNameQuadruple("emc_module12.dat", "emc_module3_2012_new.root", "emc_module4_StraightGeo24.4.root", "emc_mechanics_and_module5_fsc.root");
    MapperVersion = 9;
    break;

  case 20: // old version of barrel emc geometry
    SetGeometryFileNameQuadruple("emc_module12.dat", "emc_module3_2012_new.root", "emc_module4_2017.root", "emc_module5_fsc.root");
    MapperVersion = 9;
    break;

  default:
    SetGeometryFileNameQuadruple("emc_module12.dat", "emc_module3new.root", "emc_module4_2017.root", "emc_module5_fsc.root");
    MapperVersion = 9;
    break;
  }

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
  PndEmcGeoPar *par = (PndEmcGeoPar *)(rtdb->getContainer("PndEmcGeoPar"));

  par->SetMapperVersion(MapperVersion);
  par->SetGeometryVersion(GeoNumber);
  par->setChanged();
  // par->setInputVersion(fRun->GetRunId(),1);
}

void PndEmc::SetGeometryFileNameDouble(TString fname, TString fname2, Int_t fwbwchoice, TString geoVer)
{

  SetGeometryFileName(fname, geoVer);

  // fgeoVer=geoVer;
  TString work = getenv("VMCWORKDIR");

  if (fwbwchoice == 0) {
    fwendcap = kTRUE;
    fgeoName2 = work + "/geometry/";
    fgeoName2 += fname2;
    cout << "---> _new_ Forward End-Cap has been used:  " << fgeoName2 << endl;
  } else {
    bwendcap = kTRUE;
    fgeoName3 = work + "/geometry/";
    fgeoName3 += fname2;
    cout << "---> _new_ Backward End-Cap has been used:  " << fgeoName3 << endl;
  }
}
void PndEmc::SetGeometryFileNameTriple(TString fname, TString fname2, TString fname3, TString geoVer)
{
  fwendcap = kTRUE;
  bwendcap = kTRUE;

  SetGeometryFileName(fname, geoVer);

  // SetGeometryFileNameDouble(fname, fname2, geoVer);

  // fgeoVer=geoVer;
  TString work = getenv("VMCWORKDIR");

  fgeoName2 = work + "/geometry/";
  fgeoName2 += fname2;

  fgeoName3 = work + "/geometry/";
  fgeoName3 += fname3;

  // cout <<"fgeoName3 "<< fgeoName3<< endl;
}

void PndEmc::SetGeometryFileNameQuadruple(TString fname, TString fname2, TString fname3, TString fname4, TString geoVer)
{
  fwendcap = kTRUE;
  bwendcap = kTRUE;

  SetGeometryFileName(fname, geoVer);

  // SetGeometryFileNameDouble(fname, fname2, geoVer);

  // fgeoVer=geoVer;
  TString work = getenv("VMCWORKDIR");

  fgeoName2 = work + "/geometry/";
  fgeoName2 += fname2;

  fgeoName3 = work + "/geometry/";
  fgeoName3 += fname3;

  fgeoName4 = work + "/geometry/";
  fgeoName4 += fname4;

  // cout <<"fgeoName4 "<< fgeoName4<< endl;
}

// ----------------------------------------------------------------------------
// -----   Public method ConstructGeometry   ----------------------------------
void PndEmc::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();

  cout << "fwendcap & bwendcap flags == " << fwendcap << " / " << bwendcap << endl;

  if (!fwendcap && !bwendcap) {

    if (fileName.EndsWith(".dat")) {
      std::cout << "                                               " << std::endl;
      std::cout << " ====== EMC::  ConstructASCIIGeometry() ====== " << std::endl;
      std::cout << " ============================================= " << std::endl;
      ConstructASCIIGeometry();
    } else if (fileName.EndsWith("new.root") || fileName.EndsWith("proto60.root") || fileName.EndsWith("proto192.root")) {
      std::cout << "                                              " << std::endl;
      std::cout << " ====== EMC::  ConstructROOTGeometry() m3 === " << std::endl;
      std::cout << " ============================================ " << std::endl;
      ConstructRootGeometry();
    } else if (fileName.EndsWith("4_FwEndCapGeo.root") || fileName.EndsWith("4_StraightGeo26.root") || fgeoName.EndsWith("4_StraightGeo26_Al.root") ||
               fileName.EndsWith("4_StraightGeo24.4.root") || fileName.EndsWith("4_StraightGeo24.4_Al2.root") || fileName.EndsWith("4_2017.root")) {
      std::cout << "                                              " << std::endl;
      std::cout << " ====== EMC::  ConstructROOTGeometry() m4 === " << std::endl;
      std::cout << " ============================================ " << std::endl;
      ConstructRootGeomMod4();
    } else if (fileName.EndsWith("5_fsc.root")) {
      std::cout << "                                              " << std::endl;
      std::cout << " ====== EMC::  ConstructROOTGeometry() m5 === " << std::endl;
      std::cout << " ============================================ " << std::endl;
      ConstructRootGeomMod5();
    } else {
      std::cout << "Geometry format not supported " << std::endl;
    }
  } else {
    if (fileName.EndsWith(".dat")) {
      std::cout << "                                               " << std::endl;
      std::cout << " ====== EMC 2)::  ConstructASCIIGeometry() === " << std::endl;
      std::cout << " ============================================= " << std::endl;
      ConstructASCIIGeometry();
    } else if (fileName.EndsWith("2018v1.root")) {
      std::cout << "                                               " << std::endl;
      std::cout << " ====== EMC 2)::  ConstructROOTGeometry() === " << std::endl;
      std::cout << " ============================================= " << std::endl;
      ConstructRootGeomMod12();
    } else {
      std::cout << "You do not provide an ASCII file " << std::endl;
    }

    Bool_t bEmc3 = kFALSE, bEmc4 = kFALSE, bEmc5 = kFALSE;
    if (fgeoName2.EndsWith("new.root")) {
      std::cout << "                                               " << std::endl;
      std::cout << " ====== EMC::  ConstructRootGeometry() m3a === " << std::endl;
      std::cout << " ============================================= " << std::endl;
      ConstructRootGeometry();
      bEmc3 = kTRUE;
    }
    if (fgeoName3.EndsWith("4_FwEndCapGeo.root") || fgeoName3.EndsWith("4_StraightGeo26.root") || fgeoName3.EndsWith("4_StraightGeo26_Al.root") ||
        fgeoName3.EndsWith("4_StraightGeo24.4.root") || fgeoName3.EndsWith("4_StraightGeo24.4_Al2.root") || fgeoName3.EndsWith("4_2017.root")) {
      std::cout << "                                               " << std::endl;
      std::cout << " ====== EMC::  ConstructRootGeometry() m4a === " << std::endl;
      std::cout << " ============================================= " << std::endl;
      std::cout << "fgeoName3:: " << fgeoName3 << std::endl;
      ConstructRootGeomMod4();
      bEmc4 = kTRUE;
    }
    if (fgeoName4.EndsWith("5_fsc.root")) {
      std::cout << "                                               " << std::endl;
      std::cout << " ====== EMC::  ConstructRootGeometry() m5a === " << std::endl;
      std::cout << " ============================================= " << std::endl;
      std::cout << "fgeoName4:: " << fgeoName4 << std::endl;
      ConstructRootGeomMod5();
      bEmc5 = kTRUE;
    }
    if (!bEmc3 && !bEmc4 && !bEmc5) {
      std::cout << "You do not provide a ROOT file " << std::endl;
    }
  }
}

void PndEmc::ConstructRootGeometry()
{

  TFile *f;
  TString filename;
  if (!fwendcap) {
    std::cout << "File name = " << GetGeometryFileName().Data() << std::endl;
    filename = GetGeometryFileName();
  } else {
    std::cout << "File name = " << fgeoName2 << std::endl;
    filename = fgeoName2;
  }
  f = new TFile(filename);

  TGeoVolume *Volume;
  TGeoCombiTrans *TransRotMatrix;
  if (filename.EndsWith("proto60.root")) {

    f->GetObject("Proto60", Volume);
    TransRotMatrix = new TGeoCombiTrans(0., 0., 0., new TGeoRotation());
  } else {
    Volume = (TGeoVolume *)f->Get("Emc3");
    TGeoRotation rotVolume;
    rotVolume.RotateY(180.);
    if (filename.Contains("proto")) {
      rotVolume.RotateY(-6.51324892093148211e0);
      rotVolume.RotateX(5.15340666154607074e0);
      TransRotMatrix = new TGeoCombiTrans(-3.66092554252584108e+01, -2.84174235113817986e+01, 1.02907349180644744e+02, new TGeoRotation(rotVolume));
      // Moving Proto192 so that beam axis is on bottom right corner of row 49 crystal 21 (X4Y3-7)

    } else {
      TransRotMatrix = new TGeoCombiTrans(0., 0., 213.9, new TGeoRotation(rotVolume)); // distance of the FwEndCap module to the target point was obtained to be around 2139 mm.}
    }
  }
  if (Volume == nullptr) {
    printf("Could not get geometry from file %s!.\nIs this the right file?\n", filename.Data());
    return;
  }
  TGeoVolume *Cave = gGeoManager->GetTopVolume();
  TGeoNode *n = Volume->GetNode(0);

  gGeoManager->AddVolume(Volume);
  TGeoVoxelFinder *voxels = Volume->GetVoxels();
  if (voxels)
    voxels->SetNeedRebuild();
  TGeoMatrix *M = n->GetMatrix();
  M->SetDefaultName();
  gGeoManager->GetListOfMatrices()->Remove(M);
  TGeoHMatrix *global = gGeoManager->GetHMatrix();
  gGeoManager->GetListOfMatrices()->Remove(global); // Remove the Identity matrix

  Cave->AddNode(Volume, 0, TransRotMatrix);

  ExpandNode(Volume, Cave);
}

void PndEmc::ConstructRootGeomMod12()
{ // Added for barrel root file
  // volume from the root file
  TFile *f = new TFile(GetGeometryFileName());
  std::cout << "File name = " << GetGeometryFileName().Data() << std::endl;
  TGeoVolume *Volume;
  Volume = (TGeoVolume *)f->Get("BarrelEMC");
  gGeoManager->AddVolume(Volume);
  TGeoVoxelFinder *voxels = Volume->GetVoxels();
  if (voxels)
    voxels->SetNeedRebuild();

  // cave
  TGeoVolume *Cave = gGeoManager->GetTopVolume();
  TGeoNode *n = Volume->GetNode(0);
  TGeoMatrix *M = n->GetMatrix();
  M->SetDefaultName();
  gGeoManager->GetListOfMatrices()->Remove(M);
  TGeoHMatrix *global = gGeoManager->GetHMatrix();
  gGeoManager->GetListOfMatrices()->Remove(global); // Remove the Identity matrix

  // place the volume to the cave
  TGeoCombiTrans *TransRotMatrix = new TGeoCombiTrans(0., 0., 0., new TGeoRotation());
  Cave->AddNode(Volume, 0, TransRotMatrix);

  ExpandNode(Volume, Cave);
}

void PndEmc::ConstructRootGeomMod4()
{

  TFile *fb;
  if (!bwendcap) {
    std::cout << "File name Bw = " << GetGeometryFileName().Data() << std::endl;
    fb = new TFile(GetGeometryFileName().Data());
  } else {
    std::cout << "File name Bw1= " << fgeoName3 << std::endl;
    fb = new TFile(fgeoName3);
  }

  TGeoVolume *BwEmc = (TGeoVolume *)fb->Get("Emc4");
  TGeoVolume *Cave = gGeoManager->GetTopVolume();
  TGeoNode *n = BwEmc->GetNode(0);

  gGeoManager->AddVolume(BwEmc);
  TGeoVoxelFinder *voxels = BwEmc->GetVoxels();
  if (voxels)
    voxels->SetNeedRebuild();
  TGeoMatrix *M = n->GetMatrix();
  M->SetDefaultName();
  gGeoManager->GetListOfMatrices()->Remove(M);
  TGeoHMatrix *global = gGeoManager->GetHMatrix();
  gGeoManager->GetListOfMatrices()->Remove(global); // Remove the Identity matrix

  TGeoRotation rotBwEmc;
  rotBwEmc.RotateY(0.);

  // The first position of the BwEndCap crystals (center of the crystal!!) was -66 cm
  // Cave->AddNode(BwEmc,0, new TGeoCombiTrans(0., 0., -66.,new TGeoRotation(rotBwEmc)));

  // According to the last geometry integration of BwEndCap (22.04.09)
  // the position of the BwEndCap crystals (center of the crystal!!)
  // is -69.4 cm : -(3.+0.2+0.2+56.)-10 cm, which correspods to:
  //               -(insulation+carbon fiber+safety distance+front face of crystals)-half size of crystal
  //--------------------------------------------------------------------
  // 2017 geometry version: the distance from the interaction point to the
  // outer surface of the bw EMC is fixed to 560 mm (could change in the future)
  static const Double_t distTargetBWEC = 56.0; // cm
  // The position of the Node is calculated from the z dimension of the outer volume
  TGeoNode *outerVol = BwEmc->FindNode("BWECouterVol_0");
  if (outerVol != nullptr) {

    TGeoCompositeShape *shape = (TGeoCompositeShape *)outerVol->GetVolume()->GetShape();
    Double_t origin[3] = {0, 0, 0}, zdir[3] = {0, 0, 1};
    Double_t halfLength = shape->DistFromInside(origin, zdir);
    Double_t bwEmcZpos = -(distTargetBWEC + halfLength);
    printf("PndEmc::ConstructRootGeomMod4: halfLength = %e, fullLength = %e, z-pos = %e, distTargetBWEC = %e\n", halfLength, 2. * halfLength, bwEmcZpos, distTargetBWEC);
    Cave->AddNode(BwEmc, 0, new TGeoCombiTrans(0., 0., bwEmcZpos, new TGeoRotation(rotBwEmc)));

  } else { // for older geometry versions:
    Cave->AddNode(BwEmc, 0, new TGeoCombiTrans(0., 0., -69.4, new TGeoRotation(rotBwEmc)));
  }

  ExpandNode(BwEmc, Cave);
}

void PndEmc::ConstructRootGeomMod5()
{

  TFile *fb = nullptr;
  TString FileName = GetGeometryFileName().Data();
  if (FileName.EndsWith("5_fsc.root"))
    fb = new TFile(FileName);
  else if (fgeoName4.EndsWith("5_fsc.root")) {
    fb = new TFile(fgeoName4);
    FileName = fgeoName4;
  }

  std::cout << "File name Fsc= " << FileName << std::endl;

  TGeoVolume *Fsc = (TGeoVolume *)fb->Get("Emc5");
  TGeoVolume *Cave = gGeoManager->GetTopVolume();
  TGeoNode *n = Fsc->GetNode(0);
  if (fVerboseLevel > 2)
    cout << "=====PndEmc::ConstructRootGeomMod5()=====" << endl;
  if (fVerboseLevel > 2)
    Cave->Print();
  if (fVerboseLevel > 2)
    Cave->PrintNodes();

  if (fVerboseLevel > 2)
    Fsc->Print();
  if (fVerboseLevel > 2)
    Fsc->PrintNodes();

  gGeoManager->AddVolume(Fsc);
  TGeoVoxelFinder *voxels = Fsc->GetVoxels();
  if (voxels)
    voxels->SetNeedRebuild();
  TGeoMatrix *M = n->GetMatrix();
  M->SetDefaultName();
  gGeoManager->GetListOfMatrices()->Remove(M);
  TGeoHMatrix *global = gGeoManager->GetHMatrix();
  gGeoManager->GetListOfMatrices()->Remove(global); // Remove the Identity matrix

  TGeoRotation rotFsc;
  Cave->AddNode(Fsc, 0, new TGeoCombiTrans(0., 0., 818.775, new TGeoRotation(rotFsc)));

  ExpandNode(Fsc, Cave);
}

void PndEmc::ExpandNode(TGeoVolume *fVol, TGeoVolume *Cave)
{

  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  FairGeoMedia *Media = geoFace->getMedia();
  FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();

  TObjArray *nodeList = fVol->GetNodes();
  Int_t nodes = nodeList->GetEntries();

  for (Int_t nod = 0; nod < nodes; nod++) {

    TGeoNode *fNode = (TGeoNode *)nodeList->At(nod);
    TGeoVolume *v = fNode->GetVolume();

    if (fNode->GetNdaughters() > 0)
      ExpandNode(v, Cave);

    TGeoMedium *med1 = v->GetMedium();
    // if(fVerboseLevel>2) std::cout<< "DEBUG NodeName = " << fNode->GetName() << std::endl;
    if (med1) {
      // if(fVerboseLevel>2) std::cout<< "DEBUG medium  = " << med1->GetName() << std::endl;
      TGeoMaterial *mat1 = v->GetMaterial();
      TGeoMaterial *newMat = gGeoManager->GetMaterial(mat1->GetName());
      if (newMat == 0) {
        // std::cout<< "Material " << mat1->GetName() << " is not defined " << std::endl;
        FairGeoMedium *CbmMedium = Media->getMedium(mat1->GetName());
        if (!CbmMedium) {
          // std::cout << "Material is not defined in ASCII file nor in Root file" << std::endl;
          CbmMedium = new FairGeoMedium(mat1->GetName());
          Media->addMedium(CbmMedium);
        }
        // std::cout << "Create Medium " << mat1->GetName() << std::endl;
        Int_t nmed = geobuild->createMedium(CbmMedium);
        v->SetMedium(gGeoManager->GetMedium(nmed));
        gGeoManager->SetAllIndex();
      } else {
        // if(fVerboseLevel>2)
        // std::cout<< "DEBUG material was defined  MaterialName= " << mat1->GetName() << std::endl;
        TGeoMedium *med2 = gGeoManager->GetMedium(mat1->GetName());
        v->SetMedium(med2);
      }
    }

    TString name = v->GetName();

    if (!gGeoManager->FindVolumeFast(v->GetName())) {
      v->RegisterYourself();
      if (fVerboseLevel > 2)
        std::cout << "Volume ->  " << v->GetName() << " --> Registered to gGeoManager" << endl;
      if (name.Contains("Crystal") || name.Contains("FscSci") || name.Contains("FscFiber")) {
        AddSensitiveVolume(v);
        if (fVerboseLevel > 2)
          std::cout << "Volume " << v->GetName() << " is added to sensitives " << endl;
      }
    }
  }
}

void PndEmc::ConstructASCIIGeometry()
{
  // Definition of materials

  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  FairGeoMedia *Media = geoFace->getMedia();
  FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();

  FairGeoMedium *CbmMediumPb = Media->getMedium("lead");
  FairGeoMedium *CbmMediumPWO = Media->getMedium("PWO");
  FairGeoMedium *CbmMediumFsc = Media->getMedium("FscScint");

  // Int_t nmedPb= //[R.K.03/2017] unused variable
  geobuild->createMedium(CbmMediumPb);
  // Int_t nmedPWO= //[R.K.03/2017] unused variable
  geobuild->createMedium(CbmMediumPWO);
  // Int_t nmedFsc= //[R.K.03/2017] unused variable
  geobuild->createMedium(CbmMediumFsc);

  TGeoVolume *flayer1 = new TGeoVolumeAssembly("EmcLayer1");
  TGeoVolume *flayer2 = new TGeoVolumeAssembly("EmcLayer2");
  TGeoVolume *flayer2Hole = new TGeoVolumeAssembly("EmcLayer2Hole");
  TGeoVolume *flayer3 = new TGeoVolumeAssembly("Emc3");
  TGeoVolume *flayer4 = new TGeoVolumeAssembly("Emc4");
  TGeoVolume *flayer5 = new TGeoVolumeAssembly("Fsc");
  TGeoVolume *flayer6 = new TGeoVolumeAssembly("EmcTest");

  Bool_t bIsModuleOn[6] = {kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE};
  // Bool_t isFirst = kTRUE; //[R.K. 01/2017] unused variable?

  PndEmcReader read(GetGeometryFileName());
  for (Int_t module = read.GetMinModules(); module <= read.GetMaxModules(); module++) {
    cout << "Emc module = " << module;
    if (module == 5 && bIsFastFsc) {
      cout << "\t FAST" << endl << "******** " << endl;
      continue;
    }
    cout << endl << "******** " << endl;

    for (Int_t row = read.GetMinRows(module); row <= read.GetMaxRows(module); row++) {
      for (Int_t crystal = read.GetMinCrystals(module, row); crystal <= read.GetMaxCrystals(module, row); crystal++) {

        Text_t buffer[30];
        sprintf(buffer, "emc0%dr%dc%d", module, row, crystal);
        DataG4 data = read.GetData(module, row, crystal);

        if (data.module == -1)
          continue; // if the pad is not present, do not create geometry

        if ((module < 5) || (module == 6)) { // Construction of target spectrometer geometry
          TGeoTrap *trap = new TGeoTrap(data.pDz / 10., data.pTheta, data.pPhi, data.pDy1 / 10., data.pDx1 / 10., data.pDx2 / 10., data.pAlp1, data.pDy2 / 10., data.pDx3 / 10.,
                                        data.pDx4 / 10., data.pAlp2);
          TGeoVolume *volume;
          volume = new TGeoVolume(buffer, trap, gGeoManager->GetMedium("PWO"));

          volume->SetLineColor(5);
          TGeoRotation rot;
          rot.RotateZ(data.tau);
          rot.RotateY(data.theta);
          rot.RotateZ(data.phi);

          if (module == 1)
            flayer1->AddNode(
              volume, 0, new TGeoCombiTrans(data.posX / 10., data.posY / 10., data.posZ / 10. + 3.7, new TGeoRotation(rot))); // shift of 37 mm with respect to interaction point
          if (module == 2)
            flayer2->AddNode(
              volume, 0, new TGeoCombiTrans(data.posX / 10., data.posY / 10., data.posZ / 10. + 3.7, new TGeoRotation(rot))); // shift of 37 mm with respect to interaction point
          if ((module == 2) && !((crystal >= 4 && crystal <= 6) && (row <= 3)))
            flayer2Hole->AddNode(
              volume, 0, new TGeoCombiTrans(data.posX / 10., data.posY / 10., data.posZ / 10. + 3.7, new TGeoRotation(rot))); // shift of 37 mm with respect to interaction point
          if (module == 3)
            flayer3->AddNode(volume, 0, new TGeoCombiTrans(data.posX / 10., data.posY / 10., data.posZ / 10., new TGeoRotation(rot)));
          if (module == 4)
            flayer4->AddNode(volume, 0, new TGeoCombiTrans(data.posX / 10., data.posY / 10., data.posZ / 10., new TGeoRotation(rot)));
          if (module == 6)
            flayer6->AddNode(volume, 0, new TGeoCombiTrans(data.posX / 10., data.posY / 10., data.posZ / 10., new TGeoRotation(rot)));
          bIsModuleOn[module - 1] = kTRUE;
          AddSensitiveVolume(volume);
        }

        if (module == 5 && !bIsFastFsc) { // Construction of forward spectrometer geometry
          // For the forward calorimeter the box geometry was written giving different meaning to the DataG4 members:
          // Dx1 Dy1 : half-widths in XY of the absorber
          // Dx2 Dy2 : half-widths of XY of the scintillator
          // pAlp1 pAlp2 : half-withs on Z of absorber/scintillator
          // pDz : full-width of space in between absorber and scintillator
          // pDx3 : number of layers of Pb/Scint
          TGeoVolume *volAbs, *volSci, *volCrystal;
          TGeoRotation rot;
          //	    TGeoVolume *volume = new TGeoVolumeAssembly(buffer);
          Double_t BoxZ = 0.5 * data.pDx3 * (2. * data.pDz + 2. * data.pAlp1 + 2. * data.pAlp2) / 10.;
          TGeoBBox *volume = new TGeoBBox(data.pDx1 / 10., data.pDy1 / 10., BoxZ);
          volCrystal = new TGeoVolume(buffer, volume, gGeoManager->GetMedium("air"));
          volCrystal->SetLineColor(3);
          //  cout << "\t Check   " << buffer << "******** " << endl;

          TGeoBBox *absorber = new TGeoBBox(data.pDx1 / 10., data.pDy1 / 10., data.pAlp1 / 10.);
          TGeoBBox *scintillator = new TGeoBBox(data.pDx2 / 10., data.pDy2 / 10., data.pAlp2 / 10.);
          volAbs = new TGeoVolume("FscAbsorber", absorber, gGeoManager->GetMedium("lead"));
          volSci = new TGeoVolume("FscScintillator", scintillator, gGeoManager->GetMedium("FscScint"));
          volSci->SetLineColor(6);

          TGeoVolume *ffsclay = new TGeoVolumeAssembly("FscLayer");
          ffsclay->AddNode(volAbs, 0, new TGeoCombiTrans(0., 0., data.pAlp1 / 10., new TGeoRotation(rot)));
          ffsclay->AddNode(volSci, 0, new TGeoCombiTrans(0., 0., 2 * data.pAlp1 / 10 + data.pDz / 10. + data.pAlp2 / 10., new TGeoRotation(rot)));
          AddSensitiveVolume(volSci);

          for (Int_t ll = 0; ll < data.pDx3; ll++) {
            volCrystal->AddNode(ffsclay, ll, new TGeoCombiTrans(0., 0., ll * (2. * data.pDz + 2. * data.pAlp1 + 2. * data.pAlp2) / 10. - BoxZ, new TGeoRotation(rot)));
            // cout << " check " <<  volCrystal->GetName() << " has " <<  ffsclay->GetName() << "  At  " << ll*(2.*data.pDz+2.*data.pAlp1+2.*data.pAlp2)/10. << endl;
          }
          flayer5->AddNode(volCrystal, 0, new TGeoCombiTrans(data.posX / 10., data.posY / 10., data.posZ / 10. + BoxZ, new TGeoRotation(rot)));
          bIsModuleOn[module - 1] = kTRUE;
        }
      }
    }
  }

  // Fast construction of the fsc geometry
  if (bIsFastFsc) {
    TGeoVolume *volAbs1, *volSci1, *volAbs2, *volSci2, *volAbs3, *volSci3;
    TGeoRotation rot;
    TGeoVolume *volume = new TGeoVolumeAssembly("FscBox");
    Int_t padX = 28, padY = 14,
          // holeX = 4,  //[R.K. 01/2017] unused variable?
      holeY = 2, padX1 = 11, padX2 = 13;
    Float_t pDx1 = 11., pDx2 = 11., pDy1 = 11., pDy2 = 11., pAlp1 = 0.0275, pAlp2 = 0.15, pDz = 0.00375, posZ = 760.;

    TGeoBBox *absorber1 = new TGeoBBox(pDx1 * padX / 2., pDy1 * (padY - holeY) / 4., pAlp1 / 2.);
    TGeoBBox *scintillator1 = new TGeoBBox(pDx2 * padX / 2., pDy2 * (padY - holeY) / 4., pAlp2 / 2.);
    TGeoBBox *absorber2 = new TGeoBBox(pDx1 * padX1 / 2., pDy1 * holeY / 4., pAlp1 / 2.);
    TGeoBBox *scintillator2 = new TGeoBBox(pDx2 * padX1 / 2., pDy2 * holeY / 4., pAlp2 / 2.);
    TGeoBBox *absorber3 = new TGeoBBox(pDx1 * padX2 / 2., pDy1 * holeY / 4., pAlp1 / 2.);
    TGeoBBox *scintillator3 = new TGeoBBox(pDx2 * padX2 / 2., pDy2 * holeY / 4., pAlp2 / 2.);

    volAbs1 = new TGeoVolume("FscAbsorber1", absorber1, gGeoManager->GetMedium("lead"));
    volSci1 = new TGeoVolume("FscScintillator1", scintillator1, gGeoManager->GetMedium("FscScint"));
    volAbs2 = new TGeoVolume("FscAbsorber2", absorber2, gGeoManager->GetMedium("lead"));
    volSci2 = new TGeoVolume("FscScintillator2", scintillator2, gGeoManager->GetMedium("FscScint"));
    volAbs3 = new TGeoVolume("FscAbsorber3", absorber3, gGeoManager->GetMedium("lead"));
    volSci3 = new TGeoVolume("FscScintillator3", scintillator3, gGeoManager->GetMedium("FscScint"));

    // TGeoVolume *volSci1X  =  //[R.K.03/2017] unused variable
    gGeoManager->Division("FscScintillator1X", "FscScintillator1", 1, padX, -pDx2 * padX / 2., pDx2);
    TGeoVolume *volSci1XY = gGeoManager->Division("FscScintillator1XY", "FscScintillator1X", 2, (padY - holeY) / 2, -pDy2 * (padY - holeY) / 4., pDy2);
    // TGeoVolume *volSci2X  =  //[R.K.03/2017] unused variable
    gGeoManager->Division("FscScintillator2X", "FscScintillator2", 1, padX1, -pDx2 * padX1 / 2., pDx2);
    TGeoVolume *volSci2XY = gGeoManager->Division("FscScintillator2XY", "FscScintillator2X", 2, holeY / 2, -pDy2 * holeY / 4., pDy2);
    // TGeoVolume *volSci3X  =  //[R.K.03/2017] unused variable
    gGeoManager->Division("FscScintillator3X", "FscScintillator3", 1, padX2, -pDx2 * padX2 / 2., pDx2);
    TGeoVolume *volSci3XY = gGeoManager->Division("FscScintillator3XY", "FscScintillator3X", 2, holeY / 2, -pDy2 * holeY / 4., pDy2);

    TGeoVolume *ffsclay = new TGeoVolumeAssembly("FscLayer");
    ffsclay->AddNode(volAbs1, 0, new TGeoCombiTrans(0., pDy1 * (padY + holeY) / 4., pAlp1 / 2., new TGeoRotation(rot)));
    ffsclay->AddNode(volSci1, 0, new TGeoCombiTrans(0., pDy2 * (padY + holeY) / 4., pAlp1 + pDz + pAlp2 / 2., new TGeoRotation(rot)));
    ffsclay->AddNode(volAbs2, 0, new TGeoCombiTrans(pDx1 * (-padX + padX1) / 2., pDy1 * holeY / 4., pAlp1 / 2., new TGeoRotation(rot)));
    ffsclay->AddNode(volSci2, 0, new TGeoCombiTrans(pDx2 * (-padX + padX1) / 2., pDy2 * holeY / 4, pAlp1 + pDz + pAlp2 / 2., new TGeoRotation(rot)));
    ffsclay->AddNode(volAbs3, 0, new TGeoCombiTrans(pDx1 * (padX - padX2) / 2., pDy1 * holeY / 4., pAlp1 / 2., new TGeoRotation(rot)));
    ffsclay->AddNode(volSci3, 0, new TGeoCombiTrans(pDx2 * (padX - padX2) / 2., pDy2 * holeY / 4., pAlp1 + pDz + pAlp2 / 2., new TGeoRotation(rot)));

    AddSensitiveVolume(volSci1XY);
    AddSensitiveVolume(volSci2XY);
    AddSensitiveVolume(volSci3XY);

    for (Int_t ll = 1; ll <= 300; ll++) {
      volume->AddNode(ffsclay, ll, new TGeoCombiTrans(0., 0., ll * (2. * pDz + pAlp1 + pAlp2), new TGeoRotation(rot)));
    }

    flayer5->AddNode(volume, 0, new TGeoCombiTrans(0., 0., posZ, new TGeoRotation(rot)));
    bIsModuleOn[4] = kTRUE;
  }

  TGeoVolume *flayer12 = new TGeoVolumeAssembly("Emc12");
  TGeoVolume *flayer12Hole = new TGeoVolumeAssembly("Emc12Hole");
  if (bIsModuleOn[0])
    flayer12->AddNode(flayer1, 0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(0)));
  if (bIsModuleOn[1])
    flayer12->AddNode(flayer2, 0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(0)));
  if (bIsModuleOn[0])
    flayer12Hole->AddNode(flayer1, 0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(0)));
  if (bIsModuleOn[1])
    flayer12Hole->AddNode(flayer2Hole, 0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(0)));

  TString vname = "cave";
  vname = vname.Strip();
  TGeoVolume *vcave = gGeoManager->FindVolumeFast(vname.Data());
  // if (bIsModuleOn[0] || bIsModuleOn[1]) vcave->AddNode(flayer12, 1);
  if (bIsModuleOn[2])
    vcave->AddNode(flayer3, 1);
  if (bIsModuleOn[3])
    vcave->AddNode(flayer4, 1);
  if (bIsModuleOn[4])
    vcave->AddNode(flayer5, 1);
  if (bIsModuleOn[5])
    vcave->AddNode(flayer6, 1);

  // 15 copies for barrel part of EMC (1st copy exists in emc_module12345.dat)
  if (bIsModuleOn[0] || bIsModuleOn[1])
    for (Int_t n = 0; n <= 15; n++) {
      TGeoRotation rot1;
      rot1.RotateZ(22.5 * n);
      if (n == 0 || n == 8)
        vcave->AddNode(flayer12Hole, n + 1, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(rot1)));
      else
        vcave->AddNode(flayer12, n + 1, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(rot1)));
    }

  // 3 copies for forward endcap of EMC  (1st copy exists in emc_module12345.dat)
  if (bIsModuleOn[2])
    for (Int_t n = 1; n <= 3; n++) {
      TGeoCombiTrans reflection;
      if (n == 1)
        reflection.ReflectY(1);
      if (n == 2) {
        reflection.ReflectY(1);
        reflection.ReflectX(1);
      }
      if (n == 3)
        reflection.ReflectX(1);
      vcave->AddNode(flayer3, n + 1, new TGeoCombiTrans(reflection));
    }

  // 3 copies for backward endcap of EMC  (1st copy exists in emc_module12345.dat)
  if (bIsModuleOn[3])
    for (Int_t n = 1; n <= 3; n++) {
      TGeoCombiTrans reflection;
      if (n == 1)
        reflection.ReflectY(1);
      if (n == 2) {
        reflection.ReflectY(1);
        reflection.ReflectX(1);
      }
      if (n == 3)
        reflection.ReflectX(1);
      vcave->AddNode(flayer4, n + 1, new TGeoCombiTrans(reflection));
    }

  // 3 copies for forward calorimeter (1st copy exists in emc_module12345.dat)
  if (bIsModuleOn[4]) {
    if (bIsFastFsc) {
      TGeoCombiTrans reflection;
      reflection.ReflectY(1);
      vcave->AddNode(flayer5, 2, new TGeoCombiTrans(reflection));
    } else
      for (Int_t n = 1; n <= 3; n++) {
        TGeoCombiTrans reflection;
        if (n == 1)
          reflection.ReflectY(1);
        if (n == 2) {
          reflection.ReflectY(1);
          reflection.ReflectX(1);
        }
        if (n == 3)
          reflection.ReflectX(1);
        vcave->AddNode(flayer5, n + 1, new TGeoCombiTrans(reflection));
      }
  }
}

// -----   Private method AddHit   --------------------------------------------
PndEmcPoint *PndEmc::AddHit(Int_t trackID, Int_t detID, Int_t evtID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss, Short_t mod, Short_t row,
                            Short_t crys, Short_t copy, Bool_t entering, Bool_t exiting)
{
  TClonesArray &clref = *fEmcCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel > 1)
    cout << "-I- PndEmc: Adding Point at IN (" << pos.X() << ", " << pos.Y() << ", " << pos.Z() << ") cm, detector " << detID << ", evt " << evtID << ", track " << trackID
         << ", energy loss " << eLoss * 1e06 << " keV, module " << mod << " row " << row << " crystal " << crys << " copy " << copy << ", entering " << entering << ", exiting "
         << exiting << endl;

  PndEmcPoint *myPoint = new (clref[size]) PndEmcPoint(trackID, detID, evtID, pos, mom, time, length, eLoss, mod, row, crys, copy, entering, exiting);
  myPoint->SetLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), "MCTrack", trackID)); // 14.09.10 Stefano FIX
  return myPoint;
}

inline void PndEmc::ResetParameters()
{
  fTrackID = -999;
  fVolumeID = -999;
  fEventID = -999;
  fPos.SetXYZT(0., 0., 0., 0.);
  fMom.SetXYZT(0., 0., 0., 0.);
  fTime = -999;
  fLength = -999;
  fELoss = -999;
}

void PndEmc::SetSpecialPhysicsCuts()
{
  FairRun *fRun = FairRun::Instance();

  // check for GEANT3, else abort
  if (strcmp(fRun->GetName(), "TGeant3") == 0) {

    // get material ID for customs settings

    std::string sMedium[3] = {"FscScint", "FscFiber", "lead"};
    int matIdVMC;
    for (Int_t i = 0; i < 3; i++) {
      TGeoMedium *medium = gGeoManager->GetMedium(sMedium[i].c_str());
      if (medium == 0)
        continue;
      matIdVMC = medium->GetId();
      double cut_el = 1.0E-4;  // 100 KeV
      double cut_had = 1.0E-4; // 100 KeV
      // double tofmax = 1.E10;    // (s) //[R.K. 01/2017] unused variable?

      // Set new properties, physics cuts etc. for the FSC
      //		gMC->Gstpar(matIdVMC,"PAIR",1); /** pair production*/
      //		gMC->Gstpar(matIdVMC,"COMP",1); /**Compton scattering*/
      //		gMC->Gstpar(matIdVMC,"PHOT",1); /** photo electric effect */
      //		gMC->Gstpar(matIdVMC,"PFIS",0); /**photofission*/
      //		gMC->Gstpar(matIdVMC,"DRAY",1); /**delta-ray*/
      //		gMC->Gstpar(matIdVMC,"ANNI",1); /**annihilation*/
      //		gMC->Gstpar(matIdVMC,"BREM",1); /**bremsstrahlung*/
      //		gMC->Gstpar(matIdVMC,"HADR",1); /**hadronic process*/
      //		gMC->Gstpar(matIdVMC,"MUNU",1); /**muon nuclear interaction*/
      //		gMC->Gstpar(matIdVMC,"DCAY",1); /**decay*/
      //		gMC->Gstpar(matIdVMC,"LOSS",1); /**energy loss*/
      //		gMC->Gstpar(matIdVMC,"MULS",1); /**multiple scattering*/
      //		gMC->Gstpar(matIdVMC,"STRA",1); // collision sampling method to simulate energy loss in thin materials, particularly gases
      //		gMC->Gstpar(matIdVMC,"RAYL",1); // Rayleigh scattering

      gMC->Gstpar(matIdVMC, "CUTGAM", cut_el);  /** gammas (GeV)*/
      gMC->Gstpar(matIdVMC, "CUTELE", cut_el);  /** electrons (GeV)*/
      gMC->Gstpar(matIdVMC, "CUTNEU", cut_had); /** neutral hadrons (GeV)*/
      gMC->Gstpar(matIdVMC, "CUTHAD", cut_had); /** charged hadrons (GeV)*/
      gMC->Gstpar(matIdVMC, "CUTMUO", cut_el);  /** muons (GeV)*/
      gMC->Gstpar(matIdVMC, "BCUTE", cut_el);   /** electron bremsstrahlung (GeV)*/
      gMC->Gstpar(matIdVMC, "BCUTM", cut_el);   /** muon and hadron bremsstrahlung(GeV)*/
      gMC->Gstpar(matIdVMC, "DCUTE", cut_el);   /** delta-rays by electrons (GeV)*/
      gMC->Gstpar(matIdVMC, "DCUTM", cut_el);   /** delta-rays by muons (GeV)*/
      gMC->Gstpar(matIdVMC, "PPCUTM", cut_el);  /** direct pair production by muons (GeV)*/
    }
    gMC->SetMaxNStep((int)1E6);

    std::cout << "\n************************************************************\n"
              << "PndEmc::SetSpecialPhysicsCuts():\n"
              << "   using special physics cuts ...\n";
    std::cout << "************************************************************" << std::endl;
  }
}
// ----

ClassImp(PndEmc)
