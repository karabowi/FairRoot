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
// -----                 PndEmcHitProducer header file                 -----
// -----                 Created 14/08/06  by S.Spataro                -----
// -------------------------------------------------------------------------
#ifndef PNDEMCHITPRODUCER_H
#define PNDEMCHITPRODUCER_H

#include <PndPersistencyTask.h>
#include <map>
#include <string>
#include <limits>

#include "TH1F.h"
#include "TFile.h"
#include "TVector3.h"

#include "PndEmcHit.h"
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"

#include "PndMCTrack.h"
#include "TClonesArray.h"

#include <set>
#include <vector>

class TObjectArray;

class PndEmcDigiPar;
class PndEmcGeoPar;
class PndEmcDigiNonuniformityPar;

using std::cout;
using std::endl;
using std::map;

/**
 * @brief creates PndEmcHit%s from PndEmcPoint%s
 * @ingroup PndEmc
 */
 
struct PndEmcVolume {
  virtual bool InsideVolume(TVector3 point) = 0;
};

struct PndEmcVolumeDisk : public PndEmcVolume { // needed for MC matching. Assume barrel structure
  double fMinR;
  double fMaxR;
  double fMinZ;
  double fMaxZ;
  PndEmcVolumeDisk(double minR, double maxR, double minZ, double maxZ) : fMinR(minR), fMaxR(maxR), fMinZ(minZ), fMaxZ(maxZ){};
  bool InsideVolume(TVector3 point)
  {
    if (point.Z() > fMinZ && point.Z() < fMaxZ) {
      if (point.Perp() > fMinR && point.Perp() < fMaxR) {
        return true;
      }
    }
    return false;
  };
};

struct PndEmcVolumeBox : public PndEmcVolume {
  double fMinX, fMaxX;
  double fMinY, fMaxY;
  double fMinZ, fMaxZ;

  PndEmcVolumeBox(double minX, double maxX, double minY, double maxY, double minZ, double maxZ) : fMinX(minX), fMaxX(maxX), fMinY(minY), fMaxY(maxY), fMinZ(minZ), fMaxZ(maxZ){};
  bool InsideVolume(TVector3 point)
  {
    if (point.Z() > fMinZ && point.Z() < fMaxZ)
      if (point.Y() > fMinY && point.Y() < fMaxY)
        if (point.X() > fMinX && point.X() < fMaxX)
          return true;
    return false;
  };
};

class PndEmcHitProducer : public PndPersistencyTask {

 public:
  typedef std::map<Int_t, Float_t> mapper;

  /** Default constructor **/
  PndEmcHitProducer();

  PndEmcHitProducer(Bool_t val);

  /** Destructor **/
  ~PndEmcHitProducer();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  PndEmcHit *AddHit(Int_t trackID, Int_t detID, Float_t energy, Float_t time, std::vector<Int_t> &mctruth, FairMultiLinkedData entering, FairMultiLinkedData exiting,
                    std::set<Int_t> &clusterIDs);

  // PndEmcHit* AddHit(Int_t trackID, Int_t detID, Float_t energy, Float_t time,std::vector<PndEmcPoint*> pointList);
  // not implemented
  // void CreateStructure();

  void SetStorageOfData(Bool_t val); // Method to specify whether hits are stored or not.
  void SetNonuniformityFile(const char *filename) { fNonuniformityFile = filename; };
  void SetDayOne(bool d = true) { fDayOne = d; };

  void FinishEvent() { fEntryNr++; }
  void FinishTask();

 protected:
  int FindMCIndex(PndEmcPoint *point);
  int FindMCIndexIterative(PndEmcPoint *point, int mcIndex);

 private:
  Int_t fUse_nonuniformity;
  TString fNonuniformityFile;
  /** Input array of PndEmcPoints **/
  TClonesArray *fPointArray;
  TClonesArray *fMCTrackArray;

  /** Output array of PndEmcHit **/
  TClonesArray *fHitArray;
  // TClonesArray* fMcTrackArray;

  TObjArray *fVolumeArray;

  /** Geo file to use **/
  Int_t fMapVersion;
  Float_t fEnergyThreshold;

  mapper emcX;
  mapper emcY;
  mapper emcZ;

  PndEmcStructure *fEmcStr;
  PndEmcMapper *fMapper;

  PndEmcDigiPar *fDigiPar; /** Digitisation parameter container **/
  PndEmcGeoPar *fGeoPar;   /** Geometry parameter container **/
  PndEmcDigiNonuniformityPar *fNonuniformityPar;

  /** Get parameter containers **/
  virtual void SetParContainers();

  Bool_t fStoreHits; // Flag which specify whether hits are stored or not

  void cleansortmclist(std::vector<Int_t> &newlist, TClonesArray *mcTrackArray);

  PndEmcHitProducer(const PndEmcHitProducer &L);
  PndEmcHitProducer &operator=(const PndEmcHitProducer &) { return *this; }

  ClassDef(PndEmcHitProducer, 1);

  map<Int_t, Float_t> fTrackEnergy;
  map<Int_t, Float_t> fTrackTime;                 // time of first point
  map<Int_t, std::vector<Int_t>> fTrackMcTruth;   // McTruth MC track which deposited energy in the crystal
  map<Int_t, std::vector<Int_t>> fPointMatch;     // DetId , PointIds with same DetId
  map<Int_t, FairMultiLinkedData> fTrackEntering; // DetId, link to tracks entering same DetId
  map<Int_t, FairMultiLinkedData> fTrackExiting;  // DetId, link to track exiting same DetId
  map<Int_t, std::set<Int_t>> fClusterIDs;        // DetId, cluster ids of all point with energy above threshold
  map<Int_t, PndEmcPoint *> fLastPointForTrack;   // <mcTrackId, last point of track seen>
  map<Int_t, std::map<Int_t, Double_t>> fShower;  // <DetId <MCTrackIndex, DepositedEnergy>> MCTruth deposited energy per crystal and (primary) mc particle type
  std::vector<PndEmcVolume *> fEmcVolumes;        //! used to determine if a point is inside the EMC or outside for MC Matching
  double fCutSameTrack;
  double fCutMotherParticle;
  double fCutMotherParticleShashlyk;
  int fEntryNr;

  bool fDayOne;
  bool AcceptDayOne(PndEmcPoint *p);
};
#endif
