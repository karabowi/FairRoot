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
 *@author Anna Scholl <a.scholl@fz-juelich.de>
 *@since 04.7.2019
 *@updated 02.02.2021
 *@version 1.0
 **
 ** PANDA class divides hits in tracklets depending on phi
 ** Task level RECO
 **/

#ifndef PndHoughSegment_H_
#define PndHoughSegment_H_

#include "PndTrackCand.h"

#include "FairHit.h"
#include "TH1F.h"
#include "TClonesArray.h"
#include "PndHoughData.h"

class PndHoughSegment {

 public:
  PndHoughSegment(PndHoughData *fData);
  virtual ~PndHoughSegment();

  /** @brief Sets the number of bins for a range from -180 to 180. The efficiency of the algorithm depends strongly on that parameter. */
  virtual void SetNBins(Int_t n) { fNBins = n; };
  /** @brief Creates track candidates. */
  TClonesArray *CreateTrackCands(PndTrackCand &Cand);

 protected:
  TH1F *CreateHisto(PndTrackCand &Cand);
  std::vector<double> FindCuts2Zeros(TH1F &histo);
  void FillSingleCand(TVector3 &myHit, std::vector<double> &cuts, FairLink &link, PndTrackCand fTrackCandMap[]);
  TClonesArray *FillTrackCands(PndTrackCand &Cand, std::vector<double> &cuts);

 protected:
  Int_t fNBins;
  std::map<FairLink, FairHit *> fMapFairLinktoFairHit;
  std::map<int, PndTrackCand> fTrackCandMap;

  ClassDef(PndHoughSegment, 2);
};

#endif /* PndHoughSegment_H_ */
