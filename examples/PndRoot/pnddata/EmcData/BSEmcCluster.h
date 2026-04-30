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

//----------------------------------------------------------------------
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Steve Playfer           University of Edinburgh
//	Stephen Gowdy           University of Edinburgh
//
//-----------------------------------------------------------------------

#ifndef BSEMCCLUSTER_HH
#define BSEMCCLUSTER_HH

#include <string> // for string
#include <vector>

#include "Rtypes.h"     // for BSEmcCluster::Class, BSEmcCluster::...
#include "RtypesCore.h" // for Double_t, Int_t, Bool_t, kFALSE
#include "TVector3.h"   // for TVector3

#include "FairMultiLinkedData.h" // for FairMultiLinkedData
#include "FairTimeStamp.h"       // for FairTimeStamp

#include "BSEmcDataBranchNames.h"

class BSEmcDigi; // lines 24-24
class FairLink;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief simple Struct used in Cluster classes to index crystal. Includes the weight of the crystal on the current cluster
 * Used instead of EmcSharedDigi
 * @author Ben Salisbury
 * @ingroup EmcData
 */
struct BSEmcDigiInfo_t {
  Int_t fDigiIdx;
  Int_t fDetectorId;
  Double_t fWeight;
  BSEmcDigiInfo_t(Int_t t_digiIdx = -1, Int_t t_detId = 0, Double_t t_weight = 1.0) : fDigiIdx(t_digiIdx), fDetectorId(t_detId), fWeight(t_weight) {}
  Bool_t operator==(const BSEmcDigiInfo_t &t_other) { return fDigiIdx == t_other.fDigiIdx; }
};

/**
 * @brief a cluster (group of neighboring crystals) of hit emc crystals
 * @author Xiaorong Shi            Lawrence Livermore National Lab
 * @author Steve Playfer           University of Edinburgh
 * @author Stephen Gowdy           University of Edinburgh
 * @ingroup EmcData
 */
class BSEmcCluster : public FairTimeStamp {

 public:
  // Constructors
  BSEmcCluster();
  BSEmcCluster(const BSEmcCluster &t_other);

  virtual void Print(const Option_t *t_opt = "") const;
  // Destructor
  virtual ~BSEmcCluster();

  Double_t GetEnergy() const;
  TVector3 GetPosition() const;
  Int_t GetNumberOfDigis() const { return fDigis.size(); }
  Int_t NumberOfDigis() const;

  Int_t GetNumberOfSubCluster() const { return fNSubClusters; }

  // Modifiers
  void SetNSubClusters(UInt_t nSubClusters);
  void SetEnergy(Double_t t_energy)
  {
    fEnergy = t_energy;
    fEnergyValid = kTRUE;
  }
  void SetPosition(const TVector3 &t_pos)
  {
    fWhere = t_pos;
    fWhereValid = kTRUE;
  }
  void SetTrackEntering(const FairMultiLinkedData &t_tracks) { fTrackEntering = t_tracks; }
  void SetTrackExiting(const FairMultiLinkedData &t_tracks) { fTrackExiting = t_tracks; }

  std::vector<Int_t> GetMcList(Int_t t_branchId) const;
  FairMultiLinkedData GetTrackEntering() const { return fTrackEntering; }
  FairMultiLinkedData GetTrackExiting() const { return fTrackExiting; }

  const std::vector<BSEmcDigiInfo_t> &GetDigis() const { return fDigis; }
  const std::vector<BSEmcDigiInfo_t> &GetMaximaDigis() const { return fMaxima; }

  const BSEmcDigi *GetMaximumDigi(const std::vector<const BSEmcDigi *> &t_digis) const;
  Double_t GetMaximumEnergy(const std::vector<const BSEmcDigi *> &t_digis) const;
  BSEmcDigiInfo_t GetMaximumDigiInfo(const std::vector<const BSEmcDigi *> &t_digis) const;
  Double_t CalculateTimeStamp(const std::vector<const BSEmcDigi *> &t_digis) const;

  void AddDigi(const BSEmcDigiInfo_t &t_digiInfo);
  void AddDigi(Int_t t_arrayindex, Int_t t_detectorId, Double_t t_weight = 1);
  void AddDigiLink(FairLink t_entrynr);
  void RemoveDigi(Int_t t_arrayindex);
  void AddCluster(const BSEmcCluster &t_cluster);
  void AddMaximum(const BSEmcDigiInfo_t &t_maximum);
  void AddMaximum(Int_t t_arrayindex, Int_t t_detectorid);
  void RemoveMaximum(Int_t t_arrayindex);
  void Clear()
  {
    fDigis.clear();
    fMaxima.clear();
  }

  void SetRadius(Double_t t_clusradius) { fRadius = t_clusradius; }
  // Add single crystal radius to precluster radius, because distance to crystal centre is taken and not its size.
  // If this correction is not implemented, preclusters consisting of a single crystal will get a radius of 0 and will
  // never be merged to another cluster. Added multiplier n to allow the use of the diameter, or manually increase the
  // radius of a cluster
  void SetXRadius(Double_t t_xrad) { fXRadius = t_xrad; }
  void SetYRadius(Double_t t_yrad) { fYRadius = t_yrad; }

  Double_t GetRadius() const { return fRadius; }
  Double_t GetXRadius() const { return fXRadius; }
  Double_t GetYRadius() const { return fYRadius; }

 protected:
  void invalidateCache(Bool_t);

 protected:
  std::vector<BSEmcDigiInfo_t> fDigis{};
  std::vector<BSEmcDigiInfo_t> fMaxima{};

  Bool_t fEnergyValid{kFALSE};
  Double_t fEnergy{-1};
  Bool_t fWhereValid{kFALSE};
  TVector3 fWhere{0, 0, 0};
  UInt_t fNSubClusters{0};

  FairMultiLinkedData fTrackEntering{};
  FairMultiLinkedData fTrackExiting{};

  Double_t fRadius{-1};
  Double_t fXRadius{-1};
  Double_t fYRadius{-1};
  ClassDef(BSEmcCluster, 3)
};

#endif /*BSEMCCLUSTER_HH*/
