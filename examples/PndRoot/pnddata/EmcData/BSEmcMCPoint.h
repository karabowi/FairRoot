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

// ------------------------------------------------------------------------
// -----                     BSEmcMCPoint header file                  -----
// -----               Created 14/08/06  by S.Spataro                 -----
// ------------------------------------------------------------------------
#ifndef BSEMCMCPOINT_HH
#define BSEMCMCPOINT_HH

#include <string> // for string

#include "Rtypes.h"     // for BSEmcMCPoint::Class, BSEmcMCPoint::Streamer
#include "RtypesCore.h" // for Int_t, kFALSE, Bool_t, Double_t, Option_t
#include "TVector3.h"   // for TVector3

#include "FairLink.h"    // for FairLink
#include "FairMCPoint.h" // for FairMCPoint

#include "BSEmcDataBranchNames.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief represents a mc hit in an emc crystal
 * @author S.Spataro
 * @ingroup EmcData
 */
class BSEmcMCPoint : public FairMCPoint {

 public:
  /** Default constructor **/
  BSEmcMCPoint();

  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param posIn    Ccoordinates at entrance to active volume [cm]
   *@param posOut   Coordinates at exit of active volume [cm]
   *@param momIn    Momentum of track at entrance [GeV]
   *@param momOut   Momentum of track at exit [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   *@param entering	Existing particle entering the crystal (needed for MC match)
   *@param exiting	Particle leaving the crystal (needed for MC match)
   **/
  BSEmcMCPoint(Int_t t_trackID, Int_t t_detID, Int_t t_evtID, const TVector3 &t_pos, const TVector3 &t_mom, Double_t t_tof, Double_t t_length, Double_t t_eLoss,
               Bool_t t_entering = kFALSE, Bool_t t_exiting = kFALSE);

  /** Copy constructor **/
  BSEmcMCPoint(const BSEmcMCPoint &t_point);

  /** Destructor **/
  virtual ~BSEmcMCPoint();

  Bool_t GetEntering() const { return fEntering; }
  Bool_t GetExiting() const { return fExiting; }
  /** Modifiers **/

  virtual void SetTrackID(Int_t t_trackId)
  {
    FairMCPoint::SetTrackID(t_trackId);
    SetLink(FairLink("MCTrack", t_trackId));
  } // 14.09.10 Stefano FIX

  /** Output to screen **/
  virtual void Print(const Option_t *t_opt) const;

  Bool_t operator<(const BSEmcMCPoint &t_rhs) const;

  Int_t GetDetectorId() const { return fDetectorID; }
  void SetDetectorId(Int_t t_detectorId) { fDetectorID = t_detectorId; }
  TVector3 GetPosition() const { return TVector3{GetX(), GetY(), GetZ()}; }

  Int_t GetClusterID() const { return fClusterID; }
  void SetClusterID(Int_t t_clusterId) { fClusterID = t_clusterId; }

 protected:
  Bool_t fEntering{kFALSE}; //< Is particle entering into crystal
  Bool_t fExiting{kFALSE};  //< Is particle exiting the crystal
  Int_t fClusterID{-1};
  ClassDef(BSEmcMCPoint, 1)
};

#endif /*BSEMCMCPOINT_HH*/
