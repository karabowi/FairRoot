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

/* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemEventBuilderOnTracks source file         -----
// -----                  Created 22/08/2013 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemEventBuilderOnTracks
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 22.08.2013
 *@version 1.0
 **
 ** PANDA task class for event building basing on reconstructed GEM tracks.
 ** Task level RECO
 **/

#include "PndGemEventBuilderOnTracks.h"

#include "PndTrack.h"

#include "PndDetectorList.h"
#include "FairRecoEventHeader.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

#include <iomanip>

using std::cerr;
using std::cout;
using std::endl;
using std::fixed;
using std::flush;
using std::left;
using std::map;
using std::right;
using std::set;
using std::setprecision;
using std::setw;

using std::sort;

bool CompareRecoEvents(RecoEvent ev1, RecoEvent ev2)
{
  return (ev1.meanTime < ev2.meanTime);
}

// -----   Default constructor   ------------------------------------------
PndGemEventBuilderOnTracks::PndGemEventBuilderOnTracks()
  : FairEventBuilder("GEMEvTrack", "PndTrack", "GEM", kTRUE), fGemTracks(nullptr), fTNofEvents(0), fTNofTracks(0), fTNofRecoEvents(0), fExecTime(0.), fGemTrackDelay(0.)
{
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
PndGemEventBuilderOnTracks::PndGemEventBuilderOnTracks(const char *name, Int_t iVerbose)
  : FairEventBuilder("GEMEvTrack", "PndTrack", "GEM", kTRUE), fGemTracks(nullptr), fTNofEvents(0), fTNofTracks(0), fTNofRecoEvents(0), fExecTime(0.), fGemTrackDelay(0.)
{
  SetBuilderName(name);
  fVerbose = iVerbose;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemEventBuilderOnTracks::~PndGemEventBuilderOnTracks() {}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
std::vector<std::pair<double, FairRecoEventHeader *>> PndGemEventBuilderOnTracks::FindEvents()
{
  if (fVerbose)
    cout << endl << "======== PndGemEventBuilderOnTracks::FindEvents(Event = " << fTNofEvents << " ) ====================" << endl;

  fTimer.Start();

  fRecoEvents.clear();

  fTNofTracks += fGemTracks->GetEntries();

  PndTrack *tempTrack = nullptr;

  if (fVerbose)
    cout << "There are " << fGemTracks->GetEntries() << " tracks." << endl;

  for (Int_t itrack = 0; itrack < fGemTracks->GetEntries(); itrack++) {
    tempTrack = (PndTrack *)fGemTracks->At(itrack);

    if (fVerbose > 1) {
      cout << "    " << fTNofEvents << " . " << itrack << " @ " << tempTrack->GetTimeStamp() << "ns" << endl
           << "         trying to match to one of " << fRecoEvents.size() << " reco events" << endl;
    }

    Int_t revNo = CompareTrackToPreviousEvents(itrack, tempTrack);

    FillNewData(tempTrack, tempTrack->GetTimeStamp(), tempTrack->GetTimeStamp());
  }

  fTNofEvents += 1;

  fTNofRecoEvents += fRecoEvents.size();

  std::sort(fRecoEvents.begin(), fRecoEvents.end(), CompareRecoEvents);

  std::vector<std::pair<double, FairRecoEventHeader *>> result;
  std::pair<double, FairRecoEventHeader *> singleResult;

  if (fVerbose)
    cout << "There are " << fRecoEvents.size() << " events." << endl;

  for (Int_t irev = 0; irev < fRecoEvents.size(); irev++) {
    FairRecoEventHeader recoEvent;
    recoEvent.SetEventTime(fRecoEvents[irev].meanTime - fGemTrackDelay, 5.);
    recoEvent.SetIdentifier(GetIdentifier());
    SetMaxAllowedTime(recoEvent.GetEventTime());

    fEvent_map[recoEvent] = recoEvent.GetEventTime();

    singleResult.first = recoEvent.GetEventTime();
    singleResult.second = (FairRecoEventHeader *)&fEvent_map.rbegin()->first;

    if (fVerbose)
      cout << "FOUND EVENT @ " << singleResult.first << endl;

    result.push_back(singleResult);
  }

  fExecTime += fTimer.RealTime();
  fTimer.Stop();

  return result;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void PndGemEventBuilderOnTracks::Print()
{
  cout << " print " << endl;
  cout << " fEventMap has " << fEvent_map.size() << " entries" << endl;
  std::map<FairRecoEventHeader, double>::iterator iter;
  for (iter = fEvent_map.begin(); iter != fEvent_map.end(); iter++) {
    cout << " // for time " << iter->second << " got " << iter->first.GetEventTime() << " at an address " << &iter->first << endl;
  }
  cout << "-------" << endl;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void PndGemEventBuilderOnTracks::StoreEventData(FairRecoEventHeader *recoEvent)
{
  typedef std::multimap<double, std::pair<double, FairTimeStamp *>>::iterator startTimeMapIter;

  if (fVerbose) {
    cout << "* " << fData_map.size() << " in fData_map "
         << "* " << fStartTime_map.size() << " in fStartTime_map "
         << " TO PUT IN EVENT AT " << recoEvent->GetEventTime() << endl;
    cout << "CHECK TRACKS WITH TimeStamps:" << endl;
    for (startTimeMapIter jter = fStartTime_map.begin(); jter != fStartTime_map.end(); ++jter) {
      std::pair<double, FairTimeStamp *> datb = jter->second;
      cout << "@@@ " << datb.second->GetTimeStamp() << endl;
    }
  }

  startTimeMapIter iter = fStartTime_map.begin();
  while (iter != fStartTime_map.end()) {
    std::pair<double, FairTimeStamp *> data = iter->second;
    if (fVerbose) {
      cout << " ---> check data at " << data.second->GetTimeStamp() << " /// " << data.second->GetTimeStamp() - recoEvent->GetEventTime() - fGemTrackDelay << endl;
    }
    if (TMath::Abs(data.second->GetTimeStamp() - recoEvent->GetEventTime() - fGemTrackDelay) < 3.) {
      if (fVerbose)
        cout << "WILL ADD THIS ONE" << endl;
      FillDataToDeadTimeMap(data.second, data.first);

      startTimeMapIter save = iter;
      save++;
      fStartTime_map.erase(iter);
      iter = save;
    } else
      iter++;
  }
}

std::vector<std::pair<double, PndTrack *>> PndGemEventBuilderOnTracks::Modify(std::pair<double, PndTrack *> oldData, std::pair<double, PndTrack *> newData)
{
  std::vector<std::pair<double, PndTrack *>> result;
  result.push_back(newData);
  return result;
}

void PndGemEventBuilderOnTracks::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fBranchName);
  if (fVerbose > 1)
    std::cout << "Data Inserted: " << *(PndTrack *)(data) << std::endl;
  new ((*myArray)[myArray->GetEntries()]) PndTrack(*(PndTrack *)(data));
}

double PndGemEventBuilderOnTracks::FindTimeForData(FairTimeStamp *data)
{
  std::map<PndTrack, double>::iterator it;
  PndTrack myData = *(PndTrack *)data;
  it = fData_map.find(myData);
  if (it == fData_map.end())
    return -1;
  else
    return it->second;
}
void PndGemEventBuilderOnTracks::FillDataMap(FairTimeStamp *data, double activeTime)
{
  PndTrack myData = *(PndTrack *)data;
  fData_map[myData] = activeTime;
}

void PndGemEventBuilderOnTracks::EraseDataFromDataMap(FairTimeStamp *data)
{
  PndTrack myData = *(PndTrack *)data;
  if (fData_map.find(myData) != fData_map.end())
    fData_map.erase(fData_map.find(myData));
}

// -----   Private method CompareTrackToPreviousEvents   -------------------
Int_t PndGemEventBuilderOnTracks::CompareTrackToPreviousEvents(Int_t trackId, PndTrack *tempTrack)
{
  Double_t trackTime = tempTrack->GetTimeStamp();
  Int_t recoEvent = -1;
  for (Int_t irev = 0; irev < fRecoEvents.size(); irev++) {
    RecoEvent iterREV = fRecoEvents[irev];

    // SHOULD BE SOMETHING MORE SERIOUS THAN "2."
    // 26.08 - just checked the reconstructed track time difference,
    // got a distribution with sigma of 9.49795e-01
    // Setting to 3.0
    if (TMath::Abs(trackTime - iterREV.meanTime) < 3.) {
      if (fVerbose && recoEvent != -1) {
        cout << "track @ " << trackTime << " matches to two reco events: " << irev << " and " << recoEvent << endl;
      } else {
        recoEvent = irev;
      }
    }
  }

  // matching event found
  if (recoEvent != -1) {
    fRecoEvents[recoEvent].meanTime = (fRecoEvents[recoEvent].meanTime * fRecoEvents[recoEvent].nofTracks + trackTime) / ((Double_t)(fRecoEvents[recoEvent].nofTracks + 1));
    fRecoEvents[recoEvent].nofTracks = fRecoEvents[recoEvent].nofTracks + 1;
    fRecoEvents[recoEvent].trackIndex.push_back(trackId);
  } else {
    RecoEvent tempREV;
    tempREV.meanTime = trackTime;
    tempREV.nofTracks = 1;
    tempREV.trackIndex.push_back(trackId);

    fRecoEvents.push_back(tempREV);
  }

  return recoEvent;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGemEventBuilderOnTracks::SetParContainers()
{

  // Get run and runtime database
  FairRunAna *run = FairRunAna::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
Bool_t PndGemEventBuilderOnTracks::Init()
{

  // Get input array
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman)
    Fatal("Init", "No FairRootManager");

  fGemTracks = (TClonesArray *)ioman->GetObject("GEMTrack");

  // fGemOutTracks = new TClonesArray("PndTrack",100);
  // ioman->Register("GEMTrackQQQ", "Gem Tracks", fGemOutTracks, kTRUE);
  // ioman->RegisterWriteoutBuffer("GEMDigi", fDataBuffer);
  // fDataBuffer->ActivateBuffering(fTimeOrderedDigi);

  fGemTrackDelay = 4.96229;

  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
Bool_t PndGemEventBuilderOnTracks::ReInit()
{

  // Create sectorwise digi sets
  //  MakeSets();

  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void PndGemEventBuilderOnTracks::Finish()
{

  cout << "-------------------- " << GetBuilderName() << " : Summary -----------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << " Tracks:        " << setw(10) << fTNofTracks << "    ( " << (Double_t)fTNofTracks / ((Double_t)fTNofEvents) << " per event )" << endl;
  cout << " Reco Events:   " << setw(10) << fTNofRecoEvents << endl;
  cout << " . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . " << endl;
  cout << " >>> EB >>> all   time  = " << fExecTime << "s" << endl;
  cout << "---------------------------------------------------------------------" << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndGemEventBuilderOnTracks)
