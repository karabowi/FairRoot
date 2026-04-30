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

#include "PndGeanePro.h"
#include "PndPropagator.h"
#include "PndGeaneGeometryMethods.h"

#include "FairGeaneApplication.h"   // for FairGeaneApplication
#include "FairGeaneUtil.h"          // for FairGeaneUtil
#include "FairLogger.h"
#include "FairTrackPar.h"    // for FairTrackPar
#include "FairTrackParH.h"   // for FairTrackParH
#include "FairTrackParP.h"   // for FairTrackParP

#include <TDatabasePDG.h>   // for TDatabasePDG
#include <TGeoTorus.h>      // for TGeoTorus
#include <TMath.h>          // for pow, ACos, Cos, sqrt
#include <TMathBase.h>      // for Abs
#include <TVector3.h>       // for TVector3, operator-, etc
#include <TVirtualMC.h>     // for gMC
#include <cmath>            // IWYU pragma: keep for fabs
#include <iostream>         // for operator<<, basic_ostream, etc

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndGeanePro::PndGeanePro() : PndPropagator("Geane", "Propagate Tracks"),
  gMC3(static_cast<TGeant3*>(TVirtualMC::GetMC())), afErtrio(nullptr),
  fDBpdg(TDatabasePDG::Instance()),
  fPropOption(""), fNrep(1), fGeantCode(0), fProMode(0), fTrackTime(0.),
  fGeoMethods(PndGeaneGeometryMethods())
{
  if (gMC3 == NULL) {
    LOG(fatal) << "PndGeanePro::TGeant3 has not been initialized! ABORTING!";
    throw;
  }
  afErtrio = gMC3->fErtrio;

  fDestinationLengthArray[0] = 0.;

  for (Int_t i = 0; i < 5; i++)
    for (Int_t j = 0; j < 5; j++) {
      fTransportMatrix[i][j] = 0.;
    }

  for (Int_t i = 0; i < 15; i++) {
    fInErrorMatrix[i] = 0.;
    if (i < 3) {
      fFinalPositionArray[i] = 0;
      fFinalMomentumArray[i] = 0;
      fInitialPositionArray[i] = 0;
      fInitialMomentumArray[i] = 0;
    }
  }

  fInitialPlane[0] = 1.;
  fInitialPlane[1] = 0.;
  fInitialPlane[2] = 0.;
  fInitialPlane[3] = 0.;
  fInitialPlane[4] = 1.;
  fInitialPlane[5] = 0.;

  fFinalPlane[0] = 0.;
  fFinalPlane[1] = 0.;
  fFinalPlane[2] = 0.;
  fFinalPlane[3] = 1.;
  fFinalPlane[4] = 0.;
  fFinalPlane[5] = 0.;
  fFinalPlane[6] = 0.;
  fFinalPlane[7] = 1.;
  fFinalPlane[8] = 0.;
  fFinalPlane[9] = 0.;
  fFinalPlane[10] = 0.;
  fFinalPlane[11] = 1.;
}

PndGeanePro::~PndGeanePro() {}

Bool_t PndGeanePro::Propagate(FairTrackPar* TStart, FairTrackPar* TEnd, Int_t PDG)
{
  Int_t ch = 1; // charge of particle
  Double_t fCov[15], fCovOut[15];

  if (dynamic_cast<FairTrackParH*>(TStart)) { // TODO: Check if ClassName() is faster than using dynamic_cast
    dynamic_cast<FairTrackParH*>(TStart)->GetCovQ(fCov);
  } else if (dynamic_cast<FairTrackParP*>(TStart)) {
    dynamic_cast<FairTrackParP*>(TStart)->GetCovQ(fCov);
  } else {
    LOG(error) << "Need input of type FairTrackParH* or FairTrackParP*!";
    return kFALSE;
  }

  Init(TStart);
  Double_t Q = TStart->GetQ();
  if (fabs(Q) > 1.E-8) {
    ch = Int_t(Q / TMath::Abs(Q));
  }

  // TODO: unify fProMode and fPropOption
  if (fProMode == 1) {   // Propagate to Volume
    if (dynamic_cast<FairTrackParP*>(TStart)) {
      LOG(warning) << "Propagate Parabola parameter to Volume is not implimented yet";
      return kFALSE;
    }
    //***** We have the right representation go further
    for (Int_t i = 0; i < 15; i++) {
      fInErrorMatrix[i] = fCov[i];
    }
    if (fPropOption.Contains("V")) {
      // LOG(debug) << "Propagate Helix to Volume";
      Int_t option;
      if (fVolumeEnter) option = 1;  
      else option = 2;
      gMC3->Eufilv(1, fInErrorMatrix, const_cast<char*>(fVolumeName.Data()), &fVolumeCopyNo, &option);
    } else if (fPropOption.Contains("L")) {
      if (fPcaMode == 0) {
        gMC3->Eufill(fNrep, fInErrorMatrix, fDestinationLengthArray);
      } else if (fPcaMode != 0) {
        // max length estimate:
        // we calculate the geometrical distance of the start point
        // from the point/wire extremity and multiply it * 2
        TVector3 start = TVector3(TStart->GetX(), TStart->GetY(), TStart->GetZ());
        Double_t maxdistance = 0;
        if (fPcaMode == 1) {
          maxdistance = (fPoint - start).Mag();
        } else if (fPcaMode == 2) {
          Double_t distance1, distance2;
          distance1 = (fWire1 - start).Mag();
          distance2 = (fWire2 - start).Mag();
          if (distance1 < distance2) {
            maxdistance = distance2;
          } else {
            maxdistance = distance1;
          }
        }
        maxdistance *= 2.;

        // output
        PndProp::PCAOutputStruct pcaoutput = FindPCA(fPcaMode, PDG, fPoint, fWire1, fWire2, maxdistance);
        if (pcaoutput.PCAStatusFlag != 0) return kFALSE;
        fPcaOutput = pcaoutput;

        // reset parameters
        Init(TStart);
        Float_t tracklength = fPcaOutput.TrackLength;
        gMC3->Eufill(fNrep, fInErrorMatrix, &tracklength);
      }
    }
  } else if (fProMode == 3) {
    if (dynamic_cast<FairTrackParH*>(TStart)) {
      LOG(warning) << "Propagate Helix parameter to Plane is not implimented yet";
      return kFALSE;
    }
    /** We have the right representation go further*/
    for (Int_t i = 0; i < 15; i++) {
      fInErrorMatrix[i] = fCov[i];
    }

    if (fPropOption.Contains("P")) {
      gMC3->Eufilp(fNrep, fInErrorMatrix, fInitialPlane, fFinalPlane);
    } else if (fPropOption.Contains("L")) {
      if (fPcaMode == 0) {
        LOG(warning) << "Propagate Parabola to Parabola in Length not yet implemented";
      } else if (fPcaMode != 0) {
        // max length estimate:
        // we calculate the geometrical distance of the start point
        // from the point/wire extremity and multiply it * 2
        TVector3 start = TVector3(TStart->GetX(), TStart->GetY(), TStart->GetZ());
        Double_t maxdistance = 0;
        if (fPcaMode == 1) maxdistance = (fPoint - start).Mag();
        else if (fPcaMode == 2) {
          Double_t distance1, distance2;
          distance1 = (fWire1 - start).Mag();
          distance2 = (fWire2 - start).Mag();
          if (distance1 < distance2) maxdistance = distance2;
          else maxdistance = distance1;  
        }
        maxdistance *= 2.;

        // output
        PndProp::PCAOutputStruct pcaoutput = FindPCA(fPcaMode, PDG, fPoint, fWire1, fWire2, maxdistance);
        if (pcaoutput.PCAStatusFlag != 0) {
          return kFALSE;
        }
        fPcaOutput = pcaoutput;
        // reset parameters
        Init(TStart);

        // find plane
        // unitary vector along distance
        TVector3 fromwiretoextr = pcaoutput.OnTrackPCA - pcaoutput.OnWirePCA;
        fromwiretoextr.SetMag(1.);
        if (fabs(fromwiretoextr.Mag() - 1) > 1E-4) {
          LOG(fatal) << "fromwire.Mag()!=1";
          return kFALSE;
        }

        // for wires:
        // unitary vector along the wire
        TVector3 wiredirection = fWire2 - fWire1;
        if (fPcaMode == 1) {   // point
          TVector3 mom(TStart->GetPx(), TStart->GetPy(), TStart->GetPz());
          wiredirection = mom.Cross(fromwiretoextr);
        }
        wiredirection.SetMag(1.);
        // check orthogonality
        if (fabs(fromwiretoextr * wiredirection) > 1e-3) {
            return kFALSE;  // throw away the event
                            // wiredirection = (fromwiretoextr.Cross(wiredirection)).Cross(fromwiretoextr);
                            // wiredirection.SetMag(1.);
        }

        TVector3 jver = dynamic_cast<FairTrackParP*>(TStart)->GetJVer();
        TVector3 kver = dynamic_cast<FairTrackParP*>(TStart)->GetKVer();
        Bool_t backtracking = kFALSE;
        if (fPropOption.Contains("B")) backtracking = kTRUE;
        SetOriginPlane(jver, kver);
        SetDestinationPlane(fPcaOutput.OnWirePCA, fromwiretoextr, wiredirection);
        if (backtracking == kTRUE) fPropOption = "BPE";

        gMC3->Eufilp(fNrep, fInErrorMatrix, fInitialPlane, fFinalPlane);
      }
    }
  }
  // Propagate
  if (Propagate(PDG) == kFALSE) return kFALSE;

  for (Int_t i = 0; i < 15; i++) {
    fCovOut[i] = afErtrio->errout[i];
    if (i == 0) fCovOut[i] = fCovOut[i] * ch * ch;
    if (i > 0 && i < 5) fCovOut[i] = fCovOut[i] * ch;
  }

  // do not remove (useful for debug)
  if (fabs(fFinalMomentumArray[0]) < 1e-9 && fabs(fFinalMomentumArray[1]) < 1e-9 && fabs(fFinalMomentumArray[2]) < 1e-9) {
    return kFALSE;
  }

  if (dynamic_cast<FairTrackParH*>(TEnd)) {
    dynamic_cast<FairTrackParH*>(TEnd)->SetTrackPar(fFinalPositionArray[0], fFinalPositionArray[1], fFinalPositionArray[2], fFinalMomentumArray[0], fFinalMomentumArray[1], fFinalMomentumArray[2], ch, fCovOut);
  }
  else if (dynamic_cast<FairTrackParH*>(TEnd)) {
    TVector3 origin(fFinalPlane[6], fFinalPlane[7], fFinalPlane[8]);
    TVector3 dj(fFinalPlane[0], fFinalPlane[1], fFinalPlane[2]);
    TVector3 dk(fFinalPlane[3], fFinalPlane[4], fFinalPlane[5]);
    TVector3 di(fFinalPlane[9], fFinalPlane[10], fFinalPlane[11]);
    dynamic_cast<FairTrackParP*>(TEnd)->SetTrackPar(fFinalPositionArray[0], fFinalPositionArray[1], fFinalPositionArray[2], fFinalMomentumArray[0], fFinalMomentumArray[1], fFinalMomentumArray[2], ch, fCovOut, origin, di, dj, dk);
  }
  return kTRUE;
}

Bool_t PndGeanePro::Propagate(Float_t* X1, Float_t* P1, Float_t* X2, Float_t* P2, Int_t PDG)
{
  fGeantCode = fDBpdg->ConvertPdgToGeant3(PDG);
  fDestinationLengthArray[0] = 1000;
  gMC3->Eufill(1, fInErrorMatrix, fDestinationLengthArray);
  gMC3->Ertrak(X1, P1, X2, P2, fGeantCode, "L");
  if (X2[0] < -1.E29) return kFALSE;
  if (gMC3->IsTrackOut()) return kFALSE;
  return kTRUE;
}

Bool_t PndGeanePro::Propagate(Int_t PDG)
{
  // main propagate call to fortran ERTRAK
  fGeantCode = fDBpdg->ConvertPdgToGeant3(PDG);
  
  //cout <<endl<<"inipos:"; for (int i=0;i<3;++i) cout <<fInitialPositionArray[i]<<" ";
  //cout <<endl<<"inimom:"; for (int i=0;i<3;++i) cout <<fInitialMomentumArray[i]<<" ";
  //cout <<endl<<"finpos:"; for (int i=0;i<3;++i) cout <<fFinalPositionArray[i]<<" ";
  //cout <<endl<<"finmom:"; for (int i=0;i<3;++i) cout <<fFinalMomentumArray[i]<<" ";
  //cout <<endl;
  
  // check initial parameters
  if (!CheckIniParOk()) {
    LOG(warning) << "PndGeanePro::FindPCA -- initial pos/mom contains a NaN";
    return kFALSE;
  }
  
  gMC3->Ertrak(fInitialPositionArray, fInitialMomentumArray, fFinalPositionArray, fFinalMomentumArray, fGeantCode, fPropOption.Data());
  if (fFinalPositionArray[0] < -1.E29) return kFALSE;
  if (gMC3->IsTrackOut()) return kFALSE;

  fPcaOutput.TrackLength = gMC3->TrackLength();
  fTrackTime = gMC3->TrackTime();

  Double_t trasp[25];
  for (Int_t i = 0; i < 25; i++) {
    trasp[i] = afErtrio->erdtrp[i];
  }
  FairGeaneUtil fUtil;
  fUtil.FromVecToMat(fTransportMatrix, trasp);
  return kTRUE;
}

void PndGeanePro::Init(FairTrackPar* TParam)
{
  // starting and ending point initialization
  fInitialPositionArray[0] = TParam->GetX();
  fInitialPositionArray[1] = TParam->GetY();
  fInitialPositionArray[2] = TParam->GetZ();
  fInitialMomentumArray[0] = TParam->GetPx();
  fInitialMomentumArray[1] = TParam->GetPy();
  fInitialMomentumArray[2] = TParam->GetPz();

  fFinalPositionArray[0] = 0;
  fFinalPositionArray[1] = 0;
  fFinalPositionArray[2] = 0;
  fFinalMomentumArray[0] = 0;
  fFinalMomentumArray[1] = 0;
  fFinalMomentumArray[2] = 0;
}

Bool_t PndGeanePro::SetOriginPlane(const TVector3& v1, const TVector3& v2)
{
  // define initial plane (option "P")
  TVector3 v1u = v1.Unit();
  TVector3 v2u = v2.Unit();
  fInitialPlane[0] = v1u.X();
  fInitialPlane[1] = v1u.Y();
  fInitialPlane[2] = v1u.Z();
  fInitialPlane[3] = v2u.X();
  fInitialPlane[4] = v2u.Y();
  fInitialPlane[5] = v2u.Z();
  return kTRUE;
}

Bool_t PndGeanePro::SetDestinationPlane(const TVector3& v0, const TVector3& v1, const TVector3& v2)
{
  // define final plane (option "P")
  // uncomment to set the initial error to zero
  //  for(Int_t i=0;i<15;i++) fInErrorMatrix[i]=0.00;
  TVector3 v1u = v1.Unit();
  TVector3 v2u = v2.Unit();

  fFinalPlane[0] = v1u.X();
  fFinalPlane[1] = v1u.Y();
  fFinalPlane[2] = v1u.Z();
  fFinalPlane[3] = v2u.X();
  fFinalPlane[4] = v2u.Y();
  fFinalPlane[5] = v2u.Z();

  fFinalPlane[6] = v0.X();
  fFinalPlane[7] = v0.Y();
  fFinalPlane[8] = v0.Z();

  TVector3 v3 = v1u.Cross(v2u);

  fFinalPlane[9] = v3(0);
  fFinalPlane[10] = v3(1);
  fFinalPlane[11] = v3(2);

  fPropOption = "PE";
  fProMode = 3;   // need errors in representation 3 (SD)(see Geane doc)

  return kTRUE;
}

Bool_t PndGeanePro::SetDestinationVolume(std::string VolName, Int_t CopyNo, Int_t option)
{
  // define final volume (option "V")
  for (Int_t i = 0; i < 15; i++) {
    fInErrorMatrix[i] = 0.00;
  }
  fVolumeName = VolName;
  fVolumeCopyNo = CopyNo;
  if (option == 1) fVolumeEnter = kTRUE;
  else fVolumeEnter = kFALSE;
  fPropOption = "VE";
  fProMode = 1;   // need errors in representation 1 (SC) (see Geane doc)
  return kTRUE;
}

Bool_t PndGeanePro::SetDestinationLength(Float_t length)
{
  // define final length (option "L")
  if (length < 0) {
    fDestinationLengthArray[0] = -length;
    fPropOption = "BLE";
  } else {
    fDestinationLengthArray[0] = length;
    fPropOption = "LE";
  }
  fProMode = 1;   // need errors in representation 1 (SC)(see Geane doc)
  return kTRUE;
}

Bool_t PndGeanePro::SetDestinationPoint(TVector3 point)
{
  fPropOption = "LE";
  fProMode = 1;
  fPoint = point;

  return kTRUE;
}

Bool_t PndGeanePro::SetDestinationWire(TVector3 wire1, TVector3 wire2)
{
  fPropOption = "LE";
  fProMode = 1;
  fWire1 = wire1;
  fWire2 = wire2;

  return kTRUE;
}

Bool_t PndGeanePro::SetPropagateOnlyParameters()
{
  Int_t index = fPropOption.Index("E");
  if (index == -1) {
    fPropOption.Append("O");
  } else {
    fPropOption.Replace(index, 1, "O");
  }
  return kTRUE;
}

Bool_t PndGeanePro::SetPCAPropagation(Int_t pca, Int_t dir, FairTrackPar* par)
{
  if (par) {
    Init(par);
    for (Int_t i = 0; i < 15; i++) {
      fInErrorMatrix[i] = 0.00;
    }
  }
  // through track length
  if (dir > 0) fPropOption = "LE";
  else if (dir < 0) fPropOption = "BLE";
  else LOG(warning) << "PndGeanePro::SetPCAPropagation ERROR: no direction set";
  
  fProMode = 1;   // need errors in representation 1 (SC)(see Geane doc)
  fPcaMode = pca;
  // initialization
  fTrackTime = 0;
  return kTRUE;
}

//=====================

// check intial parameters for a NaN element
Bool_t PndGeanePro::CheckIniParOk()
{
  for (int i=0;i<3;++i) 
    if (isnan(fInitialPositionArray[i]) || isnan(fInitialMomentumArray[i]))
      return false;
      
  return true;
}

//=====================

PndProp::PCAOutputStruct PndGeanePro::FindPCA(Int_t pca,
                                              Int_t PDGCode,
                                              TVector3 point,
                                              TVector3 wire1,
                                              TVector3 wire2,
                                              Double_t maxdistance)
{  
  PndProp::PCAOutputStruct pcastruct = PndProp::PCAOutputStruct();

  // check initial parameters
  if (!CheckIniParOk()) {
    LOG(warning) << "PndGeanePro::FindPCA -- initial pos/mom contains a NaN";
    return pcastruct;
  }

  Float_t pf[3] = {static_cast<Float_t>(point.X()), static_cast<Float_t>(point.Y()), static_cast<Float_t>(point.Z())};
  Float_t w1[3] = {static_cast<Float_t>(wire1.X()), static_cast<Float_t>(wire1.Y()), static_cast<Float_t>(wire1.Z())};
  Float_t w2[3] = {static_cast<Float_t>(wire2.X()), static_cast<Float_t>(wire2.Y()), static_cast<Float_t>(wire2.Z())};

  fGeantCode = fDBpdg->ConvertPdgToGeant3(PDGCode);

  // flags Rotondi's function
  Int_t flg = 0;

  // cl track length to the three last points of closest approach
  // dst assigned distance between initial point in ERTRAK and PFINAL along straight line (currently noy used)
  Float_t cl[3], dst;

  // GEANE filled points
  Float_t po1[3], po2[3], po3[3];

  // cl track length to the three last points of closest approach
  Float_t clen[3];

  // track length to add to GEANE computed one
  Double_t Le = 0.0;
  Double_t dist1, dist2;

  // initialization of some variables
  dst = 999.;
  cl[0] = 0;
  cl[1] = 0;
  cl[2] = 0;

  // GEANE filled points
  po1[0] = 0;
  po1[1] = 0;
  po1[2] = 0;
  po2[0] = 0;
  po2[1] = 0;
  po2[2] = 0;
  po3[0] = 0;
  po3[1] = 0;
  po3[2] = 0;

  gMC3->SetClose(pca, pf, dst, w1, w2, po1, po2, po3, cl);

  // maximum distance calculated 2 * geometric distance
  // start point - end point (the point to which we want
  // to find the pca)
  Float_t stdlength[1] = {static_cast<Float_t>(maxdistance)};

  gMC3->Eufill(1, fInErrorMatrix, stdlength);

  // check needed for low momentum tracks
  gMC3->Ertrak(fInitialPositionArray, fInitialMomentumArray, fFinalPositionArray, fFinalMomentumArray, fGeantCode, fPropOption.Data());
  if (fFinalPositionArray[0] < -1.E29) return pcastruct;
  if (gMC3->IsTrackOut()) return pcastruct;

  gMC3->GetClose(po1, po2, po3, clen);

  // check on cases when only two steps are performed!
  // in these cases po1[i] = 0 ==> let' s copy po2 into
  // po1 in order to use only the two actually extrapolated
  // points po2 and po3 to complete the pca calculation
  if (clen[0] == 0 && clen[1] == 0) {
    po1[0] = po2[0];
    po1[1] = po2[1];
    po1[2] = po2[2];
  }
  
  if (pca == 1) {
    if ((po1[0] == po2[0] && po1[1] == po2[1] && po1[2] == po2[2])
        || (po2[0] == po3[0] && po2[1] == po3[1] && po2[2] == po3[2])) {
      pcastruct = fGeoMethods.Track2ToPoint(TVector3(po1), TVector3(po3), TVector3(pf));
      Le = pcastruct.TrackLength;
      if (pcastruct.PCAStatusFlag != 0) {
        LOG(warn) << "PndGeanePro:FindPCA: Track2ToPoint quitFlag " << pcastruct.PCAStatusFlag << " ABORT";
        return pcastruct;
      }
    } // endif po1==po2 || po2==po3
    else {
      pcastruct = fGeoMethods.Track3ToPoint(TVector3(po1), TVector3(po2), TVector3(po3), TVector3(pf));
      Le = pcastruct.TrackLength;
      flg = pcastruct.PCAStatusFlag;
      if (flg == 1) {
        pcastruct = fGeoMethods.Track2ToPoint(TVector3(po1), TVector3(po3), TVector3(pf));
        Le = pcastruct.TrackLength;
        if (pcastruct.PCAStatusFlag != 0) {
          LOG(warn) << "PndGeanePro:FindPCA: Track2ToPoint quitFlag " << pcastruct.PCAStatusFlag << " ABORT";
          return pcastruct;
        }
      } // endif flg==1
      else if (flg == 2) {
        LOG(warn) << "PndGeanePro:FindPCA: Track3ToPoint flg " << flg << " ABORT";
        return pcastruct;
      } // endif flg==2
    } // end else po1==po2 || po2==po3

    // if the propagation to closest approach to a POINT  is performed
    // vwi is the point itself (with respect to which the pca is calculated)
    pcastruct.OnWirePCA = point;
  } // endif pca==1
  else if (pca == 2) {
    if ((po1[0] == po2[0] && po1[1] == po2[1] && po1[2] == po2[2])
        || (po2[0] == po3[0] && po2[1] == po3[1] && po2[2] == po3[2]))
    {
      pcastruct = fGeoMethods.Track2ToLine(TVector3(po1), TVector3(po3), TVector3(w1), TVector3(w2));
        Le = pcastruct.TrackLength;
        flg = pcastruct.PCAStatusFlag;
        if (flg == 1) {
          dist1 = (pcastruct.OnWirePCA - TVector3(w1)).Mag();
          dist2 = (pcastruct.OnWirePCA - TVector3(w2)).Mag();

          if (dist1 < dist2) pcastruct = fGeoMethods.Track2ToPoint(TVector3(po1), TVector3(po3), TVector3(w1));
          else pcastruct = fGeoMethods.Track2ToPoint(TVector3(po1), TVector3(po3), TVector3(w2));
          
          Le = pcastruct.TrackLength;
          if (pcastruct.PCAStatusFlag != 0) {
            LOG(warn) << "PndGeanePro:FindPCA: Track2ToPoint quitFlag " << pcastruct.PCAStatusFlag << " ABORT";
            return pcastruct;
          }
        } // endif flg==1
        else if (flg == 2) {
          LOG(warn) << "PndGeanePro:FindPCA: Track2ToLine flg " << flg << " ABORT";
          return pcastruct;
        } // endif flg==2
    } // endif po1==po2 || po2==po3
    else {
      pcastruct = fGeoMethods.Track3ToLine(TVector3(po1), TVector3(po2), TVector3(po3), TVector3(w1), TVector3(w2));
      Le = pcastruct.TrackLength;
      flg = pcastruct.PCAStatusFlag;
      if (flg == 1) {
        pcastruct = fGeoMethods.Track2ToLine(TVector3(po1), TVector3(po3), TVector3(w1), TVector3(w2));
        Le = pcastruct.TrackLength;
        flg = pcastruct.PCAStatusFlag;
        if (flg == 1) {
          dist1 = (pcastruct.OnWirePCA - TVector3(w1)).Mag();
          dist2 = (pcastruct.OnWirePCA - TVector3(w2)).Mag();
          
          if (dist1 < dist2) pcastruct = fGeoMethods.Track2ToPoint(TVector3(po1), TVector3(po3), TVector3(w1));
          else pcastruct = fGeoMethods.Track2ToPoint(TVector3(po1), TVector3(po3), TVector3(w2));
          Le = pcastruct.TrackLength;
          if (pcastruct.PCAStatusFlag != 0) {
            LOG(warn) << "PndGeanePro:FindPCA: Track2ToPoint quitFlag " << pcastruct.PCAStatusFlag << " ABORT";
            return pcastruct;
          }
        } // endif flg==1
        else if (flg == 2) {
          LOG(warn) << "PndGeanePro:FindPCA: Track2ToLine flg " << flg << " ABORT";
          return pcastruct;
        } // endif flg==2
      } // endif flg==1
      else if (flg == 2) {
        dist1 = (pcastruct.OnWirePCA - TVector3(w1)).Mag();
        dist2 = (pcastruct.OnWirePCA - TVector3(w2)).Mag();

        if (dist1 < dist2) pcastruct = fGeoMethods.Track3ToPoint(TVector3(po1),
                                                                 TVector3(po2),
                                                                 TVector3(po3),
                                                                 TVector3(w1));
        else pcastruct = fGeoMethods.Track3ToPoint(TVector3(po1),
                                                   TVector3(po2),
                                                   TVector3(po3),
                                                   TVector3(w2));
        Le = pcastruct.TrackLength;
        flg = pcastruct.PCAStatusFlag;
        if (flg == 2) {
          LOG(warn) << "PndGeanePro:FindPCA: Track3ToLine flg " << flg << " ABORT";
          return pcastruct;
        }
      } // endif flg==2
      else if (flg == 3) {
        dist1 = (pcastruct.OnWirePCA - TVector3(w1)).Mag();
        dist2 = (pcastruct.OnWirePCA - TVector3(w2)).Mag();

        if (dist1 < dist2) pcastruct = fGeoMethods.Track2ToPoint(TVector3(po1), TVector3(po3), TVector3(w1));
        else pcastruct = fGeoMethods.Track2ToPoint(TVector3(po1), TVector3(po3), TVector3(w2));
        Le = pcastruct.TrackLength;
        if (pcastruct.PCAStatusFlag != 0) {
          LOG(warn) << "PndGeanePro:FindPCA: Track2ToPoint quitFlag " << pcastruct.PCAStatusFlag << " ABORT";
          return pcastruct;
        }
      } // endif flg==3
      else if (flg == 4) {
        pcastruct = fGeoMethods.Track2ToLine(TVector3(po1), TVector3(po3), TVector3(w1), TVector3(w2));
        Le = pcastruct.TrackLength;
        flg = pcastruct.PCAStatusFlag;
        if (flg == 2) {
          LOG(warn) << "PndGeanePro:FindPCA: Track2ToLine flg " << flg << " ABORT";
          return pcastruct;
        }
      } // endif flg==4
    }
  }

  // calculated track length corresponding
  // to the point of closest approach
  pcastruct.TrackLength = clen[0] + Le;

  // pca before starting point
  if (pcastruct.TrackLength < 0) {
    return pcastruct;
  }

  pcastruct.PCAStatusFlag = 0;
  return pcastruct;
}

ClassImp(PndGeanePro);
