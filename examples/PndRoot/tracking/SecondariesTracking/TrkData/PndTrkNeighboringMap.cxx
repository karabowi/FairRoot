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

//
// PndTrkNeighboringMap.cxx
//
//
//
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//
////////////////////////////////////////////////////////////

#include "PndTrkNeighboringMap.h"

#include "PndTrkHit.h"
#include "PndSttTube.h"

using namespace std;

PndTrkNeighboringMap::PndTrkNeighboringMap(TClonesArray *tubearray) : fTubeArray(tubearray), fStandalone(TObjArray()), fOneNeigh(TObjArray()), fTwoNeigh(TObjArray()) {}

PndTrkNeighboringMap::PndTrkNeighboringMap(const PndTrkNeighboringMap &thismap) : TObject(thismap)
{
  *this = thismap;
}

PndTrkNeighboringMap::~PndTrkNeighboringMap()
{
  delete fTubeArray;
}

// CHECK this might still have problems
PndTrkNeighboringMap &PndTrkNeighboringMap::operator=(const PndTrkNeighboringMap &thismap)
{

  TMapIter *it = (TMapIter *)hit2neigh.MakeIterator();
  TObjArray *hits;
  while (PndTrkHit *hit = (PndTrkHit *)it->Next()) {
    hits = (TObjArray *)thismap.hit2neigh.GetValue(hit);
    hit2neigh.Add(hit, hits);
  }

  TMapIter *it2 = (TMapIter *)hit2indiv.MakeIterator();
  TObjArray *hits2;
  while (PndTrkHit *hit = (PndTrkHit *)it2->Next()) {
    hits2 = (TObjArray *)thismap.hit2indiv.GetValue(hit);
    hit2indiv.Add(hit, hits2); // FIXME [R.K. 03/2017] Should here be hits or hits2? I put hits2 now.
  }

  fTubeArray = thismap.fTubeArray;
  fStandalone = thismap.fStandalone;
  fOneNeigh = thismap.fOneNeigh;
  fTwoNeigh = thismap.fTwoNeigh;
  return *this;
}

void PndTrkNeighboringMap::Clear()
{

  hit2neigh.Clear();
  hit2indiv.Clear();
  fStandalone.Clear(); // CHECK
  fOneNeigh.Clear();   // CHECK
  fTwoNeigh.Clear();   // CHECK
  //  fTubeArray->Clear(); // CHECK
}

void PndTrkNeighboringMap::AddNeighboringsToHit(PndTrkHit *hit, TObjArray *hits)
{

  //  hit->DrawTube(kGreen); // CHECK
  TObjArray *neighs = new TObjArray(*hits);
  hit2neigh.Add(hit, neighs);
  int tubeID = hit->GetTubeID();
  PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

  if (hits->GetEntriesFast() == 0)
    fStandalone.Add(hit);
  else if (hits->GetEntriesFast() == 1)
    fOneNeigh.Add(hit);
  else if (hits->GetEntriesFast() == 2)
    fTwoNeigh.Add(hit);
  else if (tube->GetLayerID() == 0)
    fTwoNeigh.Add(hit);

  // fill indivisible map -------------------------------------------

  // up to 2 hits
  if (hits->GetEntriesFast() <= 2) {
    hit2indiv.Add(hit, hits);
    //   cout << "Bset up map " << hit->GetHitID() << " " << hits->GetEntriesFast() << endl;
    return;
  }

  // more hits
  std::vector<int> removefromlist;
  int counter = 0;
  for (int k = 0; k < hits->GetEntriesFast(); k++) {
    PndTrkHit *hit2 = (PndTrkHit *)hits->At(k);
    PndSttTube *tube2 = (PndSttTube *)fTubeArray->At(hit2->GetTubeID());
    if (tube->GetLayerID() == tube2->GetLayerID()) {
      removefromlist.push_back(k);
      continue;
    }
    counter++;
  }

  for (int k = removefromlist.size() - 1; k >= 0; k--) {
    int delk = removefromlist[k];
    PndTrkHit *hit2 = (PndTrkHit *)hits->At(delk);
    hits->Remove(hit2);
    hits->Compress();
  }

  if (counter > 2) {
    TObjArray *hits2 = new TObjArray();
    for (int iobj = 0; iobj < hits->GetEntriesFast(); iobj++)
      hits2->Add(hits->At(iobj));
    if (counter > 2) {
      for (int k = hits2->GetEntriesFast() - 1; k >= 0; k--) {
        PndTrkHit *hit2 = (PndTrkHit *)hits2->At(k);
        hits2->Remove(hit2);
      }
    }
    hit2indiv.Add(hit, hits2);
  } else
    hit2indiv.Add(hit, hits);

  //  cout << "Aset up map " << hit->GetHitID() << " " << hits->GetEntriesFast() << endl;
}

TObjArray PndTrkNeighboringMap::GetSeeds()
{
  TObjArray seeds;
  for (int ihit = 0; ihit < fOneNeigh.GetEntriesFast(); ihit++) {
    PndTrkHit *hit = (PndTrkHit *)fOneNeigh.At(ihit);
    int tubeID = hit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    TObjArray neighs = GetNeighboringsToHit(hit);

    // if it has only one neighboring tube
    if (neighs.GetEntriesFast() == 1) {
      PndTrkHit *hit2 = (PndTrkHit *)neighs.At(0);
      int tubeID2 = hit2->GetTubeID();
      PndSttTube *tube2 = (PndSttTube *)fTubeArray->At(tubeID2);
      bool difflayer = false;
      // if the k-neigh is on the same layer...
      if (tube->GetLayerID() == tube2->GetLayerID()) {
        TObjArray neighs2 = GetNeighboringsToHit(hit2);
        if (neighs2.GetEntriesFast() <= 1)
          continue;
        // ...and has more than 1 neighboring, then loop over them:
        // if there is at least one on a different layer, then hit is not a seed
        for (int jhit = 0; jhit < neighs2.GetEntriesFast(); jhit++) {
          PndTrkHit *hit2b = (PndTrkHit *)neighs2.At(jhit);
          if (hit2b == hit)
            continue;
          Int_t tubeID2b = hit2b->GetTubeID();
          PndSttTube *tube2b = (PndSttTube *)fTubeArray->At(tubeID2b);
          if (tube2->GetLayerID() != tube2b->GetLayerID())
            difflayer = true;
        }
        if (difflayer == true)
          continue;
      }
      seeds.Add(hit);
    }
    //    else { // else, it means it belongs to the first layer
    //       bool difflayer = false;
    //       for(int jhit = 0; jhit < neighs->GetEntriesFast(); jhit++) {
    // 	PndTrkHit *hit2 = (PndTrkHit*) neighs->At(jhit);
    // 	int tubeID2 = hit2->GetTubeID();
    // 	PndSttTube *tube2 = (PndSttTube*) fTubeArray->At(tubeID2);
    // 	if(tube2->GetLayerID() != tube->GetLayerID()) difflayer = true;
    //       }
    //       if(difflayer == false)  continue;
    //       seeds.Add(hit);
    //     }
  }
  return seeds;
}

TObjArray PndTrkNeighboringMap::GetCandseeds()
{
  TObjArray candidateseeds;

  for (int ihit = 0; ihit < fTwoNeigh.GetEntriesFast(); ihit++) {
    int samelayer = -1;
    int standalone = -2;
    PndTrkHit *hit = (PndTrkHit *)fTwoNeigh.At(ihit);
    int tubeID = hit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    TObjArray neighs = GetNeighboringsToHit(hit);

    // if it is a candidate seed because it comes from the 1st layer
    if (tube->GetLayerID() == 0) {
      bool difflayer = false;
      for (int jhit = 0; jhit < neighs.GetEntriesFast(); jhit++) {
        PndTrkHit *hit2 = (PndTrkHit *)neighs.At(jhit);
        int tubeID2 = hit2->GetTubeID();
        PndSttTube *tube2 = (PndSttTube *)fTubeArray->At(tubeID2);
        if (tube2->GetLayerID() != tube->GetLayerID())
          difflayer = true;
      }
      if (difflayer == false)
        continue;
      candidateseeds.Add(hit);
    } else { // because it has 2 neigh, whose one on the same layer
      for (int jhit = 0; jhit < neighs.GetEntriesFast(); jhit++) {
        PndTrkHit *hit2 = (PndTrkHit *)neighs.At(jhit);
        if (GetNeighboringsToHit(hit2).GetEntriesFast() == 1)
          standalone = jhit;
        int tubeID2 = hit2->GetTubeID();
        PndSttTube *tube2 = (PndSttTube *)fTubeArray->At(tubeID2);
        if (tube->GetLayerID() == tube2->GetLayerID())
          samelayer = jhit;
      }
      if (samelayer == standalone)
        candidateseeds.Add(hit);
    }
  }
  return candidateseeds;
}

TObjArray PndTrkNeighboringMap::GetIndivisibles()
{
  return fTwoNeigh;
}

TObjArray PndTrkNeighboringMap::GetIndivisiblesToHit(PndTrkHit *hit)
{
  TMapIter *it2 = (TMapIter *)hit2indiv.MakeIterator();
  while (PndTrkHit *hit2 = (PndTrkHit *)it2->Next()) {
    if (hit->GetHitID() == hit2->GetHitID() && hit->GetDetectorID() == hit2->GetDetectorID()) {
      if (((TObjArray *)hit2indiv.GetValue(hit2))->GetEntriesFast() == 0)
        return TObjArray(0);
      return *((TObjArray *)hit2indiv.GetValue(hit2));
    }
  }
  return TObjArray(0);
}

void PndTrkNeighboringMap::PrintIndivisibleMap()
{
  TMapIter *it2 = (TMapIter *)hit2indiv.MakeIterator();
  TObjArray *hits2;
  while (PndTrkHit *hit = (PndTrkHit *)it2->Next()) {
    hits2 = (TObjArray *)hit2indiv.GetValue(hit);
    cout << hit->GetHitID() << "(tube: " << hit->GetTubeID() << ") has " << hits2->GetEntriesFast() << " indivisibles: ";
    for (int ihit = 0; ihit < hits2->GetEntriesFast(); ihit++) {
      PndTrkHit *hit2 = (PndTrkHit *)hits2->At(ihit);
      cout << " " << hit2->GetHitID() << "(tube: " << hit2->GetTubeID() << "), ";
    }
    cout << endl;
  }
}

// Returns 0 if not found.
TObjArray PndTrkNeighboringMap::GetNeighboringsToHit(PndTrkHit *hit)
{
  //   TObjArray *neighs = (TObjArray*) hit2neigh.GetValue(hit);
  //   if(neighs == nullptr) return 0;
  //   return *(neighs);

  TMapIter *it2 = (TMapIter *)hit2neigh.MakeIterator();
  while (PndTrkHit *hit2 = (PndTrkHit *)it2->Next()) {
    if (hit->GetHitID() == hit2->GetHitID() && hit->GetDetectorID() == hit2->GetDetectorID()) {
      if (((TObjArray *)hit2neigh.GetValue(hit2))->GetEntriesFast() == 0)
        return TObjArray(0);
      return *((TObjArray *)hit2neigh.GetValue(hit2));
    }
  }
  return TObjArray(0);
}

TMapIter *PndTrkNeighboringMap::GetIterator()
{
  return (TMapIter *)hit2neigh.MakeIterator();
}

TObjArray PndTrkNeighboringMap::GetHitWithNNeighborings(int nofhits)
{
  TObjArray listofhits;

  TMapIter *it = (TMapIter *)hit2neigh.MakeIterator();
  TObjArray *hits;
  while (PndTrkHit *hit = (PndTrkHit *)it->Next()) {
    hits = (TObjArray *)hit2neigh.GetValue(hit);
    if (hits->GetEntriesFast() == nofhits)
      listofhits.Add(hit);
  }
  return listofhits;
}
