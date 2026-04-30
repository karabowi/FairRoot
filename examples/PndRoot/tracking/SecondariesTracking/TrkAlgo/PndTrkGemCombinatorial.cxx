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

////////////////////////////////////////////////////////////
//
// PndTrkGemCombinatorial
//
// Class for GEM combinatorial suppression
//
// authors: Lia Lavezzi - University of Torino (2014)
//
////////////////////////////////////////////////////////////

#include "PndTrkGemCombinatorial.h"

#include "PndTrkTools.h"

// fairroot
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
// ROOT
#include "TClonesArray.h"
#include "TVector3.h"
#include "TMarker.h"

#include "PndGemHit.h"
#include "PndGemMCPoint.h"

#include <iostream>

using namespace std;

int nhits = 0, npoints = 0, nassocorrect = 0, nassowrong = 0, ndontusecorrect = 0, ndontusewrong = 0, nasso = 0, nunasso = 0;

// -----   Default constructor   -------------------------------------------
PndTrkGemCombinatorial::PndTrkGemCombinatorial() : fGemHitArray(nullptr), fGemPointArray(nullptr), fDisplayOn(kFALSE), fMCEval(kFALSE) {}

PndTrkGemCombinatorial::PndTrkGemCombinatorial(TClonesArray *gemhitarray, int verbose)
  : fGemHitArray(gemhitarray), fGemPointArray(nullptr), fDisplayOn(kFALSE), fMCEval(kFALSE), fVerbose(verbose)
{
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrkGemCombinatorial::~PndTrkGemCombinatorial()
{

  delete fGemHitArray;
  delete fGemPointArray;
  delete hxy;
  delete hxy1;
  delete hxy2;
  delete hxy3;
  delete display;
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
void PndTrkGemCombinatorial::SwitchOnDisplay()
{
  fDisplayOn = kTRUE;
  display = new TCanvas("display", "display", 0, 0, 800, 800); // CHECK
  display->Divide(2, 2);
  hxy = new TH2F("hxy", "xy plane", 130, -65, 65, 130, -65, 65);
  hxy1 = new TH2F("hxy1", "xy plane 1", 76, -38, 38, 76, -38, 38);
  hxy2 = new TH2F("hxy2", "xy plane 2", 94, -47, 47, 94, -47, 47);
  hxy3 = new TH2F("hxy3", "xy plane 3", 130, -65, 65, 130, -65, 65);
}

// -------------------------------------------------------------------------
std::map<int, bool> PndTrkGemCombinatorial::CombinatorialSuppression()
{
  std::map<int, bool> hitTousability;

  if (fMCEval == kTRUE && fGemPointArray == nullptr) {
    cerr << "PndTrkGemCombinatorial::CombinatorialSuppression youwant evaluate performances but you did not set the mc TCA!" << endl;
    return hitTousability;
  }

  if (fVerbose)
    cout << "---------------------- " << endl;
  double distlimit = 999;
  // fill maps  ===================================
  // hitid    --> position (0...5)
  // position --> hitid
  std::map<int, int> hitidTopos;
  std::multimap<int, int> posTohitid;
  std::multimap<int, int> posTohitid1;
  std::multimap<int, int> posTohitid2;

  if (fDisplayOn) {
    DrawGeometry();
    if (fMCEval)
      DrawMCPoints();
  }

  nhits += fGemHitArray->GetEntriesFast();
  if (fMCEval)
    npoints += fGemPointArray->GetEntriesFast();

  for (int ihit = 0; ihit < fGemHitArray->GetEntriesFast(); ihit++) {
    PndGemHit *hit = (PndGemHit *)fGemHitArray->At(ihit);

    if (fMCEval) {
      if (hit->GetRefIndex() == -1)
        nunasso++;
      else
        nasso++;
    }

    // statid = 1, 2, 3
    // sensid = 1, 2
    // posid = [sensid + 2 * (statid - 1)] - 1
    // --> position 0, 1, 2, 3, 4, 5

    int statid = hit->GetStationNr();
    int sensid = hit->GetSensorNr();
    int posid = (sensid + 2 * (statid - 1)) - 1;

    //     if(fVerbose) cout << ihit << " hit is on position " << posid << endl;
    hitidTopos[ihit] = posid;
    posTohitid.insert(std::pair<int, int>(posid, ihit));

    // two separate lists for first and second sensor on each station
    if (sensid == 1)
      posTohitid1.insert(std::pair<int, int>(posid, ihit));
    else
      posTohitid2.insert(std::pair<int, int>(posid, ihit));

    if (fDisplayOn) {
      TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 4);
      display->cd(statid);
      mrk->Draw("SAME");

      display->cd(4);
      mrk->Draw("SAME");
      display->Update();
      display->Modified();
    }
  }

  //  std::multimap< int, int >::iterator it;
  //   for (it = posTohitid.begin(); it != posTohitid.end(); ++it) {
  //     if(fVerbose) cout << "position " << (*it).first << " has hit => " << (*it).second << endl;
  //   }
  // ==============================================

  // loop on "first" planes on each station 0, 2, 4
  // and for each of them loop on the corresponding +1
  std::multimap<int, int>::iterator it1;
  std::multimap<int, int>::iterator it2;

  std::multimap<int, std::pair<int, double>> hit1Topairhit2_distance;
  std::multimap<int, std::pair<int, double>> hit2Topairhit1_distance;
  std::vector<int> alone1;
  std::vector<int> alone2;
  PndGemHit *hit1 = nullptr, *hit2 = nullptr;
  for (it1 = posTohitid1.begin(); it1 != posTohitid1.end(); ++it1) {
    int posid1 = (*it1).first;
    int posid2 = posid1 + 1;
    int hitid1 = (*it1).second;
    hit1 = (PndGemHit *)fGemHitArray->At(hitid1);
    TVector2 pos1 = hit1->GetPosition().XYvector();

    std::pair<std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
    ret = posTohitid2.equal_range(posid2);

    for (it2 = ret.first; it2 != ret.second; ++it2) {
      //        if(fVerbose) cout << "--- accoppiamenti1 " <<   (*it1).first << " " <<  (*it1).second << endl;
      //        if(fVerbose) cout << "+++ accoppiamenti2 " <<   (*it2).first << " " <<  (*it2).second << endl;
      int hitid2 = (*it2).second;
      if (hitid1 == hitid2)
        continue;
      hit2 = (PndGemHit *)fGemHitArray->At(hitid2);
      TVector2 pos2 = hit2->GetPosition().XYvector();

      // now we have hit1 and hit2 and their distance in xy
      double distancexy = (pos2 - pos1).Mod();
      std::pair<int, double> hit2_distance(hitid2, distancexy);
      hit1Topairhit2_distance.insert(std::pair<int, std::pair<int, double>>(hitid1, hit2_distance));
      std::pair<int, double> hit1_distance(hitid1, distancexy);
      hit2Topairhit1_distance.insert(std::pair<int, std::pair<int, double>>(hitid2, hit1_distance));
    }

    if (hit1Topairhit2_distance.count(hitid1) == 0)
      alone1.push_back(hitid1);
    if (fVerbose)
      cout << "---> pos 1 hit " << hitid1 << " has counts " << hit1Topairhit2_distance.count(hitid1) << endl;
  }

  for (size_t ihit = 0; ihit < alone1.size(); ihit++) {
    hit1 = (PndGemHit *)fGemHitArray->At(alone1[ihit]);
    if (fVerbose)
      cout << "**********alone1 " << alone1[ihit] << " " << hit1->GetRefIndex() << endl;
  }

  for (it2 = posTohitid2.begin(); it2 != posTohitid2.end(); ++it2) {
    int hitid2 = (*it2).second;
    if (hit2Topairhit1_distance.count(hitid2) == 0)
      alone2.push_back(hitid2);
    if (fVerbose)
      cout << "---> pos 2 hit " << hitid2 << " has counts " << hit2Topairhit1_distance.count(hitid2) << endl;
  }

  for (size_t ihit = 0; ihit < alone2.size(); ihit++) {
    hit2 = (PndGemHit *)fGemHitArray->At(alone2[ihit]);
    if (fVerbose)
      cout << "**********alone2 " << alone2[ihit] << " " << hit2->GetRefIndex() << endl;
  }

  // create a chosenmap of chosen couples hit1 - hit2
  std::multimap<int, std::pair<int, double>>::iterator iter;
  int tmphitid1 = -1;
  int tmphitid2 = -1;

  int tmpdigi1[2] = {-1, -1};
  int tmpdigi2[2] = {-1, -1};

  double tmpdistance = 1000;
  std::map<int, std::pair<int, double>> chosenmap;
  std::multimap<int, int> digiTohitid1;
  std::multimap<int, int> digiTohitid2;
  for (iter = hit1Topairhit2_distance.begin(); iter != hit1Topairhit2_distance.end(); ++iter) {

    int hitid1 = (*iter).first;
    int hitid2 = ((*iter).second).first;
    double distance = ((*iter).second).second;
    if (fVerbose)
      cout << "hit1: " << hitid1 << " associated to hit2: " << hitid2 << " with dist " << distance << endl;

    if (tmphitid1 == -1)
      tmphitid1 = hitid1;

    if (hitid1 == tmphitid1) { // comparison
      if (distance < tmpdistance) {
        tmpdistance = distance;
        tmphitid2 = hitid2;
        hit1 = (PndGemHit *)fGemHitArray->At(tmphitid1);
        hit2 = (PndGemHit *)fGemHitArray->At(tmphitid2);
        tmpdigi1[0] = hit1->GetDigiNr(0);
        tmpdigi1[1] = hit1->GetDigiNr(1);
        tmpdigi2[0] = hit2->GetDigiNr(0);
        tmpdigi2[1] = hit2->GetDigiNr(1);
      }
    } else {                         // new one
      if (tmpdistance < distlimit) { // 1 cm is the limit
        if (fVerbose)
          cout << "chosen " << tmphitid1 << " " << tmphitid2 << " " << tmpdistance << endl;
        chosenmap[tmphitid1] = std::pair<int, double>(tmphitid2, tmpdistance);
        chosenmap[tmphitid2] = std::pair<int, double>(tmphitid1, tmpdistance);
        // 	if(fVerbose) cout << "digi1 " <<  tmpdigi1[0] << " " << tmpdigi1[1] << endl;
        // 	if(fVerbose) cout << "digi2 " <<  tmpdigi2[0] << " " << tmpdigi2[1] << endl;

        digiTohitid1.insert(std::pair<int, int>(tmpdigi1[0], tmphitid1));
        digiTohitid1.insert(std::pair<int, int>(tmpdigi1[1], tmphitid1));
        digiTohitid2.insert(std::pair<int, int>(tmpdigi2[0], tmphitid2));
        digiTohitid2.insert(std::pair<int, int>(tmpdigi2[1], tmphitid2));
      }
      tmpdistance = distance;
      tmphitid2 = hitid2;
      tmphitid1 = hitid1;
      hit1 = (PndGemHit *)fGemHitArray->At(tmphitid1);
      hit2 = (PndGemHit *)fGemHitArray->At(tmphitid2);
      tmpdigi1[0] = hit1->GetDigiNr(0);
      tmpdigi1[1] = hit1->GetDigiNr(1);
      tmpdigi2[0] = hit2->GetDigiNr(0);
      tmpdigi2[1] = hit2->GetDigiNr(1);
    }
  }

  // last one
  if (tmpdistance < distlimit) { // 1 cm is the limit
    if (fVerbose)
      cout << "chosen " << tmphitid1 << " " << tmphitid2 << " " << tmpdistance << endl;
    chosenmap[tmphitid1] = std::pair<int, double>(tmphitid2, tmpdistance);
    chosenmap[tmphitid2] = std::pair<int, double>(tmphitid1, tmpdistance);
    //    if(fVerbose) cout << "digi1 " <<  tmpdigi1[0] << " " << tmpdigi1[1] << endl;
    //     if(fVerbose) cout << "digi2 " <<  tmpdigi2[0] << " " << tmpdigi2[1] << endl;

    digiTohitid1.insert(std::pair<int, int>(tmpdigi1[0], tmphitid1));
    digiTohitid1.insert(std::pair<int, int>(tmpdigi1[1], tmphitid1));
    digiTohitid2.insert(std::pair<int, int>(tmpdigi2[0], tmphitid2));
    digiTohitid2.insert(std::pair<int, int>(tmpdigi2[1], tmphitid2));
  }

  std::map<int, int>::iterator ditr;
  int tmpdigiid = -1;

  std::map<int, std::pair<int, double>> chosenmap2;
  for (ditr = digiTohitid1.begin(); ditr != digiTohitid1.end(); ++ditr) {
    int digiid = (*ditr).first;
    if (digiid == tmpdigiid)
      continue;

    tmpdigiid = digiid;
    int ntimes = digiTohitid1.count(digiid);
    if (ntimes > 1) {
      std::pair<std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret2;
      ret2 = digiTohitid1.equal_range(digiid);
      std::multimap<int, int>::iterator itr3;

      int tmphit1 = -1, tmphit2 = -1;
      double tmpdist = 1000;
      for (itr3 = ret2.first; itr3 != ret2.second; ++itr3) {
        int hitid1 = (*itr3).second;
        int hitid2 = chosenmap[hitid1].first;
        double distance = chosenmap[hitid1].second;
        if (distance < tmpdist) {
          tmphit1 = hitid1;
          tmphit2 = hitid2;
          tmpdist = distance;
        }
        //  	if(fVerbose) cout << "DIGI " << digiid << " ntimes " << ntimes << " hit1 " << hitid1 << " hit2 " << hitid2 << " distance " << distance << endl;
      }
      chosenmap2[tmphit1] = std::pair<int, double>(tmphit2, tmpdist);
      chosenmap2[tmphit2] = std::pair<int, double>(tmphit1, tmpdist);
    } else {
      int hitid1 = (*ditr).second;
      int hitid2 = chosenmap[hitid1].first;
      double distance = chosenmap[hitid1].second;
      chosenmap2[hitid1] = std::pair<int, double>(hitid2, distance);
      chosenmap2[hitid2] = std::pair<int, double>(hitid1, distance);
    }
  }

  std::map<int, std::pair<int, double>>::iterator citr;
  for (citr = chosenmap2.begin(); citr != chosenmap2.end(); ++citr) {
    if (fVerbose)
      cout << (*citr).first << " finally associated to " << ((*citr).second).first << " with dist " << ((*citr).second).second << endl;

    if (fMCEval) {
      PndGemHit *hitA = (PndGemHit *)fGemHitArray->At((*citr).first);
      //     if(fVerbose) cout << "digi " << hit->GetDigiNr(0) << " " << hit->GetDigiNr(1) << endl;

      if (hitA->GetRefIndex() == -1)
        nassowrong++;
      else
        nassocorrect++;
    }
  }

  std::vector<int> dontuse;
  for (int ihit = 0; ihit < fGemHitArray->GetEntriesFast(); ihit++) {
    hitTousability[ihit] = true;
    if (chosenmap2.count(ihit) > 0)
      continue;
    dontuse.push_back(ihit);
    hitTousability[ihit] = false;
  }

  for (size_t ihit = 0; ihit < dontuse.size(); ihit++) {
    PndGemHit *hit = (PndGemHit *)fGemHitArray->At(dontuse[ihit]);
    if (fVerbose)
      cout << "dontuse " << dontuse[ihit] << " " << hit->GetRefIndex() << endl;

    if (fMCEval) {
      if (hit->GetRefIndex() == -1)
        ndontusecorrect++;
      else
        ndontusewrong++;
    }

    if (fDisplayOn) {
      TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 4);
      display->cd(hit->GetStationNr());
      mrk->SetMarkerColor(kRed);
      mrk->Draw("SAME");

      display->cd(4);
      mrk->Draw("SAME");
      display->Update();
      display->Modified();
    }
  }

  if (fDisplayOn) {
    char goOnChar;
    cin >> goOnChar;
  }
  if (fMCEval) {
    if (fVerbose > 2) {
      cout << "TOTAL HITS " << nhits << " TOTAL MC POINTS " << npoints << endl;
      cout << "ASSO " << nasso << " UN-ASSO " << nunasso << endl;
      cout << "dontuse WRONG " << ndontusewrong << " RIGHT " << ndontusecorrect << endl;
      cout << "asso    WRONG " << nassowrong << " RIGHT " << nassocorrect << endl;
      cout << endl;
    }

    //   if(fVerbose)
    {
      cout << "TOTAL number of hits =  " << nhits << " of which " << 100. * nasso / nhits << "% TRUE, " << 100. * nunasso / nhits << "% FAKE" << endl;
      cout << endl;
      cout << "correctly assiged over the ones which should be " << 100. * nassocorrect / nasso << "%, of the totally assigned "
           << 100. * nassocorrect / (nassowrong + nassocorrect) << "%" << endl;
      cout << "wrongly assiged over the totally assigned " << 100. * nassowrong / (nassowrong + nassocorrect) << "%" << endl;
      cout << endl;
      cout << "correctly NOT assiged over the ones which should not " << 100. * ndontusecorrect / nunasso << "%, of the totally un-assigned "
           << 100. * ndontusecorrect / (ndontusecorrect + ndontusewrong) << "%" << endl;
      cout << "wrongly NOT assiged over the totally un-assigned " << 100. * ndontusewrong / (ndontusecorrect + ndontusewrong) << "%" << endl;
    }
  }

  return hitTousability;
}

void PndTrkGemCombinatorial::DrawGeometry()
{

  if (hxy == nullptr)
    hxy = new TH2F("hxy", "xy plane", 110, -55, 55, 110, -55, 55);
  else
    hxy->Reset();
  if (hxy1 == nullptr)
    hxy1 = new TH2F("hxy1", "xy plane 1", 110, -55, 55, 110, -55, 55);
  else
    hxy1->Reset();
  if (hxy2 == nullptr)
    hxy2 = new TH2F("hxy2", "xy plane 2", 110, -55, 55, 110, -55, 55);
  else
    hxy2->Reset();
  if (hxy3 == nullptr)
    hxy3 = new TH2F("hxy3", "xy plane 3", 110, -55, 55, 110, -55, 55);
  else
    hxy3->Reset();
  display->cd(1);
  hxy1->SetStats(kFALSE);
  hxy1->Draw();
  display->cd(2);
  hxy2->SetStats(kFALSE);
  hxy2->Draw();
  display->cd(3);
  hxy3->SetStats(kFALSE);
  hxy3->Draw();
  display->cd(4);
  hxy->SetStats(kFALSE);
  hxy->Draw();

  display->Update();
  display->Modified();
}

void PndTrkGemCombinatorial::DrawMCPoints()
{
  for (int ipnt = 0; ipnt < fGemPointArray->GetEntriesFast(); ipnt++) {
    PndGemMCPoint *point = (PndGemMCPoint *)fGemPointArray->At(ipnt);
    int sensid = point->GetSensorId();
    int statid = 0;

    // CHECK these are not fine
    if (sensid == 273 || sensid == 289)
      statid = 1;
    else if (sensid == 529 || sensid == 545)
      statid = 2;
    if (sensid == 785 || sensid == 801)
      statid = 3;

    TVector3 pos;
    point->Position(pos);
    TMarker *mrk = new TMarker(pos.X(), pos.Y(), 3);
    display->cd(statid);
    mrk->SetMarkerColor(kBlue);
    mrk->Draw("SAME");

    display->cd(4);
    mrk->Draw("SAME");
    display->Update();
    display->Modified();
  }
}

ClassImp(PndTrkGemCombinatorial)
