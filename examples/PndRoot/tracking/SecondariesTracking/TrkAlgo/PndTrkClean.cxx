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
// PndTrkClean.cxx
//
//
//
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkClean.h"

#include "PndTrkTools.h"
#include "PndTrkLegendreCluster.h"
#include "PndSttTube.h"

#include "TArc.h"
#include <iostream>

using namespace std;

PndTrkClean::PndTrkClean() : fTubeArray(nullptr)
{
  fGeoH = PndGeoHandling::Instance();
}

PndTrkClean::PndTrkClean(TClonesArray *tubearray)
{
  fGeoH = PndGeoHandling::Instance();
  fTubeArray = tubearray;
}

PndTrkClean::~PndTrkClean() {}

// /**
// LAY NAME

// barrel, layers from innermost to outermost:
// 1   PixeloBlo1
// 6   PixeloBlo2
// 15  StripoBl3o(Silicon)
// 17  StripoBl4o(Silicon)

// forward wheels, inner, increasing z:
// left
// 2   PixeloSdkoco(Silicon)_1
// 3   PixeloSdkoco(Silicon)_2
// 7   PixeloLdkoco(Silicon)_1
// 8   PixeloLdkoco(Silicon)_2
// 9   PixeloLdkoco(Silicon)_3
// 10  PixeloLdkoco(Silicon)_4

// right
// 4   PixeloSdkoco(Silicon)_3
// 5   PixeloSdkoco(Silicon)_4
// 11  PixeloLdkoco(Silicon)_5
// 12  PixeloLdkoco(Silicon)_6
// 13  PixeloLdkoco(Silicon)_7
// 14  PixeloLdkoco(Silicon)_8

// forward wheels, last two, outer:
// left
// 16  Fwdo(Silicon)_1
// right
// 20  StripoLdko5-6oTrapSo(Silicon)_1

// **/
// int PndTrkClean::FindMvdLayer(int sensorID) {
//   TString geoPath;
//   int Layer = 0;

//   bool flag = true;

//   geoPath=fGeoH->GetPath(sensorID);
//   cout << "path " << geoPath << endl;
//   TVector3 o, u, v;
//   fGeoH->GetOUVPath(geoPath, o, u, v);
//   o.Print();
//   u.Print();
//   v.Print();

//   if (flag && geoPath.Contains("PixeloBlo1")){Layer=1;flag=false;}
//   if (flag && geoPath.Contains("PixeloSdko(Silicon)_1")){Layer=2;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloSdkoco(Silicon)_1")){Layer=2;flag=false;}
//   if (flag && geoPath.Contains("PixeloSdko(Silicon)_2")){Layer=3;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloSdkoco(Silicon)_2")){Layer=3;flag=false;}
//   if (flag && geoPath.Contains("PixeloSdko(Silicon)_3")){Layer=4;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloSdkoco(Silicon)_3")){Layer=4;flag=false;}
//   if (flag && geoPath.Contains("PixeloSdko(Silicon)_4")){Layer=5;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloSdkoco(Silicon)_4")){Layer=5;flag=false;}
//   if (flag && geoPath.Contains("PixeloBlo2")){Layer=6;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoio(Silicon)_1")){Layer=7;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_1")){Layer=7;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoiio(Silicon)_1")){Layer=8;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_2")){Layer=8;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoiiio(Silicon)_1")){Layer=9;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_3")){Layer=9;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoiiio(Silicon)_2")){Layer=10;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_4")){Layer=10;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoiio(Silicon)_2")){Layer=11;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_5")){Layer=11;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoio(Silicon)_2")){Layer=12;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_6")){Layer=12;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoiiio(Silicon)_4")){Layer=13;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_7")){Layer=13;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoiiio(Silicon)_3")){Layer=14;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_8")){Layer=14;flag=false;}

//   if (flag && geoPath.Contains("StripoBl3o(Silicon)")){Layer=15;flag=false;}
//   if (flag && geoPath.Contains("Fwdo(Silicon)_1")){Layer=16;flag=false;}
//   if (flag && geoPath.Contains("StripoBl4o(Silicon)")){Layer=17;flag=false;}
//   //if (flag && geoPath.Contains("Fwdo(Silicon)_2")){Layer=10;flag=false;}

//   if (flag && geoPath.Contains("StripoLdkoTrapSoRingAoSilicon_1")){Layer=18;flag=false;}
//   if (flag && geoPath.Contains("StripoLdkoTrapSoRingAoSilicon_2")){Layer=19;flag=false;}
//   if (flag && geoPath.Contains("StripoLdkoTrapSoRingBoSilicon_1")){Layer=18;flag=false;}
//   if (flag && geoPath.Contains("StripoLdkoTrapSoRingBoSilicon_2")){Layer=19;flag=false;}
//   if (flag && geoPath.Contains("StripoLdko5-6oTrapSo(Silicon)_1")){Layer=20;flag=false;}
//   return Layer;
// }

/** MY ONE
LAY NAME

left side
barrel, layers from innermost to outermost:
1   PixeloBlo1*
3   PixeloBlo2*
5   StripoBl3o(Silicon)*
7   StripoBl4o(Silicon)*

forward wheels, inner, increasing z:
9   PixeloSdkoco(Silicon)_1
11  PixeloSdkoco(Silicon)_2
13  PixeloLdkoco(Silicon)_1
15  PixeloLdkoco(Silicon)_2
17  PixeloLdkoco(Silicon)_3
19  PixeloLdkoco(Silicon)_4

forward wheels, last two, outer:
21  Fwdo(Silicon)_1
23  Fwdo(Silicon)_1

right
barrel, layers from innermost to outermost:
2   PixeloBlo1*
4   PixeloBlo2*
6   StripoBl3o(Silicon)*
8   StripoBl4o(Silicon)*

forward wheels, inner, increasing z:
10  PixeloSdkoco(Silicon)_3
12  PixeloSdkoco(Silicon)_4
14  PixeloLdkoco(Silicon)_5
16  PixeloLdkoco(Silicon)_6
18  PixeloLdkoco(Silicon)_7
20  PixeloLdkoco(Silicon)_8

forward wheels, last two, outer:
22  Fwdo(Silicon)_2
24  Fwdo(Silicon)_2


**/
int PndTrkClean::FindMvdLayer(int sensorID)
{
  TString geoPath;
  int Layer = 0;

  geoPath = fGeoH->GetPath(sensorID);
  //  cout << "path " << geoPath << endl;
  TVector3 o, u, v;
  fGeoH->GetOUVPath(geoPath, o, u, v);
  //   o.Print();
  //   u.Print();
  //   v.Print();

  // barrel
  // left
  if (geoPath.Contains("PixeloBlo1")) {
    Layer = 1;
    // right
    if (o.X() < 0)
      Layer += 1;
  } else if (geoPath.Contains("PixeloBlo2")) {
    Layer = 3;
    // right
    if (o.X() < 0)
      Layer += 1;
  } else if (geoPath.Contains("StripoBl3o(Silicon)")) {
    Layer = 5;
    // right
    if (o.X() < 0)
      Layer += 1;
  } else if (geoPath.Contains("StripoBl4o(Silicon)")) {
    Layer = 7;
    // right
    if (o.X() < 0)
      Layer += 1;
  }
  // inner wheels
  // left
  else if (geoPath.Contains("PixeloSdkoco(Silicon)_1")) {
    Layer = 9;
  } else if (geoPath.Contains("PixeloSdkoco(Silicon)_2")) {
    Layer = 11;
  }
  // right0
  else if (geoPath.Contains("PixeloSdkoco(Silicon)_3")) {
    Layer = 10;
  } else if (geoPath.Contains("PixeloSdkoco(Silicon)_4")) {
    Layer = 12;
  }
  //
  // left
  else if (geoPath.Contains("PixeloLdkoco(Silicon)_1")) {
    Layer = 13;
  } else if (geoPath.Contains("PixeloLdkoco(Silicon)_2")) {
    Layer = 15;
  } else if (geoPath.Contains("PixeloLdkoco(Silicon)_3")) {
    Layer = 17;
  } else if (geoPath.Contains("PixeloLdkoco(Silicon)_4")) {
    Layer = 19;
  }
  // right
  else if (geoPath.Contains("PixeloLdkoco(Silicon)_5")) {
    Layer = 14;
  } else if (geoPath.Contains("PixeloLdkoco(Silicon)_6")) {
    Layer = 16;
  } else if (geoPath.Contains("PixeloLdkoco(Silicon)_7")) {
    Layer = 18;
  } else if (geoPath.Contains("PixeloLdkoco(Silicon)_8")) {
    Layer = 20;
  }

  // last two wheels
  else if (geoPath.Contains("Fwdo(Silicon)_1")) { // left
    // 1st
    Layer = 21;
    // 2nd
    if (o.Z() > 20)
      Layer += 2;
  } else if (geoPath.Contains("Fwdo(Silicon)_2")) { // right
    // 1st
    Layer = 22;
    // 2nd
    if (o.Z() > 20)
      Layer += 2;
  } else
    cout << "I have no idea" << endl;

  return Layer;
}

Bool_t PndTrkClean::CheckPairOfHits(PndTrkHit *hit1, PndTrkHit *hit2)
{

  //   cout << "hit1 " << hit1 << endl;
  //   cout << "hit2 " << hit2 << endl;
  // both mvd
  if (hit1->IsMvd() && hit2->IsMvd()) {
    //     cout << "BOTH MVD " << hit1->GetDistance(hit2) << endl;
    if (hit1->GetDistance(hit2) < 7)
      return kTRUE;
  }

  // one mvd - one stt
  else if ((hit1->IsMvd() && hit2->IsStt()) || (hit1->IsStt() && hit2->IsMvd())) {
    //     cout << "ONE MVD/ONE STT " << hit1->GetXYDistance(hit2) << endl;

    // if the stt one is skew -> WRONG for sure!
    if ((hit2->IsStt() && hit2->IsSttSkew()) || (hit1->IsStt() && hit1->IsSttSkew()))
      return kFALSE;
    // if parallel:
    else if (hit1->GetXYDistance(hit2) < 8)
      return kTRUE;
  }

  // both stt
  else if (hit1->IsStt() && hit2->IsStt()) {
    // one parallel one skew
    if ((hit1->IsSttParallel() && hit2->IsSttSkew()) || (hit2->IsSttParallel() && hit1->IsSttSkew())) {

      //       cout << "PARALLEL/SKEW" << endl;
      //       cout << hit1->IsSttSkew() << " " << hit1->GetHitID() << " " << hit1->GetDetectorID() << endl;
      //       cout << hit2->IsSttSkew() << " " << hit2->GetHitID() << " " << hit2->GetDetectorID() << endl;
      //       cout << hit1->GetXYDistance(hit2) << endl;

      if (hit1->GetXYDistance(hit2) < 3)
        return kTRUE;
    }
    // both parallel
    else if (hit1->IsSttParallel() && hit2->IsSttParallel()) {
      PndSttTube *tube1 = (PndSttTube *)fTubeArray->At(hit1->GetTubeID());
      int layerID1 = tube1->GetLayerID();
      PndSttTube *tube2 = (PndSttTube *)fTubeArray->At(hit2->GetTubeID());
      int layerID2 = tube2->GetLayerID();
      //
      //   cout << "layers " << layerID1 << " " << layerID2 << endl;

      // same layer
      if (layerID1 == tube2->GetLayerID())
        return kTRUE;
      // both inner/outer parallel layers
      else if ((layerID1 < 9 && layerID2 < 9) || (layerID1 > 8 && layerID2 > 8)) {
        // adjacent layers
        if (layerID1 == layerID2 - 1)
          return kTRUE;
        else if (layerID1 == layerID2 + 1)
          return kTRUE;
      }
      // one inner(outer) one outer(inner)
      else {
        // must be on boundaries // CHECK isboundary
        if (layerID1 == 7 && layerID2 == 16)
          return kTRUE;
        else if (layerID1 == 16 && layerID2 == 7)
          return kTRUE;
      }
    }
    // both skew
    else if (hit1->IsSttSkew() && hit2->IsSttSkew()) {
      PndSttTube *tube1 = (PndSttTube *)fTubeArray->At(hit1->GetTubeID());
      int layerID1 = tube1->GetLayerID();
      PndSttTube *tube2 = (PndSttTube *)fTubeArray->At(hit2->GetTubeID());
      int layerID2 = tube2->GetLayerID();
      // same  layer
      if (layerID1 == layerID2)
        return kTRUE;
      // adjacent layers
      else if (layerID1 == layerID2 - 1)
        return kTRUE;
      else if (layerID1 == layerID2 + 1)
        return kTRUE;
    }
  }

  return kFALSE;
}

// vicinity of hits
PndTrkClusterList PndTrkClean::Cleanup2(PndTrkCluster *cluster)
{

  // get most distance point
  PndTrkHit *disthit = nullptr;
  double tmpdistance = -1;
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    double dist = hit->GetXYDistance(TVector3(0., 0., 0.));
    if (dist > tmpdistance) {
      disthit = hit;
      tmpdistance = dist;
    }
  }
  // sort
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    double dist = hit->GetXYDistance(disthit);
    hit->SetSortVariable(dist);
  }
  cluster->Sort();
  // for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) { //[R.K. 01/2017] unused variable
  // PndTrkHit *hit = cluster->GetHit(ihit); //[R.K. 01/2017] unused variable
  //  cout << "sorted hit " << ihit << " " << hit->GetDetectorID() << " " << hit->GetHitID() << " " << hit->GetSortVariable() << " " << hit->IsSortable() << endl;

  //} //[R.K. 01/2017] unused variable
  std::vector<int> failedhits, breakpoints;
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    //     cout << "HIT " << hit->GetHitID() << " " << hit->GetDetectorID() << endl;
    //     hit->GetPosition().Print();

    PndTrkHit *follow = cluster->GetNextHit(ihit);
    PndTrkHit *previous = cluster->GetPreviousHit(ihit);
    //    cout << "pre " << previous << " fol " << follow << endl;
    Bool_t pre = kTRUE, fol = kTRUE;
    if (previous != nullptr)
      pre = CheckPairOfHits(previous, hit);
    if (follow != nullptr)
      fol = CheckPairOfHits(follow, hit);
    // cout << "pre " << pre << " fol " << fol << endl;

    // isolated hit wrong
    if (pre == kFALSE && fol == kFALSE) {
      // cout << "isolated " << hit->GetDetectorID() << " " << hit->GetHitID() << " " << pre << " " << fol << endl;
      failedhits.push_back(ihit);
    }
    // first hit wrong
    else if (previous == nullptr && fol == kFALSE) {
      PndTrkHit *follow2 = cluster->GetNextHit(ihit + 1);
      if (CheckPairOfHits(follow, follow2) == kFALSE) {
        failedhits.push_back(ihit);
        // cout << "first " << hit->GetDetectorID() << " " << hit->GetHitID() << " " << pre << " " << fol << endl;
      }
    }
    // last hit wrong
    else if (follow == nullptr && pre == kFALSE) {
      PndTrkHit *previous2 = cluster->GetPreviousHit(ihit - 1);
      if (CheckPairOfHits(previous, previous2) == kFALSE) {
        // cout << "last " << hit->GetDetectorID() << " " << hit->GetHitID() << " " << pre << " " << fol << endl;
        failedhits.push_back(ihit);
      }
    }
    // breakpoints
    else if (pre == kTRUE && fol == kFALSE) {
      if (find(failedhits.begin(), failedhits.end(), ihit - 1) == failedhits.end()) {
        //	cout << "BREAKPOINT " <<  hit->GetHitID() << " " << hit->GetDetectorID() << endl;
        breakpoints.push_back(ihit);
      }
    }
  }

  //     cout << "CLEANUP PROCEDURE " << endl;
  //     cout << "failedhits " << failedhits.size() << " breakpoints " << breakpoints.size() << endl;

  //    if(display == kTRUE)
  {
    for (size_t ihit = 0; ihit < failedhits.size(); ihit++) {
      // 	cout << "failedhits " << failedhits.size() << " " << failedhits[ihit] << endl;
      int hitno = failedhits[ihit];
      PndTrkHit *hit = cluster->GetHit(hitno);
      hit->Draw(kRed);
    }

    for (size_t ihit = 0; ihit < breakpoints.size(); ihit++) {
      int hitno = breakpoints[ihit];
      PndTrkHit *hit = cluster->GetHit(hitno);
      hit->Draw(kGreen);
    }
  }
  //     cout << "before delete " << cluster-> GetNofHits() << endl;
  cluster->DeleteHits(failedhits);
  //     cout << "dopo delete " << cluster-> GetNofHits() << endl;
  //
  // 0 1 2 3 4 5 6 7 8 9
  // failed ihit 2 3 5
  // 0 1     4   6 7 8 9
  // break orig ihit 4 8
  // break fix  ihit 2 5
  // fix the breakcounter
  for (size_t jhit = 0; jhit < breakpoints.size(); jhit++) {
    int counter = 0;
    for (size_t ihit = 0; ihit < failedhits.size(); ihit++) {
      if (failedhits[ihit] < breakpoints[jhit])
        counter++;
      else
        break;
    }
    breakpoints[jhit] -= counter;
  }

  //    cout << "dopo delete " << cluster-> GetNofHits() << endl;

  //  PndTrkClusterList list = Split(cluster, breakpoints);

  PndTrkClusterList list;
  list.AddCluster(cluster);
  //    cout << "dopo delete " << cluster-> GetNofHits() << " " << list.GetNofClusters() << endl;
  return list;
}

// CHECK if it works
Bool_t PndTrkClean::SplitAtHit(PndTrkCluster *hitlist, PndTrkHit *athit, PndTrkCluster &cluster1, PndTrkCluster &cluster2)
{

  // PndTrkHit *tmphit = hitlist->GetHit(0); //[R.K. 01/2017] unused variable?

  cluster1.AddHit(athit);
  cluster2.AddHit(athit);
  TVector3 tmpposition = athit->GetPosition();
  int tmptubeid = athit->GetTubeID();
  PndSttTube *tmptube = (PndSttTube *)fTubeArray->At(tmptubeid);
  PndTrkHit *secondhit = nullptr;

  //  cout << "CLUS1: " << tmphit->GetHitID() << endl;
  for (int ihit = 0; ihit < hitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = hitlist->GetHit(ihit);
    if (*hit == *athit) {
      cout << "hit == ahit " << endl;
      continue;
    }
    //  if(hit->GetXYDistance(tmpposition) < STTPARALDISTANCE) { // CHECK
    int tubeid = hit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeid);
    int layerid = tube->GetLayerID();
    tmptube = (PndSttTube *)fTubeArray->At(tmptubeid);
    int tmplayerid = tmptube->GetLayerID();
    cout << "*** " << hit->GetHitID() << " " << layerid << " " << tmplayerid << endl;
    if (layerid == tmplayerid || layerid == tmplayerid + 1 || layerid == tmplayerid - 1) {

      //    if(hit->GetTubeXYDistance(tmpposition) < STTPARALDISTANCE) { // CHECK
      cluster1.AddHit(hit);
      tmpposition = hit->GetPosition();
      tmptubeid = tubeid;
      if (secondhit == nullptr) {
        //	cout << "secondhit " << endl;
        secondhit = hit;
      }
      //      else cout << "sechit " << secondhit << endl;
      //      cout << "CLUS1: " << hit->GetHitID() << endl;
    }
  }

  //  cout << "CLUS2: " << tmphit->GetHitID() << endl;

  tmpposition = athit->GetPosition();
  for (int ihit = 0; ihit < hitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = (PndTrkHit *)hitlist->GetHit(ihit);
    //     cout << "athit " << athit << endl;
    //     cout << "hit " << hit << endl;
    //     cout << "secondhit " << secondhit <<  endl;
    if (*hit == *athit)
      continue;
    if (*hit == *secondhit)
      continue;

    if (hit->GetXYDistance(tmpposition) < STTPARALDISTANCE) { // CHECK
      cluster2.AddHit(hit);
      tmpposition = hit->GetPosition();
      //       cout << "CLUS2: " << hit->GetHitID() << endl;
    }
  }

  return kTRUE;
}

// CHECK if it works now
PndTrkClusterList PndTrkClean::Split(PndTrkCluster *cluster, std::vector<int> breakpoints)
{

  PndTrkClusterList list;
  if (breakpoints.size() == 0) {
    list.AddCluster(cluster);
    //     cout << "0add cluster to list " << cluster->GetNofHits() << endl;
    return list;
  }

  // 0 1 2 3 4 5 6 7 8 9
  // split @ ihit = 4 and 7
  // 4 - 0 --> 0123 456789
  // 7 - 4 --> 456  789

  PndTrkCluster cluster1, cluster2;
  PndTrkCluster *tmpcluster = cluster;
  for (size_t ihit = 0; ihit < breakpoints.size(); ihit++) {
    int hitno = breakpoints[ihit] - cluster1.GetNofHits();
    PndTrkHit *athit = tmpcluster->GetHit(hitno);
    SplitAtHit(tmpcluster, athit, cluster1, cluster2);
    tmpcluster = &cluster2;
    list.AddCluster(&cluster1);
    //     cout << "1add cluster to list " << cluster1.GetNofHits() << endl;
  }

  //   cout << "2add cluster to list " << cluster2.GetNofHits() << endl;
  list.AddCluster(&cluster2);
  tmpcluster = nullptr;
  delete tmpcluster;
  return list;
}

// SECTORS

int PndTrkClean::CheckSectorDistribution(PndTrkCluster *cluster)
{
  int sectorcounter[6] = {0, 0, 0, 0, 0, 0}; // CHECK hard coded nof sectors
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
    int sectorID = tube->GetSectorID();
    sectorcounter[sectorID]++;
  }

  int tmpsector = -1, tmpcounter = -1;
  for (int isec = 0; isec < 6; isec++) { // CHECK hard coded nof sectors
    if (sectorcounter[isec] > tmpcounter) {
      tmpcounter = sectorcounter[isec];
      tmpsector = isec;
    }
  }
  return tmpsector;
}

PndTrkCluster PndTrkClean::CleanSectors(PndTrkCluster *cluster, int sector)
{
  PndTrkCluster newcluster;
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
    int sectorID = tube->GetSectorID();
    if (fabs(sectorID - sector) < 1.2)
      newcluster.AddHit(hit);
  }
  return newcluster;
}

// MERGING

PndTrkClusterList PndTrkClean::MergeClusters(PndTrkClusterList *clusterlist)
{
  int nmax = clusterlist->GetNofClusters();

  cout << "MERGING on " << nmax << endl;
  std::vector<int> merged;
  PndTrkClusterList newclusterlist;
  for (int imax = 0; imax < nmax; imax++) {

    if (find(merged.begin(), merged.end(), imax) != merged.end())
      continue;

    PndTrkLegendreCluster *iclus = (PndTrkLegendreCluster *)clusterlist->GetCluster(imax);

    // dont try to merge 1 cluster alone
    if (nmax < 2) {
      newclusterlist.AddCluster(iclus);
      continue;
    }
    PndTrkLegendreCluster *merge = iclus;

    for (int jmax = imax + 1; jmax < nmax; jmax++) {
      cout << "COMPARING " << imax << " " << jmax << endl;
      PndTrkLegendreCluster *jclus = (PndTrkLegendreCluster *)clusterlist->GetCluster(jmax);
      bool isSimilar = iclus->IsSimilarTo(jclus);
      if (isSimilar) {
        cout << imax << " is similar to " << jmax << endl;
        merge->MergeTo(jclus); // CHECK  //int nhits =  //[R.K.03/2017] unused variable
        merged.push_back(jmax);
        cout << "MERGING " << iclus->GetNofHits() << " + " << jclus->GetNofHits() << " = " << merge->GetNofHits() << endl;
        //	iclus->Print();
        //	jclus->Print();
        //	merge->Print();
        cout << "theta/r " << merge->GetTheta() << " " << merge->GetR() << endl;
      }
    }

    newclusterlist.AddCluster(merge);
    cout << "ADD CLUSTER " << merge->GetNofHits() << endl;
  }
  cout << "FINAL CHECK " << newclusterlist.GetNofClusters() << endl;
  for (int iclus = 0; iclus < newclusterlist.GetNofClusters(); iclus++) {
    PndTrkLegendreCluster *cluster = (PndTrkLegendreCluster *)newclusterlist.GetCluster(iclus);
    cluster->Print();
    cout << "theta-r " << cluster->GetTheta() << " " << cluster->GetR() << endl;
  }

  return newclusterlist;
}

ClassImp(PndTrkClean)
