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

#include "PndMasterRunAna.h"

#include "PndMasterDigiTask.h"
#include "PndMasterDigiOnlyTask.h"
#include "PndMasterRecoTask.h"
#include "PndMasterLocalRecoTask.h"
#include "PndMasterRecoIdealTask.h"
// #include "PndMasterPidTask.h"
#include "PndMasterMultiPidTask.h"
#include "PndFileNameCreator.h"
#include "PndEventCounterTask.h"

#include "FairFileSource.h"
#include "FairFileHeader.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairRuntimeDb.h"
#include "FairSystemInfo.h"
#include "FairLogger.h"
#include "FairGeane.h"
#include "FairRootFileSink.h"
#include "TGeoManager.h"
#include "TROOT.h"
#include "TObjString.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMasterRunAna::PndMasterRunAna()
  : FairRunAna(), fInput(), fParamRootFile(), fParamAsciiFile(), fOptions(""), fEventCounterRate(100), fNoGeane(kTRUE), fGenerateRunInfo(kFALSE), fUseFairLinks(kTRUE), fTimer()
{
  fTimer.Start();
}
// -----   Default destructor   -------------------------------------------
PndMasterRunAna::~PndMasterRunAna()
{
  if (gROOT->GetVersionInt() >= 60602 && gGeoManager != nullptr) {
    gGeoManager->GetListOfVolumes()->Delete();
    gGeoManager->GetListOfShapes()->Delete();
    delete gGeoManager;
  }
}

// -----   Setup   ---------------------------------------------------------
Bool_t PndMasterRunAna::Setup(TString outprefix)
{
  TString inputName = outprefix;

  // If no prefix is given, we create one from fInput and force lower-case
  if (inputName == "") {
    inputName = fInput;
    inputName.ToLower();
  }

  if (inputName.EndsWith(".dec"))
    inputName.Remove(inputName.Length() - 4, 4);
  inputName.ReplaceAll(":", "_");

  PndFileNameCreator creator(inputName.Data());

  if (fRootManager->GetSource() == nullptr) { // todo: refactor. Move this part out of setup.
    FairFileSource *fileSource{};
    if (fFriendFiles.empty()) {
      fileSource = new FairFileSource(creator.GetSimFileName().data());
      fFriendFiles.emplace_back(creator.GetSimFileName());
    } else if (fFriendFiles[0].EndsWith(".root")) { // full file names given. No need for FileNameCreator
      fileSource = new FairFileSource(fFriendFiles[0]);
      for (auto iFile = 1u; iFile < fFriendFiles.size(); ++iFile) {
        const TFile testFile{fFriendFiles[iFile]};
        if (testFile.IsZombie()) {
          LOG(WARNING) << "File: " << fFriendFiles[iFile] << " does not exists." << std::endl;
        } else {
          fileSource->AddFriend(fFriendFiles[iFile]);
        }
      }
    } else {
      fileSource = new FairFileSource(creator.GetCustomFileName(fFriendFiles[0].Data())); // use FileNameCreator to generate file names
      fFriendFiles[0] = creator.GetCustomFileName(fFriendFiles[0].Data());

      for (auto iFile = 1u; iFile < fFriendFiles.size(); ++iFile) {
        const auto customFileName = creator.GetCustomFileName(fFriendFiles[iFile].Data());
        const TFile testFile{customFileName.c_str()};
        if (testFile.IsZombie()) {
          LOG(warning) << "File: " << customFileName << " does not exists. It will not be added as friend!" << std::endl;
        } else {
          fileSource->AddFriend(customFileName.c_str());
          fFriendFiles[iFile] = customFileName;
        }
      }
    }
    SetSource(fileSource);
  }

  // This set the output file name
  if (fOutFile.EndsWith(".root")) {
    SetOutputFile(fOutFile.Data());
  } else {
    SetOutputFile(creator.GetCustomFileName(fOutFile.Data()).data());
  }
  // This set the string for the output file name, used by Finish()
  if (fOutFile.EndsWith(".root")) {
    SetOutput(fOutFile.Data());
  } else {
    SetOutput(creator.GetCustomFileName(fOutFile.Data()));
  }
  if (fParamRootFile.EndsWith(".root")) {
    SetParamRootFile(fParamRootFile);
  } else {
    SetParamRootFile(creator.GetParFileName().data());
  }

  SetGenerateRunInfo(fGenerateRunInfo);
  SetUseFairLinks(fUseFairLinks);
  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += fParamAsciiFile;

  std::cout << "ParamRootFile Name: " << fParamRootFile << std::endl;

  FairRuntimeDb *rtdb = GetRuntimeDb();
  FairParRootFileIo *parInput1 = new FairParRootFileIo();
  parInput1->open(fParamRootFile.Data());

  FairParAsciiFileIo *parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(), "in");

  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);

  // -----   Event Counter   --------------------------------
  AddTask(new PndEventCounterTask("Event Counter", 0, fEventCounterRate));

  return kTRUE;
}

// -----   AddDigiTasks   ---------------------------------------------------
void PndMasterRunAna::AddDigiTasks(Bool_t pers)
{
  PndMasterDigiTask *digi = new PndMasterDigiTask(fOptions);
  if (!pers)
    digi->SetPersistency(kFALSE);
  digi->SetPersistency(pers);
  AddTask(digi);
}

// -----   AddDigiTasks   ---------------------------------------------------
void PndMasterRunAna::AddDigiOnlyTasks(Bool_t pers)
{
  PndMasterDigiOnlyTask *digi = new PndMasterDigiOnlyTask(fOptions);
  if (!pers)
    digi->SetPersistency(kFALSE);
  digi->SetPersistency(pers);
  AddTask(digi);
}

// -----   AddRecoTasks   ---------------------------------------------------
void PndMasterRunAna::AddRecoTasks(Bool_t pers)
{
  // -----   Geane   ---------------------------------------
  if (fNoGeane) {
    AddTask(new FairGeane());
    fNoGeane = false;
  }
  PndMasterRecoTask *reco = new PndMasterRecoTask(fOptions);
  if (!pers)
    reco->SetPersistency(kFALSE);
  reco->SetPersistency(pers);
  AddTask(reco);
}

// -----   AddLocalRecoTasks   ---------------------------------------------------
void PndMasterRunAna::AddLocalRecoTasks(Bool_t pers)
{
  // -----   Geane   ---------------------------------------
  if (fNoGeane) {
    AddTask(new FairGeane());
    fNoGeane = false;
  }
  PndMasterLocalRecoTask *reco = new PndMasterLocalRecoTask(fOptions);
  if (!pers)
    reco->SetPersistency(kFALSE);
  reco->SetPersistency(pers);
  AddTask(reco);
}

// -----   AddRecoTasks   ---------------------------------------------------
void PndMasterRunAna::AddRecoIdealTasks(Bool_t pers)
{
  // -----   Geane   ---------------------------------------
  if (fNoGeane) {
    AddTask(new FairGeane());
    fNoGeane = false;
  }
  PndMasterRecoIdealTask *recoIdeal = new PndMasterRecoIdealTask(fOptions);
  if (!pers)
    recoIdeal->SetPersistency(kFALSE);
  recoIdeal->SetPersistency(pers);
  AddTask(recoIdeal);
}

// -----   AddPidTasks   ----------------------------------------------------
void PndMasterRunAna::AddPidTasks(Bool_t pers)
{
  if (fNoGeane) {
    AddTask(new FairGeane());
    fNoGeane = false;
  }
  std::cout << "MasterRunAna: PID Task creation. Options are \"" << fOptions.Data() << "\"" << std::endl;
  // if (fOptions.Contains("multikalman"))
  //{
  PndMasterMultiPidTask *pid = new PndMasterMultiPidTask(fOptions);
  if (!pers)
    pid->SetPersistency(kFALSE);
  pid->SetPersistency(pers);
  AddTask(pid);
  //} else {
  // PndMasterPidTask *pid = new PndMasterPidTask(fOptions);
  // if (!pers) pid->SetPersistency(kFALSE);
  // pid->SetPersistency(pers);
  // AddTask(pid);
  //}
}

void PndMasterRunAna::PrintListOfTasks(TList *list, TString prefix)
{
  if (list == nullptr)
    return;
  for (const TObject *obj : *list) {
    cout << prefix.Data() << obj->ClassName() << " (" << obj->GetName() << ")" << endl;
    PrintListOfTasks(((FairTask *)obj)->GetListOfTasks(), "  " + prefix);
  }
}

// -----   Finish   ---------------------------------------------------------
void PndMasterRunAna::Finish()
{

  cout << "PndMasterRunAna::Finish(): Tasks that ran just now:" << endl;
  FairSink *sink = fRootManager->GetSink();
  TFile *outfile = nullptr;
  if (sink->GetSinkType() == kFILESINK) {
    outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();

    bool wasopen = outfile->IsOpen();
    if (!wasopen) {
      cout << "file is " << ((wasopen) ? "" : "not ") << "open" << endl;
      cout << "Opening output file: " << outfile->GetName() << endl;
      outfile = TFile::Open(outfile->GetName(), "UPDATE");
    }
    outfile->cd();

    cout << "The run had the option string\n" << fOptions.Data() << endl;

    PrintListOfTasks();

    TObjString outoptions(fOptions);
    outfile->WriteTObject(&outoptions, "PndOptions", "Overwrite");
    // outoptions.Write("PndOptions", kOverwrite);

    outfile->Write();
    if (!wasopen) {
      outfile->Close();
      delete (outfile);
    }
  }
  cout << endl;

  // safety delete for newer ROOT
  if (gROOT->GetVersionInt() >= 60602 && gROOT->GetVersionInt() < 61800) {
    gGeoManager->GetListOfVolumes()->Delete();
    gGeoManager->GetListOfShapes()->Delete();
    delete gGeoManager;
  }

  // Extract the maximal used memory an add is as Dart measurement
  // This line is filtered by CTest and the value send to CDash
  FairSystemInfo sysInfo;
  Float_t maxMemory = sysInfo.GetMaxMemory();
  cout << "<DartMeasurement name=\"MaxMemory\" type=\"numeric/double\">";
  cout << maxMemory;
  cout << "</DartMeasurement>" << endl;

  fTimer.Stop();
  Double_t rtime = fTimer.RealTime();
  Double_t ctime = fTimer.CpuTime();

  Float_t cpuUsage = ctime / rtime;
  cout << "<DartMeasurement name=\"CpuLoad\" type=\"numeric/double\">";
  cout << cpuUsage;
  cout << "</DartMeasurement>" << endl;

  cout << endl;
  cout << "Output file is\t\t" << fOutFile << endl;
  for (auto files : fFriendFiles) {
    cout << "Friend file is\t\t" << files << endl;
  }

  cout << "Parameter ROOT file is\t" << fParamRootFile << endl;
  cout << "Parameter ASCII file is\t" << fParamAsciiFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl;
  cout << "CPU usage " << cpuUsage * 100. << "%" << endl;
  cout << "Max Memory " << maxMemory << " MB" << endl;

  cout << "Macro finished successfully." << endl;
}

/** @cond CLASSIMP */
ClassImp(PndMasterRunAna);
/** @endcond */
