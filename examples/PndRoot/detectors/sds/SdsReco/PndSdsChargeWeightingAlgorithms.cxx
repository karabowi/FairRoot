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

#include <iostream>
#include <map>
#include <vector>
#include <fstream>

//#include "runinfo.h"
#include "PndSdsChargeWeightingAlgorithms.h"
#include "PndSdsDigi.h"
#include "PndSdsCluster.h"
#include "PndSdsCalcStrip.h"

#include "TArray.h"
// using namespace std;
ClassImp(PndSdsChargeWeightingAlgorithms);

PndSdsChargeWeightingAlgorithms::PndSdsChargeWeightingAlgorithms(TClonesArray *arr) : TObject(), fDigiArray(arr), fCalcStrip(nullptr), fChargeConverter(nullptr), fVerbose(0) {}
PndSdsChargeWeightingAlgorithms::~PndSdsChargeWeightingAlgorithms() {}

/*
returns the (modified) error function
this is a local (private) function
*/
Double_t PndSdsChargeWeightingAlgorithms::Erfmod(Double_t x, Double_t p0, Double_t p1, Double_t p2, Double_t p3)
{
  return ((TMath::Erf(p0 * (x - p1))) * p2 + p3);
}

std::pair<Double_t, Double_t> PndSdsChargeWeightingAlgorithms::CenterOfGravity(const PndSdsCluster *Cluster)
{
  Int_t nrHits = Cluster->GetClusterSize();
  if (nrHits < 2)
    return Binary(Cluster);
  std::pair<Double_t, Double_t> result;
  Double_t x_g = 0., chargesum = 0, charge = 0, stripno = 0;
  Double_t xerror = 0., xtmp = 0., err = 0.;
  Double_t noise = fCalcStrip->GetNoise();
  Double_t threshold = fCalcStrip->GetThreshold();
  Double_t Tthr = threshold / noise;

  for (Int_t l = 0; l < nrHits; ++l) // loop over all hits
  {                                  //           ( q_i*chan_i )
    // x_g= SUM ( ------------ )
    //           (   Q_sum    )
    charge = DigiCharge(Cluster->GetDigiIndex(l));
    stripno = DigiStripno(Cluster->GetDigiIndex(l));
    chargesum += charge;
    x_g += charge * stripno;
    if (fVerbose > 2)
      Info("CenterOfGravity", "Adding digi values (stripno,charge) = (%f,%f)", stripno, charge);
  }
  x_g = x_g / chargesum;
  result.first = x_g;

  Double_t chanmax = DigiStripno(Cluster->GetDigiIndex(nrHits - 1));
  Double_t chanmin = DigiStripno(Cluster->GetDigiIndex(0));
  for (Int_t l = 0; l < nrHits; ++l) // loop over all hits
  {
    stripno = DigiStripno(Cluster->GetDigiIndex(l));
    if (stripno > chanmax)
      chanmax = stripno;
    if (stripno < chanmin)
      chanmin = stripno;
    xtmp = stripno - x_g;
    xtmp *= xtmp;
    xerror += xtmp;
  }
  chanmax++;
  chanmax -= x_g;
  chanmax *= chanmax;
  chanmin--;
  chanmin -= x_g;
  chanmin *= chanmin;
  xtmp = chanmax;
  xtmp += chanmin;
  xtmp *= Tthr;
  xtmp *= Tthr;
  xtmp /= 12.; // Thr is uniform distr.
  err = xerror;
  err += xtmp;
  err = sqrt(err);
  err *= noise;
  err /= chargesum;

  if (err < 1e-15)
    Warning("CenterOfGravity", "Got bad error value: Cluster with %i digis. Position %f +-%f chn.", nrHits, x_g, xerror);
  result.second = err;

  if (fVerbose > 1)
    Info("CenterOfGravity", "Got a cluster with %i digis. Position %f +- %f chn.", nrHits, result.first, result.second);
  return result;
}

std::pair<Double_t, Double_t> PndSdsChargeWeightingAlgorithms::HeadTail(const PndSdsCluster *Cluster)
{
  Int_t nrHits = Cluster->GetClusterSize();
  if (nrHits < 2)
    return Binary(Cluster);
  if (nrHits == 2)
    return CenterOfGravity(Cluster);

  // We assume that the digis in each cluster are sorted.
  std::pair<Double_t, Double_t> result;
  Double_t noise = fCalcStrip->GetNoise();

  Double_t x_h = DigiStripno(Cluster->GetDigiIndex(0));
  Double_t x_t = DigiStripno(Cluster->GetDigiIndex(nrHits - 1));

  Double_t Q = 0., q_inner = 0., charge = 0., stripno = -1., err = 0.;
  Int_t k_h = 0, k_t = nrHits - 1;
  for (Int_t kk = 0; kk < nrHits; kk++) {
    charge = DigiCharge(Cluster->GetDigiIndex(kk));
    stripno = DigiStripno(Cluster->GetDigiIndex(kk));
    Q += charge;
    if (x_h > stripno) {
      k_h = kk;
      x_h = stripno;
    }
    if (x_t < stripno) {
      k_t = kk;
      x_t = stripno;
    }
  }
  Double_t x_ht = x_h + 0.5 * nrHits;
  Double_t q_h = DigiCharge(Cluster->GetDigiIndex(k_h));
  Double_t q_t = DigiCharge(Cluster->GetDigiIndex(k_t));
  q_inner = Q - q_h - q_t;
  q_inner /= (nrHits - 2.); // this number is just a good estiamte for the charge being lost of such a particle in one strip
  Double_t w = (q_t - q_h) / (2. * q_inner);
  x_ht += w;
  err = sqrt(2.) * noise / q_inner;

  result.first = x_ht;
  result.second = err;

  return result;
}

std::pair<Double_t, Double_t> PndSdsChargeWeightingAlgorithms::Median(const PndSdsCluster *Cluster)
{
  Int_t nrHits = Cluster->GetClusterSize();
  std::pair<Double_t, Double_t> result;
  Double_t channel = Cluster->GetDigiIndex(0); // assume sorted digis in cluster from k to k+n
  channel += 0.5 * nrHits;
  result.first = channel;
  result.second = 1. / sqrt(12.);
  if (fVerbose > 1)
    Info("Median", "Got a cluster with %i digis. Position %f +- %f chn.", nrHits, result.first, result.second);
  return result;
}

std::pair<Double_t, Double_t> PndSdsChargeWeightingAlgorithms::Binary(const PndSdsCluster *Cluster)
{
  Int_t nrHits = Cluster->GetClusterSize();
  std::pair<Double_t, Double_t> result;
  Double_t charge = 0., chargemax = 0;
  Double_t channel = 0;
  for (Int_t i = 0; i < nrHits; ++i) {
    charge = DigiCharge(Cluster->GetDigiIndex(i));
    if (charge > chargemax) { // choose highest signal
      chargemax = charge;
      channel = DigiStripno(Cluster->GetDigiIndex(i));
    }
  }
  result.first = channel;
  result.second = 1. / sqrt(12.);
  if (fVerbose > 1)
    Info("Binary", "Got a cluster with %i digis. Position %f +- %f chn.", nrHits, result.first, result.second);
  return result;
}

// std::pair<Double_t,Double_t> PndSdsChargeWeightingAlgorithms::Eta(const PndSdsCluster* Cluster)
//{
//  //TODO: Make a good MVD eta-par file. Eta Algo needed at all?
//  std::pair<Double_t,Double_t> result;				// first calc eta!
//  Int_t nrHits = Cluster->GetClusterSize();
//  if(nrHits>1)
//  {
//    Double_t ql=DigiCharge(Cluster->GetDigiIndex(0));
//    Double_t qr=DigiCharge(Cluster->GetDigiIndex(nrHits-1));
//
//    Int_t f=1, o=nrHits-2;
//    Double_t xl=DigiStripno(Cluster->GetDigiIndex(0))*1.;
//
//    Double_t p0,pe0,p1,pe1,p2,pe2,p3,pe3;				// f(eta) parameters calc before
//    std::ifstream InPar("/home/student/Desktop/repository/calibPar/Eta_para.par");
//    InPar >> p0>>pe0>>p1>>pe1>>p2>>pe2>>p3>>pe3;
//    InPar.close();
//    while(f<o)							// build groups nearly same charge value
//    {
//      if(ql<=qr) 						// who is smaller
//      {
//        ql=ql+DigiCharge(Cluster->GetDigiIndex(f));
//        ++f;
//      }
//      if(qr<ql)
//      {
//        qr=qr+DigiCharge(Cluster->GetDigiIndex(o));
//        //             xl=xl+Erfmod(sube, p0, p1, p2, p3);
//        --o;
//      }
//    }
//
//    Double_t e = qr/(qr+ql);					// total eta
//
//    result.first=xl+f-1+Erfmod(e, p0, p1, p2, p3);				// reconstruct
//  }else{
//    return Binary(Cluster);
//  }
//  result.second=0.;
//  return result;
//}

std::pair<Double_t, Double_t> PndSdsChargeWeightingAlgorithms::Eta(const PndSdsCluster *Cluster, const TH2F *PosVsEta) //, const TH1F* pitch3)
{
  Int_t nrHits = Cluster->GetClusterSize();

  if (nrHits < 2.) {
    return Binary(Cluster);
  }
  if (nrHits > 2.) {
    return CenterOfGravity(Cluster);
  }

  std::pair<Double_t, Double_t> result;
  // if(nrHits == 2. && DigiStripno(Cluster->GetDigiIndex(1))-DigiStripno(Cluster->GetDigiIndex(0))==1.)

  std::pair<Double_t, Double_t> eta_value;
  Double_t stripno = 0.;
  Int_t NmbOfStrips = 0;

  eta_value = EtaValue(Cluster, stripno, NmbOfStrips);

  Int_t Clustercharge = (Int_t)ceil((DigiCharge(Cluster->GetDigiIndex(0)) + DigiCharge(Cluster->GetDigiIndex(1))) / 2500);
  if (Clustercharge > 21)
    Clustercharge = 21;

  result.first = stripno + PosVsEta->GetBinContent((Int_t)ceil(eta_value.first * 500.), Clustercharge); // etadist histogram contains 200 bins.

  result.second = (PosVsEta->GetBinContent((Int_t)ceil((eta_value.first + eta_value.second) * 500.), Clustercharge) -
                   PosVsEta->GetBinContent((Int_t)ceil((eta_value.first - eta_value.second) * 500.), Clustercharge)) /
                  2.;

  return result;
}

std::pair<Double_t, Double_t> PndSdsChargeWeightingAlgorithms::EtaValue(const PndSdsCluster *Cluster, Double_t &stripno, Int_t &NmbOfStrips)
{

  Int_t nrHits = Cluster->GetClusterSize();
  std::pair<Double_t, Double_t> result;

  if (nrHits == 2. && DigiStripno(Cluster->GetDigiIndex(1)) - DigiStripno(Cluster->GetDigiIndex(0)) == 1.) // 2 strips fired
  {
    NmbOfStrips = 2;
    Double_t ql = 0., qr = 0., noise = 0., cherrl = 0., cherrr = 0.;
    noise = fCalcStrip->GetNoise();

    ql = DigiCharge(Cluster->GetDigiIndex(0));
    qr = DigiCharge(Cluster->GetDigiIndex(1));
    cherrr = DigiChargeError(Cluster->GetDigiIndex(1));
    cherrr = sqrt(noise * noise + cherrr * cherrr);

    cherrl = DigiChargeError(Cluster->GetDigiIndex(0));
    cherrl = sqrt(noise * noise + cherrl * cherrl);

    stripno = DigiStripno(Cluster->GetDigiIndex(0));

    result.first = qr / (qr + ql);

    // unused??
    // PndSdsDigiStrip* digil = (PndSdsDigiStrip*)(fDigiArray->At(Cluster->GetDigiIndex(0)));
    // PndSdsDigiStrip* digir = (PndSdsDigiStrip*)(fDigiArray->At(Cluster->GetDigiIndex(1)));

    result.second = sqrt(((ql / (qr + ql)) * (1. / (qr + ql)) * (ql / (qr + ql)) * (1. / (qr + ql)) * cherrr * cherrr) +
                         ((qr / (qr + ql)) * (1. / (qr + ql)) * (qr / (qr + ql)) * (1. / (qr + ql)) * cherrl * cherrl));
    return result;
  }

  if (nrHits == 3 || (DigiStripno(Cluster->GetDigiIndex(1)) - DigiStripno(Cluster->GetDigiIndex(0)) == 2. && nrHits == 2)) // 3 strips fired, sometimes middle strip is empty
  {

    Double_t ql = 0., qr = 0., qm = 0., noise = 0., cherrl = 0., cherrr = 0.;
    noise = fCalcStrip->GetNoise();
    NmbOfStrips = 3;

    if (nrHits == 3) {
      ql = DigiCharge(Cluster->GetDigiIndex(0));
      qm = DigiCharge(Cluster->GetDigiIndex(1));
      qr = DigiCharge(Cluster->GetDigiIndex(2));
      cherrr = sqrt(TMath::Power(DigiChargeError(Cluster->GetDigiIndex(2)), 2.) + TMath::Power((DigiChargeError(Cluster->GetDigiIndex(1)) / 2.), 2.));
      cherrl = sqrt(TMath::Power(DigiChargeError(Cluster->GetDigiIndex(0)), 2.) + TMath::Power((DigiChargeError(Cluster->GetDigiIndex(1)) / 2.), 2.));

    } else {
      ql = DigiCharge(Cluster->GetDigiIndex(0));
      qr = DigiCharge(Cluster->GetDigiIndex(1));
      qm = 3000.;
      cherrr = sqrt(TMath::Power(DigiChargeError(Cluster->GetDigiIndex(1)), 2.) + TMath::Power((noise / 2.), 2.));
      cherrl = sqrt(TMath::Power(DigiChargeError(Cluster->GetDigiIndex(0)), 2.) + TMath::Power((noise / 2.), 2.));
      PndSdsDigiStrip *digil = (PndSdsDigiStrip *)(fDigiArray->At(Cluster->GetDigiIndex(0)));
      PndSdsDigiStrip *digir = (PndSdsDigiStrip *)(fDigiArray->At(Cluster->GetDigiIndex(1)));
      std::cout << "strip no charge: UNBL:   " << DigiStripno(Cluster->GetDigiIndex(0)) << "  " << digil->GetCharge() << "  " << DigiStripno(Cluster->GetDigiIndex(1)) << "  "
                << digir->GetCharge() << std::endl;
    }

    ql += qm / 2.;
    qr += qm / 2.;

    cherrr = sqrt(noise * noise + cherrr * cherrr);
    cherrl = sqrt(noise * noise + cherrr * cherrr);
    stripno = DigiStripno(Cluster->GetDigiIndex(0));
    result.first = qr / (qr + ql);
    result.second = sqrt(((ql / (qr + ql)) * (1. / (qr + ql)) * (ql / (qr + ql)) * (1. / (qr + ql)) * cherrr * cherrr) +
                         ((qr / (qr + ql)) * (1. / (qr + ql)) * (qr / (qr + ql)) * (1. / (qr + ql)) * cherrl * cherrl));
    return result;
  }

  result.first = -1.;
  result.second = -1.;
  return result;
}

std::pair<Double_t, Double_t> PndSdsChargeWeightingAlgorithms::AutoSelect(const PndSdsCluster *Cluster)
{
  Warning("auto_select", "Do not use, this selection is still wrong!");
  switch (Cluster->GetClusterSize()) {
  case 1: return Binary(Cluster); break;
  case 2:
    // return eta(Cluster); // switched off (no par file)
    return CenterOfGravity(Cluster);
    break;
  case 3: return CenterOfGravity(Cluster); break;
  case 4: return CenterOfGravity(Cluster); break;
  default: return HeadTail(Cluster); break;
  }
  return HeadTail(Cluster);
}
/*
void PndSdsChargeWeightingAlgorithms::MakedNdEta(const std::vector<PndSdsCluster>& Cluster, RunInfo& info)
{
for(Int_t cID=0;cID<Cluster->size();cID++)
{
Int_t nrHits = Cluster[cID].GetClusterSize();
if(nrHits>1)
{
Double_t ql=Cluster[cID].GetHit(0).GetCharge()*1.;
Double_t qr=Cluster[cID].GetHit(nrHits-1).GetCharge()*1.;

Int_t f=1, o=nrHits-2;
Double_t xl=Cluster[cID].GetHit(0).GetChannel()*1.;
while(f<o)							// build groups nearly same charge value
{
if(ql<=qr) 						// who is smaller
{
ql=ql+Cluster[cID].GetHit(f).GetCharge()*1.;
++f;
}
if(qr<ql)
{
qr=qr+Cluster[cID].GetHit(o).GetCharge()*1.;
//             xl=xl+Erfmod(sube, p0, p1, p2, p3);
--o;
}
}
Double_t e = qr/(qr+ql);					// total eta
info.FilldNdEta(e);
}
}
return;
}
*/

Double_t PndSdsChargeWeightingAlgorithms::DigiCharge(Int_t digiIndex)
{
  PndSdsDigiStrip *digi = (PndSdsDigiStrip *)(fDigiArray->At(digiIndex));
  Double_t charge = fChargeConverter->DigiValueToCharge(digi->GetCharge());
  // Info("DigiCharge","digi=%p chargedigi=%f charge=%f",digi,digi->GetCharge(), charge);
  // digi->Print();
  return charge;
}

Double_t PndSdsChargeWeightingAlgorithms::DigiChargeError(Int_t digiIndex)
{
  Double_t cherr = DigiCharge(digiIndex);
  cherr *= fChargeConverter->GetRelativeError(cherr);
  return cherr;
}

Int_t PndSdsChargeWeightingAlgorithms::DigiStripno(Int_t digiIndex)
{
  Int_t strip;
  SensorSide side;
  PndSdsDigiStrip *myDigi = (PndSdsDigiStrip *)fDigiArray->At(digiIndex);
  fCalcStrip->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
  return strip;
}
