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

/*
 * PndSttSkewStrawPzFinderAnalysisTask.cxx
 *
 *  Created on: Apr 6, 2016
 *      Author: walan603
 */

#include "PndSttSkewStrawPzFinderAnalysisTask.h"
#include <PndSttSkewStrawPzFinderData.h>

#include <FairEventHeader.h>
#include <FairRootManager.h>
#include <FairLogger.h>
#include <Rtypes.h>
#include <TArc.h>
#include <TAttFill.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TGenericClassInfo.h>
#include <TLine.h>
#include <TObjArray.h>
#include <TPad.h>
#include <TString.h>
#include <iostream>

#include "TGraph.h"
#include "TGraphErrors.h"
#include "TAxis.h"
#include "TMarker.h"
#include "TText.h"
#include "TColor.h"
#include "TPaveLabel.h"
#include "TArrow.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "PndRiemannTrack.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"
#include "TMultiGraph.h"


#include "PndGeoSttPar.h"
#include "PndStt2GeoHandler.h"
//#include "PndSttMapCreator.h"
//#include "PndMCMatch.h"
//#include "PndMCEntry.h"
//#include "PndMCResult.h"
#include "PndMCTrack.h"
//#include "PndHelixPropagator.h"

#include "TVector2.h"
#include "TVector3.h"

#include "PndSttTube.h"
#include "PndSttHit.h"

#include "FairMultiLinkedData.h"
ClassImp(PndSttSkewStrawPzFinderAnalysisTask);

using namespace std;

PndSttSkewStrawPzFinderAnalysisTask::PndSttSkewStrawPzFinderAnalysisTask() : fVerbose(0), fDraw(false)
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");

  fPointBranchMap["MVDHitsPixel"] = "MVDPoint";
  fPointBranchMap["MVDHitsStrip"] = "MVDPoint";
  fPointBranchMap["STTHit"] = "STTPoint";
  fPointBranchMap["GEMHit"] = "GEMPoint";
  fPointBranchMap["FTSHit"] = "FTSPoint";

  fPointBranchMap["SciTHit"] = "SciTPoint";
  fPointBranchMap["MdtHit"] = "MdtPoint";

  fPointBranchMap["DircHit"] = ""; // no FairLinks Provided!
  fPointBranchMap["FTofHit"] = ""; // no FairLinks Provided!
  fPointBranchMap["RichHit"] = ""; // no FairLinks Provided!
}

PndSttSkewStrawPzFinderAnalysisTask::~PndSttSkewStrawPzFinderAnalysisTask() {}

InitStatus PndSttSkewStrawPzFinderAnalysisTask::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    LOG(error) << " PndSttSkewStrawPzFinderAnalysisTask::Init: RootManager not instantiated!";
    return kFATAL;
  }

  fMCTrack = (TClonesArray *)ioman->GetObject("MCTrack");

  fEventHeader = (TClonesArray *)ioman->GetObject("EventHeader.");
  if (!fEventHeader) {
    LOG(warn) << "  PndSttSkewStrawPzFinderAnalysisTask::Init: No EventHeader array! Needed for EventNumber";
    return kERROR;
  }

  if (IdealTrackCandBranchName == "")
    IdealTrackCandBranchName = "IdealTrackCand";
  fIdealTrackCand = (TClonesArray *)ioman->GetObject(IdealTrackCandBranchName);
  if (!fIdealTrackCand) {
    LOG(warn) << "  PndSttSkewStrawPzFinderAnalysisTask::Init: No IdealTrackCand array!";
    return kERROR;
  }

  if (IdealTrackBranchName == "")
    IdealTrackBranchName = "IdealTrack";
  fIdealTrack = (TClonesArray *)ioman->GetObject(IdealTrackBranchName);
  if (!fIdealTrack) {
    LOG(warn) << "  PndSttSkewStrawPzFinderAnalysisTask::Init: No IdealTrack array!";
    return kERROR;
  }

  if (TrackCandBranchName == "")
    TrackCandBranchName = "FinalTrackCand";
  fFinalTrackCand = (TClonesArray *)ioman->GetObject(TrackCandBranchName);
  if (!fFinalTrackCand) {
    LOG(warn) << "  PndSttSkewStrawPzFinderAnalysisTask::Init: No FinalTrackCand array!";
    return kERROR;
  }

  if (TrackBranchName == "")
    TrackBranchName = "FinalTrack";
  fFinalTrack = (TClonesArray *)ioman->GetObject(TrackBranchName);
  if (!fFinalTrack) {
    LOG(warn) << "  PndSttSkewStrawPzFinderAnalysisTask::Init: No FinalTrack array!";
    return kERROR;
  }

  //	fFinalRiemannTrack = (TClonesArray*) ioman->GetObject("FinalRiemannTrack");
  //	if (!fFinalRiemannTrack) {
  //		cout << "-W-  PndSttSkewStrawPzFinderAnalysisTask::Init: No FinalRiemannTrack array!"
  //				<< endl;
  //		return kERROR;
  //	}
  if (PzDataBranchName == "")
    PzDataBranchName = "FinalPzData";
  fPzData = (TClonesArray *)ioman->GetObject(PzDataBranchName);
  if (!fPzData) {
    LOG(warn) << "  PndSttSkewStrawPzFinderAnalysisTask::Init: No PzData array!";
    return kERROR;
  }

  if (fBranchNames.size() == 0) {
    AddHitsBranchName("MVDHitsPixel");
    AddHitsBranchName("MVDHitsStrip");
    AddHitsBranchName("STTHit");
    AddHitsBranchName("GEMHit");
    // AddHitsBranchName("FTSHit");
  }

  for (size_t i = 0; i < fBranchNames.size(); i++) {
    if (ioman->GetObject(fBranchNames[i]) != 0) {
      //			fBranchMap[fBranchNames[i]] = (TClonesArray*)ioman->GetObject(fBranchNames[i]);
      ioman->GetObject(fPointBranchMap[fBranchNames[i]]); // initialise the used FairMcPoint Branches
    }
  }

  /*
   * Initialize QA histograms
   */

  hSttSZErr = new TH1F("hSttSZErr", "hSttSZErr", 2000, -50., 50);
  hSttSZErr->GetXaxis()->SetTitle("#Delta z [cm]");
  hSttSZErrAbs = new TH1F("hSttSZErrAbs", "hSttSZErrAbs", 1000, 0., 50);
  hSttSZErrAbs->GetXaxis()->SetTitle("|#Delta z| [cm]");
  hSttSZmct = new TH1F("hSttSZmct", "hSttSZmct", 10, -1, 3);
  hSttSZErrTrue = new TH1F("hSttSZErrTrue", "hSttSZErrTrue", 2000, -50., 50);
  hSttSZErrTrue->GetXaxis()->SetTitle("#Delta z [cm]");
  hSttSZErrAbsTrue = new TH1F("hSttSZErrAbsTrue", "hSttSZErrAbsTrue", 1000, 0., 50);
  hSttSZErrAbsTrue->GetXaxis()->SetTitle("|#Delta z| [cm]");
  hSttSZmctTrue = new TH1F("hSttSZmctTrue", "hSttSZmctTrue", 10, -1, 3);

  // Get tube array
  fTubeArray = PndStt2GeoHandler::Instance(fSttParameters)->GetTubeArray();
  //PndStt2GeoHandler *geoH = new PndStt2GeoHandler(fSttParameters);
  //fTubeArray = geoH->GetTubeArray();
  //PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  //fTubeArray = mapper->FillTubeArray();

  return kSUCCESS;
}

void PndSttSkewStrawPzFinderAnalysisTask::Exec(Option_t *opt)
{
  FairRootManager *ioman = FairRootManager::Instance();

  FairEventHeader *myEventHeader = (FairEventHeader *)fEventHeader;
  Int_t eventNumber = myEventHeader->GetMCEntryNumber();

  //	FairField* Field = FairRunAna::Instance()->GetField();
  //	Double_t po[3], BB[3];
  //	po[0]=0.;
  //	po[1]=0.;
  //	po[2]=0.;
  //	Field->GetFieldValue(po,BB);
  //	cout<<"Field Strength: "<<BB[2]/10.<<endl;

  if (eventNumber > 10)
    fDraw = false;
  if (fDraw) {
    SttXYproj = new TCanvas();
    SttXYproj->SetCanvasSize(700, 700); // 1000,1000
    SttXYproj->SetTitle("SttXYproj Event: " + TString::Itoa(eventNumber, 10));
    Double_t xmin = -42;
    Double_t xmax = 42;
    Double_t ymin = -42;
    Double_t ymax = 42;
    SttXYproj->SetFixedAspectRatio();
    SttXYproj->Range(xmin, ymin, xmax, ymax);
    SttXYproj->cd();

    // Drawing Stt here
    DrawStt(false);
  }

  /*
   * IN EVENT WINDOW
   * Draw ideal tracks
   */
  cout << "Ideal track info:" << endl;
  for (int i = 0; i < fIdealTrack->GetEntriesFast(); i++) {

    PndTrack *temp = (PndTrack *)fIdealTrack->At(i);
    cout << "px,py,pz: " << temp->GetParamFirst().GetPx() << " " << temp->GetParamFirst().GetPy() << " " << temp->GetParamFirst().GetPz() << endl;
    //		DrawTrackParams(temp);
    //		DrawTrack(temp);
  }

  /*
   * IN EVENT WINDOW
   * Draw reconstructed tracks
   */
  cout << "Reconstructed track info:" << endl;
  for (int i = 0; i < fFinalTrack->GetEntriesFast(); i++) {

    PndTrack *temp = (PndTrack *)fFinalTrack->At(i);
    if (fDraw) {
      DrawTrackParams(temp);
    }
    double rad;
    TVector2 center;
    GetCenterOfTrack(temp, center, rad);
    if (fDraw) {
      TArc *circ = new TArc(center.X(), center.Y(), rad);
      circ->SetFillStyle(0);
      circ->Draw("same");
    }
    //		DrawTrack(temp);
    cout << "px,py,pz: " << temp->GetParamFirst().GetPx() << " " << temp->GetParamFirst().GetPy() << " " << temp->GetParamFirst().GetPz() << endl;
    //		FairMultiLinkedData mctracks = temp->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));
    //		for (int j = 0; j < mctracks.GetNLinks(); j++) {
    //			PndMCTrack *mc = (PndMCTrack *) FairRootManager::Instance()->GetCloneOfLinkData(mctracks.GetLink(j));
    //		}

    // Here we draw isochrones of skewed hits
    if (fDraw) {
      PndSttSkewStrawPzFinderData *data = (PndSttSkewStrawPzFinderData *)fPzData->At(i);
      vector<vector<PndSttHit>> myCorrxy = data->getAlignedSkewStt();
      for (unsigned int j = 0; j < myCorrxy.size(); j++) {
        for (unsigned int k = 0; k < myCorrxy.at(j).size(); k++) {
          Double_t isor = myCorrxy.at(j).at(k).GetIsochrone();
          Double_t isox = myCorrxy.at(j).at(k).GetX();
          Double_t isoy = myCorrxy.at(j).at(k).GetY();
          TArc *circ = new TArc(isox, isoy, isor);
          circ->SetFillStyle(0);
          circ->SetLineColor(kBlue);
          circ->Draw("SAME");
        }
      }
    }

    /*
     * Here, we loop over all reconstructed hits of a track
     * The loop goes over one type of hit (stt, mvdpixel etc.) at the time
     */
    PndTrackCand *cand = temp->GetTrackCandPtr();
    //		PndRiemannTrack *candriemann = new PndRiemannTrack(cand);
    //		candriemann->refit(false);
    //		TVector2 orig(candriemann->orig()[0],candriemann->orig()[1]);
    //		cout<<"x: "<<orig.X()<<"y: "<<orig.Y()<<endl;
    for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++) {

      /*
       * Here, we loop over all hits of type fBranchNames[branchIndex]
       */
      FairMultiLinkedData trackData = cand->GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex]));
      for (int j = 0; j < trackData.GetNLinks(); j++) {
        FairHit *myhit = (FairHit *)FairRootManager::Instance()->GetCloneOfLinkData(trackData.GetLink(j));
        PndSttHit *mystthit;
        if (fBranchNames[branchIndex] == "STTHit")
          mystthit = (PndSttHit *)FairRootManager::Instance()->GetCloneOfLinkData(trackData.GetLink(j));
        if (fDraw) {
          if (fBranchNames[branchIndex] == "STTHit") {
            DrawSkewTubeLines(mystthit);
            PndSttTube *myTube = (PndSttTube *)fTubeArray->At(mystthit->GetTubeID());
            if (!myTube->IsSkew()) {
              DrawHitIsochrone(mystthit, kBlue);
            }
          } else if (fBranchNames[branchIndex] == "MVDHitsPixel") {
            TMarker *mark = new TMarker(myhit->GetX(), myhit->GetY(), 1);
            mark->SetMarkerColor(kBlue);
            mark->SetMarkerStyle(25);
            mark->Draw("SAME");
          } else if (fBranchNames[branchIndex] == "MVDHitsStrip") {
            TMarker *mark = new TMarker(myhit->GetX(), myhit->GetY(), 1);
            mark->SetMarkerColor(kBlue);
            mark->SetMarkerStyle(26);
            mark->Draw("SAME");
          } else if (fBranchNames[branchIndex] == "GEMHits") {
            TMarker *mark = new TMarker(myhit->GetX(), myhit->GetY(), 1);
            mark->SetMarkerColor(kBlue);
            mark->SetMarkerStyle(27);
            mark->Draw("SAME");
          }
        }

        /*
         * Here, we draw all MC points from the hits included in the track
         */
        //				FairMultiLinkedData links = myhit->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fPointBranchMap[fBranchNames[branchIndex]]));
        //				for (int k = 0; k < links.GetNLinks(); k++) {
        //					FairMCPoint *point = (FairMCPoint*) FairRootManager::Instance()->GetCloneOfLinkData(links.GetLink(k));;
        //					if (point) {
        //						if (fDraw) {
        ////							TMarker *mark = new TMarker(point->GetX(), point->GetY(), 3);
        ////							mark->SetMarkerColor(kRed);
        ////							mark->Draw("SAME");
        //						}
        //					}
        //				}
      }
    }
    if (fDraw) {
      gPad->Modified();
      TString titleSTT = "SttXYproj";
      titleSTT += eventNumber;
      titleSTT += "_";
      titleSTT += i;
      SttXYproj->cd();
      SttXYproj->Write(titleSTT);
    }
  }

  /*
   * Draws in SZ TRACK WINDOWS
   * Draw reconstructed pzdata
   */

  for (int i = 0; i < fPzData->GetEntriesFast(); ++i) {
    PndSttSkewStrawPzFinderData *data = (PndSttSkewStrawPzFinderData *)fPzData->At(i);
    PndTrack *temp = (PndTrack *)fFinalTrack->At(i);
    //		PndTrackCand* cand = temp->GetTrackCandPtr();
    double rad = data->getRadius();
    TVector2 center = data->getCenter();
    double S0 = data->getS0();
    Double_t charge = (Double_t)temp->GetParamFirst().GetQ();

    vector<PndSttHit> myhits = data->getSttHits();
    vector<vector<TVector2>> mySZ = data->getSZPairVector();
    vector<pair<unsigned int, unsigned int>> trueIndex = data->getTrueSZindex();
    //		vector<pair<unsigned int, unsigned int>> mctcheck;

    if (fDraw) {
      SZCanvas = new TCanvas();
      SZCanvas->SetCanvasSize(500, 500);
      SZCanvas->SetTitle("SZCanvas");

      DrawSZBase(data);
      //			DrawCombiLines(mySZ);
    }
    for (unsigned int j = 0; j < mySZ.size(); j++) {
      double Strue;
      double Ztrue;
      bool isMCT = false;

      FairMultiLinkedData links = myhits.at(j).GetLinksWithType(FairRootManager::Instance()->GetBranchId(fPointBranchMap["STTHit"]));
      for (int k = 0; k < links.GetNLinks(); k++) {
        if (k > 0)
          continue;
        FairMCPoint *point = (FairMCPoint *)FairRootManager::Instance()->GetCloneOfLinkData(links.GetLink(k));
        ;
        if (point) {
          isMCT = true;
          Double_t x0 = point->GetX() - center.X();
          Double_t y0 = point->GetY() - center.Y();
          TVector2 pos(x0, y0);
          Double_t phi = pos.Phi();
          Strue = (phi * rad - S0) * charge * -1.;
          Ztrue = point->GetZ();
          if (Strue < 0.)
            Strue += rad * 2. * TMath::Pi();

          if (fDraw) {
            TMarker *mark = new TMarker(Strue, Ztrue, 3);
            mark->SetMarkerColor(kGreen);
            mark->Draw("SAME");
          }
        }
      }
      if (!isMCT)
        continue;
      unsigned int truej = 0;
      double dist2 = 999999.;
      for (unsigned int k = 0; k < mySZ.at(j).size(); k++) {
        double Srec = mySZ.at(j).at(k).X();
        double Zrec = mySZ.at(j).at(k).Y();
        hSttSZErr->Fill(Zrec - Ztrue);
        hSttSZErrAbs->Fill(TMath::Abs(Zrec - Ztrue));
        //				if(isMCT){
        double temp2 = (Srec - Strue) * (Srec - Strue) + (Zrec - Ztrue) * (Zrec - Ztrue);
        if (temp2 < dist2) {
          dist2 = temp2;
          truej = k;
        }
        //				}
      }
      //			pair<unsigned int,unsigned int> a(j,truej);
      for (unsigned int k = 0; k < trueIndex.size(); k++) {
        if (trueIndex.at(k).first == j) {
          //					double Srec = mySZ.at(j).at(trueIndex.at(k).second).X();
          double Zrec = mySZ.at(j).at(trueIndex.at(k).second).Y();
          hSttSZErrTrue->Fill(Zrec - Ztrue);
          hSttSZErrAbsTrue->Fill(TMath::Abs(Zrec - Ztrue));
          if (trueIndex.at(k).second == truej) {
            hSttSZmctTrue->Fill(1);
          } else {
            hSttSZmctTrue->Fill(0);
          }
        }
      }
    }

    // Use Fairlinks to draw MC info
    //		FairMultiLinkedData trackData = cand->GetLinksWithType(ioman->GetBranchId("STTHit"));
    //		for (int j = 0; j < trackData.GetNLinks(); j++) {
    //			PndSttHit *myhit = (PndSttHit*) FairRootManager::Instance()->GetCloneOfLinkData(trackData.GetLink(j));
    //			PndSttTube myTube = *(PndSttTube*) fTubeArray->At(myhit->GetTubeID());
    //			if (!myTube.IsSkew()) continue;
    //			/*
    //			 * Here, we draw all MC points from the hits included in the track
    //			 */
    //			FairMultiLinkedData links = myhit->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fPointBranchMap["STTHit"]));
    //			for (int k = 0; k < links.GetNLinks(); k++) {
    //				FairMCPoint *point = (FairMCPoint*) FairRootManager::Instance()->GetCloneOfLinkData(links.GetLink(k));;
    //				if (point) {
    //					Double_t x0 = point->GetX() - center.X();
    //					Double_t y0 = point->GetY() - center.Y();
    //					TVector2 pos(x0,y0);
    //					Double_t phi = pos.Phi();
    //					Double_t S = (phi*rad-S0)*charge*-1.;
    //					if (S < 0.) S+= rad*2.*TMath::Pi();
    //
    //					if (fDraw) {
    //						TMarker *mark = new TMarker(S, point->GetZ(), 3);
    //						mark->SetMarkerColor(kGreen);
    //						mark->Draw("SAME");
    //					}
    //
    //				}
    //			}
    //		}

    //			DrawCombiLines(SZPairVector);
    //			DrawCombiLinesResult(TrueSZ);
    if (fDraw) {
      gPad->Modified();
      TString titleSz = "SZCanvas";
      titleSz += eventNumber;
      titleSz += "_";
      titleSz += i;

      SZCanvas->Write(titleSz);
    }
  }
}

void PndSttSkewStrawPzFinderAnalysisTask::Finish()
{
  fIdealTrackCand->Delete();
  fIdealTrack->Delete();
  fFinalTrackCand->Delete();
  fFinalTrack->Delete();
  //	fFinalRiemannTrack->Delete();

  fPzData->Delete();

  hSttSZErr->Write();
  hSttSZErrAbs->Write();
  hSttSZmct->Write();
  hSttSZErrTrue->Write();
  hSttSZErrAbsTrue->Write();
  hSttSZmctTrue->Write();
}

/*
 * Private functions
 */

void PndSttSkewStrawPzFinderAnalysisTask::GetCenterOfTrack(PndTrack *temp, TVector2 &center, double &radius)
{
  Double_t pfirstx = temp->GetParamFirst().GetPx();
  Double_t pfirsty = temp->GetParamFirst().GetPy();
  Double_t plastx = temp->GetParamLast().GetPx();
  Double_t plasty = temp->GetParamLast().GetPy();
  Double_t dpfirstx = pfirsty;  //*temp->GetParamFirst().GetQ();
  Double_t dpfirsty = -pfirstx; //*temp->GetParamFirst().GetQ();
  Double_t dplastx = plasty;    //*temp->GetParamLast().GetQ();
  Double_t dplasty = -plastx;   //*temp->GetParamLast().GetQ();

  Double_t kfirst = dpfirsty / dpfirstx;
  Double_t klast = dplasty / dplastx;

  Double_t mfirst = temp->GetParamFirst().GetY() - kfirst * temp->GetParamFirst().GetX();
  Double_t mlast = temp->GetParamLast().GetY() - klast * temp->GetParamLast().GetX();

  Double_t x0 = (mlast - mfirst) / (kfirst - klast);
  Double_t y0 = klast * x0 + mlast;
  TVector2 r(x0 - temp->GetParamFirst().GetX(), y0 - temp->GetParamFirst().GetY());
  center.SetX(x0);
  center.SetY(y0);
  radius = r.Mod();
}

void PndSttSkewStrawPzFinderAnalysisTask::DrawTrack(PndTrack *temp)
{
  Double_t pfirstx = temp->GetParamFirst().GetPx();
  Double_t pfirsty = temp->GetParamFirst().GetPy();
  Double_t plastx = temp->GetParamLast().GetPx();
  Double_t plasty = temp->GetParamLast().GetPy();
  Double_t dpfirstx = pfirsty;  //*temp->GetParamFirst().GetQ();
  Double_t dpfirsty = -pfirstx; //*temp->GetParamFirst().GetQ();
  Double_t dplastx = plasty;    //*temp->GetParamLast().GetQ();
  Double_t dplasty = -plastx;   //*temp->GetParamLast().GetQ();

  Double_t kfirst = dpfirsty / dpfirstx;
  Double_t klast = dplasty / dplastx;

  Double_t mfirst = temp->GetParamFirst().GetY() - kfirst * temp->GetParamFirst().GetX();
  Double_t mlast = temp->GetParamLast().GetY() - klast * temp->GetParamLast().GetX();

  Double_t x0 = (mlast - mfirst) / (kfirst - klast);
  Double_t y0 = klast * x0 + mlast;
  TVector2 r(x0 - temp->GetParamFirst().GetX(), y0 - temp->GetParamFirst().GetY());
  TVector2 r2(x0 - temp->GetParamLast().GetX(), y0 - temp->GetParamLast().GetY());
  Double_t r0 = r.Mod();
  TArc *circ = new TArc(x0, y0, r0);
  circ->SetFillStyle(0);
  circ->Draw("same");
}

void PndSttSkewStrawPzFinderAnalysisTask::DrawStt(bool isskew)
{
  TArc *riecirc = new TArc(0., 0., 41.);
  riecirc->SetFillStyle(0);
  riecirc->Draw("SAME");
  PndSttTube *tube;
  TVector3 pos;
  double r, x, y; //,z;
  for (int j = 0; j < fTubeArray->GetEntriesFast(); ++j) {
    if (!fTubeArray->At(j))
      continue;
    tube = (PndSttTube *)fTubeArray->At(j);
    pos = tube->GetPosition();
    x = pos.X();
    y = pos.Y();
    //		z = pos.Z();
    if (tube->IsParallel()) {
      r = tube->GetRadIn();
      TArc *circ = new TArc(x, y, r);
      circ->SetFillStyle(0);
      circ->SetLineColor(kGreen - 8);
      circ->Draw("SAME");
    } else if (isskew && tube->IsSkew()) {
      if (tube->GetLayerID() == 10 || tube->GetLayerID() == 11 || tube->GetLayerID() == 14 || tube->GetLayerID() == 15) {
        r = tube->GetRadIn();
        TArc *circ = new TArc(x, y, r);
        circ->SetFillStyle(0);
        circ->SetLineColor(kRed);
        circ->Draw("SAME");
      } else {
        r = tube->GetRadIn();
        TArc *circ = new TArc(x, y, r);
        circ->SetFillStyle(0);
        circ->SetLineColor(kBlue);
        circ->Draw("SAME");
      }
    }
  }

  TText *xa = new TText(30, 0.1, "X-axis");
  xa->SetTextAlign(11);
  xa->SetTextSize(0.05);
  xa->Draw("SAME");
  TText *ya = new TText(-0.1, 30, "Y-axis");
  ya->SetTextAlign(11);
  ya->SetTextAngle(90);
  ya->SetTextSize(0.05);
  ya->Draw("SAME");
  TText *le = new TText(20, -12, "41 cm");
  le->SetTextAlign(11);
  le->SetTextAngle(-31);
  le->SetTextSize(0.05);
  le->Draw("SAME");
  TArrow *firstLine = new TArrow(0., 0., 35., -TMath::Sqrt(41. * 41. - 35. * 35.), 0.9, "|>");
  firstLine->SetLineColor(kBlack);
  firstLine->SetLineWidth(3);
  firstLine->Draw("SAME");

  // Mark origin
  TLine *xLine = new TLine(-50, 0, 50, 0);
  TLine *yLine = new TLine(0, 50, 0, -50);
  xLine->Draw("SAME");
  yLine->Draw("SAME");
}

void PndSttSkewStrawPzFinderAnalysisTask::DrawSkewTubeLines(PndSttHit *hit)
{
  PndSttTube *myTube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
  if (!myTube->IsSkew())
    return;
  Double_t tubeLengthHalf = myTube->GetHalfLength();
  TVector3 wireDirection = myTube->GetWireDirection();
  TVector3 tubePosition = myTube->GetPosition();
  TVector3 startPoint = tubePosition - (tubeLengthHalf * wireDirection);
  TVector3 endPoint = tubePosition + (tubeLengthHalf * wireDirection);

  TLine *tubeLine = new TLine(startPoint.X(), startPoint.Y(), endPoint.X(), endPoint.Y());
  tubeLine->Draw("SAME");
}

void PndSttSkewStrawPzFinderAnalysisTask::DrawHitIsochrone(PndSttHit *hit, Color_t color)
{
  Double_t isor = hit->GetIsochrone();
  Double_t isox = hit->GetX();
  Double_t isoy = hit->GetY();
  TArc *circ = new TArc(isox, isoy, isor);
  circ->SetFillStyle(0);
  circ->SetLineColor(color);
  circ->Draw("SAME");
}

void PndSttSkewStrawPzFinderAnalysisTask::DrawHitSkewedIsochrone(PndSttHit *hit, Color_t color, Double_t beta, Double_t a, Double_t b)
{
  //	Double_t isor = hit->GetIsochrone();
  Double_t isox = hit->GetX();
  Double_t isoy = hit->GetY();
  TEllipse *ell = new TEllipse(isox, isoy, a, b, 0, 360, -beta);
  ell->SetFillStyle(0);
  ell->SetLineColor(color);
  ell->Draw("SAME");
}

void PndSttSkewStrawPzFinderAnalysisTask::DrawTrackParams(PndTrack *track)
{
  FairTrackParP trackParamFirst = track->GetParamFirst();
  FairTrackParP trackParamLast = track->GetParamLast();
  Double_t xfirst = trackParamFirst.GetX();
  Double_t yfirst = trackParamFirst.GetY();
  TVector2 Ptfirst(trackParamFirst.GetPx(), trackParamFirst.GetPy());
  Ptfirst = Ptfirst.Unit();
  Double_t xlast = trackParamLast.GetX();
  Double_t ylast = trackParamLast.GetY();
  TVector2 Ptlast(trackParamLast.GetPx(), trackParamLast.GetPy());
  Ptlast = Ptlast.Unit();
  TArrow *firstLine = new TArrow(xfirst, yfirst, xfirst + Ptfirst.X(), yfirst + Ptfirst.Y(), 1, "|>");
  firstLine->SetLineColor(kRed);
  firstLine->SetLineWidth(2);
  TArrow *lastLine = new TArrow(xlast, ylast, xlast + Ptlast.X(), ylast + Ptlast.Y(), 1, "|>");
  lastLine->SetLineColor(kBlue);
  lastLine->SetLineWidth(2);
  firstLine->Draw("SAME");
  lastLine->Draw("SAME");
}
// todo:zphi changed to SZ, make sure this function works properly!!
// void PndSttCellTrackingTestTask::DrawCombiLines(vector<vector< TVector2> > SZPairVector){
//	for (int i = 0; i < SZPairVector.size()-1; i++){//Loop over straws
//		for (int j = 0; j < SZPairVector.at(i).size(); j++){//Loop over all Z-phi in straw i-1
//			for (int k = 0; k < SZPairVector.at(i+1).size(); k++){//Loop over all Z-phi in straw i
//				TLine* ALine = new TLine(SZPairVector.at(i+1).at(k).X(), SZPairVector.at(i+1).at(k).Y(), SZPairVector.at(i).at(j).X(), SZPairVector.at(i).at(j).Y());
//				ALine->Draw("SAME");
//			}
//		}
//	}
//}

// todo:zphi changed to SZ, make sure this function works properly!!
void PndSttSkewStrawPzFinderAnalysisTask::DrawCombiLines(vector<vector<TVector2>> SZPairVector)
{
  if (SZPairVector.size() < 3)
    return;
  TVector2 v1, v2;
  //	double CosAngle, Angle;
  double x0, y0;
  TLine *ALine = new TLine();
  for (unsigned int i = 1; i < SZPairVector.size() - 1; i++) {             // Loop over straws
    for (unsigned int j = 0; j < SZPairVector.at(i - 1).size(); j++) {     // Loop over all Z-phi in straw i-1
      for (unsigned int k = 0; k < SZPairVector.at(i).size(); k++) {       // Loop over all Z-phi in straw i
        for (unsigned int l = 0; l < SZPairVector.at(i + 1).size(); l++) { // Loop over all Z-phi in straw i+1
          v1 = SZPairVector.at(i + 1).at(l) - SZPairVector.at(i).at(k);
          v2 = SZPairVector.at(i - 1).at(j) - SZPairVector.at(i).at(k);
          //					CosAngle = v1*v2/(TMath::Sqrt(v1*v1)*TMath::Sqrt(v2*v2));
          //					Angle = TMath::ACos(CosAngle) * 180./TMath::Pi();
          // if (Angle > 160) {
          x0 = (SZPairVector.at(i).at(k).X() + SZPairVector.at(i - 1).at(j).X()) / 2;
          y0 = (SZPairVector.at(i).at(k).Y() + SZPairVector.at(i - 1).at(j).Y()) / 2;
          ALine->DrawLine(SZPairVector.at(i).at(k).X(), SZPairVector.at(i).at(k).Y(), x0, y0);

          x0 = (SZPairVector.at(i).at(k).X() + SZPairVector.at(i + 1).at(l).X()) / 2;
          y0 = (SZPairVector.at(i).at(k).Y() + SZPairVector.at(i + 1).at(l).Y()) / 2;
          ALine->DrawLine(SZPairVector.at(i).at(k).X(), SZPairVector.at(i).at(k).Y(), x0, y0);
          //}
        }
      }
    }
  }
  /*
   * Plot start- and end lines
   */
  for (int j = 0; j < 2; j++) {
    for (int k = 0; k < 2; k++) {
      x0 = (SZPairVector.at(0).at(j).X() + SZPairVector.at(1).at(k).X()) / 2;
      y0 = (SZPairVector.at(0).at(j).Y() + SZPairVector.at(1).at(k).Y()) / 2;
      ALine->DrawLine(SZPairVector.at(0).at(j).X(), SZPairVector.at(0).at(j).Y(), x0, y0);
      ALine->DrawLine(SZPairVector.at(0).at(j).X(), SZPairVector.at(0).at(j).Y(), x0, y0);

      x0 = (SZPairVector.at(SZPairVector.size() - 1).at(j).X() + SZPairVector.at(SZPairVector.size() - 2).at(k).X()) / 2;
      y0 = (SZPairVector.at(SZPairVector.size() - 1).at(j).Y() + SZPairVector.at(SZPairVector.size() - 2).at(k).Y()) / 2;
      ALine->DrawLine(SZPairVector.at(SZPairVector.size() - 1).at(j).X(), SZPairVector.at(SZPairVector.size() - 1).at(j).Y(), x0, y0);
      ALine->DrawLine(SZPairVector.at(SZPairVector.size() - 1).at(j).X(), SZPairVector.at(SZPairVector.size() - 1).at(j).Y(), x0, y0);
    }
  }
}
// todo:zphi changed to SZ, make sure this function works properly!!
// void PndSttSkewStrawPzFinderAnalysisTask::DrawCombiLinesResult(vector<vector< TVector2>> TrueSZ){
//	//TODO::remove this function maybe? Or figure out a nice way to represent this
//	//	for (int i = 0; i < TrueSZ.size()-1; i++){//Loop over straws
//	//		TLine* ALine = new TLine(TrueSZ.at(i+1).X(), TrueSZ.at(i+1).Y(), TrueSZ.at(i).X(), TrueSZ.at(i).Y());
//	//		ALine->Draw("SAME");
//	//	}
//}

// This function creates a canvas for the representation of SZ hits for a given track PzData
void PndSttSkewStrawPzFinderAnalysisTask::DrawSZBase(PndSttSkewStrawPzFinderData *data)
{
  vector<vector<TVector2>> SZPairVector = data->getSZPairVector();
  vector<vector<TVector2>> TrueSZ = data->getTrueSZ();

  vector<TVector2> SZPairVectorMvdPixel = data->getSZPairVectorMvdPixel();
  vector<TVector2> TrueSZMvdPixel = data->getTrueSZMvdPixel();

  vector<TVector2> SZPairVectorMvdStrip = data->getSZPairVectorMvdStrip();
  vector<TVector2> TrueSZMvdStrip = data->getTrueSZMvdStrip();

  vector<TVector2> SZPairVectorGem = data->getSZPairVectorGem();
  vector<TVector2> TrueSZGem = data->getTrueSZGem();

  Double_t slope = data->getLineSlope();
  Double_t intercept = data->getLineIntercept();

  //	Double_t MCslope = 0;//dummyslope
  //	Double_t MCintercept = 0;//dummyintercept
  Size_t marksize = 1;

  /*
   * Store all zphi from SttSkewStraws
   */

  double xstt[SZPairVector.size() * 2];
  double ystt[SZPairVector.size() * 2];
  //	double sigstt[SZPairVector.size()*2];
  //	double sigxstt[SZPairVector.size()*2];
  unsigned int l = 0;
  for (unsigned int j = 0; j < SZPairVector.size(); j++) {
    for (int k = 0; k < 2; k++) {
      xstt[l] = SZPairVector.at(j).at(k).X();
      ystt[l] = SZPairVector.at(j).at(k).Y();
      //			sigstt[l] = 0.3;
      l++;
    }
  }
  //	TGraphErrors *SZPoints = new TGraphErrors(SZPairVector.size()*2,xstt,ystt, sigxstt, sigstt);
  TGraph *SZPoints = new TGraphErrors(SZPairVector.size() * 2, xstt, ystt);
  SZPoints->SetMarkerStyle(28);
  SZPoints->SetMarkerColor(kRed);
  SZPoints->SetMarkerSize(marksize);
  SZPoints->SetDrawOption("AP");

  /*
   * Store selected zphi among SttSkewStraws
   */

  unsigned int truesizestt = 0;
  for (unsigned int i = 0; i < TrueSZ.size(); i++) {
    truesizestt += TrueSZ.at(i).size();
  }
  double xtruestt[truesizestt];
  double ytruestt[truesizestt];
  //	double sigtruestt[truesizestt];
  //	double sigxtruestt[truesizestt];
  unsigned int cnt = 0;
  for (unsigned int i = 0; i < TrueSZ.size(); i++) {

    for (unsigned int j = 0; j < TrueSZ.at(i).size(); j++) {
      xtruestt[cnt] = TrueSZ.at(i).at(j).X();
      ytruestt[cnt] = TrueSZ.at(i).at(j).Y();
      //			sigtruestt[cnt] = 0.3;
      cnt++;
    }
  }
  //	TGraphErrors *TrueSZPoints = new TGraphErrors(cnt,xtruestt,ytruestt, sigxtruestt, sigtruestt);
  TGraph *TrueSZPoints = new TGraphErrors(cnt, xtruestt, ytruestt);
  TrueSZPoints->SetMarkerStyle(4);
  TrueSZPoints->SetMarkerSize(marksize);
  TrueSZPoints->SetDrawOption("AP");
  TrueSZPoints->SetMarkerColor(kBlue);

  /*
   * Store all zphi from MvdPixels
   */

  double xmvdpixel[SZPairVectorMvdPixel.size()];
  double ymvdpixel[SZPairVectorMvdPixel.size()];
  //	double sigmvdpixel[SZPairVectorMvdPixel.size()];
  //	double sigxmvdpixel[SZPairVectorMvdPixel.size()];
  for (unsigned int j = 0; j < SZPairVectorMvdPixel.size(); j++) {
    xmvdpixel[j] = SZPairVectorMvdPixel.at(j).X();
    ymvdpixel[j] = SZPairVectorMvdPixel.at(j).Y();
    //		sigmvdpixel[j] = 0.0058;
  }
  //	TGraphErrors *SZPointMvdPixel = new TGraphErrors(SZPairVectorMvdPixel.size(),xmvdpixel,ymvdpixel, sigxmvdpixel, sigmvdpixel);
  TGraph *SZPointMvdPixel = new TGraphErrors(SZPairVectorMvdPixel.size(), xmvdpixel, ymvdpixel);
  SZPointMvdPixel->SetMarkerStyle(26);
  SZPointMvdPixel->SetMarkerColor(kRed);
  SZPointMvdPixel->SetMarkerSize(marksize);
  SZPointMvdPixel->SetDrawOption("AP");

  /*
   * Store selected zphi among MvdPixels
   */
  double xtruemvdpixel[TrueSZMvdPixel.size()];
  double ytruemvdpixel[TrueSZMvdPixel.size()];
  //	double sigtruemvdpixel[TrueSZMvdPixel.size()];
  //	double sigxtruemvdpixel[TrueSZMvdPixel.size()];
  for (unsigned int j = 0; j < TrueSZMvdPixel.size(); j++) {
    xtruemvdpixel[j] = TrueSZMvdPixel.at(j).X();
    ytruemvdpixel[j] = TrueSZMvdPixel.at(j).Y();
    //		sigtruemvdpixel[j] = 0.0058;
  }
  //	TGraphErrors *TrueSZPointsMvdPixel = new TGraphErrors(TrueSZMvdPixel.size(),xtruemvdpixel,ytruemvdpixel, sigxtruemvdpixel, sigtruemvdpixel);
  TGraph *TrueSZPointsMvdPixel = new TGraphErrors(TrueSZMvdPixel.size(), xtruemvdpixel, ytruemvdpixel);
  TrueSZPointsMvdPixel->SetMarkerStyle(4);
  TrueSZPointsMvdPixel->SetMarkerSize(marksize);
  TrueSZPointsMvdPixel->SetDrawOption("AP");
  TrueSZPointsMvdPixel->SetMarkerColor(kBlue);

  /*
   * Store all zphi from MvdStrips
   */

  double xmvdstrip[SZPairVectorMvdStrip.size()];
  double ymvdstrip[SZPairVectorMvdStrip.size()];
  //	double sigmvdstrip[SZPairVectorMvdStrip.size()];
  //	double sigxmvdstrip[SZPairVectorMvdStrip.size()];
  for (unsigned int j = 0; j < SZPairVectorMvdStrip.size(); j++) {
    xmvdstrip[j] = SZPairVectorMvdStrip.at(j).X();
    ymvdstrip[j] = SZPairVectorMvdStrip.at(j).Y();
    //		sigmvdstrip[j] = 0.0087;
  }
  //	TGraphErrors *SZPointMvdStrip = new TGraphErrors(SZPairVectorMvdStrip.size(),xmvdstrip,ymvdstrip, sigxmvdstrip,sigmvdstrip);
  TGraph *SZPointMvdStrip = new TGraphErrors(SZPairVectorMvdStrip.size(), xmvdstrip, ymvdstrip);
  SZPointMvdStrip->SetMarkerStyle(25);
  SZPointMvdStrip->SetMarkerColor(kRed);
  SZPointMvdStrip->SetMarkerSize(marksize);
  SZPointMvdStrip->SetDrawOption("AP");

  /*
   * Store selected zphi among MvdStrips
   */

  double xtruemvdstrip[TrueSZMvdStrip.size()];
  double ytruemvdstrip[TrueSZMvdStrip.size()];
  //	double sigtruemvdstrip[TrueSZMvdStrip.size()];
  //	double sigxtruemvdstrip[TrueSZMvdStrip.size()];
  for (unsigned int j = 0; j < TrueSZMvdStrip.size(); j++) {
    xtruemvdstrip[j] = TrueSZMvdStrip.at(j).X();
    ytruemvdstrip[j] = TrueSZMvdStrip.at(j).Y();
    //		sigtruemvdstrip[j] = 0.0087;
  }
  //	TGraphErrors *TrueSZPointsMvdStrip = new TGraphErrors(TrueSZMvdStrip.size(),xtruemvdstrip,ytruemvdstrip, sigxtruemvdstrip, sigtruemvdstrip);
  TGraph *TrueSZPointsMvdStrip = new TGraphErrors(TrueSZMvdStrip.size(), xtruemvdstrip, ytruemvdstrip);
  TrueSZPointsMvdStrip->SetMarkerStyle(4);
  TrueSZPointsMvdStrip->SetMarkerSize(marksize);
  TrueSZPointsMvdStrip->SetDrawOption("AP");
  TrueSZPointsMvdStrip->SetMarkerColor(kBlue);

  /*
   * Store all zphi from Gems
   */

  double xgem[SZPairVectorGem.size()];
  double ygem[SZPairVectorGem.size()];
  //  double siggem[SZPairVectorGem.size()];
  //  double sigxgem[SZPairVectorGem.size()];
  for (unsigned int j = 0; j < SZPairVectorGem.size(); j++) {
    xgem[j] = SZPairVectorGem.at(j).X();
    ygem[j] = SZPairVectorGem.at(j).Y();
    //    siggem[j] = 0.0087;
  }
  //  TGraphErrors *SZPointGem = new TGraphErrors(SZPairVectorGem.size(),xgem,ygem, sigxgem,siggem);
  TGraph *SZPointGem = new TGraphErrors(SZPairVectorGem.size(), xgem, ygem);
  SZPointGem->SetMarkerStyle(25);
  SZPointGem->SetMarkerColor(kRed);
  SZPointGem->SetMarkerSize(marksize);
  SZPointGem->SetDrawOption("AP");

  /*
   * Store selected zphi among Gems
   */

  double xtruegem[TrueSZGem.size()];
  double ytruegem[TrueSZGem.size()];
  //  double sigtruegem[TrueSZGem.size()];
  //  double sigxtruegem[TrueSZGem.size()];
  for (unsigned int j = 0; j < TrueSZGem.size(); j++) {
    xtruegem[j] = TrueSZGem.at(j).X();
    ytruegem[j] = TrueSZGem.at(j).Y();
    //    sigtruegem[j] = 0.0087;
  }
  //  TGraphErrors *TrueSZPointsGem = new TGraphErrors(TrueSZGem.size(),xtruegem,ytruegem, sigxtruegem, sigtruegem);
  TGraph *TrueSZPointsGem = new TGraphErrors(TrueSZGem.size(), xtruegem, ytruegem);
  TrueSZPointsGem->SetMarkerStyle(4);
  TrueSZPointsGem->SetMarkerSize(marksize);
  TrueSZPointsGem->SetDrawOption("AP");
  TrueSZPointsGem->SetMarkerColor(kBlue);

  /*
   * Plot all points
   */

  TMultiGraph *mg = new TMultiGraph();
  mg->SetTitle("");
  if (SZPoints->Sizeof() > 0)
    mg->Add(SZPoints, "P");
  if (TrueSZPoints->Sizeof() > 0)
    mg->Add(TrueSZPoints, "P");

  if (SZPairVectorMvdPixel.size() > 0)
    mg->Add(SZPointMvdPixel, "P");
  if (TrueSZMvdPixel.size() > 0)
    mg->Add(TrueSZPointsMvdPixel, "P");

  if (SZPairVectorMvdStrip.size() > 0)
    mg->Add(SZPointMvdStrip, "P");
  if (TrueSZMvdStrip.size() > 0)
    mg->Add(TrueSZPointsMvdStrip, "P");

  if (SZPairVectorGem.size() > 0)
    mg->Add(SZPointGem, "P");
  if (TrueSZGem.size() > 0)
    mg->Add(TrueSZPointsGem, "P");

  mg->Draw("a");
  TF1 *fa = new TF1("fa", "[0]*x + [1]", mg->GetXaxis()->GetXmin(), mg->GetXaxis()->GetXmax());
  fa->SetParameter(0, slope);
  fa->SetParameter(1, intercept);
  TGraph *lineGraph = new TGraph(fa, "");
  lineGraph->SetLineColor(kBlue);
  mg->Add(lineGraph);
  mg->GetXaxis()->SetTitle("S /cm");
  mg->GetXaxis()->SetTitleSize(0.06);
  mg->GetXaxis()->SetTitleOffset(0.7);
  mg->GetXaxis()->SetLabelOffset(0.);
  mg->GetXaxis()->SetLabelSize(0.055);
  mg->GetYaxis()->SetTitle("z /cm"); // Change back to ("R*#phi /cm")
  mg->GetYaxis()->SetTitleSize(0.06);
  mg->GetYaxis()->SetTitleOffset(0.8);
  mg->GetYaxis()->SetLabelOffset(0);
  mg->GetYaxis()->SetLabelSize(0.055);
}
