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
// PndSttHelixHitProducer
//
// Class for hit on helix track
//
/////////////////////////////////////////////////////////////

#include "PndSttHelixHitProducer.h"

#include "PndSttHit.h"
#include "PndSttTrack.h"
#include "PndSttPoint.h"
#include "PndSttHelixHit.h"
#include "PndSttSingleStraw.h"
#include "PndSttTube.h"
#include "PndSttMapCreator.h"

#include "PndTrackCand.h"
#include "PndTrackCandHit.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "FairRootFileSink.h"

#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TGeoVolume.h"
// #include "TGeoNode.h"
// #include "TGeoMatrix.h"
#include "TVector3.h"
#include "TRandom.h"
#include <TFile.h>
#include "TH1F.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TGeoTube.h"
#include "FairLogger.h"

#include <iostream>
#include <cmath>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndSttHelixHitProducer::PndSttHelixHitProducer() : PndPersistencyTask("STT HELIX Hit Producer")
{
  SetPersistency(kTRUE);
  fVerbose = 1;
}
// -------------------------------------------------------------------------

PndSttHelixHitProducer::PndSttHelixHitProducer(Int_t verbose) : PndPersistencyTask("STT HELIX Hit Producer")
{
  SetPersistency(kTRUE);
  fVerbose = verbose;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSttHelixHitProducer::~PndSttHelixHitProducer() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndSttHelixHitProducer::Init()
{

  hx = new TH1F("hx", "x: mc - reco", 100, -1, 1);
  hy = new TH1F("hy", "y: mc - reco", 100, -1, 1);
  hz = new TH1F("hz", "z: mc - reco", 100, -3, 3);

  hxs = new TH1F("hxs", "x: mc - reco", 100, -1, 1);
  hys = new TH1F("hys", "y: mc - reco", 100, -1, 1);
  hzs = new TH1F("hzs", "z: mc - reco", 100, -3, 3);

  hzresvsslope = new TH2F("hzresvsslope", "z: mc - reco vs slope", 100, -3.5, 3.5, 100, -3., 3.);

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndSttHelixHitProducer::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get SttTrack array
  fTrackArray = (TClonesArray *)ioman->GetObject("STTTrack");
  if (!fTrackArray) {
    LOG(error) << " CbmSttFitTracks::Init: No SttTrack array!";
    return kERROR;
  }

  // Get SttTrackCand array
  fTrackCandArray = (TClonesArray *)ioman->GetObject("STTTrackCand");
  if (!fTrackCandArray) {
    LOG(error) << " CbmSttFitTracks::Init: No SttTrack Cand  array!";
    return kERROR;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("STTPoint");
  if (!fPointArray) {
    cout << "-W- PndSttHelixHitProducer::Init: "
         << "No STTPoint array!" << endl;
    return kERROR;
  }

  // Get input array
  fHitArray = (TClonesArray *)ioman->GetObject("STTHit");
  if (!fHitArray) {
    cout << "-W- PndSttHelixHitProducer::Init: "
         << "No STTHit array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHelixHitArray = new TClonesArray("PndSttHelixHit");
  ioman->Register("SttHelixHit", "STT", fHelixHitArray, GetPersistency());

  // CHECK added
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();

  LOG(info) << " PndSttHelixHitProducer: Intialization successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// CHECK added
void PndSttHelixHitProducer::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

// -----   Public method Exec   --------------------------------------------
void PndSttHelixHitProducer::Exec(Option_t *)
{

  // Reset output array
  if (!fHelixHitArray)
    Fatal("Exec", "No HelixHitArray");

  fHelixHitArray->Clear();

  // Declare some variables
  PndSttTrack *pTrack = nullptr;
  PndTrackCand *pTrackCand = nullptr;

  if (!fTrackArray)
    Fatal("Exec", "No fTrackArray");

  // Loop over tracks
  Int_t nTracks = fTrackArray->GetEntriesFast();

  for (Int_t j = 0; j < nTracks; j++) {
    pTrack = (PndSttTrack *)fTrackArray->At(j);
    if (!pTrack)
      continue;
    Int_t trackCandID = pTrack->GetTrackCandIndex();
    pTrackCand = (PndTrackCand *)fTrackCandArray->At(trackCandID);
    if (!pTrackCand)
      continue;

    //    if(pTrack->GetFlag() < 3) continue; // only prefit-fit-zfit CHECK
    // --------------------------- THE TRACK ----------------------------
    // xy
    // Int_t hh = -(Int_t) pTrack->GetCharge(); //[R.K. 01/2017] unused variable?
    Double_t d0 = pTrack->GetDist();
    Double_t phi0 = pTrack->GetPhi();
    Double_t Rad = pTrack->GetRad();
    // z
    Double_t z0 = pTrack->GetZ();
    Double_t zslope = pTrack->GetTanL();
    // center of curvature of helix
    TVector2 vec((d0 + Rad) * cos(phi0), (d0 + Rad) * sin(phi0));
    // -------------------------------------------------------------------

    Int_t hitcounter = pTrackCand->GetNHits();
    Int_t hotcounter = 0;
    TVector2 point; // point
    Double_t radius = 0;

    PndSttHelixHit *helixhit = nullptr;
    for (Int_t k = 0; k < hitcounter; k++) {
      PndTrackCandHit candhit = pTrackCand->GetSortedHit(k);
      Int_t iHit = candhit.GetHitId();
      PndSttHit *currenthit = (PndSttHit *)fHitArray->At(iHit);
      if (!currenthit) {
        cout << "PndSttHelixHitProducer::Exec: no hit at " << iHit << endl;
        continue;
      }

      // tubeID  CHECK added
      Int_t tubeID = currenthit->GetTubeID();
      PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

      Int_t refindex = currenthit->GetRefIndex();
      // get point
      PndSttPoint *iPoint = (PndSttPoint *)fPointArray->At(refindex);
      if (!iPoint) {
        if (!iPoint) {
          cout << "PndSttHelixHitProducer::Exec: no point at " << refindex << " associated to hit " << iHit << endl;
          continue;
        }

        continue;
      }

      TClonesArray &clref = *fHelixHitArray;
      Int_t size = clref.GetEntriesFast();
      //  cout << "filling HelixHit" << endl;

      // CHECK remember to SET the errors on position!!!!!!!
      TVector3 pos = tube->GetPosition();
      TVector3 posErr(0, 0, 0);
      helixhit =
        new (clref[size]) PndSttHelixHit(currenthit->GetDetectorID(), tubeID, iHit, refindex, pos, posErr, currenthit->GetIsochrone(), currenthit->GetIsochroneError(), 0.0);

      pTrack->AddHelixHit(hitcounter, k, size);

      //      helixhit->Print();

      // get point
      // [xp, yp] point = coordinates xy of the centre of the firing tube
      point.Set(tube->GetPosition().X(), tube->GetPosition().Y());
      radius = currenthit->GetIsochrone();

      TVector3 wiredirection = tube->GetWireDirection();

      // ================= NON SKEWED =======================
      if (wiredirection == TVector3(0., 0., 1.)) {

        // x y plane / non skewed tubes -------------------------------------------------

        //==========
        // POINT ----------------------------------------------------
        // 1. find the cooordinates of the point fired wire of the track:
        // the coordinates of the point are taken from the intersection
        // between the circumference from the drift time and the Rad radius of
        // curvature. -------------------------------------------------------

        // 2. find the intersection between the little circle and the line // Rad
        // 2.a
        // find the line passing throught [xc, yc] (centre of curvature) and [xp, yp] (first wire)
        // y = mx + q
        Double_t m = (point.Y() - vec.Y()) / (point.X() - vec.X());
        Double_t q = point.Y() - m * point.X();

        /**                             CHECK!!! **/
        // cut on radius
        // if the simulated radius is too small, the pMhit
        // is not used for the fit
        if (radius < 0.1) {
          /**
             marray.AddAt(-999, k);
             pMhit->SetX(-999);
             pMhit->SetY(-999);
             continue;  // CHECK throw away the hit
          **/
        }

        // 2.b
        // intersection little circle and line --> [x1, y1]
        // + and - refer to the 2 possible intersections
        // +
        Double_t x1 = (-(m * (q - point.Y()) - point.X()) + sqrt(fabs((m * (q - point.Y()) - point.X()) * (m * (q - point.Y()) - point.X()) -
                                                                      (m * m + 1) * ((q - point.Y()) * (q - point.Y()) + point.X() * point.X() - radius * radius)))) /
                      (m * m + 1);
        Double_t y1 = m * x1 + q;
        // -
        Double_t x2 = (-(m * (q - point.Y()) - point.X()) - sqrt(fabs((m * (q - point.Y()) - point.X()) * (m * (q - point.Y()) - point.X()) -
                                                                      (m * m + 1) * ((q - point.Y()) * (q - point.Y()) + point.X() * point.X() - radius * radius)))) /
                      (m * m + 1);
        Double_t y2 = m * x2 + q;

        // 2.c intersection between line and circle
        // +
        Double_t xb1 = (-(m * (q - vec.Y()) - vec.X()) + sqrt(fabs((m * (q - vec.Y()) - vec.X()) * (m * (q - vec.Y()) - vec.X()) -
                                                                   (m * m + 1) * ((q - vec.Y()) * (q - vec.Y()) + vec.X() * vec.X() - (pTrack->GetRad()) * (pTrack->GetRad()))))) /
                       (m * m + 1);
        Double_t yb1 = m * xb1 + q;
        // -
        Double_t xb2 = (-(m * (q - vec.Y()) - vec.X()) - sqrt(fabs((m * (q - vec.Y()) - vec.X()) * (m * (q - vec.Y()) - vec.X()) -
                                                                   (m * m + 1) * ((q - vec.Y()) * (q - vec.Y()) + vec.X() * vec.X() - (pTrack->GetRad()) * (pTrack->GetRad()))))) /
                       (m * m + 1);
        Double_t yb2 = m * xb2 + q;

        // calculation of the distance between [xb, yb] and [xp, yp]
        Double_t distb1 = sqrt((yb1 - point.Y()) * (yb1 - point.Y()) + (xb1 - point.X()) * (xb1 - point.X()));
        Double_t distb2 = sqrt((yb2 - point.Y()) * (yb2 - point.Y()) + (xb2 - point.X()) * (xb2 - point.X()));

        // choice of [xb, yb]
        TVector2 xyb;
        if (distb1 > distb2)
          xyb.Set(xb2, yb2);
        else
          xyb.Set(xb1, yb1);

        // calculation of the distance between [x, y] and [xb. yb]
        Double_t dist1 = sqrt((xyb.Y() - y1) * (xyb.Y() - y1) + (xyb.X() - x1) * (xyb.X() - x1));
        Double_t dist2 = sqrt((xyb.Y() - y2) * (xyb.Y() - y2) + (xyb.X() - x2) * (xyb.X() - x2));

        // choice of [x, y]
        TVector2 *xy;
        if (dist1 > dist2)
          xy = new TVector2(x2, y2);
        else
          xy = new TVector2(x1, y1); // <========= THIS IS THE NEW POINT to be used for the fit

        // set to helix hit x and y
        helixhit->SetX(xy->X());
        helixhit->SetY(xy->Y());
        //	  cout << "helix final hit "<<k << " "  << helixhit->GetX() << " " << helixhit->GetY() << endl;

        //=====================
        // z plane / non skewed tubes -------------------------------------------------
        Double_t scosl = pTrack->CalculateScosl(helixhit->GetX(), helixhit->GetY());
        Double_t zcoord = z0 + zslope * scosl;
        helixhit->SetZ(zcoord);

        hx->Fill(iPoint->GetX() - helixhit->GetX());
        hy->Fill(iPoint->GetY() - helixhit->GetY());
        hz->Fill(iPoint->GetZ() - helixhit->GetZ());

        // 	  cout << "hit on helix " << helixhit->GetX() << " " << helixhit->GetY() << " " << helixhit->GetZ() << endl;
        // 	  cout << "mc point     " << iPoint->GetX() << " "<< iPoint->GetY() << " " << iPoint->GetZ() << endl;

        //	  helixhit->Print();

      } else { // =========== SKEWED TUBE ==================

        // 	if(currenthit->GetZ() != -999)
        // 	  {
        // 	    // get the track fit result
        // 	    helixhit->SetX(currenthit->GetX());
        // 	    helixhit->SetY(currenthit->GetY());
        // 	    helixhit->SetZ(currenthit->GetZ());
        // 	  }
        // 	else
        {
          // CHECK the reason why these are different from the previous ones!

          TVector3 *tofit, *tofit2;

          wiredirection *= tube->GetHalfLength();
          TVector3 cenposition = tube->GetPosition();

          TVector3 min, max;
          min = cenposition - wiredirection;
          max = cenposition + wiredirection;

          // first extremity
          Double_t x_1 = min.X();
          Double_t y_1 = min.Y();
          Double_t z_1 = min.Z();

          // second extremity
          Double_t x_2 = max.X();
          Double_t y_2 = max.Y();
          Double_t z_2 = max.Z();

          Double_t x1 = -9999.;
          Double_t y1 = -9999.;
          Double_t x2 = -9999.;
          Double_t y2 = -9999.;

          // from xy plane fit
          // Double_t x0 = d0*TMath::Cos(phi0); //[R.K. 01/2017] unused variable?
          // Double_t y0 = d0*TMath::Sin(phi0); //[R.K. 01/2017] unused variable?
          // in xy plane: angle of the PCA to the origin
          // with respect to the curvature center
          // Double_t Phi0 = TMath::ATan2((y0 - vec.Y()),(x0 - vec.X())); //[R.K. 01/2017] unused variable?

          Double_t a = -999;
          Double_t b = -999;

          // intersection point between the reconstructed
          // circumference and the line joining the centres
          // of the reconstructed circle and the i_th drift circle
          if (fabs(x_2 - x_1) > 0.0001) {
            a = (y_2 - y_1) / (x_2 - x_1);
            b = (y_1 - a * x_1);
            Double_t A = a * a + 1;
            Double_t B = vec.X() + a * vec.Y() - a * b;
            Double_t C = vec.X() * vec.X() + vec.Y() * vec.Y() + b * b - Rad * Rad - 2 * vec.Y() * b;
            if ((B * B - A * C) > 0) {
              x1 = (B + TMath::Sqrt(B * B - A * C)) / A;
              x2 = (B - TMath::Sqrt(B * B - A * C)) / A;
              y1 = a * x1 + b;
              y2 = a * x2 + b;
            }
          } else if (fabs(y_2 - y_1) > 0.0001) {
            Double_t A = 1;
            Double_t B = vec.Y();
            Double_t C = vec.Y() * vec.Y() + (x_1 - vec.X()) * (x_1 - vec.X()) - Rad * Rad;

            if ((B * B - A * C) > 0) {
              y1 = (B + TMath::Sqrt(B * B - A * C)) / A;
              y2 = (B - TMath::Sqrt(B * B - A * C)) / A;
              x1 = x2 = x_1;
            }
          } else {
            if (fVerbose == 2)
              LOG(error) << " intersection point not found";
            continue;
          }

          // x1 and x2 are the 2 intersection points
          Double_t d1 = TMath::Sqrt((x1 - cenposition.X()) * (x1 - cenposition.X()) + (y1 - cenposition.Y()) * (y1 - cenposition.Y()));
          Double_t d2 = TMath::Sqrt((x2 - cenposition.X()) * (x2 - cenposition.X()) + (y2 - cenposition.Y()) * (y2 - cenposition.Y()));

          Double_t x_ = x1;
          Double_t y_ = y1;

          // the intersection point nearest to the drift circle's centre is taken
          if (d2 < d1) {
            x_ = x2;
            y_ = y2;
          }

          // now we need to find the actual centre of the drift circle,
          // by translating the drift circle until it becomes tangent
          // to the reconstructed circle.
          // Two solutions are possible (left rigth abiguity),
          // they are both kept, only the following zed fit will discard the wrong ones.
          // Using the parametric equation of the 3d-straigth line and taking the
          // x points just obtained, the zed coordinate of the skewed tube centre is calculated.

          if (x_1 == x_2) {
            x1 = x_;
            y1 = y_ + radius;
            x2 = x_;
            y2 = y_ - radius;
          } else {
            // solving the equation to find out the centre of the tangent circle
            Double_t A = a * a + 1;
            Double_t B = -(a * b - a * y_ - x_);
            Double_t C = x_ * x_ + y_ * y_ + b * b - 2 * b * y_ - radius * radius;
            if ((B * B - A * C) > 0) {
              x1 = (B + TMath::Sqrt(B * B - A * C)) / A;
              x2 = (B - TMath::Sqrt(B * B - A * C)) / A;
              y1 = a * x1 + b;
              y2 = a * x2 + b;
            } else if ((B * B - A * C) == 0) {
              x1 = B / A;
              x2 = x1;
              y1 = a * x1 + b;
              y2 = a * x2 + b;
            } else {
              if (fVerbose == 2)
                cout << "NO WAY2" << endl;
              continue;
            }
          }

          d1 = TMath::Sqrt((x1 - cenposition.X()) * (x1 - cenposition.X()) + (y1 - cenposition.Y()) * (y1 - cenposition.Y()));
          d2 = TMath::Sqrt((x2 - cenposition.X()) * (x2 - cenposition.X()) + (y2 - cenposition.Y()) * (y2 - cenposition.Y()));

          Double_t xcen0 = x1;
          Double_t xcen1 = x2;
          Double_t ycen0 = y1;
          Double_t ycen1 = y2;

          if (d2 < d1) { // z2 contains the points nearest (in x-y) to the initial centre of the skewed tube
            xcen0 = x2;
            xcen1 = x1;
            ycen0 = y2;
            ycen1 = y1;
          }

          // zed association
          Double_t t_, z_, t_bis, z_bis;
          if (fabs(x_2 - x_1) < 0.001) {
            t_ = (ycen0 - y_1) / (y_2 - y_1);    // k= a_y*t + y_1 [t=1 y=y_2]
            z_ = (z_2 - z_1) * t_ + z_1;         // z= a_z*t + z_1 [t=1 z=z_2]
            t_bis = (ycen1 - y_1) / (y_2 - y_1); // from y_'s (the 2 solutions of the 2nd order equation)
            z_bis = (z_2 - z_1) * t_bis + z_1;   // and the 2 parametric equations the z coord. are obtained
          } else {
            t_ = (xcen0 - x_1) / (x_2 - x_1);    // x= a_x*t + x_1 [t=1 x=x_2]
            z_ = (z_2 - z_1) * t_ + z_1;         // z= a_z*t + z_1 [t=1 z=z_2]
            t_bis = (xcen1 - x_1) / (x_2 - x_1); // from x_'s (the 2 solutions of the 2nd order equation)
            z_bis = (z_2 - z_1) * t_bis + z_1;   // and the 2 parametric equations the z coord. are obtained
          }

          //	tofit = new TVector3(xcen0,ycen0,z_);
          tofit = new TVector3(x_, y_, z_);
          //	tofit2 = new TVector3(xcen1,ycen1,z_bis);
          tofit2 = new TVector3(x_, y_, z_bis);

          // I have 2 choices, I prefer the nearest to the line CHECK (DEVE ESSERE MESSO TUTTO A POSTO L' ASSOCIAZIONE DELLA Z!)
          // calculate scosl
          Double_t scosl_ = pTrack->CalculateScosl(x_, y_);
          Double_t zcoord_ = z0 + zslope * scosl_;
          TVector3 *tofit3 = new TVector3(x_, y_, zcoord_);

          double distance_1 = sqrt((tofit->X() - tofit3->X()) * (tofit->X() - tofit3->X()) + (tofit->Y() - tofit3->Y()) * (tofit->Y() - tofit3->Y()) +
                                   (tofit->Z() - tofit3->Z()) * (tofit->Z() - tofit3->Z()));

          double distance_2 = sqrt((tofit2->X() - tofit3->X()) * (tofit2->X() - tofit3->X()) + (tofit2->Y() - tofit3->Y()) * (tofit2->Y() - tofit3->Y()) +
                                   (tofit2->Z() - tofit3->Z()) * (tofit2->Z() - tofit3->Z()));

          if (distance_1 < distance_2)
            helixhit->SetPosition(*tofit);
          else
            helixhit->SetPosition(*tofit2);
        }

        //=====================
        // z plane / non skewed tubes -------------------------------------------------
        // Double_t scosl = pTrack->CalculateScosl(helixhit->GetX(), helixhit->GetY()); //[R.K. 01/2017] unused variable?
        // Double_t zcoord = z0 + zslope * scosl; //[R.K. 01/2017] unused variable?
        //	helixhit->SetZ(zcoord);

        //	cout << "helix hit skewed " << helixhit->GetX() << " " << helixhit->GetY() << " " << helixhit->GetZ() << endl; // CHECK the procedure!!
        //  	cout << "mc point     " << iPoint->GetX() << " "<< iPoint->GetY() << " " << iPoint->GetZ() << endl;

        hxs->Fill(iPoint->GetX() - helixhit->GetX());
        hys->Fill(iPoint->GetY() - helixhit->GetY());
        hzs->Fill(iPoint->GetZ() - helixhit->GetZ());
        hzresvsslope->Fill(zslope, (iPoint->GetZ() - helixhit->GetZ()));
      }

      // dE/dx calculation ==================

      TString tubename;
      TGeoVolume *gastube = NULL; // CHECK we may use tube (fTubeArray) instead of repeating the procedure...
      TObjArray *volumeArray = gGeoManager->GetListOfVolumes();

      for (int i = 0; i < volumeArray->GetEntriesFast(); i++) {
        tubename = volumeArray->At(i)->GetName();
        if (tubename.Contains("stt") && tubename.Contains("gas")) {
          gastube = (TGeoVolume *)volumeArray->At(i);
          break;
        }
      }
      if (gastube == nullptr) {
        LOG(error) << "No gastube found!";
        return;
      }
      TGeoTube *geotube = (TGeoTube *)gastube->GetShape();
      Double_t tuberadius = geotube->GetRmax();
      Double_t distance = 2 * sqrt(tuberadius * tuberadius - radius * radius); // cm
      Double_t coslam = TMath::Cos(TMath::ATan(zslope));
      distance = distance / coslam;

      Double_t dedx = 0.;
      if (distance != 0)
        dedx = currenthit->GetDepCharge() / (1000000 * distance); // in arbitrary units

      helixhit->SetdEdx(dedx);

      hotcounter++;
    }

    // Track summary
    if (fVerbose == 2) {
      LOG(info) << " PndSttHelixHitProducer: " << j << " track " << hitcounter << " SttHits, " << hotcounter << " HelixHits created.";
      cout << "----------------------------------------" << endl;
    }
  }
}

void PndSttHelixHitProducer::WriteHistograms()
{

  FairSink *sink = FairRootManager::Instance()->GetSink();

  if (sink->GetSinkType() == kFILESINK) {
    TDirectory::TContext restorecwd{};
    TFile *outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();
    outfile->mkdir("PndSttHelixHit");
    outfile->cd("PndSttHelixHit");

    outfile->WriteTObject(hx);
    delete (hx);
    hx = nullptr;

    outfile->WriteTObject(hy);
    delete (hy);
    hy = nullptr;

    outfile->WriteTObject(hz);
    delete (hz);
    hz = nullptr;

    outfile->WriteTObject(hxs);
    delete (hxs);
    hxs = nullptr;

    outfile->WriteTObject(hys);
    delete (hys);
    hys = nullptr;

    outfile->WriteTObject(hzs);
    delete (hzs);
    hzs = nullptr;

    outfile->WriteTObject(hzresvsslope);
    delete (hzresvsslope);
    hzresvsslope = nullptr;
  }
  //  TFile *file = FairRootManager::Instance()->GetOutFile();
  //  file->cd();
  //  file->mkdir("PndSttHelixHit");
  //  file->cd("PndSttHelixHit");
  //
  //  hx->Write();
  //  delete hx;
  //  hy->Write();
  //  delete hy;
  //  hz->Write();
  //  delete hz;
  //
  //  hxs->Write();
  //  delete hxs;
  //  hys->Write();
  //  delete hys;
  //  hzs->Write();
  //  delete hzs;
  //
  //  hzresvsslope->Write();
  //  delete hzresvsslope;
}

ClassImp(PndSttHelixHitProducer)
