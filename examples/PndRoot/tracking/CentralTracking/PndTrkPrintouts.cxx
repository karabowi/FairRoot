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

#include "PndTrkPrintouts.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"

#include "PndSttTube.h"
#include <iostream>
#include <cmath>

// Root includes
#include "TROOT.h"

using namespace std;

//----------begin of function PndTrkPrintouts::stampaMvdHits

void PndTrkPrintouts::stampaMvdHits(char *fMvdPixelBranch, char *fMvdStripBranch, Short_t nMvdPixelHit, Short_t nMvdStripHit, Double_t *refindexMvdPixel,
                                    Double_t *refindexMvdStrip, Double_t *sigmaXMvdPixel, Double_t *sigmaXMvdStrip, Double_t *sigmaYMvdPixel, Double_t *sigmaYMvdStrip,
                                    Double_t *sigmaZMvdPixel, Double_t *sigmaZMvdStrip, Double_t *XMvdPixel, Double_t *XMvdStrip, Double_t *YMvdPixel, Double_t *YMvdStrip,
                                    Double_t *ZMvdPixel, Double_t *ZMvdStrip)
{

  cout << "da PndTrkTraking  :  n. Mvd Pixel Hits = " << nMvdPixelHit << endl;
  cout << "da PndTrkTracking  :  n. Mvd Strip Hits = " << nMvdStripHit << endl;
  cout << "da PndTrkTracking  :  info hits di Mvd pixels ------------------\n";
  for (int i = 0; i < nMvdPixelHit; i++) {
    cout << "      Pixel hit n. " << i << " Info : X  = " << XMvdPixel[i] << ";  Y  = " << YMvdPixel[i] << ";  Z  = " << ZMvdPixel[i]
         << ", R=sqrt(X**2+Y**2) = " << sqrt(XMvdPixel[i] * XMvdPixel[i] + YMvdPixel[i] * YMvdPixel[i]) << endl;
    cout << "\t\tPixel Info : sigmaX  = " << sigmaXMvdPixel[i] << ";  sigmaY  = " << sigmaYMvdPixel[i] << ";  sigmaZ  = " << sigmaZMvdPixel[i] << endl
         << "\t suo RefIndex = " << refindexMvdPixel[i]
         << ", suo FairRootManager::Instance()->GetBranchId(fMvdPixelBranch) = " << FairRootManager::Instance()->GetBranchId(fMvdPixelBranch) << endl;
  }
  cout << "        ------------------\n";

  cout << "\tInfo hits di Mvd strips; n. strip hits = " << nMvdStripHit << " ------------------\n";
  for (int i = 0; i < nMvdStripHit; i++) {
    cout << "      Strip hit n. " << i << " Info : X  = " << XMvdStrip[i] << ";  Y  = " << YMvdStrip[i] << ";  Z  = " << ZMvdStrip[i]
         << ", R=sqrt(X**2+Y**2) = " << sqrt(XMvdStrip[i] * XMvdStrip[i] + YMvdStrip[i] * YMvdStrip[i]) << endl;
    cout << "\t\tStrip Info : sigmaX  = " << sigmaXMvdStrip[i] << ";  sigmaY  = " << sigmaYMvdStrip[i] << ";  sigmaZ  = " << sigmaZMvdStrip[i] << endl
         << "\t suo RefIndex = " << refindexMvdStrip[i]
         << ", suo FairRootManager::Instance()->GetBranchId(fMvdStripBranch) = " << FairRootManager::Instance()->GetBranchId(fMvdStripBranch) << endl;
  }
  cout << "        ------------------\n";

  cout << "-----------------------------------------------------\n";
}

//----------end of function PndTrkPrintouts::stampaMvdHits

//----------begin of function PndTrkPrintouts::stampaMvdHits2

void PndTrkPrintouts::stampaMvdHits2(char *fMvdPixelBranch, char *fMvdStripBranch, Short_t nMvdPixelHit, Short_t nMvdStripHit, Double_t *refindexMvdPixel,
                                     Double_t *refindexMvdStrip, Short_t *fMCtrack_of_Pixel, Short_t *fMCtrack_of_Strip, Double_t *sigmaXMvdPixel, Double_t *sigmaXMvdStrip,
                                     Double_t *sigmaYMvdPixel, Double_t *sigmaYMvdStrip, Double_t *sigmaZMvdPixel, Double_t *sigmaZMvdStrip, Double_t *XMvdPixel,
                                     Double_t *XMvdStrip, Double_t *YMvdPixel, Double_t *YMvdStrip, Double_t *ZMvdPixel, Double_t *ZMvdStrip)
{

  cout << "da PndTrkTraking  :  n. Mvd Pixel Hits = " << nMvdPixelHit << endl;
  cout << "da PndTrkTracking  :  n. Mvd Strip Hits = " << nMvdStripHit << endl;
  cout << "da PndTrkTracking  :  info hits di Mvd pixels ------------------\n";
  for (int i = 0; i < nMvdPixelHit; i++) {

    cout << "      Pixel hit n. " << i << " Info : X  = " << XMvdPixel[i] << ";  Y  = " << YMvdPixel[i] << ";  Z  = " << ZMvdPixel[i]
         << ", R=sqrt(X**2+Y**2) = " << sqrt(XMvdPixel[i] * XMvdPixel[i] + YMvdPixel[i] * YMvdPixel[i]) << endl;
    cout << "\t\tPixel Info : sigmaX  = " << sigmaXMvdPixel[i] << ";  sigmaY  = " << sigmaYMvdPixel[i] << ";  sigmaZ  = " << sigmaZMvdPixel[i] << endl
         << "\t\tsuo RefIndex = " << refindexMvdPixel[i] << ", suo ...Instance()->GetBranchId(fMvdPixelBranch) = " << FairRootManager::Instance()->GetBranchId(fMvdPixelBranch);

    if (refindexMvdPixel[i] < 0) {
      cout << ", electronic noise hit non associato ad alcuna MC track;" << endl << endl;
    } else {
      cout << ", traccia MC a cui e' associato = " << fMCtrack_of_Pixel[i] << endl << endl;
    }
  }
  cout << "        ------------------\n";

  cout << "\tInfo hits di Mvd strips; n. strip hits = " << nMvdStripHit << " ------------------\n";
  for (int i = 0; i < nMvdStripHit; i++) {
    cout << "      Strip hit n. " << i << " Info : X  = " << XMvdStrip[i] << ";  Y  = " << YMvdStrip[i] << ";  Z  = " << ZMvdStrip[i]
         << ", R=sqrt(X**2+Y**2) = " << sqrt(XMvdStrip[i] * XMvdStrip[i] + YMvdStrip[i] * YMvdStrip[i]) << endl;
    cout << "\t\tStrip Info : sigmaX  = " << sigmaXMvdStrip[i] << ";  sigmaY  = " << sigmaYMvdStrip[i] << ";  sigmaZ  = " << sigmaZMvdStrip[i] << endl
         << "\t\tsuo RefIndex = " << refindexMvdStrip[i] << ", suo ...Instance()->GetBranchId(fMvdStripBranch) = " << FairRootManager::Instance()->GetBranchId(fMvdStripBranch);

    if (refindexMvdStrip[i] < 0) {
      cout << ", electronic noise hit non associato ad alcuna MC track;" << endl << endl;
    } else {
      cout << ", traccia MC a cui e' associato = " << fMCtrack_of_Strip[i] << endl << endl;
    }
  }
  cout << "        ------------------\n";

  cout << "-----------------------------------------------------\n";
}

//----------end of function PndTrkPrintouts::stampaMvdHits2

//----------begin of function PndTrkPrintouts::stampaSttHits

void PndTrkPrintouts::stampaSttHits(Short_t iHit, Short_t ipunto, Double_t dradius, Double_t *WDX, Double_t *WDY, Double_t *WDZ, FairMCPoint *puntator, PndSttTube *pSttTube)
{

  Short_t i = iHit;

  cout << "\tStt iHit " << i << " e n. punto MC ottenuto con RefIndex = " << ipunto << endl;
  if (ipunto < 0) {
    cout << "hit di background da mixing!\n";
    cout << "             hit wire pos. in middle " << pSttTube->GetPosition().X() << " " << pSttTube->GetPosition().Y() << " " << pSttTube->GetPosition().Z()
         << "; R = " << sqrt(pSttTube->GetPosition().X() * pSttTube->GetPosition().X() + pSttTube->GetPosition().Y() * pSttTube->GetPosition().Y())
         << ", suo drift radius = " << dradius << endl;
    cout << "             wire direction, X, Y, Z (Z direction set always positive)" << WDX[i] << "  " << WDY[i] << "  " << WDZ[i] << endl;
  } else {
    cout << "             hit X, Y, Z space position " << puntator->GetX() << " " << puntator->GetY() << " " << puntator->GetZ() << endl;
    cout << "             hit wire pos. in middle " << pSttTube->GetPosition().X() << " " << pSttTube->GetPosition().Y() << " " << pSttTube->GetPosition().Z()
         << "; R = " << sqrt(pSttTube->GetPosition().X() * pSttTube->GetPosition().X() + pSttTube->GetPosition().Y() * pSttTube->GetPosition().Y())
         << ", suo drift radius = " << dradius << endl;
    cout << "             wire direction, X, Y, Z (Z direction set always positive)" << WDX[i] << "  " << WDY[i] << "  " << WDZ[i] << endl
         << "             this hit belongs to MC track n. " << puntator->GetTrackID() << endl;
  }
}

//----------end of function PndTrkPrintouts::stampaSttHits

//----------begin of function PndTrkPrintouts::stampaSttHits2

void PndTrkPrintouts::stampaSttHits2(Short_t iHit, Short_t ipunto, Double_t dradius, Double_t *WDX, Double_t *WDY, Double_t *WDZ, FairMCPoint *puntator, PndSttTube *pSttTube,
                                     Short_t tubeID)
{

  Short_t i = iHit;

  cout << "\tStt iHit " << i << " e n. punto MC ottenuto con RefIndex = " << ipunto << ", the straw was hit with tube ID = " << tubeID << endl;
  if (ipunto < 0) {
    cout << "hit di background da mixing!\n";
    cout << "             hit wire pos. in middle " << pSttTube->GetPosition().X() << " " << pSttTube->GetPosition().Y() << " " << pSttTube->GetPosition().Z()
         << "; R = " << sqrt(pSttTube->GetPosition().X() * pSttTube->GetPosition().X() + pSttTube->GetPosition().Y() * pSttTube->GetPosition().Y())
         << ", suo drift radius = " << dradius << ", semilength " << pSttTube->GetHalfLength() << endl;
    cout << "             wire direction, X, Y, Z (Z direction set always positive)" << WDX[i] << "  " << WDY[i] << "  " << WDZ[i] << endl;
  } else {
    cout << "             hit X, Y, Z space position " << puntator->GetX() << " " << puntator->GetY() << " " << puntator->GetZ() << endl;
    cout << "             hit wire pos. in middle " << pSttTube->GetPosition().X() << " " << pSttTube->GetPosition().Y() << " " << pSttTube->GetPosition().Z()
         << "; R = " << sqrt(pSttTube->GetPosition().X() * pSttTube->GetPosition().X() + pSttTube->GetPosition().Y() * pSttTube->GetPosition().Y())
         << ", suo drift radius = " << dradius << ", semilength " << pSttTube->GetHalfLength() << endl;
    cout << "             wire direction, X, Y, Z (Z direction set always positive)" << WDX[i] << "  " << WDY[i] << "  " << WDZ[i] << endl
         << "             this hit belongs to MC track n. " << puntator->GetTrackID() << endl;
  }
}

//----------end of function PndTrkPrintouts::stampaSttHits2

//----------begin of function PndTrkPrintouts::stampetta

void PndTrkPrintouts::stampetta(int IVOLTE, bool *keepit, Short_t *ListMvdPixelHitsinTrack, Short_t *ListMvdStripHitsinTrack, Short_t *ListSttParHitsinTrack,
                                Short_t *ListSttSkewHitsinTrack, Short_t *ListSciTilHitsinTrack, Short_t *nMvdPixelHitsinTrack, Short_t *nMvdStripHitsinTrack,
                                Short_t *nSttParHitsinTrack, Short_t *nSttSkewHitsinTrack, Short_t *nSciTilHitsinTrack, Short_t nTotalCand, Short_t Cand,
                                int MAXMVDPIXELHITSINTRACK, int MAXMVDSTRIPHITSINTRACK, int MAXSCITILHITSINTRACK, int MAXSTTHITSINTRACK, Double_t *R, Double_t *Ox, Double_t *Oy,
                                Double_t *FI0, Double_t *KAPPA)
{
  int i, j, nend, start;

  if (Cand < 0) {
    // print all candidates;
    start = 0;
    nend = nTotalCand;
  } else {
    // print only the particular candidate Cand;
    start = Cand;
    nend = Cand + 1;
  }

  cout << "da PndTrkTracking, inizia stampetta ---------------------------\n";
  cout << "da PndTrkTracking, evt. " << IVOLTE << ",  n. SttTrackCand totali = " << nTotalCand << endl;
  for (i = start; i < nend; i++) {
    cout << "\tcandidate n. " << i << ", keepit = " << keepit[i] << ", Ox " << Ox[i] << ", Oy " << Oy[i] << ", R " << R[i] << ", FI0 " << FI0[i] << ", KAPPA " << KAPPA[i] << endl;
    cout << "-----------------\n";
    if (!keepit[i])
      continue;
    cout << "da PndTrkTracking --------------------------------------\n"
         << "	SttTrackCand n.  " << i << ";  n. Hits in Pixels associati = " << nMvdPixelHitsinTrack[i] << "   e loro lista \n";
    for (j = 0; j < nMvdPixelHitsinTrack[i]; j++) {
      cout << "\t\tMvd Pixel Hit n. " << ListMvdPixelHitsinTrack[i * MAXMVDPIXELHITSINTRACK + j] << endl;
    }
    cout << "da PndTrkTracking --------------------------------------\n"
         << "	SttTrackCand n.  " << i << ";  n. Hits in Strips associati = " << nMvdStripHitsinTrack[i] << "   e loro lista \n";
    for (j = 0; j < nMvdStripHitsinTrack[i]; j++) {
      cout << "\t\tMvd Strip Hit n. " << ListMvdStripHitsinTrack[i * MAXMVDSTRIPHITSINTRACK + j] << endl;
    }
    cout << "da PndTrkTracking --------------------------------------\n"
         << "	SttTrackCand n.  " << i << ";  n. || Hits in Stt in Track = " << nSttParHitsinTrack[i] << "   e loro lista \n";
    for (j = 0; j < nSttParHitsinTrack[i]; j++) {
      cout << "\t\t|| Stt Hit n. " << ListSttParHitsinTrack[i * MAXSTTHITSINTRACK + j] << endl;
    }
    cout << "da PndTrkTracking --------------------------------------\n"
         << "	SttTrackCand n.  " << i << ";  n. skew Hits in Stt Track = " << nSttSkewHitsinTrack[i] << "   e loro lista \n";
    for (j = 0; j < nSttSkewHitsinTrack[i]; j++) {
      cout << "\t\tskew Stt Hit n. " << ListSttSkewHitsinTrack[i * MAXSTTHITSINTRACK + j] << endl;
    }
    cout << "da PndTrkTracking --------------------------------------\n"
         << "	SttTrackCand n.  " << i << ";  n. SciTil Hits in Stt Track = " << nSciTilHitsinTrack[i] << "   e loro lista \n";
    for (j = 0; j < nSciTilHitsinTrack[i]; j++) {
      cout << "\t\tscitil Hit n. " << ListSciTilHitsinTrack[i * MAXSCITILHITSINTRACK + j] << endl;
    }
  } // end of   for(  i= start; i< nend; i++)

  cout << "---------------fine stampetta----------------------" << endl << endl;
  return;
}
//----------end of function PndTrkPrintouts::stampetta

//----------begin of function PndTrkPrintouts::stampetta2

void PndTrkPrintouts::stampetta2(bool *keepit, Short_t *ListMvdPixelHitsinTrack, Short_t *ListMvdStripHitsinTrack, Short_t *ListSttParHitsinTrack, Short_t *ListSttSkewHitsinTrack,
                                 Short_t *ListSciTilHitsinTrack, Short_t *nMvdPixelHitsinTrack, Short_t *nMvdStripHitsinTrack, Short_t *nSttParHitsinTrack,
                                 Short_t *nSttSkewHitsinTrack, Short_t *nSciTilHitsinTrack, Short_t nTotalCand, Short_t Cand, int MAXMVDPIXELHITSINTRACK,
                                 int MAXMVDSTRIPHITSINTRACK, int MAXSCITILHITSINTRACK, int MAXSTTHITSINTRACK, Double_t *R, Double_t *Ox, Double_t *Oy, Double_t *FI0,
                                 Double_t *KAPPA)
{
  int i, j, nend, start;

  if (Cand < 0) {
    // print all candidates;
    start = 0;
    nend = nTotalCand;
  } else {
    // print only the particular candidate Cand;
    start = Cand;
    nend = Cand + 1;
  }

  cout << "inizia stampetta ---------------------------\n";
  cout << "\tn. SttTrackCand totali = " << nTotalCand << endl;
  for (i = start; i < nend; i++) {
    cout << "\tcandidate n. " << i << ", keepit = " << keepit[i] << ", Ox " << Ox[i] << ", Oy " << Oy[i] << ", R " << R[i] << ", FI0 " << FI0[i] << ", KAPPA " << KAPPA[i] << endl;
    cout << "-----------------\n";
    if (!keepit[i])
      continue;
    cout << " --------------------------------------\n"
         << "	SttTrackCand n.  " << i << ";  n. Hits in Pixels associati = " << nMvdPixelHitsinTrack[i] << "   e loro lista \n";
    for (j = 0; j < nMvdPixelHitsinTrack[i]; j++) {
      cout << "\t\tMvd Pixel Hit n. " << ListMvdPixelHitsinTrack[i * MAXMVDPIXELHITSINTRACK + j] << endl;
    }
    cout << " --------------------------------------\n"
         << "	SttTrackCand n.  " << i << ";  n. Hits in Strips associati = " << nMvdStripHitsinTrack[i] << "   e loro lista \n";
    for (j = 0; j < nMvdStripHitsinTrack[i]; j++) {
      cout << "\t\tMvd Strip Hit n. " << ListMvdStripHitsinTrack[i * MAXMVDSTRIPHITSINTRACK + j] << endl;
    }
    cout << " --------------------------------------\n"
         << "	SttTrackCand n.  " << i << ";  n. || Hits in Stt in Track = " << nSttParHitsinTrack[i] << "   e loro lista \n";
    for (j = 0; j < nSttParHitsinTrack[i]; j++) {
      cout << "\t\t|| Stt Hit n. " << ListSttParHitsinTrack[i * MAXSTTHITSINTRACK + j] << endl;
    }
    cout << " --------------------------------------\n"
         << "	SttTrackCand n.  " << i << ";  n. skew Hits in Stt Track = " << nSttSkewHitsinTrack[i] << "   e loro lista \n";
    for (j = 0; j < nSttSkewHitsinTrack[i]; j++) {
      cout << "\t\tskew Stt Hit n. " << ListSttSkewHitsinTrack[i * MAXSTTHITSINTRACK + j] << endl;
    }
    cout << " --------------------------------------\n"
         << "	SttTrackCand n.  " << i << ";  n. SciTil Hits in Stt Track = " << nSciTilHitsinTrack[i] << "   e loro lista \n";
    for (j = 0; j < nSciTilHitsinTrack[i]; j++) {
      cout << "\t\tscitil Hit n. " << ListSciTilHitsinTrack[i * MAXSCITILHITSINTRACK + j] << endl;
    }
  } // end of   for(  i= start; i< nend; i++)

  cout << "---------------fine stampetta----------------------" << endl << endl;
  return;
}
//----------end of function PndTrkPrintouts::stampetta2

ClassImp(PndTrkPrintouts);
