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

/////////////////////////////////////////////////////////////
//  PndHoughSegment
//  Divides hits in tracklets depending on phi
/////////////////////////////////////////////////////////////////

/** PndHoughSegment
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 04.7.2019
 *@updated 23.11.2021
 *@version 2.1
 **
 ** PANDA class divides hits in tracklets depending on phi
 ** Task level RECO
 **/

#include "PndHoughSegment.h"
#include "FairRootManager.h"
#include "FairMCPoint.h"
#include "FairHit.h"

#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndMCTrack.h"
//#include "PndGemMCPoint.h"
//#include "PndGemHit.h"

#include "TRandom.h"

#include "TH1F.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"

//#include "PndHoughUtilities.h"

ClassImp(PndHoughSegment);

PndHoughSegment::PndHoughSegment(PndHoughData *fData) : fMapFairLinktoFairHit(fData->GetMapFairLinktoFairHit()), fNBins(90.) {}

PndHoughSegment::~PndHoughSegment() {}

/**
 * @brief      Creates track candidates.
 *             Here track candidates are created based on phi:
 *              - first phi of all hits is filled in a histogram
 *              - a "cut" or "segment" is made when at least two bins in the histogram are not filled
 *              - all hits in one segment are filled in a PndTrackCand
 *
 * @param[in]   Cand  A set of hits, that should be divided into a track candidate. The set of hits are also stored in a track candidate
 *
 * @return     A TClonesArray of track candidates
 */
TClonesArray *PndHoughSegment::CreateTrackCands(PndTrackCand &Cand)
{
  TH1F *histo = CreateHisto(Cand);

  std::vector<double> cuts = FindCuts2Zeros(*histo);

  TClonesArray *CandsArray = FillTrackCands(Cand, cuts);

  delete histo;
  return CandsArray;
}

/**
 * @brief      Creates a histogram filled with the phi values of each hit
 *
 * @param[in]  Cand  A set of hits, that should be divided into a track candidate. The set of hits are also stored in a track candidate
 *
 * @return     The histogram filled with the phi values of all hits
 */
TH1F *PndHoughSegment::CreateHisto(PndTrackCand &Cand)
{
  TH1F *histo = new TH1F("histoSeg", ";phi", fNBins, -180.5, 180.5);
  histo->Reset();

  for (int i = 0; i < Cand.GetNHits(); i++) {
    FairLink hit = Cand.GetSortedHit(i);
    FairHit *myHit = fMapFairLinktoFairHit[hit];
    histo->Fill(TMath::ATan2(myHit->GetY(), myHit->GetX()) * 180 / TMath::Pi());
  }

  return histo;
}

/**
 * @brief      A "cut" or "segment" is defined when at least two bins in the histogram are not filled.
 *
 * @param[in]  histo  The histogram filled with the phi values of all hits
 *
 * @return     A vector of cut values for phi, which define a segment of hits grouped to a track candidate.
 */
std::vector<double> PndHoughSegment::FindCuts2Zeros(TH1F &histo)
{
  std::vector<double> cuts;
  cuts.clear();
  int nbinsx = histo.GetXaxis()->GetNbins();
  double first = 0;
  double last = 0;
  double first_index = 0;
  double last_index = 0;

  for (int i = 0; i < nbinsx - 1; i++) {
    Double_t binContent = histo.GetBinContent(i);
    Double_t binContentAfter = histo.GetBinContent(i + 1);

    if (binContent == 0) {
      if (first == 0 && binContentAfter == 0) {
        first = histo.GetBinCenter(i);
        first_index = i;
      }

      if (first != 0 && binContentAfter != 0 && first_index != i) {
        last = histo.GetBinCenter(i + 1);
        cuts.push_back((first + last) / 2);
        first = 0;
      }
    }
  }

  return cuts;
}

/**
 * @brief      All hits in one segment are filled in a PndTrackCand
 *
 * @param[in]  myHit          The hit which has to be assigned to a segment
 * @param[in]  cuts           A vector of cut values for phi, which define a segment. The hit myHit has to be assigned to one of the segments.
 * @param[in]  link           The FairLink of the hit which has to be assigned to a segment
 * @param[out] fTrackCandMap  The result vector of track candidates.
 */
void PndHoughSegment::FillSingleCand(TVector3 &myHit, std::vector<double> &cuts, FairLink &link, PndTrackCand fTrackCandMap[])
{
  // rho does not mean anything. its only to be possible for filling the track cands. Correct pathlängth rho of particle track will be determined after hough trafo, since knowledge
  // about the apollonius circle (track) is needed
  double phi = TMath::ATan2(myHit.Y(), myHit.X()) * 180 / TMath::Pi();
  double r = sqrt(pow(myHit.X(), 2) + pow(myHit.Y(), 2));
  double rho = r * phi * TMath::Pi() / 180;

  if (cuts.size() > 1) {
    for (int j = 0; j < cuts.size() - 1; j++) {

      if (phi > cuts[j] && phi <= cuts[j + 1]) {

        fTrackCandMap[j + 1].AddHit(link, rho);

        break;
      } else if (phi < cuts[0]) {

        fTrackCandMap[0].AddHit(link, rho);

        break;
      } else if (phi > cuts[cuts.size() - 1]) {

        fTrackCandMap[cuts.size()].AddHit(link, rho);

        break;
      }
    }
  }
  if (cuts.size() == 1) {

    if (phi < cuts[0]) {
      fTrackCandMap[0].AddHit(link, rho);
    } else {
      fTrackCandMap[cuts.size()].AddHit(link, rho);
    }
  }
}

/**
 * @brief      All hits in one segment are filled in a PndTrackCand
 * *
 * @param[in]  Cand  A set of hits, that should be divided into a track candidate. The set of hits are also stored in a track candidate
 * @param[in]  cuts  A vector of cut values for phi, which define a segment. The hit myHit has to be assigned to one of the segments.
 *
 * @return     The result TClonesArray of the found track candidates
 */
TClonesArray *PndHoughSegment::FillTrackCands(PndTrackCand &Cand, std::vector<double> &cuts)
{
  PndTrackCand fTrackCandMap[cuts.size() + 1];

  TClonesArray *TrackCand = new TClonesArray("PndTrackCand");
  TrackCand->Clear();

  if (cuts.size() > 0) {
    for (int j = 0; j < cuts.size() + 1; j++) {
      fTrackCandMap[j] = PndTrackCand();
    }
  }

  for (int i = 0; i < Cand.GetNHits(); i++) {
    FairLink link = Cand.GetSortedHit(i);

    FairHit *hit = fMapFairLinktoFairHit[link];
    TVector3 vhit(hit->GetX(), hit->GetY(), 0);

    FillSingleCand(vhit, cuts, link, fTrackCandMap);
  }

  for (int i = 0; i < cuts.size() + 1; i++)
    PndTrackCand *myTrackCand = new ((*TrackCand)[i]) PndTrackCand(fTrackCandMap[i]);

  return TrackCand;
}
