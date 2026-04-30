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

// using timestamp for the clock counts
// and timestamperr to store the spill resets (just a temporary solution)

#include "PndMvdConvertApv.h"

//#include "TsEvent.h"
#include "PndMvdSiHit.h"

#include "PndMvdApvHit.h"
#include "PndSdsDigiStrip.h"
#include "TString.h"
#include <fstream>
#include <vector>
#include <map>

using namespace std;

// -----   Constructor   --------------------------------------------

PndMvdConvertApv::PndMvdConvertApv(const TString &CalibFileName, const TString &HitFileName)
  : fCalibPars(), fNofEvents(0), fEvent(-1), fLastEvent(0), fNoCalib(kFALSE), fHitFileName(""), fDataFile(), fhitlist(), fTopModuleID(0), fBottomModuleID(0), fFake(kFALSE),
    f(nullptr), t(nullptr), tsEv(nullptr), arr(nullptr), fGeoH(nullptr)
{

  f = new TFile(HitFileName);
  fGeoH = PndGeoHandling::Instance();
  t = (TTree *)f->Get("T");
  tsEv = new PndMvdTsEvent();
  arr = new TClonesArray("PndMvdSiHit");

  cout << "---------------------------------------" << endl;
  cout << "Number of events: " << t->GetEntries() << endl;
  cout << "---------------------------------------" << endl;

  t->SetBranchAddress("events", &tsEv);
  cout << t->GetEntries() << " events in File" << endl;

  // LoadCalibration(CalibFileName, fes);
  LoadCalibration(CalibFileName);

  fNofEvents = t->GetEntries();

  cout << "** end of PndMvdConvertApv::PndMvdConvertApv(const TString& , const TString&) **" << endl;
}

Bool_t PndMvdConvertApv::Init()
{
  return kTRUE;
}

// -----   Load calibration for the Modules   --------------------------------------------

// void PndMvdConvertApv::LoadCalibration(TString CalibFileName, std::vector<Int_t> fes)
void PndMvdConvertApv::LoadCalibration(TString CalibFileName)
{

  std::ifstream calibfile(CalibFileName);
  if (!calibfile) {
    cout << "Calibration file not found" << endl;
    fNoCalib = true;
    return;
  }
  while (!calibfile.eof()) // read data
  {
    char c;
    calibfile >> c;
    if (calibfile.eof())
      break;
    if (!isdigit(c)) {
      char str[256];
      calibfile.getline(str, 256);
      continue;
    }
    calibfile.putback(c);

    int boxID, channel;
    double value;

    calibfile >> boxID >> channel >> value;

    // for(unsigned int vec=0;vec<fes.size();vec++)
    //{
    // if(feID==fes[vec])
    //{
    fCalibPars[boxID][channel] = value;
    //}
    //}
  }
  calibfile.close();
  fNoCalib = false;
  cout << "Calibration succesfully read" << endl;
  return;
}

// -----   Convert adc to e if calibration was loaded  --------------------------------------------

std::vector<PndSdsDigiStrip> PndMvdConvertApv::Calc(std::vector<PndMvdApvHit> hitlist)
{
  std::vector<PndSdsDigiStrip> result;
  for (UInt_t hitnumber = 0; hitnumber < hitlist.size(); hitnumber++) {
    Double_t q = 0.;
    if (fNoCalib) {
      q = 1. * hitlist[hitnumber].GetADC(); // no calib adc -> e !!!
    } else {
      if (fCalibPars[hitlist[hitnumber].GetModuleID()].size()) {
        if (fCalibPars[hitlist[hitnumber].GetModuleID()][hitlist[hitnumber].GetChannel()]) {
          q = fCalibPars[hitlist[hitnumber].GetModuleID()][hitlist[hitnumber].GetChannel()] * (hitlist[hitnumber].GetADC()) * 1000.; // in electrons
        }
      }
    }

    // TString detPath="Module";
    // Int_t modId=-1; //[R.K.02/2017] Unused variable?
    // if(fFake) //[R.K.02/2017] Unused variable?
    //{ //[R.K.02/2017] Unused variable?
    // if(fTopModuleID==hitlist[hitnumber].GetModuleID() || fBottomModuleID==hitlist[hitnumber].GetModuleID())
    // modId = 1; //[R.K.02/2017] Unused variable?
    // if(fBottomModuleID==hitlist[hitnumber].GetModuleID()) hitlist[hitnumber].SetFeID(hitlist[hitnumber].GetFeID()+3); //[R.K.02/2017] Unused variable?
    //}else{ //[R.K.02/2017] Unused variable?
    // modId = hitlist[hitnumber].GetModuleID(); //[R.K.02/2017] Unused variable?
    //} //[R.K.02/2017] Unused variable?
    /* detPath+=modId;
     detPath+="Rect";
     //     std::cout<<detPath.Data()<<"   |    "<<modId<<std::endl;
     //TGeoVolume* Vol=gGeoManager->FindVolumeFast(detPath);
     TGeoVolume* Vol=0;
     if(Vol!=0) {
       // 		std::cout<<Vol->GetName()<<std::endl;
       // 		Vol->GetNode(-1)->cd();
       detPath="/SiliconTestStation_1/DummysensorAss_0/";
       detPath+=Vol->GetName();
       detPath+="_0";
       gGeoManager->cd(detPath.Data());
     }
     else {
       // 		std::cout<<" -E- PndMvdConvertApv::Calc(): "<<detPath.Data()<<" does not exist"<<std::endl;
       detPath+="_nonexistent";
     }*/
    //     std::cout<<detPath.Data()<<std::endl;
    //     std::cout<<gGeoManager->GetPath()<<std::endl;
    //     if (0==Vol) std::cout<<"0";
    //     else std::cout<<"1";
    //     std::cout<<Vol->GetName();

    //     detPath = Vol->GetName();
    // 	std::cout << "write Digi with "<< detPath.Data()<<" ( "<<fGeoH->GetID(detPath)<<" )"<<std::endl;
    if (fFake) {
      PndSdsDigiStrip DigiHit(hitlist[hitnumber].GetEventID(),               // index
                              static_cast<int>(DetectorType::kMVDHitsStrip), // panda detID
                              -1,                                            // No  SensorID (from geopath)
                              hitlist[hitnumber].GetFeID(),                  // fe
                              hitlist[hitnumber].GetChannel(),               // chan
                              q /*/1000/1000*/,                              // charge
                              hitlist[hitnumber].GetTimestamp()              // timestamp
      );
      DigiHit.SetTimeStampError(hitlist[hitnumber].GetTriggerID());

      //      cout << "Ev. " << hitlist[hitnumber].GetEventID() << ", FE: " << hitlist[hitnumber].GetFeID() << ", ch: " << hitlist[hitnumber].GetChannel() << ", FAKE" << endl;

      result.push_back(DigiHit);
    } else {
      PndSdsDigiStrip DigiHit(hitlist[hitnumber].GetEventID(), static_cast<int>(DetectorType::kMVDHitsStrip), hitlist[hitnumber].GetModuleID(), hitlist[hitnumber].GetFeID(),
                              hitlist[hitnumber].GetChannel(), q /*/1000/1000*/, hitlist[hitnumber].GetTimestamp());
      DigiHit.SetTimeStampError(hitlist[hitnumber].GetTriggerID());

      //   cout << "Ev. " << hitlist[hitnumber].GetEventID() << ", FE: " << hitlist[hitnumber].GetChannel()/128 << ", ch: " << hitlist[hitnumber].GetChannel()%128 << ", sens" <<
      //   hitlist[hitnumber].GetModuleID()  << endl;
      result.push_back(DigiHit);
    }
  }
  return result;
}

// -----   Returns the number of events in the hit file   --------------------------------------------

long int PndMvdConvertApv::GetNofEvents()
{
  return fNofEvents;
}

// -----   read the next event from hitfile   --------------------------------------------

std::vector<PndSdsDigiStrip> PndMvdConvertApv::ReadNext()
{

  std::vector<PndSdsDigiStrip> digiList;
  // bool work=true;

  // int triggID=0;
  int fe = 0;
  // int ts=0;
  int frame = 0;
  int ch = 0;
  int l = 0;
  int moduleID = 0;
  double q = 0.;
  long int ev = 0;
  UInt_t ClockReset = 0.;
  ULong64_t ClockCounts = 0.;

  if (fEvent >= -1 && fEvent <= fNofEvents) {

    t->GetEvent(fEvent);

    ClockCounts = tsEv->GetExtClockTimeStamp(ClockReset);
    // Storing ClockReset in fTriggerID
    // Storing ClockCounts in fTimestamp

    arr = tsEv->GetSiHitList();
    fhitlist.clear();

    for (Int_t kk = 0; kk < arr->GetEntries(); kk++) {

      PndMvdSiHit *hit = (PndMvdSiHit *)arr->At(kk);
      ev = tsEv->GetEventId();
      // fe = (Int_t) (hit->fChannel)/128;
      // ch = (Int_t) (hit->fChannel)%128;
      ch = (Int_t)(hit->fChannel);
      q = hit->fAdc;
      l = hit->fNumFrames;
      moduleID = hit->fBox;

      // if(fhitlist.size()>20) fhitlist.clear();
      //			digiList = Calc(fhitlist);

      fLastEvent = ev;
      // PndMvdApvHit Hit(ev, moduleID, fe, triggID, ts, frame, ch, q, l);
      PndMvdApvHit Hit(ev, moduleID, fe, ClockReset, ClockCounts, frame, ch, q, l);
      // cout << "Ev. " << fEvent << ", trigg: " << triggID << ", sens: " << moduleID << ", ch: " << ch << ", fe: " << (Int_t) (hit->fChannel)/128 << ", channel: " << (Int_t)
      // (hit->fChannel)%128 << endl;
      fhitlist.push_back(Hit);
    }

    // if(fhitlist.size()>20) fhitlist.clear();
    digiList = Calc(fhitlist);
    fEvent++;
  }

  if (!(fEvent % 10000))
    cout << "[ " << (fEvent * 100) / fNofEvents << " %] " << fEvent << " events converted ..." << endl;
  if (fEvent == fNofEvents)
    cout << "[100 %] " << fEvent << " events converted" << endl;

  return digiList;
}

// -----   read all events from hitfile   --------------------------------------------

std::vector<PndSdsDigiStrip> PndMvdConvertApv::ReadAll()
{
  std::vector<PndSdsDigiStrip> result;
  while (fEvent != fNofEvents) {
    std::vector<PndSdsDigiStrip> dummy = ReadNext();
    for (unsigned int i = 0; i < dummy.size(); ++i)
      result.push_back(dummy[i]);
  }
  return result;
}

void PndMvdConvertApv::SetFakePair(Int_t TopModuleID, Int_t BottomModuleID)
{
  fFake = true;
  fTopModuleID = TopModuleID;
  fBottomModuleID = BottomModuleID;
  return;
}

// PndSdsDigiStrip PndMvdConvertApv::DigiHit(hitlist [])
// {
// }
