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

#include "PndMvdAllDataEventAna.h"
#include "PndSdsMCPoint.h"
#include "PndSdsPixel.h"
#include "GFTrackCand.h"
#include "TVector3.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TLegend.h"
#include "TGeoMatrix.h"
#include "PndFileNameCreator.h"

#include "TGeoTrack.h"
#include <sstream>

ClassImp(PndMvdAllDataEventAna);

PndMvdAllDataEventAna::PndMvdAllDataEventAna(TString fileName) : PndMvdEventAna(fileName)
{
  Init(fileName);
  SetCanvasColumns(6);

  fGeoH = PndGeoHandling::Instance();

  fRecoVolume = gGeoManager->MakeSphere("RecoHit", gGeoManager->GetMedium("vacuum"), 0, 0.1);
  fRecoVolume->SetLineColor(kRed);

  TGeoVolume *mcHit = gGeoManager->MakeSphere("MCHit", gGeoManager->GetMedium("vacuum"), 0, 0.1);
  mcHit->SetLineColor(kGreen);

  TGeoShape *stripShape = gGeoManager->GetVolume("StripSensor")->GetShape();
  TGeoVolume *stripVol = new TGeoVolume("StripSensorRed", stripShape, gGeoManager->GetMedium("vacuum"));
  stripVol->SetLineColor(kRed);
  gGeoManager->AddVolume(stripVol);
  stripVol = gGeoManager->GetVolume("StripSensorRed");

  TGeoShape *pixShape1 = gGeoManager->GetVolume("SensorActiveAreao[NrFE=8ooE]")->GetShape();
  TGeoVolume *pixVol1 = new TGeoVolume("PixelSensor8", pixShape1, gGeoManager->GetMedium("vacuum"));
  pixVol1->SetLineColor(kRed);
  gGeoManager->AddVolume(pixVol1);
  pixVol1 = gGeoManager->GetVolume("PixelSensor8");

  TGeoShape *pixShape2 = gGeoManager->GetVolume("SensorActiveAreao[NrFE=4]")->GetShape();
  TGeoVolume *pixVol2 = new TGeoVolume("PixelSensor4", pixShape2, gGeoManager->GetMedium("vacuum"));
  pixVol2->SetLineColor(kRed);
  gGeoManager->AddVolume(pixVol2);
  pixVol2 = gGeoManager->GetVolume("PixelSensor4");

  TGeoShape *pixShape3 = gGeoManager->GetVolume("SensorActiveAreao[NrFE=5]")->GetShape();
  TGeoVolume *pixVol3 = new TGeoVolume("PixelSensor5", pixShape3, gGeoManager->GetMedium("vacuum"));
  pixVol3->SetLineColor(kRed);
  gGeoManager->AddVolume(pixVol3);
  pixVol3 = gGeoManager->GetVolume("PixelSensor5");

  TGeoVolume *stripVolG = new TGeoVolume("StripSensorGreen", stripShape, gGeoManager->GetMedium("vacuum"));
  stripVolG->SetLineColor(kGreen);
  gGeoManager->AddVolume(stripVolG);
  stripVolG = gGeoManager->GetVolume("StripSensorGreen");

  TGeoVolume *pixVolG1 = new TGeoVolume("PixelSensor8Green", pixShape1, gGeoManager->GetMedium("vacuum"));
  pixVolG1->SetLineColor(kGreen);
  gGeoManager->AddVolume(pixVolG1);
  pixVolG1 = gGeoManager->GetVolume("PixelSensor8Green");

  TGeoVolume *pixVolG2 = new TGeoVolume("PixelSensor4Green", pixShape2, gGeoManager->GetMedium("vacuum"));
  pixVolG2->SetLineColor(kGreen);
  gGeoManager->AddVolume(pixVolG2);
  pixVolG2 = gGeoManager->GetVolume("PixelSensor4Green");

  TGeoVolume *pixVolG3 = new TGeoVolume("PixelSensor5Green", pixShape3, gGeoManager->GetMedium("vacuum"));
  pixVolG3->SetLineColor(kGreen);
  gGeoManager->AddVolume(pixVolG3);
  pixVolG3 = gGeoManager->GetVolume("PixelSensor5Green");

  fMvdTopVolume = gGeoManager->GetVolume("MVDoOption1");
  fGeoList = new PndEventDisplay();

  fGeoList->AddNewGroup("RecoHits", new PndGeoHitList("RecoHits", "MVDoOption1", fRecoVolume));
  fGeoList->AddNewGroup("MCHits", new PndGeoHitList("MCHits", "MVDoOption1", mcHit));
  fGeoList->AddNewGroup("StripHits", new PndGeoHitList("StripHits", "MVDoOption1", stripVol));
  fGeoList->AddNewGroup("PixHits8", new PndGeoHitList("PixHits8", "MVDoOption1", pixVol1));
  fGeoList->AddNewGroup("PixHits4", new PndGeoHitList("PixHits4", "MVDoOption1", pixVol2));
  fGeoList->AddNewGroup("PixHits5", new PndGeoHitList("PixHits5", "MVDoOption1", pixVol3));
  fGeoList->AddNewGroup("StripHitsGreen", new PndGeoHitList("StripHitsGreen", "MVDoOption1", stripVolG));
  fGeoList->AddNewGroup("PixHits8Green", new PndGeoHitList("PixHits8Green", "MVDoOption1", pixVolG1));
  fGeoList->AddNewGroup("PixHits4Green", new PndGeoHitList("PixHits4Green", "MVDoOption1", pixVolG2));
  fGeoList->AddNewGroup("PixHits5Green", new PndGeoHitList("PixHits5Green", "MVDoOption1", pixVolG3));

  fPixelCon = new PndSdsCalcFePixel(76, 84, 10);
}

void PndMvdAllDataEventAna::Init(TString fileName)
{
  PndFileNameCreator nameCreator(fileName.Data());
  fFile = new TFile(fileName.Data());
  fTree = (TTree *)fFile->Get("pndsim");

  fTree->AddFriend("digi=pndsim", nameCreator.GetDigiFileName().c_str());
  fTree->AddFriend("reco=pndsim", nameCreator.GetRecoFileName().c_str());
  fTree->AddFriend("trackF=pndsim", nameCreator.GetTrackFindingFileName().c_str());

  fHitArray = new TClonesArray("PndSdsMCPoint");
  fDigiArray = new TClonesArray("PndSdsDigiPixel");
  fClusterArray = new TClonesArray("PndSdsCluster");
  fRecoArray = new TClonesArray("PndSdsHit");
  fGeoTrackArray = new TClonesArray("TGeoTrack");
  fTrackFArray = new TClonesArray("GFTrackCand");

  fTree->SetBranchAddress("MVDPoint", &fHitArray);
  fTree->SetBranchAddress("MVDPixelDigis", &fDigiArray);
  fTree->SetBranchAddress("MVDClusterCand", &fClusterArray);
  fTree->SetBranchAddress("MVDClusterHit", &fRecoArray);
  fTree->SetBranchAddress("MVDTrackCand", &fTrackFArray);
  if (fTree->FindBranch("GeoTracks") != 0)
    fTree->SetBranchAddress("GeoTracks", &fGeoTrackArray);

  fAllHitPerClusterHistos = new TH1I("hitPerCluster", "hitPerCluster", 21, 0, 20);
  fAllHitResolutionHistos = new TH1D("AllHitResolution", "AllHitResolution", 1000, 0, 0.1);
  f3DMCHisto = new TH3D("h3D", "h3D", 50, -15, 15, 50, -15, 15, 50, -30, 30);
  f3DRecoHisto = new TH3D("h3D", "h3D", 50, -15, 15, 50, -15, 15, 50, -30, 30);
}

void PndMvdAllDataEventAna::InitBranch() {}

void PndMvdAllDataEventAna::InitHistos() {}

void PndMvdAllDataEventAna::ClearHistos()
{
  ClearAllHMaps();
  ClearAllVectors();

  fGeoList->ClearHits("RecoHits");
  fGeoList->ClearHits("MCHits");
  fGeoList->ClearHits("StripHits");
  fGeoList->ClearHits("PixHits4");
  fGeoList->ClearHits("PixHits5");
  fGeoList->ClearHits("PixHits8");
  fGeoList->ClearHits("StripHitsGreen");
  fGeoList->ClearHits("PixHits4Green");
  fGeoList->ClearHits("PixHits5Green");
  fGeoList->ClearHits("PixHits8Green");
}

void PndMvdAllDataEventAna::AnaHits()
{
  fHitArray->Delete();
  fDigiArray->Delete();
  fClusterArray->Delete();
  fRecoArray->Delete();

  fTree->GetEntry(fActiveEvent);

  std::cout << "hitArray: " << fHitArray->GetEntries() << std::endl;
  std::cout << "digiArray: " << fDigiArray->GetEntries() << std::endl;
  std::cout << "clusterArray: " << fClusterArray->GetEntries() << std::endl;
  std::cout << "recoArray: " << fRecoArray->GetEntries() << std::endl;

  PrintHitArray();
  PrintDigiArray();
  PrintClusterArray();
  PrintRecoArray();

  FillHitHistos();
  FillDigiHistos();
  FillClusterHistos();
  FillRecoHistos();
  FillHitPerClusterHistos();
  FillHitResolutionHistos();
  Fill3DHisto();
  FillHitProjHistos();

  Create3DGeoHits();
}

void PndMvdAllDataEventAna::PrintHitArray()
{
  for (Int_t i = 0; i < fHitArray->GetEntries(); i++) {
    std::cout << i << ": ";
    PndSdsMCPoint *myPoint = (PndSdsMCPoint *)fHitArray->At(i);
    std::cout << *myPoint;
  }
}
void PndMvdAllDataEventAna::PrintDigiArray()
{
  for (Int_t i = 0; i < fDigiArray->GetEntries(); i++) {
    PndSdsDigiPixel *myDigi = (PndSdsDigiPixel *)fDigiArray->At(i);
    std::cout << i << ": ";
    // myDigi->Print();
    std::cout << *myDigi;
  }
}
void PndMvdAllDataEventAna::PrintClusterArray()
{
  for (Int_t i = 0; i < fClusterArray->GetEntries(); i++) {
    std::cout << "Cluster " << i << ": " << std::endl;
    PndSdsCluster *myCluster = (PndSdsCluster *)fClusterArray->At(i);
    // myCluster->Print();
    std::vector<Int_t> myClusterList = myCluster->GetClusterList();
    for (UInt_t j = 0; j < myClusterList.size(); j++) {
      PndSdsDigiPixel *myDigi = (PndSdsDigiPixel *)fDigiArray->At(myClusterList[j]);
      std::cout << myClusterList[j] << ": ";
      std::cout << *myDigi;
    }
  }
}
void PndMvdAllDataEventAna::PrintRecoArray()
{
  for (Int_t i = 0; i < fRecoArray->GetEntries(); i++) {
    std::cout << i << ": ";
    PndSdsHit *myHit = (PndSdsHit *)fRecoArray->At(i);
    std::cout << *myHit;
  }
}

TVector3 PndMvdAllDataEventAna::GetLocalHitPoints(TString detName, TVector3 input)
{
  TVector3 result;
  Double_t in[3];
  Double_t local[3];

  in[0] = input.X();
  in[1] = input.Y();
  in[2] = input.Z();

  gGeoManager->cd(fGeoH->GetPath(detName.Data()));
  TGeoHMatrix *transMat = gGeoManager->GetCurrentMatrix();

  transMat->MasterToLocal(in, local);

  TGeoVolume *actVolume = gGeoManager->GetCurrentVolume();
  TGeoBBox *actBox = (TGeoBBox *)(actVolume->GetShape());

  result.SetX(local[0] + actBox->GetDX());
  result.SetY(local[1] + actBox->GetDY());
  result.SetZ(local[2] + actBox->GetDZ());

  return result;
}

void PndMvdAllDataEventAna::FillHitHistos()
{
  for (Int_t i = 0; i < fHitArray->GetEntries(); i++) {
    PndSdsMCPoint *myPoint = (PndSdsMCPoint *)fHitArray->At(i);
    TString detName = myPoint->GetDetName();
    // if (hit->GetDetName().Contains("119_2")){
    if (fHistos[detName] == 0) {
      fHistos[detName] = new TH2I("MCHisto", detName.Data(), 1000, 0, 1000, 201, 0, 200);
      // fDrawOption[hit->GetDetName()] = "colz";
    }
    // TH2* tempHisto = (TH2*)(fHistos[detName]);
    gGeoManager->cd(fGeoH->GetPath(detName.Data()));
    // TGeoHMatrix* transMat = gGeoManager->GetCurrentMatrix();

    TVector3 in(myPoint->GetX(), myPoint->GetY(), myPoint->GetZ());
    TVector3 out(myPoint->GetXOut(), myPoint->GetYOut(), myPoint->GetZOut());
    TVector3 inLocal, outLocal;
    inLocal = GetLocalHitPoints(detName, in);
    outLocal = GetLocalHitPoints(detName, out);

    fHistos[detName]->Fill(inLocal.X() * 100, inLocal.Y() * 100);
    fHistos[detName]->Fill(outLocal.X() * 100, outLocal.Y() * 100);

    //    std::cout << i << ": " << std::endl;
    //     for (Int_t i = 0; i < 3; i++){
    //          std::cout << "posInLocal "<< i << ": " << inLocal(i) << std::endl;
    //            std::cout << "posOutLocal "<< i << ": " << outLocal(i) << std::endl;
    //        }
  }
}
void PndMvdAllDataEventAna::FillDigiHistos()
{
  for (Int_t i = 0; i < fDigiArray->GetEntries(); i++) {
    PndSdsDigiPixel *hit = (PndSdsDigiPixel *)fDigiArray->At(i);
    // if (hit->GetDetName().Contains("119_2")){
    if (fDigiHistos[hit->GetDetName()] == 0) {
      fDigiHistos[hit->GetDetName()] = new TH2I("digiHisto", hit->GetDetName().Data(), 1000, 0, 1000, 201, 0, 200);
      // fDrawOption[hit->GetDetName()] = "colz";
    }
    TH2 *tempHisto = (TH2 *)(fDigiHistos[hit->GetDetName()]);
    PndSdsPixel myFePixel(hit->GetSensorID(), hit->GetFE(), hit->GetPixelColumn(), hit->GetPixelRow(), hit->GetCharge());
    PndSdsPixel mySensorPixel = fPixelCon->CalcSensorHit(myFePixel);
    tempHisto->Fill(mySensorPixel.GetCol(), mySensorPixel.GetRow(), mySensorPixel.GetCharge()); //, (Double_t)(hit->GetCharge()));
  }
}
void PndMvdAllDataEventAna::FillClusterHistos()
{
  for (Int_t i = 0; i < fClusterArray->GetEntries(); i++) {
    PndSdsCluster *cluster = (PndSdsCluster *)fClusterArray->At(i);
    // if (hit->GetDetName().Contains("119_2")){
    std::vector<Int_t> clusterList = cluster->GetClusterList();
    PndSdsDigiPixel *hit = (PndSdsDigiPixel *)fDigiArray->At(clusterList[0]);
    Int_t sensorID = hit->GetSensorID();
    if (fClusterHistos[sensorID] == 0) {
      fClusterHistos[sensorID] = new TH2I("clusterHisto", detName.Data(), 1000, 0, 1000, 201, 0, 200);
      // fDrawOption[hit->GetDetName()] = "colz";
    }
    TH2 *tempHisto = (TH2 *)(fClusterHistos[detName]);
    for (Int_t j = 0; j < clusterList.size(); j++) {
      hit = (PndSdsDigiPixel *)fDigiArray->At(clusterList[j]);
      PndSdsPixel myFePixel(sensorID, hit->GetFE(), hit->GetPixelColumn(), hit->GetPixelRow(), hit->GetCharge());
      PndSdsPixel mySensorPixel = fPixelCon->CalcSensorHit(myFePixel);
      tempHisto->Fill(mySensorPixel.GetCol(), mySensorPixel.GetRow());
    }
  }
}

void PndMvdAllDataEventAna::FillRecoHistos()
{
  for (Int_t i = 0; i < fRecoArray->GetEntries(); i++) {
    PndSdsHit *myHit = (PndSdsHit *)fRecoArray->At(i);
    TString detName = myHit->GetDetName();
    // if (hit->GetDetName().Contains("119_2")){
    if (fRecoHistos[detName] == 0) {
      fRecoHistos[detName] = new TH2I("RecoHisto", detName.Data(), 1000, 0, 1000, 201, 0, 200);
      // fDrawOption[hit->GetDetName()] = "colz";
    }
    // TH2* tempHisto = (TH2*)(fRecoHistos[detName]);

    TVector3 in(myHit->GetX(), myHit->GetY(), myHit->GetZ());
    TVector3 inLocal;
    inLocal = GetLocalHitPoints(detName, in);

    fRecoHistos[detName]->Fill(inLocal.X() * 100, inLocal.Y() * 100);

    //    std::cout << i << ": " << std::endl;
    //     for (Int_t i = 0; i < 3; i++){
    //          std::cout << "posInLocal "<< i << ": " << inLocal(i) << std::endl;
    //        }
  }
}

void PndMvdAllDataEventAna::FillHitPerClusterHistos()
{
  for (Int_t i = 0; i < fClusterArray->GetEntries(); i++) {
    PndSdsCluster *cand = (PndSdsCluster *)fClusterArray->At(i);
    std::vector<Int_t> hits = GetHitPerCluster(cand);
    PndSdsMCPoint *point = (PndSdsMCPoint *)(fHitArray->At(hits[0]));
    TString detName = point->GetDetName();
    if (fHitPerClusterHistos[detName] == 0) {
      fHitPerClusterHistos[detName] = new TH1I("HitPerCluster", detName.Data(), 21, 0, 20);
      // fDrawOption[hit->GetDetName()] = "colz";
    }
    fHitPerClusterHistos[detName]->Fill(hits.size());
    fAllHitPerClusterHistos->Fill(hits.size());
    std::cout << "Hits in Cluster: " << hits.size() << " for Det: " << detName.Data() << std::endl;
  }
}

void PndMvdAllDataEventAna::FillHitResolutionHistos()
{
  for (Int_t i = 0; i < fRecoArray->GetEntries(); i++) {
    PndSdsHit *myHit = (PndSdsHit *)fRecoArray->At(i);
    TString detName = myHit->GetDetName();
    std::cout << "HitResolution for: " << detName << std::endl;
    TVector3 recoPos = myHit->GetPosition();
    std::cout << "RecoPos: " << recoPos.X() << " " << recoPos.Y() << " " << recoPos.Z() << std::endl;
    std::cout << "MC point RefIndex: " << myHit->GetRefIndex() << std::endl;
    std::cout << "Cluster Index: " << myHit->GetClusterIndex() << std::endl;
    PndSdsCluster *myCand = (PndSdsCluster *)(fClusterArray->At(myHit->GetClusterIndex()));
    std::vector<Int_t> points = GetHitPerCluster(myCand);
    for (UInt_t k = 0; k < points.size(); i++)
      std::cout << "ClusterPoints: " << points[k] << std::endl;
    TVector3 hitPos = CalcMeanHitPos(points);
    std::cout << "HitPos: " << hitPos.X() << " " << hitPos.Y() << " " << hitPos.Z() << std::endl;
    if (fHitResolutionHistos[detName] == 0) {
      fHitResolutionHistos[detName] = new TH1D("HitRes", detName.Data(), 1000, 0, 0.01);
      // fDrawOption[hit->GetDetName()] = "colz";
    }
    TVector3 resPos = recoPos - hitPos;
    fHitResolutionHistos[detName]->Fill(resPos.Mag());
    fAllHitResolutionHistos->Fill(resPos.Mag());
    std::cout << "HitRes: " << resPos.Mag() << std::endl;
  }
}

void PndMvdAllDataEventAna::Fill3DHisto()
{
  for (Int_t i = 0; i < fRecoArray->GetEntries(); i++) {
    PndSdsHit *myHit = (PndSdsHit *)fRecoArray->At(i);
    f3DRecoHisto->Fill(myHit->GetX(), myHit->GetY(), myHit->GetZ());
  }
  for (Int_t j = 0; j < fHitArray->GetEntries(); j++) {
    PndSdsMCPoint *myPoint = (PndSdsMCPoint *)fHitArray->At(j);
    f3DMCHisto->Fill(myPoint->GetX(), myPoint->GetY(), myPoint->GetZ());
  }
}

void PndMvdAllDataEventAna::FillHitProjHistos()
{
  ClearHistoVector(&fRecoHisxy);
  ClearHistoVector(&fRecoHisrz);
  unsigned int detID, hitID;
  TVector3 vec;

  for (Int_t i = 0; i < fTrackFArray->GetEntries(); i++) {
    GFTrackCand *trackC = (GFTrackCand *)fTrackFArray->At(i);
    TH2D *myHistoXY = new TH2D("recohisxy", "MVD Reco Points, xy view", 400, -15., 15., 400, -15., 15.);
    myHistoXY->SetMarkerStyle(7);
    myHistoXY->SetMarkerColor(i + 1);

    TH2D *myHistoRZ = new TH2D("recohisrz", "MVD Reco Points, rz view", 400, -20., 20., 400, -20., 20.);
    myHistoRZ->SetMarkerStyle(7);
    myHistoRZ->SetMarkerColor(i + 1);

    fRecoHisxy.push_back(myHistoXY);
    fRecoHisrz.push_back(myHistoRZ);
    for (UInt_t j = 0; j < trackC->getNHits(); j++) {
      trackC->getHit(j, detID, hitID);
      PndSdsHit *myHit = (PndSdsHit *)fRecoArray->At(hitID);
      vec.SetXYZ(myHit->GetX(), myHit->GetY(), myHit->GetZ());
      myHistoXY->Fill(vec.x(), vec.y());
      myHistoRZ->Fill(vec.z(), vec.Perp());
    }
  }
}

TVector3 PndMvdAllDataEventAna::CalcMeanHitPos(std::vector<Int_t> points)
{
  TVector3 result;
  Double_t energy = 0;
  for (UInt_t i = 0; i < points.size(); i++) {
    PndSdsMCPoint *myPoint = (PndSdsMCPoint *)fHitArray->At(points[i]);
    TVector3 posIn = myPoint->GetPosition();
    std::cout << "posIn: " << posIn.X() << " " << posIn.Y() << " " << posIn.Z() << std::endl;
    TVector3 posOut = myPoint->GetPositionOut();
    std::cout << "posOut: " << posOut.X() << " " << posOut.Y() << " " << posOut.Z() << std::endl;

    TVector3 posHit = posIn + posOut;
    posHit *= 0.5;

    std::cout << "posHit: " << posHit.X() << " " << posHit.Y() << " " << posHit.Z() << std::endl;
    // posHit *= myPoint->GetEnergyLoss();
    energy += myPoint->GetEnergyLoss();
    result += posHit;
  }
  // Double_t div = 1/energy;
  Double_t div = 1 / (Double_t)points.size();
  result *= div;
  return result;
}

void PndMvdAllDataEventAna::DrawHitHisto(TString detName, TCanvas *extCan, Int_t pad)
{
  if (extCan)
    extCan->cd(pad);

  if (fHistos[detName] != 0)
    fHistos[detName]->Draw();
}

void PndMvdAllDataEventAna::DrawDigiHisto(TString detName, TCanvas *extCan, Int_t pad)
{
  if (extCan)
    extCan->cd(pad);

  if (fDigiHistos[detName] != 0)
    fDigiHistos[detName]->Draw("colz");
}
void PndMvdAllDataEventAna::DrawClusterHisto(TString detName, TCanvas *extCan, Int_t pad)
{
  if (extCan)
    extCan->cd(pad);

  if (fClusterHistos[detName] != 0)
    fClusterHistos[detName]->Draw("colz");
}
void PndMvdAllDataEventAna::DrawRecoHisto(TString detName, TCanvas *extCan, Int_t pad)
{
  if (extCan)
    extCan->cd(pad);

  if (fRecoHistos[detName] != 0)
    fRecoHistos[detName]->Draw();
}

void PndMvdAllDataEventAna::DrawAllHistos(TString detName, TCanvas *extCan)
{
  if (extCan)
    fCan1 = extCan;
  if (fCan1 == 0)
    fCan1 = new TCanvas();
  DrawHitHisto(detName, fCan1, 1);
  DrawDigiHisto(detName, fCan1, 2);
  DrawClusterHisto(detName, fCan1, 3);
  DrawRecoHisto(detName, fCan1, 4);
  fCan1->cd(5);
  fHitPerClusterHistos[detName]->Draw();
  fCan1->cd(6);
  fHitResolutionHistos[detName]->Draw();
  fCan1->Modified(kTRUE);
  fCan1->Update();
}

void PndMvdAllDataEventAna::DrawAllHistos(Int_t index, TCanvas *extCan)
{
  std::map<TString, TH1 *>::const_iterator ki;
  Int_t histoSize = fHistos.size();

  if (index >= histoSize)
    return;
  ki = fHistos.begin();
  for (Int_t i = 0; i < index; i++)
    ki++;
  TString detName = ki->first;
  std::cout << "DetName: " << detName.Data() << std::endl;
  DrawAllHistos(detName, extCan);
}

void PndMvdAllDataEventAna::Draw3D(TString opt, TCanvas *extCan, Int_t pad)
{
  if (extCan == 0)
    extCan = new TCanvas();
  extCan->cd(pad);
  f3DMCHisto->SetMarkerStyle(8);
  f3DMCHisto->SetMarkerSize(0.5);
  f3DMCHisto->SetMarkerColor(2);
  f3DMCHisto->Draw(opt.Data());
  f3DRecoHisto->SetMarkerStyle(8);
  f3DRecoHisto->SetMarkerSize(0.5);
  f3DRecoHisto->SetMarkerColor(1);
  f3DRecoHisto->Draw("same");
}

void PndMvdAllDataEventAna::DrawAllTracks(TCanvas *extCan, Int_t pad)
{
  if (extCan == 0)
    extCan = new TCanvas();
  extCan->cd(pad);

  for (Int_t i = 0; i < fGeoTrackArray->GetEntries(); i++) {
    TGeoTrack *myTrack = (TGeoTrack *)fGeoTrackArray->At(i);
    myTrack->Print();
    myTrack->Draw();
  }
}

void PndMvdAllDataEventAna::DrawHitTracks(TCanvas *extCan, Int_t pad)
{
  if (extCan == 0)
    extCan = new TCanvas();
  extCan->cd(pad);

  std::map<Int_t, Int_t> trackUsed;
  for (Int_t i = 0; i < fHitArray->GetEntries(); i++) {
    PndSdsMCPoint *myPoint = (PndSdsMCPoint *)fHitArray->At(i);
    std::cout << "Hit " << i << " TrackID " << myPoint->GetTrackID() << std::endl;
    if (trackUsed[myPoint->GetTrackID()] == 0) {
      for (Int_t j = 0; j < fGeoTrackArray->GetEntries(); j++) {
        TGeoTrack *myTrack = (TGeoTrack *)fGeoTrackArray->At(j);
        if (myPoint->GetTrackID() == myTrack->GetId()) {
          myTrack->Draw();
          myTrack->Print();
        }
      }
      trackUsed[myPoint->GetTrackID()] = 1;
    }
  }
}

void PndMvdAllDataEventAna::DrawTopVolume(TCanvas *extCan, Int_t pad, const char *opt)
{
  if (extCan)
    extCan->cd(pad);
  //  fMvdTopVolume->Draw(""); //ogl for OpenGL Viewer
  fMvdTopVolume->Draw(opt);
}

void PndMvdAllDataEventAna::DrawHisrz(TCanvas *extCan, Int_t pad)
{
  DrawHistoVec(&fRecoHisxy, extCan, pad);
}

void PndMvdAllDataEventAna::DrawHisxy(TCanvas *extCan, Int_t pad)
{
  DrawHistoVec(&fRecoHisrz, extCan, pad);
}

void PndMvdAllDataEventAna::DrawHistoVec(std::vector<TH1 *> *vec, TCanvas *extCan, Int_t pad) const
{
  if (extCan)
    extCan->cd(pad);
  if (vec->size() > 0)
    (*vec)[0]->Draw();
  for (UInt_t i = 1; i < vec->size(); i++)
    (*vec)[i]->Draw("same");
}

void PndMvdAllDataEventAna::DrawHisto(TH1 *histo, TCanvas *extCan, Int_t pad) const
{
  if (extCan)
    extCan->cd(pad);
  histo->Draw();
}

void PndMvdAllDataEventAna::DrawEvent(bool tracks, TCanvas *extCan)
{
  if (extCan)
    fCan2 = extCan;
  if (fCan2 == 0)
    fCan2 = new TCanvas();

  fCan2->Divide(2, 2);

  fCan2->cd(1);
  DrawTopVolume();

  if (tracks)
    DrawHitTracks();

  fCan2->cd(2);
  fAllHitResolutionHistos->Draw();

  fCan2->cd(3);
  DrawHistoVec(&fRecoHisxy);

  fCan2->cd(4);
  DrawHistoVec(&fRecoHisrz);
}

std::vector<Int_t> PndMvdAllDataEventAna::GetHitPerCluster(PndSdsCluster *clusterCand)
{
  std::vector<Int_t> result;
  std::vector<Int_t> digiPos = clusterCand->GetClusterList();
  bool isInResult = false;
  for (UInt_t i = 0; i < digiPos.size(); i++) {
    PndSdsDigiPixel *digiHit = (PndSdsDigiPixel *)(fDigiArray->At(digiPos[i]));
    Int_t mcID = digiHit->GetIndex(0);
    std::cout << " -I- GetHitPerCluster: mcID: " << mcID << std::endl;
    for (UInt_t j = 0; j < result.size() && isInResult == false; j++) {
      // std::cout << "Result: " << result[j] << std::endl;
      if (mcID == result[j])
        isInResult = true;
    }
    if (isInResult == false)
      result.push_back(mcID);
    isInResult = false;
  }
  for (UInt_t k = 0; k < result.size(); k++)
    std::cout << " Result: " << k << ": " << result[k] << std::endl;
  return result;
}

void PndMvdAllDataEventAna::Create3DGeoHits()
{
  fGeoList->SetHits("RecoHits", fRecoArray);

  for (Int_t i = 0; i < fHitArray->GetEntriesFast(); i++) {
    PndSdsMCPoint *myPoint = (PndSdsMCPoint *)fHitArray->At(i);
    fGeoList->AddHit("MCHits", myPoint->GetX(), myPoint->GetY(), myPoint->GetZ());
    std::cout << "gGeoManager cd: " << myPoint->GetDetName() << " " << gGeoManager->cd(fGeoH->GetPath(myPoint->GetDetName())) << std::endl;
    TGeoHMatrix *mat = gGeoManager->GetCurrentMatrix();
    mat->Print();
    if (myPoint->GetDetName().Contains("71")) {
      std::cout << "StripHits added!" << std::endl;
      fGeoList->AddHit("StripHits", mat);
      fGeoList->AddHit("StripHitsGreen", mat, kFALSE);
    } else if (myPoint->GetDetName().Contains("89")) {
      std::cout << "PixHits4 added!" << std::endl;
      fGeoList->AddHit("PixHits4", mat);
      fGeoList->AddHit("PixHits4Green", mat, kFALSE);
    } else if (myPoint->GetDetName().Contains("56")) {
      std::cout << "PixHits8 added!" << std::endl;
      fGeoList->AddHit("PixHits8", mat);
      fGeoList->AddHit("PixHits8Green", mat, kFALSE);
    }
  }
}

void PndMvdAllDataEventAna::ClearHistoMaps(std::map<TString, TH1 *> *myMaps) const
{
  for (std::map<TString, TH1 *>::const_iterator ki = myMaps->begin(); ki != myMaps->end(); ki++) {
    ki->second->Delete();
    (*myMaps)[ki->first] = 0;
  }
  myMaps->clear();
  std::cout << " Maps.size: " << myMaps->size() << std::endl;
}

void PndMvdAllDataEventAna::ClearAllHMaps()
{
  ClearHistoMaps(&fHistos);
  ClearHistoMaps(&fRecoHistos);
  ClearHistoMaps(&fDigiHistos);
  ClearHistoMaps(&fClusterHistos);
  ClearHistoMaps(&fHitPerClusterHistos);
  ClearHistoMaps(&fHitResolutionHistos);
}

void PndMvdAllDataEventAna::ClearAllVectors()
{
  ClearHistoVector(&fHitHistoVec);
  ClearHistoVector(&fDigiHistoVec);
  ClearHistoVector(&fClusterHistoVec);
  ClearHistoVector(&fRecoHistoVec);
}
void PndMvdAllDataEventAna::ClearHistoVector(std::vector<TH1 *> *myVectors) const
{
  for (UInt_t i = 0; i < myVectors->size(); i++) {
    delete ((*myVectors)[i]);
  }
  myVectors->clear();
}

std::vector<TString> PndMvdAllDataEventAna::GetModulesHit()
{
  std::vector<TString> result;
  for (std::map<TString, TH1 *>::const_iterator ki = fRecoHistos.begin(); ki != fRecoHistos.end(); ++ki) {
    std::cout << "First: " << ki->first << std::endl;
    std::cout << "Path: " << fGeoH->GetPath(ki->first) << std::endl;
    result.push_back(fGeoH->GetPath(ki->first));
  }
  return result;
}
