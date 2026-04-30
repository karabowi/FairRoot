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

#ifndef PNDMDTHITPRODUCER_H
#define PNDMDTHITPRODUCER_H 1

#include "FairTask.h"
#include "PndMdtHit.h"
#include "TVector3.h"
#include "FairTSBufferFunctional.h"

class TClonesArray;
class PndMdtDigi;

using std::map;
using std::vector;

class PndMdtHitProducer : public FairTask {

 public:
  /** Default constructor **/
  PndMdtHitProducer();

  /** Destructor **/
  ~PndMdtHitProducer();

  /** Virtual method Init **/
  virtual InitStatus Init();

  virtual void RunTimeBased() { fTimeOrderedDigi = kTRUE; }
  virtual void FinishTask();
  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  PndMdtHit *AddHit(Int_t detID, Int_t stripID, TVector3 &pos, TVector3 &dpos, Int_t bIndex, Int_t sIndex);

  //  Bool_t MdtMapping(); // Creates maps of MDT hits
  // void Reset();        // reset maps

 private:
  virtual void Exec_old(Option_t *opt);

  /** Input array of PndMdtDigi **/
  TClonesArray *fBoxArray;
  TClonesArray *fStripArray;
  TClonesArray *fDigiArray;

  TClonesArray *fBoxClusterArray;
  TClonesArray *fStripClusterArray;
  /** Output array of PndMdtHit **/
  TClonesArray *fHitArray;

  Int_t fNumofBoxDigis;
  Int_t fNumofStripDigis;
  Int_t fNumofHits;
  Int_t fNumofGroupedBoxDigis;
  Int_t fNumofGroupedStripDigis;
  /*   map<Int_t, vector<Int_t> >mapBoxBarrel; */
  /*   map<Int_t, vector<Int_t> >mapBoxEndcap; */
  /*   map<Int_t, vector<Int_t> >mapBoxForward; */
  /*   map<Int_t, vector<Int_t> >mapStripBarrel; */
  /*   map<Int_t, vector<Int_t> >mapStripEndcap; */
  /*   map<Int_t, vector<Int_t> >mapStripForward; */
  std::map<Int_t, Int_t> fMatchMap;
  // statistics record
  std::map<Int_t, Int_t> fBoxHitMapofLayer;
  std::map<Int_t, Int_t> fStripHitMapofLayer;

  typedef std::vector<std::vector<PndMdtDigi *>> ClustersCollection;
  typedef ClustersCollection::iterator ClustersColIter;
  typedef std::vector<PndMdtDigi *> SingleCluster;
  typedef SingleCluster::iterator SingleDigiIter;

  typedef std::vector<std::vector<PndMdtDigi *>> TrackletCollection;
  typedef TrackletCollection::iterator TrackletColIter;
  typedef std::vector<PndMdtDigi *> Tracklet;
  typedef Tracklet::iterator TrackletDigiIter;

  typedef std::vector<PndMdtDigi *> VecDigi;
  typedef VecDigi::iterator VecDigiIter;

  Bool_t fTimeOrderedDigi;
  BinaryFunctor *fFunctor;
  Int_t fEventCounter;
  Double_t fTimeWindow;

  struct PndMdtDigiLess {
    bool operator()(const PndMdtDigi *lv, const PndMdtDigi *rv) const;
  };

  ClassDef(PndMdtHitProducer, 1);
};

#endif
