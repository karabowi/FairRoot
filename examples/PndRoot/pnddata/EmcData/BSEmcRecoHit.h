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

#ifndef BSEMCRECOHIT_HH
#define BSEMCRECOHIT_HH

#include <string> // for string
#include <vector>

#include "Rtypes.h"      // for BSEmcRecoHit::Class, BSEmcRecoHit::...
#include "RtypesCore.h"  // for Int_t, Double_t, Float_t
#include "TMatrixDfwd.h" // for TMatrixD
#include "TMatrixT.h"    // for TMatrixT
#include "TVector3.h"    // for TVector3

#include "FairHit.h"             // for FairHit
#include "FairMultiLinkedData.h" // for FairMultiLinkedData

#include "BSEmcDataBranchNames.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcRecoHit
 * @brief Resembles/Contains all information a particle hitting the EMC can provide for later use in the PID stage
 * @details Contains all information such as Zernike and Lateral moments, different cluster energies (E1,E9, E25),
 * error matrix, the calculated position, the index of the origin-cluster and how many subcluster that cluster formed
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcData
 */
class BSEmcRecoHit : public FairHit {
 public:
  BSEmcRecoHit();
  BSEmcRecoHit(const BSEmcRecoHit &t_other);
  virtual ~BSEmcRecoHit();

  const TVector3 &GetPosition() const { return fPosition; }
  const TVector3 &GetCorrectedPosition() const { return fCorrectedPosition; }
  Double_t GetRawEnergy() const { return fRawEnergy; }
  Double_t GetCorrectedEnergy() const { return fCalEnergy; }
  Double_t GetEnergy1() const { return fE1; }
  Double_t GetEnergy9() const { return fE9; }
  Double_t GetEnergy25() const { return fE25; }
  Double_t GetLateralMoment() const { return fLat; }
  Double_t GetZ20() const { return fZ20; }
  Double_t GetZ53() const { return fZ53; }
  Int_t GetNumberOfClusterCrystals() const { return fNumberOfCrystals; }
  Int_t GetNumberOfSubCluster() const { return fNumberOfSubCluster; }
  Int_t GetNumberOfSubClusterCrystals() const { return fNumberOfSubClusterCrystals; }
  Int_t GetModule() const { return fModule; }
  Bool_t IsSplitOff() const {return fIsSplitOff; }

  const TMatrixD &GetCov7() const { return fCov7; }
  const TMatrixD &GetCovP4() const { return fCovP4; }
  Int_t GetClusterIndex() const { return fClusterIndex; }
  Int_t GetSubClusterIndex() const { return fSubClusterIndex; }
  Int_t GetCentralCrystalId() const { return fCentralCrystalId; }

  void SetPosition(const TVector3 &t_pos)
  {
    fPosition = t_pos;
    fX = t_pos.X();
    fY = t_pos.Y();
    fZ = t_pos.Z();
  }

  void SetCorrectedPosition(const TVector3 &t_pos) { fCorrectedPosition = t_pos; }

  void SetRawEnergy(Double_t t_energy) { fRawEnergy = t_energy; }
  void SetCorrectedEnergy(Double_t t_energy) { fCalEnergy = t_energy; }
  void SetEnergy1(Double_t t_energy) { fE1 = t_energy; }
  void SetEnergy9(Double_t t_energy) { fE9 = t_energy; }
  void SetEnergy25(Double_t t_energy) { fE25 = t_energy; }
  void SetLateralMoment(Double_t t_latmom) { fLat = t_latmom; }
  void SetZ20(Double_t t_z20) { fZ20 = t_z20; }
  void SetZ53(Double_t t_z53) { fZ53 = t_z53; }
  void SetNumberOfClusterCrystals(Int_t t_ncrystals) { fNumberOfCrystals = t_ncrystals; }
  void SetNumberOfSubCluster(Int_t t_nsubcluster) { fNumberOfSubCluster = t_nsubcluster; }
  void SetNumberOfSubClusterCrystals(Int_t t_nsubclustercrystals) { fNumberOfSubClusterCrystals = t_nsubclustercrystals; }
  void SetModule(Int_t t_module) { fModule = t_module; }
  void SetCentralCrystalId(Int_t t_centralCrystal) { fCentralCrystalId = t_centralCrystal; }
  void SetSplitOff(Bool_t t_IsSplitOff) {fIsSplitOff = t_IsSplitOff; }

  void SetCov7(const TMatrixD &t_cov7) { fCov7 = t_cov7; }
  void SetCovP4(const TMatrixD &t_covP4) { fCovP4 = t_covP4; }

  void SetSubClusterIndex(Int_t t_subclusterindex) { fSubClusterIndex = t_subclusterindex; }
  void SetClusterIndex(Int_t t_clusterindex) { fClusterIndex = t_clusterindex; }

  const std::vector<Int_t> &GetMcList() const { return fMcList; };
  Int_t GetMcSize() const { return fMcList.size(); }
  Int_t GetMcIndex(Int_t t_idx = 0) const { return fMcList[t_idx]; }
  void SetMcList(const std::vector<Int_t> &t_mclist) { fMcList = t_mclist; }

  FairMultiLinkedData GetTrackEntering() const { return fTrackEntering; }
  FairMultiLinkedData GetTrackExiting() const { return fTrackExiting; }
  void SetTrackEntering(const FairMultiLinkedData &t_tracks) { fTrackEntering = t_tracks; }
  void SetTrackExiting(const FairMultiLinkedData &t_tracks) { fTrackExiting = t_tracks; }

 private:
  TVector3 fPosition{};
  TVector3 fCorrectedPosition{};
  Float_t fRawEnergy{-1};
  Float_t fCalEnergy{-1};
  Int_t fNumberOfCrystals{-1};
  Int_t fNumberOfSubCluster{-1};
  Int_t fNumberOfSubClusterCrystals{-1};
  Int_t fModule{-1};
  Int_t fClusterIndex{-1};
  Int_t fSubClusterIndex{-1};
  Int_t fCentralCrystalId{-1};
  Double_t fZ20{-1};
  Double_t fZ53{-1};
  Double_t fLat{-1};
  Double_t fE1{-1};
  Double_t fE9{-1};
  Double_t fE25{-1};
  TMatrixD fCov7{7, 7};
  TMatrixD fCovP4{4, 4};
  Bool_t fIsSplitOff{false};

  std::vector<Int_t> fMcList{};
  FairMultiLinkedData fTrackEntering{};
  FairMultiLinkedData fTrackExiting{};

  ClassDef(BSEmcRecoHit, 1);
};

#endif /*BSEMCRECOHIT_HH*/
