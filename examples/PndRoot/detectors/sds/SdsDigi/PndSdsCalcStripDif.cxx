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
// C++ Implementation: PndSdsCalcStrip
//
// Description:
//
//
// Author: HG Zaunick <hg.zaunick@physik.tu-dresden.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <cmath>

#include "PndSdsCalcStripDif.h"
#include "TRandom.h"
#include "FairLogger.h"

PndSdsCalcStripDif::PndSdsCalcStripDif()
{
  fPitch = 0.;
  fOrient = 0.;
  fAnchor = TVector2(0., 0.);
  fNrStrips = 0;
  fThreshold = 0.;
  fNoise = 0.;
  fVerboseLevel = 0;
}

PndSdsCalcStripDif::PndSdsCalcStripDif(Double_t pitch, Double_t orient, Int_t nrStrips, Int_t nrFeChannels, const TVector2 &firstStripAnchor, Double_t threshold, Double_t noise,
                                       Double_t sigma)
  : fPitch(pitch), fOrient(orient), fNrStrips(nrStrips), fNrFeChannels(nrFeChannels), fAnchor(firstStripAnchor), fThreshold(threshold), fNoise(noise)
{
  fStripDir.Set(cos(fOrient), sin(fOrient));
  fOrthoDir.Set(sin(fOrient), -cos(fOrient));
  fVerboseLevel = 0;
  fSigma = sigma;
  // Print();
}

PndSdsCalcStripDif::PndSdsCalcStripDif(const PndSdsStripDigiPar *digipar, SensorSide side)
{
  if (side == SensorSide::kTOP) {
    fPitch = digipar->GetTopPitch();
    fOrient = digipar->GetOrient();
    fAnchor = digipar->GetTopAnchor();
    fNrStrips = digipar->GetNrTopFE() * digipar->GetNrFECh();
  } else if (side == SensorSide::kBOTTOM) {
    fPitch = digipar->GetBotPitch();
    fOrient = digipar->GetOrient() + digipar->GetSkew();
    fAnchor = digipar->GetBotAnchor();
    fNrStrips = digipar->GetNrBotFE() * digipar->GetNrFECh();
  }
  fNrFeChannels = digipar->GetNrFECh();
  fThreshold = digipar->GetThreshold();
  fNoise = digipar->GetNoise();

  fStripDir.Set(cos(fOrient), sin(fOrient));
  fOrthoDir.Set(sin(fOrient), -cos(fOrient));
  fVerboseLevel = 0;
  if (fVerboseLevel > 0)
    Print();
}

std::vector<PndSdsStrip> PndSdsCalcStripDif::GetStrips(Double_t inx, Double_t iny, Double_t inz, Double_t outx, Double_t outy, Double_t outz, Double_t eLoss)
{
  if (fVerboseLevel > 2)
    LOG(info) << " PndSdsCalcStrip::GetStrips ";

  // 2d-Projection of trajectory
  TVector2 in(inx, iny);
  TVector2 out(outx, outy);
  TVector2 path = out - in;

  if (fVerboseLevel > 2) {
    std::cout << " InPoint: (" << in.X() << "," << in.Y() << ")" << std::endl;
    std::cout << " OutPoint: (" << out.X() << "," << out.Y() << ")" << std::endl;
  }

  std::vector<PndSdsStrip> strips;
  // Double_t smearedQ;

  if (path.Mod() < 1E-18) {
    LOG(warn) << " PndSdsCalcStrip::GetStrips : No Trajectory inside Sensor! (out-in).Mod() = " << path.Mod();
    return strips;
  }

  if (fVerboseLevel > 1)
    std::cout << " pathlength: " << path.Mod() << std::endl;

  Double_t nuIn = CalcStripFromPoint(inx, iny);
  Double_t nuOut = CalcStripFromPoint(outx, outy);

  if (fVerboseLevel > 2)
    std::cout << " nuIn = " << nuIn << " ; nuOut = " << nuOut << std::endl;

  Double_t Q = ChargeFromEloss(eLoss); //*1E9/3.61; // 3.6 eV/Electron in Silicon
  if (fVerboseLevel > 1)
    std::cout << " integral charge = " << Q << std::endl;

  // did we hit the active area ?
  //     if ( (nuIn<0.5 && nuOut<0.5) || (((nuIn+0.5) > Double_t(fNrStrips-1)) && ((nuOut+0.5) > Double_t(fNrStrips-1)))){
  if ((nuIn < 0. && nuOut < 0.) || ((nuIn > Double_t(fNrStrips)) && (nuOut > Double_t(fNrStrips)))) {
    if (fVerboseLevel > 1)
      LOG(warn) << " PndSdsCalcStrip::GetStrips: Hit outside active area.";
    return strips;
  }

  // is the In-Point inside active area ?
  // only charge fraction inside active area taken
  if (nuIn < 0.) {
    Q *= (nuOut) / (nuOut - nuIn);
    nuIn = 0.;
  } else if (nuIn > (Double_t(fNrStrips))) {
    Q *= ((Double_t)fNrStrips - nuOut) / (-nuOut + nuIn);
    nuIn = Double_t(fNrStrips);
  }

  // is the Out-Point inside active area ?
  if (nuOut < 0.) {
    Q *= (nuIn) / (-nuOut + nuIn);
    nuOut = 0.;
  } else if (nuOut > (Double_t(fNrStrips))) {
    Q *= ((Double_t)fNrStrips - nuIn) / (nuOut - nuIn);
    nuOut = Double_t(fNrStrips);
  }

  // path direction
  Double_t dir = (nuIn < nuOut) ? 1 : -1;

  // Strip number for FE channel
  Int_t inStrip = Int_t(nuIn);
  Int_t outStrip = Int_t(nuOut);

  /// the path portion on the first and last strip
  Double_t Path1 = (nuIn - inStrip) * fPitch;
  Double_t Path2 = (nuOut - outStrip) * fPitch;
  Double_t pathO = std::fabs(nuOut - nuIn) * fPitch;

  // only one strip hit ?
  if (inStrip == outStrip) {
    // this strip collected the entire charge
    // smearedQ = SmearCharge(Q);
    // if (smearedQ >= fThreshold)
    //  strips.push_back(PndSdsStrip(Int_t(nuOut),smearedQ));

    //--------------------------Tsito Digi with diffusion------------------------------------------------
    // Double_t path_real = path.Mod();
    Int_t index = inStrip;

    Double_t charge1, charge2, dQ, dQR, dQL, QL, QR, QM;
    Int_t id;
    Double_t q_head, q_tail;
    // charge diffusion according to the gaussian smearing
    // between the 3 strips of index-1, index, and index+1

    QL = ChargeDiffusion(0, Path1, pathO, dir, Q);          // the diffusion charge from -infinite to index
    QR = Q - ChargeDiffusion(fPitch, Path1, pathO, dir, Q); // the diffusion charge from index to infinite
    QM = Q - QR - QL;                                       // the charge left on the strip index

    if (fVerboseLevel > 1)
      std::cout << "the QL,QR,QM are: " << QL << "," << QR << "," << QM << std::endl;

    // noise smearing for the QL,QR and QM
    dQR = SmearCharge(QR);
    dQL = SmearCharge(QL);

    // is it only 1 strip involved
    if (dQL < fThreshold && dQR < fThreshold) {
      dQ = SmearCharge(QM);
      if (dQ > fThreshold)
        strips.push_back(PndSdsStrip(index, dQ));

    } else {
      Int_t i = 0;
      Int_t j = 1;

      if (QL >= fThreshold) {
        // charge distribution at the head of the track
        charge1 = ChargeDiffusion((i - 1) * fPitch, Path1, pathO, dir, Q); // charge distribution from -infinite to strip of index-1
        if (fVerboseLevel > 1)
          std::cout << "if QL>fThrehold, the charge on the strip(i-1)*fPitch is :" << charge1 << ",path1= " << Path1 << std::endl;
        charge2 = ChargeDiffusion(i * fPitch, Path1, pathO, dir, Q);
        if (fVerboseLevel > 1)
          std::cout << "if QL>fThrehold, the charge on the strip(i)*fPitch is :" << charge2 << std::endl;
        q_head = charge2 - charge1;
        id = index + i - 1;
        dQ = SmearCharge(q_head);
        if (dQ > fThreshold)
          strips.push_back(PndSdsStrip(id, dQ));
      }

      if (QR >= fThreshold) {
        // charge distribution at the head of the track
        charge1 = ChargeDiffusion(j * fPitch, Path1, pathO, dir, Q); // charge distribution from -infinite to strip of index-1
        if (fVerboseLevel > 1)
          std::cout << "if dQR>fThrehold, the charge on the strip(j)*fPitch is :" << charge1 << std::endl;
        charge2 = ChargeDiffusion((j + 1) * fPitch, Path1, pathO, dir, Q);
        if (fVerboseLevel > 1)
          std::cout << "if dQR>fThrehold, the charge on the strip(j+1)*fPitch is :" << charge2 << std::endl;
        q_tail = charge2 - charge1;
        id = index + j;
        dQ = SmearCharge(q_tail);
        if (dQ > fThreshold)
          strips.push_back(PndSdsStrip(id, dQ));
      }

      // the charge left on the center strip
      Double_t restQ = ChargeDiffusion(fPitch, Path1, pathO, dir, Q) - ChargeDiffusion(0, Path1, pathO, dir, Q);
      restQ = SmearCharge(restQ);

      if (restQ > fThreshold) // added on Sep.17,2009
        strips.push_back(PndSdsStrip(index, restQ));
      if (fVerboseLevel > 1)
        std::cout << "the rest Q on the middle strip is :" << restQ << std::endl;
    }

    //-----------------------End Tsito Digi with diffusion-------------------------------------------------

    if (fVerboseLevel > 1)
      std::cout << " -> " << strips.size() << " strips hit." << std::endl;
  } else {
    Double_t dQ = Q / std::fabs(nuOut - nuIn);
    // Double_t dPath=path.Mod()/std::fabs(nuOut-nuIn);
    // Double_t dir = (nuOut>nuIn) ? 1. : -1.;
    Int_t nrHits = 0;
    // calculate portion of track in first strip
    Int_t nextIn = Int_t(nuIn + 0.5 + 0.5 * dir);
    Double_t Q1 = dQ * std::fabs(nextIn - nuIn);
    if (fVerboseLevel > 2) {
      std::cout << " part of first strip : " << nextIn - nuIn << std::endl;
      std::cout << " charge : " << Q1 << std::endl;
      std::cout << " next strip : " << nextIn << std::endl;
    }
    // smearedQ = SmearCharge(Q1);
    if (Q1 >= fThreshold)
      strips.push_back(PndSdsStrip(Int_t(nuIn), Q1));
    nrHits++;
    Q -= Q1;

    // calculate portion of track in last strip
    Int_t prevOut = Int_t(nuOut + 0.5 - 0.5 * dir);
    Double_t Q2 = dQ * std::fabs(nuOut - prevOut);
    if (fVerboseLevel > 2) {
      std::cout << " part of last strip : " << (nuOut - prevOut) << std::endl;
      std::cout << " charge : " << Q2 << std::endl;
      std::cout << " end of previous strip : " << prevOut << std::endl;
    }
    // smearedQ = SmearCharge(Q2);
    if (Q2 >= fThreshold)
      strips.push_back(PndSdsStrip(Int_t(nuOut), Q2));
    nrHits++;
    Q -= Q2;

    // Distribute the charge among the intermediate strips
    nextIn = Int_t(nextIn - 0.5 + 0.5 * dir);
    prevOut = Int_t(prevOut - 0.5 + 0.5 * dir);
    if (fVerboseLevel > 2) {
      std::cout << " dir=" << Int_t(dir) << std::endl;
      std::cout << " begin=" << nextIn << " end=" << prevOut << std::endl;
    }

    for (Int_t n = nextIn; n != prevOut; n += Int_t(dir)) {
      if (fVerboseLevel > 2)
        std::cout << " n = " << n << std::endl;
      // smearedQ = SmearCharge(dQ);
      if (dQ >= fThreshold)
        strips.push_back(PndSdsStrip(n, dQ));
      nrHits++;
      Q -= dQ;
    }

    //--------------------------Tsito Digi with diffusion-------------------------------------------------

    if (strips.size() < 1)
      return strips;

    // for the strips
    Double_t head_Path; //, tail_Path;
    std::vector<PndSdsStrip> strip_head;
    std::vector<PndSdsStrip> strip_tail;
    std::vector<PndSdsStrip> strip_middle;
    PndSdsStrip str_head;
    PndSdsStrip str_tail;
    std::vector<PndSdsStrip> str_middle;
    Int_t size = strips.size();
    Double_t total_Q = ChargeFromEloss(eLoss);

    if (inStrip < outStrip) {
      str_head = strips[0];
      // if(!str_head)cout<<"str_head is available"<<endl;
      str_tail = strips[size - 1];
      head_Path = Path1;
      // tail_Path = Path2+(size-1)*fPitch;
      //     cout<<"when inStrip!=outStrip, the head Path is "<<head_Path<<endl;
      for (Int_t i = 1; i < size - 1; i++)
        str_middle.push_back(strips[i]);

      dir = -1;

    } else {
      str_tail = strips[0];
      // if(!str_tail)cout<<"str_tail is available, inStrip<outStrip"<<endl;
      str_head = strips[size - 1];
      head_Path = Path2;
      // tail_Path = Path1+(size-1)*fPitch;
      //	cout<<"when inStrip>outStrip, the head Path is "<<head_Path<<endl;
      //		cout<<"the dir and size is "<<dir<<","<<size<<endl;
      for (Int_t i = size - 2; i > 0; i--)
        str_middle.push_back(strips[i]);

      dir = 1;
    }

    strips.erase(strips.begin(), strips.begin() + size);

    // strips initialization
    Int_t index_head = str_head.GetIndex();
    // Double_t path_head = str_head.GetPath();

    Int_t index_tail = str_tail.GetIndex();
    // Double_t path_tail = str_tail.GetPath();

    // for the left strip of the head strip
    Double_t charge1, charge2;
    Int_t id;
    // Double_t ipath;
    // Double_t q_head,q_tail;
    Double_t dq_head = ChargeDiffusion(0, head_Path, path.Mod(), dir, total_Q); // the diffusion charge from -infinite to index
    //  cout<<"the dir is "<<dir<<endl;
    Double_t dq_tail = total_Q - ChargeDiffusion(size * fPitch, head_Path, path.Mod(), dir, total_Q); // the diffusion charge from index+size to infinite
    //	  cout<<"the dq between -infinite to 0.01 is"<<ChargeDiffusion(size*fPitch,head_Path,path,dir,total_Q)<<endl;
    Double_t q_head = ChargeDiffusion(fPitch, head_Path, path.Mod(), dir, total_Q) - dq_head; // the charge left on the strip index
    // 	  cout<<"the dq between -infinite to 0.005 is"<<ChargeDiffusion(fPitch,head_Path,path,dir,total_Q)<<endl;
    Double_t q_tail = ChargeDiffusion(size * fPitch, head_Path, path.Mod(), dir, total_Q) - ChargeDiffusion((size - 1) * fPitch, head_Path, path.Mod(), dir, total_Q);
    //  cout<<"the dir is "<<dir<<endl;
    if (fVerboseLevel > 2)
      std::cout << "the dq_head,dq_tail,q_head,q_tail are :" << dq_head << "," << dq_tail << "," << q_head << "," << q_tail << std::endl;

    Double_t dQ_head = SmearCharge(dq_head);
    Double_t dQ_tail = SmearCharge(dq_tail);
    Double_t Q_head = SmearCharge(q_head);
    Double_t Q_tail = SmearCharge(q_tail);

    if (dQ_head > fThreshold) {
      id = index_head - 1;
      strip_head.push_back(PndSdsStrip(id, dQ_head));
    }
    if (Q_head > fThreshold) {
      id = index_head;
      strip_head.push_back(PndSdsStrip(id, Q_head));
    }
    strips.insert(strips.begin(), strip_head.begin(), strip_head.end());

    if (dQ_tail > fThreshold) {
      id = index_tail + 1;
      strip_tail.push_back(PndSdsStrip(id, dQ_tail));
    }
    if (Q_tail > fThreshold) {
      id = index_tail;
      strip_head.push_back(PndSdsStrip(id, Q_tail));
    }

    strips.insert(strips.end(), strip_tail.begin(), strip_tail.end());

    // for the strips between the head and the tail
    // condition for middle strips
    nextIn = Int_t(nuIn + 0.5 + 0.5 * dir);
    prevOut = Int_t(nuOut + 0.5 - 0.5 * dir);

    nextIn = Int_t(nextIn - 0.5 + 0.5 * dir);
    prevOut = Int_t(prevOut - 0.5 + 0.5 * dir);
    if (nextIn != prevOut) {
      for (Int_t i = 1; i < size - 1; i++) {
        charge1 = ChargeDiffusion(i * fPitch, head_Path, path.Mod(), dir, total_Q);
        charge2 = ChargeDiffusion((i + 1) * fPitch, head_Path, path.Mod(), dir, total_Q);
        dQ = charge2 - charge1;
        // std::vector<PndSdsStrip>::iterator iter_ii=strip_middle.begin();
        if (dir == -1)
          id = i;
        else
          id = size - 1 - i;
        // id = iter_ii->GetIndex();
        // ipath = iter_ii->GetPath();
        Double_t tmpCharge = SmearCharge(dQ);
        if (tmpCharge > fThreshold)
          strip_middle.push_back(PndSdsStrip(id, tmpCharge));
        // iter_ii++;
      } // loop for all strips between head and tail
      strips.insert(strips.begin() + strip_head.size(), strip_middle.begin(), strip_middle.begin() + strip_middle.size());

      /*for(Int_t i=0; i<strip_head.size(); i++)
       strips.push_back(strip_head[i]);
       for(Int_t i=0; i<strip_middle.size(); i++)
       strips.push_back(strip_middle.[i]);
       for(Int_t i=0; i<strip_tail.size(); i++)
       strips.push_back(strip_tail[i]);*/
    } // if strips fired exceed 3

    //-----------------------End Tsito Digi with diffusion-------------------------------------------------

    if (fVerboseLevel > 2)
      if (fabs(Q) > 1.)
        std::cout << " charge Q = " << Q << " not detected!" << std::endl;
    if (fVerboseLevel > 1)
      std::cout << " -> " << strips.size() << " strips hit." << std::endl;
  }

  return strips;
}

Double_t PndSdsCalcStripDif::SmearCharge(Double_t charge)
{
  Double_t smeared = fRNG->Gaus(charge, fNoise);
  if (fVerboseLevel > 3)
    std::cout << " charge = " << charge << ", smeared = " << smeared << std::endl;
  return smeared;
}

Double_t PndSdsCalcStripDif::CalcStripFromPoint(Double_t x, Double_t y)
{
  // fOrthoDir is already set to magnitude == 1., makes it cheaper here.
  return ((x - fAnchor.X()) * fOrthoDir.X() + (y - fAnchor.Y()) * fOrthoDir.Y()) / fPitch;
}

void PndSdsCalcStripDif::CalcStripPointOnLine(Double_t strip, TVector2 &point) const
{
  point = fPitch * (strip + 0.5) * fOrthoDir + fAnchor;
}

Int_t PndSdsCalcStripDif::CalcFEfromStrip(Int_t stripNr) const
{
  return (stripNr / fNrFeChannels);
}

Int_t PndSdsCalcStripDif::CalcChannelfromStrip(Int_t stripNr) const
{
  return (stripNr % fNrFeChannels);
}

void PndSdsCalcStripDif::CalcFeChToStrip(Int_t fe, Int_t channel, Int_t &strip, enum SensorSide &side) const
{
  // Caution! The top side s always the reference side!
  Int_t nr = fe * fNrFeChannels + channel;
  if (nr < fNrStrips) {
    side = SensorSide::kTOP;
  } else {
    nr -= fNrStrips;
    side = SensorSide::kBOTTOM;
  }
  strip = nr;
}

// charge diffusion during collecting after ionization,
// here, the u means the range which approaching the mean value a.
// the area was expressed by the Erf function as below
// f(x) = 1/2*(1+TMath::Erf((u-a)/sigma*TMath::sqrt(2.))))
Double_t PndSdsCalcStripDif::ChargeDiffusion(Double_t u, Double_t a, Double_t path, Double_t dir, Double_t Q)
{
  Int_t N = 10;
  // Int_t n = 0;
  Double_t Npath = path / N;
  Double_t NQ = Q / N;
  Double_t Delta;  // the area of Gaussian from (-infinite,u)
  Double_t dQ = 0; // the Q in the given range
  // define sigma of charge diffusion as a function of sensor thickness
  // suppose the function is linear the sigma woulde be decreased with the thickness
  // if the sigma = 8um when thickness is 300um, then the sigma = 4um for thickness = 150um
  // the sigma shall be changed in the cycle calculation.
  //  Double_t sigma = fSigma
  // cout<<endl;
  //  cout<<"the parameters for charge diffusion are: u, a, path, dir, Q "<< u<<","<<a<<","<<path<<","<<dir<<","<<Q<<endl;

  if (path == 0.) {
    // Delta = 0.5 *(1+TMath::Erf((u-a)/fSigma*std::sqrt(2.)));
    Delta = 0.5 * (1 + TMath::Erf((u - a) / fSigma * std::sqrt(2.)));
    dQ = NQ * Delta;
    // cout<<"when path ==0 ,the dQ = "<<dQ<<endl;
  } else {
    if (dir > 0.) {
      for (Double_t da = a, n = 0; n < N; n++) { // da = a;
        da += Npath;
        //  Delta = 0.5*(1+TMath::Erf((u-da)/(fSigma*std::sqrt(2.))));
        // here the sigma square is linear as thickness
        Delta = 0.5 * (1 + TMath::Erf((u - a) / (std::sqrt(((N - n) / N)) * (fSigma / std::sqrt(2.)) * std::sqrt(2.)))); // fSigma is from para list and for 300um sensor,so
        dQ += NQ * Delta;
        //   cout<<"when dir = 1 the delta = "<<Delta<<endl;
        //   cout<<"when dir = 1 the dQ = "<<dQ<<endl;
      }
    } // cout<<"when dir = 1 the dQ = "<<dQ<<endl;

    if (dir < 0) {
      for (Double_t da = a - path, n = 0; n < N; n++) {
        da += Npath;
        //  Delta = 0.5*(1+TMath::Erf((u-da)/(fSigma*std::sqrt(2.))));
        Delta = 0.5 * (1 + TMath::Erf((u - da) / (std::sqrt(((N - n) / N)) * (fSigma / std::sqrt(2.)) * std::sqrt(2.))));
        dQ += NQ * Delta;
        //    cout<<"when dir = -1 the delta = "<<Delta<<endl;
        //    cout<<"when dir = -1 the dQ = "<<dQ<<endl;
      }
      //  cout<<"when dir = -1 the dQ = "<<dQ<<endl;
    } // dir<<0

  } // path>0
  return dQ;
}

void PndSdsCalcStripDif::Print() const
{
  LOG(info) << " PndSdsCalcStrip Info :";
  std::cout << "     pitch                  = " << fPitch * 10000. << " um" << std::endl;
  std::cout << "     orientation angle      = " << fOrient / TMath::Pi() * 180. << " deg" << std::endl;
  std::cout << "     nr of strips           = " << fNrStrips << std::endl;
  std::cout << "     nr of channels per FE  = " << fNrFeChannels << std::endl;
  std::cout << "     nr of frontends        = " << fNrStrips / fNrFeChannels << std::endl;
  std::cout << "     anchor point           = (" << fAnchor.X() << "," << fAnchor.Y() << ")" << std::endl;
  std::cout << "     strip-direction vector = (" << fStripDir.X() << "," << fStripDir.Y() << ")" << std::endl;
}
