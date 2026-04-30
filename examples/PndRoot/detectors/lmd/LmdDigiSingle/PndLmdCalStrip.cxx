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

/* * PndLmdCalStrip.CXX
 * *  Created on: Jul 28, 2009
 *      Author: huagen
 */
//********************************************************************
// the implementation of PndLmdCalStrip class
//********************************************************************

#include "PndLmdCalStrip.h"
#include "TMath.h"
#include "TRandom.h"
#include "FairLogger.h"
#include <iostream>
#include <cmath>
#include "PndLmdStrips.h"
#include "PndLmdDigiPara.h"
using namespace std;

ClassImp(PndLmdCalStrip);

PndLmdCalStrip::PndLmdCalStrip()
{
  fPitch = 0;
  fOrient = 0;
  fAnchor = TVector2(0, 0);
  fThreshold = 0;
  fNoise = 0;
  fNrFEChannels = 0;
  fNrStrips = 0;
}

PndLmdCalStrip::PndLmdCalStrip(Double_t pitch, Double_t orient, Int_t nrStrips, Int_t nrFeChannels, const TVector2 &firstStripAnchor, Double_t threshold, Double_t noise,
                               Double_t sigma)
//:fPitch(pitch),fOrient(orient),fNrStrips(nrStrips),fNrFEChannels(nrFeChannels),
// fAnchor(firstStripAnchor),fThreshold(threshold),fNoise(noise)
{
  fPitch = pitch;
  fOrient = orient;
  fAnchor = firstStripAnchor;
  fThreshold = threshold;
  fNoise = noise;
  fNrFEChannels = nrFeChannels;
  fNrStrips = nrStrips;
  fSigma = sigma;

  fStripDir.Set(cos(fOrient), sin(fOrient));
  fOrthoDir.Set(sin(fOrient), cos(fOrient));
}

PndLmdCalStrip::PndLmdCalStrip(const PndLmdDigiPara *digipara, SensorSegment segment)
{
  if (segment == kLEFT) {
    fPitch = digipara->GetLeftPitch();
    fOrient = digipara->GetLeftOrient();
    fAnchor = digipara->GetLeftAnchor();
    fNrStrips = digipara->GetNrLeftFe() * digipara->GetNrFeChannels();

    fStripDir.Set(cos(fOrient), sin(fOrient));
    fOrthoDir.Set(-sin(fOrient), cos(fOrient));
  } else if (segment == kRIGHT) {
    fPitch = digipara->GetRightPitch();
    fOrient = digipara->GetRightOrient();
    fAnchor = digipara->GetRightAnchor();
    fNrStrips = digipara->GetNrRightFe() * digipara->GetNrFeChannels();

    fStripDir.Set(cos(fOrient), sin(fOrient));
    fOrthoDir.Set(sin(fOrient), -cos(fOrient));
  }

  fNrFEChannels = digipara->GetNrFeChannels();
  fThreshold = digipara->GetThreshold();
  fNoise = digipara->GetNoise();
  fSigma = digipara->GetGausSigma();
}

/*
//calculate the strips for curved strips
PndLmdCalStrip::PndLmdCalStrip(const PndLmdDigiPara* digipara, SensorSide side)
{ if(side == SensorSide::kTOP)
  fPitch = digipara->GetCirclePitch();
//	fOrient = digipara->GetRightOrient();
  fAnchor = digipara->GetCircleAnchor();
  fNrStrips = digipara->GetNrCircleFe()*digipara->GetNrFeChannels();

//	fStripDir.Set(con(fOrient),sin(fOrient));
//	fOrthoDir.Set(sin(fOrient),-cos(fOrient));

    fNrFEChannels = digipara->GetNrFeChannels();
    fThreshold = digipara->GetThreshold();
    fNoise = digipara->GetNoise();

  }
*/

std::vector<PndLmdStrips> PndLmdCalStrip::GetStrips(Double_t inx, Double_t iny, Double_t inz, Double_t outx, Double_t outy, Double_t outz, Double_t eLoss)
{ // 2d projection of trajectory
  TVector2 in(inx, iny);
  TVector2 out(outx, outy);
  TVector2 tpath = out - in;
  Double_t path = tpath.Mod();

  std::vector<PndLmdStrips> strips;
  // Double_t fract_Q;

  if (path < 1e-18) {
    LOG(warn) << " PndLmdCalStrip::GetStrips: No Trajectory inside sensor!" << tpath.Mod();
    return strips;
  }
  Double_t nuIn = CalStripFromPoint(inx, iny);
  Double_t nuOut = CalStripFromPoint(outx, outy);
  cout << "The nuIn strip is :" << nuIn << endl;
  cout << "The outIn strip is :" << nuOut << endl;

  Double_t Q = ChargeFromEloss(eLoss);

  // the strip number for electronics channel
  Int_t inStrip = Int_t(nuIn);
  Int_t outStrip = Int_t(nuOut);

  // did the particle hit the active area?
  if (nuIn < 0. && nuOut < 0. || nuIn > Double_t(fNrStrips) && nuOut > Double_t(fNrStrips)) {
    LOG(info) << " warning: Hit outside active area.";
    return strips;
  }

  // the track direction
  Double_t dir = (inStrip < outStrip) ? 1 : -1;

  // is the In-point inside the active area?
  if (nuIn < 0.) {
    Q *= (nuOut) / (nuOut - nuIn);
    nuIn = 0;
  } else if (nuIn > (Double_t(fNrStrips))) {
    Q *= ((Double_t)fNrStrips - nuIn) / (nuOut - nuIn);
    nuIn = Double_t(fNrStrips);
  }

  // is the Out-point inside the active area?
  if (nuOut < 0.) {
    Q *= (nuIn) / (-nuOut + nuIn);
    nuOut = 0;
  } else if (nuOut > (Double_t(fNrStrips))) {
    Q *= ((Double_t)fNrStrips - nuIn) / (nuOut - nuIn);
    nuOut = Double_t(fNrStrips);
  }

  // only 1 hit happened
  if (inStrip == outStrip) {
    // fract_Q = Q;
    strips.push_back(PndLmdStrips(inStrip, path, Q));

    //    	 cout<<"Only one hit happened and the strip fired is "<<inStrip<<endl;

  } else {
    Double_t dQ = Q / std::fabs(nuOut - nuIn);
    Double_t dPath = path / std::fabs(nuOut - nuIn);
    Int_t nrHits = 0;
    // calculate the portion of track in first strip
    Int_t nextIn = Int_t(nuIn + 0.5 + 0.5 * dir);
    Double_t path1 = std::fabs(nextIn - nuIn) * dPath;
    Double_t Q1 = dQ * path1;

    if (Q1 > fThreshold)
      strips.push_back(PndLmdStrips(inStrip, path1, Q1));
    nrHits++;
    Q -= Q1;

    // calculate the portion of track in the last strip
    Int_t prevOut = Int_t(nuOut + 0.5 + 0.5 * dir);
    Double_t path2 = std::fabs(nuOut - prevOut) * dPath;
    Double_t Q2 = dQ * path2;
    if (Q2 > fThreshold)
      strips.push_back(PndLmdStrips(outStrip, path2, Q2));
    nrHits++;
    Q -= Q2;

    // distribute the charge among the intermediate strips
    nextIn = Int_t(nextIn - 0.5 + 0.5 * dir);
    prevOut = Int_t(prevOut - 0.5 + 0.5 * dir);
    for (Int_t n = nextIn; n != prevOut; n += Int_t(dir)) {
      if (dQ > fThreshold)
        strips.push_back(PndLmdStrips(n, dPath, dQ));
      nrHits++;
      Q -= dQ;
      /*
      cout<<"more than 1 hits: the strips fired are :"
      <<"inStrip is"<<inStrip<<","
      <<"the nextIn is:"<<nextIn<<","
      <<"the prevOut is :"<<prevOut<<","
      <<"the outStrip is :"<< outStrip<<endl;
        */
    }
  } // for hits>1 loop

  return strips;
}

// the public method for strip digitization, which the charge diffusion was taken into account
std::vector<PndLmdStrips> PndLmdCalStrip::DigiStripFromPoint(Double_t inx, Double_t iny, Double_t inz, Double_t outx, Double_t outy, Double_t outz, Double_t eLoss)
{
  // call the GetStrips to initialize the strips
  std::vector<PndLmdStrips> strips = GetStrips(inx, iny, inz, outx, outy, outz, eLoss);
  cout << " Number of strips initially fired :" << strips.size() << endl;

  // the charge of the whole path
  Double_t total_Q = ChargeFromEloss(eLoss);

  // Strip numbers
  Double_t nuIn = CalStripFromPoint(inx, iny);
  Double_t nuOut = CalStripFromPoint(outx, outy);

  // cout<<"the entry strip and exit strip are :"<<nuIn<<","<<nuOut<<endl;

  // Strip number for FE channel
  Int_t inStrip = Int_t(nuIn);
  Int_t outStrip = Int_t(nuOut);

  // path direction
  Double_t dir = (nuIn < nuOut) ? 1 : -1;

  // the path portion on the first and last strip
  Double_t Path1 = (nuIn - inStrip) * fPitch;
  Double_t Path2 = (nuOut - outStrip) * fPitch;

  // path projection
  Double_t path = std::fabs(nuOut - nuIn) * fPitch;
  Int_t size = strips.size();

  // digitize the strips
  // only 1 strip fired
  if (size == 1) {
    PndLmdStrips strip = strips.at(0);
    Double_t path_real = strip.GetPath();
    Double_t Q = strip.GetCharge();
    Int_t index = strip.GetIndex();
    strips.clear();

    //   cout<<"the information of strip fired including:"<<index<<","<<path_real<<","<<Q<<endl;

    Double_t charge1, charge2, dQ, dQR, dQL;
    Int_t id;
    Double_t q_head, q_tail;
    // charge diffusion according to the gaussian smearing
    // between the 3 strips of index-1, index, and index+1
    Double_t QL = ChargeDiffusion(0, Path1, path, dir, Q);          // the diffusion charge from -infinite to index
    Double_t QR = Q - ChargeDiffusion(fPitch, Path1, path, dir, Q); // the diffusion charge from index to infinite
    Double_t QM = Q - QR - QL;                                      // the charge left on the strip index

    //	  cout<<"the QL is "<<QL<<endl;
    //	  cout<<"the QR is "<<QR<<endl;
    //	  cout<<"the fNoise is :"<<fNoise<<endl;

    // noise smearing for the QL,QR and QM
    dQR = AddNoise(QR);
    dQL = AddNoise(QL);
    cout << "the QL was smeared by noise :" << dQL << endl;
    cout << "the QR was smeared by noise :" << dQR << endl;

    // is it only 1 strip involved
    if (dQL < fThreshold && dQR < fThreshold) {
      dQ = AddNoise(QM);
      if (QM >= fThreshold)
        strips.push_back(PndLmdStrips(index, path_real, dQ));
      // cout<<"the dQ for the center strip is "<<dQ<<endl;

    } else {
      Int_t i = 0;
      Int_t j = 0;

      if (dQL >= fThreshold) {
        // charge distribution at the head of the track
        charge1 = ChargeDiffusion((i - 1) * fPitch, Path1, path, dir, Q); // charge distribution from -infinite to strip of index-1
        charge2 = ChargeDiffusion(i * fPitch, Path1, path, dir, Q);
        q_head = charge2 - charge1;
        id = index + i - 1;
        dQ = AddNoise(q_head);
        if (dQ > fThreshold)
          strips.push_back(PndLmdStrips(id, 0, dQ));
      }

      if (dQR >= fThreshold) {
        // charge distribution at the head of the track
        charge1 = ChargeDiffusion(j * fPitch, Path1, path, dir, Q); // charge distribution from -infinite to strip of index-1
        charge2 = ChargeDiffusion((j + 1) * fPitch, Path1, path, dir, Q);
        q_tail = charge2 - charge1;
        id = index + j;
        dQ = AddNoise(q_tail);
        if (dQ > fThreshold)
          strips.push_back(PndLmdStrips(id, 0, dQ));
      }

      // the charge left on the center strip
      Double_t restQ = ChargeDiffusion(fPitch, Path1, path, dir, Q) - ChargeDiffusion(0, Path1, path, dir, Q);
      strips.push_back(PndLmdStrips(index, path_real, AddNoise(restQ)));
    }

  } // for size == 1
    //  else for the size>1, means the number of strips fired greater than 1
  else {
    // for the strips
    Double_t head_Path, tail_Path;
    std::vector<PndLmdStrips> strip_head;
    std::vector<PndLmdStrips> strip_tail;
    std::vector<PndLmdStrips> strip_middle;
    PndLmdStrips str_head;
    PndLmdStrips str_tail;
    PndLmdStrips str_middle;

    if (inStrip < outStrip) {
      str_head = strips.at(0);
      str_tail = strips.at(size - 1);
      head_Path = Path1;
      tail_Path = Path2 + (size - 1) * fPitch;

      for (Int_t i = 1; i < size - 1; i++) {
        str_middle = strips.at(i);
      }
    } else {
      str_tail = strips.at(0);
      str_head = strips.at(size - 1);
      head_Path = Path2;
      tail_Path = Path1 + (size - 1) * fPitch;

      for (Int_t i = size - 2; i > 0; i--) {
        str_middle = strips.at(i);
      }
    }
    strips.erase(strips.begin(), strips.begin() + size);

    // strips initialization
    Int_t index_head = str_head.GetIndex();
    Double_t path_head = str_head.GetPath();
    // Double_t q_head = str_head.GetCharge();

    Int_t index_tail = str_tail.GetIndex();
    Double_t path_tail = str_tail.GetPath();
    //    	Double_t q_tail = str_tail.GetCharge();

    // for the left strip of the head strip
    Double_t charge1, charge2, dQ;
    Int_t id;
    Double_t ipath;
    // Double_t q_head,q_tail;
    Double_t dq_head = ChargeDiffusion(0, head_Path, path, dir, total_Q);                              // the diffusion charge from -infinite to index
    Double_t dq_tail = total_Q - ChargeDiffusion(size * fPitch, tail_Path, path, (-1) * dir, total_Q); // the diffusion charge from index+size to infinite
    Double_t q_head = ChargeDiffusion(fPitch, head_Path, path, dir, total_Q);                          // the charge left on the strip index
    Double_t q_tail = ChargeDiffusion(size * fPitch, tail_Path, path, (-1) * dir, total_Q) - ChargeDiffusion((size - 1) * fPitch, tail_Path, path, (-1) * dir, total_Q);

    Double_t dQ_head = AddNoise(dq_head);
    Double_t dQ_tail = AddNoise(dq_tail);
    Double_t Q_head = AddNoise(q_head);
    Double_t Q_tail = AddNoise(q_tail);

    if (dQ_head > fThreshold) {
      id = index_head - 1;
      strip_head.push_back(PndLmdStrips(id, 0, dQ_head));
    }
    if (Q_head > fThreshold) {
      id = index_head;
      strip_head.push_back(PndLmdStrips(id, path_head, Q_head));
    }
    strips.insert(strips.begin(), strip_head.begin(), strip_head.end());

    if (dQ_tail > fThreshold) {
      id = index_tail + 1;
      strip_tail.push_back(PndLmdStrips(id, 0, dQ_tail));
    }
    if (Q_tail > fThreshold) {
      id = index_tail;
      strip_head.push_back(PndLmdStrips(id, path_tail, Q_tail));
    }
    strips.insert(strips.end(), strip_tail.begin(), strip_tail.end());

    // for the strips between the head and the tail
    for (Double_t i = 1; i < size - 1; i++) {
      charge1 = ChargeDiffusion(i * fPitch, head_Path, path, dir, total_Q);
      charge2 = ChargeDiffusion((i + 1) * fPitch, head_Path, path, dir, total_Q);
      dQ = charge2 - charge1;
      std::vector<PndLmdStrips>::iterator iter_ii = strip_middle.begin();
      id = iter_ii->GetIndex();
      ipath = iter_ii->GetPath();
      if (dQ > fThreshold)
        strip_middle.push_back(PndLmdStrips(id, ipath, dQ));
      iter_ii++;
    } //
    strips.insert(strips.begin() + strip_head.size(), strip_middle.begin(), strip_middle.begin() + strip_middle.size());

  } // for size>1
  return strips;

} // digitize the strips

Double_t PndLmdCalStrip::ChargeFromEloss(Double_t eloss) const
{
  return eloss / 3.61 * 1e9;
}

// calculator the strips from the anchor point
Double_t PndLmdCalStrip::CalStripFromPoint(Double_t x, Double_t y)
{
  return ((x - fAnchor.X()) * fOrthoDir.X() + (y - fAnchor.Y()) * fOrthoDir.Y()) / fPitch;
}

Double_t PndLmdCalStrip::CalCurveStripFromPoint(Double_t x, Double_t y)
{
  return (std::sqrt((x - fAnchor.X()) * (x - fAnchor.X()) + (y - fAnchor.Y()) * (y - fAnchor.Y())) - 30.0) / fPitch;
}

Int_t PndLmdCalStrip::CalChannelFromStrip(Int_t stripNr)
{
  return stripNr % fNrFEChannels;
}

Int_t PndLmdCalStrip::CalFeFromStrip(Int_t stripNr)
{
  return stripNr / fNrFEChannels;
}

// add the noise to the signal with gaussian distribution
Double_t PndLmdCalStrip::AddNoise(Double_t charge)
{
  Double_t smeared = fRNG->Gaus(charge, fNoise);
  return smeared;
}

// charge diffusion during collecting after ionization,
// here, the u means the range which approaching the mean value a.
// the area was expressed by the Erf function as below
// f(x) = 1/2*(1+TMath::Erf((u-a)/sigma*TMath::sqrt(2.))))
Double_t PndLmdCalStrip::ChargeDiffusion(Double_t u, Double_t a, Double_t path, Double_t dir, Double_t Q)
{
  Int_t N = 150;
  Double_t Npath = path / N;
  Double_t NQ = Q / N;
  Double_t Delta;  // the area of Gaussian from (-infinite,u)
  Double_t dQ = 0; // the Q in the given range
  if (path == 0.) {
    Delta = 0.5 * (1 + TMath::Erf((u - a) / fSigma * std::sqrt(2.)));
    dQ = NQ * Delta;
  } else {
    if (dir > 0.) {
      for (Double_t da = a; da <= a + path; da += Npath) {
        Delta = 0.5 * (1 + TMath::Erf((u - da) / fSigma * std::sqrt(2.)));
        dQ += NQ * Delta;
      }
    }
    if (dir < 0) {
      for (Double_t da = a - path; da <= a; da += Npath) {
        Delta = 0.5 * (1 + TMath::Erf((u - da) / fSigma * std::sqrt(2.)));
        dQ += NQ * Delta;
      }
    } // dir<<0

  } // path>0
  return dQ;
}

void PndLmdCalStrip::print() const
{
  LOG(info) << " PndLmdCalStrip Info :";
  std::cout << "     pitch                  = " << fPitch * 10000. << " um" << std::endl;
  std::cout << "     orientation angle      = " << fOrient / TMath::Pi() * 180. << " deg" << std::endl;
  std::cout << "     nr of strips           = " << fNrStrips << std::endl;
  std::cout << "     nr of channels per FE  = " << fNrFEChannels << std::endl;
  std::cout << "     nr of frontends        = " << fNrStrips / fNrFEChannels << std::endl;
  std::cout << "     anchor point           = (" << fAnchor.X() << "," << fAnchor.Y() << ")" << std::endl;
  std::cout << "     strip-direction vector = (" << fStripDir.X() << "," << fStripDir.Y() << ")" << std::endl;
}
