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

/** PndTrkCluster
 **
 ** @author Lia Lavezzi
 **/

#ifndef PNDTRKCLUSTER_H
#define PNDTRKCLUSTER_H 1

#include "TVector3.h"
#include "PndTrkHit.h"
#include "PndTrkHitList.h"
#include "FairHit.h"

// #include "TObjArray.h"
#include "TClonesArray.h"

#define MAXNOFHITSINCLUSTER 1000 // CHECK consistency

// class TObjArray;
class PndTrkCluster : public TObject {

 public:
  PndTrkCluster();
  // copy ctor
  PndTrkCluster(const PndTrkCluster &cluster);
  ~PndTrkCluster();
  PndTrkCluster &operator=(const PndTrkCluster &cluster);

  Bool_t operator==(const PndTrkCluster cluster) const; // CHECK this needs to be changed
  //  PndTrkCluster(const PndTrkCluster& cluster);

  void AddHit(PndTrkHit *hit);
  void AddHit(PndTrkHit hit);

  void DeleteHit(PndTrkHit *hit);
  void DeleteHitAndCompress(PndTrkHit *hit);
  void DeleteHit(Int_t index);
  void DeleteHitAndCompress(Int_t index);
  void DeleteHits(std::vector<int> todelete);

  void DeleteAllHits();

  void SetIRegion(int iregion) { fIRegion = iregion; }

  PndTrkHit *GetHit(int index);
  PndTrkHit *GetPreviousHit(int index);
  PndTrkHit *GetNextHit(int index);

  inline Int_t GetNofHits() { return fHitList.GetEntriesFast(); }
  inline Int_t GetIRegion() { return fIRegion; }

  Double_t GetMinimumXYDistanceFromHit(PndTrkHit *hit);

  PndTrkHit *SearchHit(PndTrkHit *hit);

  void Print();
  void PrintList();
  void Draw(Color_t color = kBlack);
  void LightUp();

  Bool_t DoesContain(PndTrkHit *hit);
  Bool_t IsSimilarTo(PndTrkCluster *cluster2);

  Int_t NofSharedHits(PndTrkCluster *cluster2);
  Bool_t SharedAt(PndTrkCluster *cluster2, double limit);

  int MergeTo(PndTrkCluster *cluster2);
  PndTrkCluster GetMvdStripHitList();
  PndTrkCluster GetMvdPixelHitList();
  PndTrkCluster GetMvdHitList();

  PndTrkCluster GetSttParallelHitList();
  PndTrkCluster GetSttSkewHitList();
  PndTrkCluster GetSttHitList();

  Bool_t FindExtremitiesFrom(TVector3 frompoint, PndTrkHit &firstextremity, PndTrkHit &secondextremity);
  Bool_t CheckClusterAgainsV(TVector3 frompoint, PndTrkHit *firstextremity, PndTrkHit *secondextremity);

  Bool_t ComputeCircle(TVector3 v1, TVector3 v2, TVector3 v3, double &x0, double &y0, double &R);
  PndTrkHit *GetPocaTo(TVector3 frompoint);
  Bool_t SplitAtHit(PndTrkHit *athit, PndTrkCluster &cluster1, PndTrkCluster &cluster2);
  Bool_t SplitV(PndTrkHit *athit, PndTrkHit *firstextremity, PndTrkHit *secondextremity, PndTrkCluster &cluster1, PndTrkCluster &cluster2);

  void SortFromHit(PndTrkHit *firstextremity, TString criterion);
  friend Bool_t SorterFunction(PndTrkHit *hit1, PndTrkHit *hit2);
  void Sort();
  void ReverseSort();

  Bool_t IsSorted() { return fHitList.IsSorted(); }

  void AddCluster(PndTrkCluster *cluster);
  void AddClusterAndSortFrom(PndTrkCluster *cluster, TVector3 frompoint, TString criterion);
  void Replace(PndTrkHit *hit);
  void Clear(Option_t * = "");

 protected:
  TVector3 fFromPoint;
  Int_t fIRegion;
  //  std::vector< PndTrkHit * > hitlist;
  // TObjArray hitlist;
  TClonesArray fHitList;
  ClassDef(PndTrkCluster, 1);
};

#endif
