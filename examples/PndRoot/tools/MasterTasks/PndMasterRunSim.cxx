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

#include "PndMasterRunSim.h"

#include "PndMultiField.h"
#include "PndCave.h"
#include "PndMagnet.h"
#include "PndPipe.h"
#include "PndStt.h"
#include "PndStt2.h"
#include "PndMvdDetector.h"
#include "PndGemDetector.h"
#include "PndEmc.h"
#include "PndSciT.h"
#include "PndDrc.h"
#include "PndDsk.h"
#include "PndMdt.h"
#include "PndFts.h"
#include "PndFtof.h"
#include "PndRich.h"
#include "PndEmcHitProducer.h"
#include "PndDpmDirect.h"
#include "PndFtfDirect.h"
#include "PndPythia8Direct.h"
#include "PndFtfGenerator.h"
#include "PndBoxGenerator.h"
#include "PndEvtGenDirect.h"
#include "PndPiPiGenerator.h"
#include "PndLepLepGenerator.h"
#include "PndMasterSimTask.h"
#include "PndEventCounterTask.h"
#include "PndFileNameCreator.h"
#include "PndFilteredPrimaryGenerator.h"
#include "FairAsciiGenerator.h"
#include "FairFileSource.h"
#include "FairFileHeader.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairRuntimeDb.h"
#include "FairSystemInfo.h"
#include "FairModule.h"
#include "FairDetector.h"
#include "FairPrimaryGenerator.h"
#include "FairFilteredPrimaryGenerator.h"
#include "FairGenerator.h"
#include "FairLogger.h"
#include "FairRootFileSink.h"
#include "HepParMap.h"

#include "TLorentzVector.h"
#include "TDatabasePDG.h"
#include "TGeoManager.h"
#include "TROOT.h"
#include "TFile.h"

#include <fstream>
using std::cout;
using std::endl;
using namespace parmaptypes;

// -----   Default constructor   -------------------------------------------
PndMasterRunSim::PndMasterRunSim()
  : FairRunSim(), fInput(), fInputDir(""), fOutFile(), fParamRootFile(), fParamAsciiFile(), fOptions(), fDpmFlag(1), fFtfFlag(0), fNEvents(0), fEventCounterRate(100),
    fTargetMode(0), fRtdb(), fTimer()
{
  fTimer.Start();
}
// -----   Default destructor   -------------------------------------------
PndMasterRunSim::~PndMasterRunSim()
{
  if (gROOT->GetVersionInt() >= 60602 && gGeoManager != nullptr) {
    gGeoManager->GetListOfVolumes()->Delete();
    gGeoManager->GetListOfShapes()->Delete();
    delete gGeoManager;
  }
}

void PndMasterRunSim::SetOutputFile(TString outprefix)
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
  fOutFile = creator.GetSimFileName().data();
  SetSink(new FairRootFileSink(fOutFile));
  SetParamRootFile(creator.GetParFileName().data());
  SetMaterials("media_pnd.geo");
  SetGenerateRunInfo(kFALSE);
  SetUseFairLinks(kTRUE);
}

void PndMasterRunSim::SetRtdb()
{
  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += fParamAsciiFile;

  fRtdb = this->GetRuntimeDb();
  Bool_t kParameterMerged = kFALSE; // No use until now
  FairParRootFileIo *parOutput = new FairParRootFileIo(kParameterMerged);
  parOutput->open(fParamRootFile, "RECREATE");

  FairParAsciiFileIo *parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(), "in");

  fRtdb->setFirstInput(parIo1);
  fRtdb->setOutput(parOutput);
}

void PndMasterRunSim::SetField()
{
  // -----  Create and Set the Field(s) ------------------------------------
  PndMultiField *field = new PndMultiField("AUTO");
  FairRunSim::SetField(field);
}

void PndMasterRunSim::DefineParticles()
{
  // ---- Defining PANDA particles -----------------------------------------
  Double_t mom = GetBeamMom();
  TLorentzVector fIni(0, 0, mom, sqrt(mom * mom + 9.3827203e-01 * 9.3827203e-01) + 9.3827203e-01);
  TDatabasePDG::Instance()->AddParticle("ppSystem", "ppSystem", fIni.M(), kFALSE, 0.1, 6, "", 98888);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem", "pbarpSystem", fIni.M(), kFALSE, 0.1, 0, "", 88888);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem0", "pbarpSystem0", fIni.M(), kFALSE, 0.1, 0, "", 88880);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem1", "pbarpSystem1", fIni.M(), kFALSE, 0.1, 0, "", 88881);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem2", "pbarpSystem2", fIni.M(), kFALSE, 0.1, 0, "", 88882);
}
// -----   Setup   ---------------------------------------------------------
Bool_t PndMasterRunSim::Setup(TString outprefix)
{
  SetOutputFile(outprefix);
  SetRtdb();
  SetField();
  DefineParticles();
  SetupTargetMode();

  //  if (fOptions.Contains("day1"))
  //    fTargetMode = 1;

  return kTRUE;
}

// -----   CreateGeometry   -------------------------------------------------
void PndMasterRunSim::CreateGeometry()
{
  //  if (fOptions.Contains("phase1"))
  //    CreateGeometryPhase1();
  //  else if (fOptions.Contains("day1"))
  //    CreateGeometryDay1();
  //  else
  CreateGeometryDefault();
}

void PndMasterRunSim::CreateGeometryConf(TString conf) 
{
  HepParMap pm(conf);
  TStr cave   = pm.Str("CAVE",   "pndcave.geo");
  TStr mag    = pm.Str("MAGNET", "dipole.geo");
  TStr pipe   = pm.Str("PIPE",   "beampipe_201309.root");
  TStr stt    = pm.Str("STT",    "straws_skewed_blocks_35cm_pipe.geo");
  TStr mvd    = pm.Str("MVD",    "Mvd-2.1_FullVersion.root");
  TStr gem    = pm.Str("GEM",    "gem_3Stations_realistic_v3.root");
  TvInt emc   = pm.VInt ("EMC",  {1, 0}, "", 1);
  TStr scit   = pm.Str("SCIT",   "SciTil_201601.root");
  TvStr dirc  = pm.VStr("DIRC",  {"dirc_e3_b3_l6_m40.root","0"}, "", 1);
  TvInt dsk   = pm.VInt("DSK",   {0, 0}, "", 1);
  TvStr mdt   = pm.VStr("MDT",   {"fast","fast","fast","fast","1","1","1"}, "", 1);
  TStr fts    = pm.Str("FTS",    "fts.geo");
  TStr ftof   = pm.Str("FTOF",   "ftofwall.root");
  TStr rich   = pm.Str("RICH",   "rich_v313.root");
  int verbose = pm.Int("verbose");
  
  if (verbose) {
    cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- Using configuration"<<endl;
    pm.Print(1);
  }
  
  int detnum=0;
  
  //-------------------------  CAVE      -----------------
  if (cave!="") {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding CAVE: geometry = "<<cave<<endl; 
    FairModule *Cave = new PndCave("CAVE");
    Cave->SetGeometryFileName(cave);
    AddModule(Cave);
  }
  //-------------------------  Magnet   -----------------
  if (mag!="") {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding Magnet: geometry = "<<mag<<endl; 
    FairModule *Dipole = new PndMagnet("MAGNET");
    Dipole->SetGeometryFileName(mag);
    AddModule(Dipole);
  }
  //-------------------------  Pipe     -----------------
  if (pipe!="") {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding Pipe: geometry = "<<pipe<<endl; 
    FairModule *Pipe = new PndPipe("PIPE");
    Pipe->SetGeometryFileName(pipe);
    AddModule(Pipe);
  }
  //-------------------------  STT       -----------------
  if (stt!="") {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding STT: geometry = "<<stt<<endl; 
    bool oldStt = (stt=="straws_skewed_blocks_35cm_pipe.geo" || stt.EndsWith(".root"));
    FairDetector *Stt = nullptr;
    if (oldStt) 
      Stt = new PndStt("STT", kTRUE);
    else
      Stt = new PndStt2("STT2", kTRUE);
    Stt->SetGeometryFileName(stt);
    AddModule(Stt);
  }
  //-------------------------  MVD       -----------------
  if (mvd!="") {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding MVD: geometry = "<<mvd<<endl; 
    FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
    Mvd->SetGeometryFileName(mvd);
    AddModule(Mvd);
  }
  //-------------------------  GEM       -----------------
  if (gem!="") {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding GEM: geometry = "<<gem<<endl; 
    FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
    Gem->SetGeometryFileName(gem);
    AddModule(Gem);
  }
  //-------------------------  EMC       -----------------
  if (!(pm.Exists("EMC") && pm.Str("EMC")=="")) {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding EMC: parameters = "<<pm.join(pm.i2str(emc),",")<<endl; 
    PndEmc *Emc = new PndEmc("EMC", kTRUE);
    Emc->SetGeometryVersion(emc[0]);
    Emc->SetStorageOfData(emc[1]);
    AddModule(Emc);
  }
  //-------------------------  SCITIL    -----------------
  if (scit!="") {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding SCITIL: geometry = "<<scit<<endl; 
    FairDetector *SciT = new PndSciT("SCIT", kTRUE);
    SciT->SetGeometryFileName(scit);
    AddModule(SciT);
  }
  //-------------------------  DRC       -----------------
  if (!(pm.Exists("DIRC") && pm.Str("DIRC")=="")) {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding DRC: geometry = "<<dirc[0]<<", RunCherenkov = "<<dirc[1]<<endl; 
    PndDrc *Drc = new PndDrc("DIRC", kTRUE);
    Drc->SetGeometryFileName(dirc[0]);
    Drc->SetRunCherenkov(dirc[1].Atoi());
    AddModule(Drc);
  }
  //-------------------------  DISC      -----------------
  if (!(pm.Exists("DSK") && pm.Str("DSK")=="")) {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding DISC: parameters = "<<pm.join(pm.i2str(dsk),",")<<endl; 
    PndDsk *Dsk = new PndDsk("DSK", kTRUE);
    Dsk->SetStoreCerenkovs(dsk[0]);
    Dsk->SetStoreTrackPoints(dsk[1]);
    AddModule(Dsk);
  }
  //-------------------------  MDT       -----------------
  if (!(pm.Exists("MDT") && pm.Str("MDT")=="")) {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding MDT: parameters = "<<pm.join(mdt,",")<<endl; 
    PndMdt *Muo = new PndMdt("MDT", kTRUE);
    Muo->SetBarrel(mdt[0]);
    Muo->SetEndcap(mdt[1]);
    Muo->SetMuonFilter(mdt[2]);
    Muo->SetForward(mdt[3]);
    Muo->SetMdtMagnet(mdt[4].Atoi());
    Muo->SetMdtCoil(mdt[5].Atoi());
    Muo->SetMdtMFIron(mdt[6].Atoi());
    AddModule(Muo);
  }
  //-------------------------  FTS       -----------------
  if (fts!="") {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding FTS: geometry = "<<fts<<endl; 
    FairDetector *Fts = new PndFts("FTS", kTRUE);
    Fts->SetGeometryFileName(fts);
    AddModule(Fts);
  }
  //-------------------------  FTOF      -----------------
  if (ftof!="") {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding FTOF: geometry = "<<ftof<<endl; 
    FairDetector *FTof = new PndFtof("FTOF", kTRUE);
    FTof->SetGeometryFileName(ftof);
    AddModule(FTof);
  }
  //-------------------------  RICH       ----------------
  if (rich!="") {
    if (verbose) 
      cout <<"[INFO] PndMasterRunSim::CreateGeometryConf -- ["<<detnum++<<"] Adding RICH: geometry = "<<rich<<endl; 
    PndRich *Rich = new PndRich("RICH", kTRUE);
    Rich->SetGeometryFileName(rich);
    AddModule(Rich);
  }
}


// -----   CreateGeometry   -------------------------------------------------
void PndMasterRunSim::CreateGeometryDefault()
{
  //-------------------------  CAVE      -----------------
  FairModule *Cave = new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  AddModule(Cave);
  //-------------------------  Magnet   -----------------
  // This part is commented because the MDT geometry contains the magnet now
  // FairModule *Magnet= new PndMagnet("MAGNET");
  // Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  // Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  // AddModule(Magnet);
  FairModule *Dipole = new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  AddModule(Dipole);
  //-------------------------  Pipe     -----------------
  FairModule *Pipe = new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201309.root");
  AddModule(Pipe);
  //-------------------------  STT       -----------------
  if (fOptions.Contains("stt2full")) {
    FairDetector *Stt = new PndStt2("STT2", kTRUE);
    Stt->SetGeometryFileName("stt2_full.geo");
    AddModule(Stt);
  } else if (fOptions.Contains("stt2red")) {
    FairDetector *Stt = new PndStt2("STT2", kTRUE);
    Stt->SetGeometryFileName("stt2_red.geo");
    AddModule(Stt);
  } else {
    FairDetector *Stt = new PndStt("STT", kTRUE);
    Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
    AddModule(Stt);
  }
  //-------------------------  MVD       -----------------
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  AddModule(Mvd);
  //-------------------------  GEM       -----------------
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations_realistic_v3.root");
  AddModule(Gem);
  //-------------------------  EMC       -----------------
  PndEmc *Emc = new PndEmc("EMC", kTRUE);
  Emc->SetGeometryVersion(1);
  Emc->SetStorageOfData(kFALSE);
  AddModule(Emc);
  //-------------------------  SCITIL    -----------------
  FairDetector *SciT = new PndSciT("SCIT", kTRUE);
  SciT->SetGeometryFileName("SciTil_201601.root");
  AddModule(SciT);
  //-------------------------  DRC       -----------------
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetGeometryFileName("dirc_e3_b3_l6_m40.root");
  Drc->SetRunCherenkov(kFALSE);
  AddModule(Drc);
  //-------------------------  DISC      -----------------
  PndDsk *Dsk = new PndDsk("DSK", kTRUE);
  Dsk->SetStoreCerenkovs(kFALSE);
  Dsk->SetStoreTrackPoints(kFALSE);
  AddModule(Dsk);
  //-------------------------  MDT       -----------------
  PndMdt *Muo = new PndMdt("MDT", kTRUE);
  Muo->SetBarrel("fast");
  Muo->SetEndcap("fast");
  Muo->SetMuonFilter("fast");
  Muo->SetForward("fast");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtCoil(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  AddModule(Muo);
  //-------------------------  FTS       -----------------
  FairDetector *Fts = new PndFts("FTS", kTRUE);
  Fts->SetGeometryFileName("fts.geo");
  AddModule(Fts);
  //-------------------------  FTOF      -----------------
  FairDetector *FTof = new PndFtof("FTOF", kTRUE);
  FTof->SetGeometryFileName("ftofwall.root");
  AddModule(FTof);
  //-------------------------  RICH       ----------------
  PndRich *Rich = new PndRich("RICH", kTRUE);
  Rich->SetGeometryFileName("rich_v313.root");
  AddModule(Rich);
}

// -----   AddSimTasks   ---------------------------------------------------
void PndMasterRunSim::AddSimTasks()
{
  // -----   Event Counter   --------------------------------
  AddTask(new PndEventCounterTask("Event Counter", fNEvents, fEventCounterRate));

  PndMasterSimTask *sim = new PndMasterSimTask(fOptions);
  AddTask(sim);

  return;
}

// -----   SetGenerator   --------------------------------------------------
void PndMasterRunSim::SetGenerator()
{
  CreatePrimGen();

  // temporarily go to lowercase for easy selection
  TString input = fInput;
  input.ToLower();

  if (input.EndsWith(".dec") || input.Contains(".dec:")) {
    UseEvtGenGenerator(fInput);
  } else if (input.EndsWith(".asc") || input.Contains(".asc:")) {
    UseAsciiGenerator(fInput);
  } else if (input.BeginsWith("dpm")) {
    UseDpmGenerator();
  } else if (input.BeginsWith("ftf")) {
    UseFtfGenerator(fInput);
  } else if (input.BeginsWith("pythia8")) {
    UsePythia8Generator(fInput);
  } else if (input.BeginsWith("box")) {
    UseBoxGenerator(fInput);
  } else if (input.BeginsWith("pipi")) {
    UsePiPiGenerator(fInput);
  } else if (input.BeginsWith("leplep")) {
    UseLepLepGenerator(fInput);
  } else {
    LOG(warning) << "Generator could not be identified from input '" << fInput.Data() << "'!!";
  }

  // TODO: Move the following block above, when the primary generator receives the custom target feature.
  TString tgtfile = gSystem->Getenv("VMCWORKDIR");
  switch (fTargetMode) {
  case 0: LOG(info) << "Using no Vertex smearing"; break;
  case 1:
    LOG(info) << "Using Cluster Jet Target";
    // a cluster-jet beam at the interaction zone with a horizontal width
    // of e.g. dx = 1 mm and a length in accelerator beam direction of dz = 10 mm.
    // Target TDR, page 44
    fGen->SetTarget(0., 1. / 2.355); // From FWHM to sigma
    fGen->SmearGausVertexZ(kTRUE);
    fGen->SetBeam(0., 0., 0.1, 0.1); // Assume Gaussian beam of 1x1 mm^2 by "Fair Operation Modes" document v.6 (2020)
    fGen->SmearGausVertexXY(kTRUE);
    break;
  case 2:
    LOG(info) << "Using Pellet Target";
    // At PANDA a beam diameter around 3mm is needed and one may want even
    // smaller size when PTR is not possible.
    // Target TDR, page 61
    fGen->SetTarget(0., 0.3);
    fGen->SmearGausVertexZ(kTRUE);
    fGen->SetBeam(0., 0., 0.3, 0.3);
    fGen->SmearGausVertexXY(kTRUE);
    break;
  case 3:
    LOG(info) << "Using Pellet Tracking Target";
    // A position resolution σ(x, y, z) < 0.2 mm in the interaction position
    // is desirable for event reconstruction.
    // Target TDR, page 61
    fGen->SetTarget(0., 0.02);
    fGen->SmearGausVertexZ(kTRUE);
    fGen->SetBeam(0., 0., 0.02, 0.02);
    fGen->SmearGausVertexXY(kTRUE);
    break;
  case 4: {
    // Realistic properties with a cluster jet target and residual gas in the beam pipes
    tgtfile += "/input/restgasthickness_210421_Normal_IP_with_Cryopump.txt";
    LOG(info) << "Using distributed Beam-Target profile " << tgtfile.Data();
    TObjArray *genList = fGen->GetListOfGenerators();
    for (int i = 0; i < genList->GetEntriesFast(); i++) {
      TObject *obj = genList->At(i);
      if (!obj->InheritsFrom("PndTargetGenerator"))
        continue;
      PndTargetGenerator *aGen = (PndTargetGenerator *)genList->At(i);
      aGen->SetDensityProfile(tgtfile);
      aGen->SetBeamRadius(0.1);                 // default beam spot sigma 1mm^2 by "Fair Operation Modes" document v.6 (2020)
      aGen->SetBeamPipeRadius(2.0);             // Smallest radius around interaction region
      aGen->SetConstantBeamRegion(-140., 223.); // region where the beam with stays at its minimum due to the solenoid field, edges at half Bz_max
      aGen->SetBeamDrDz(0.1);                   // default maximum beam divergence from 4-sigma emittance of 1-2 mm mrad by "Fair Operation Modes" document v.6 (2020)
      aGen->ReadDensityFile();
    }
    break;
  }
  case 5: {
    // realistic beam with a flat gas profile for testing
    tgtfile += "/input/H_flatprofile.txt";
    LOG(info) << "Using distributed Beam-Target profile " << tgtfile.Data();
    TObjArray *genList = fGen->GetListOfGenerators();
    for (int i = 0; i < genList->GetEntriesFast(); i++) {
      TObject *obj = genList->At(i);
      if (!obj->InheritsFrom("PndTargetGenerator"))
        continue;
      PndTargetGenerator *aGen = (PndTargetGenerator *)genList->At(i);
      aGen->SetDensityProfile(tgtfile);
      aGen->SetBeamRadius(0.1);                 // default beam spot sigma 1mm^2 by "Fair Operation Modes" document v.6 (2020)
      aGen->SetBeamPipeRadius(2.0);             // Smallest radius around interaction region
      aGen->SetConstantBeamRegion(-140., 223.); // region where the beam with stays at its minimum due to the solenoid field, edges at half Bz_max
      aGen->SetBeamDrDz(0.1);                   // default maximum beam divergence from 4-sigma emittance of 1-2 mm mrad by "Fair Operation Modes" document v.6 (2020)
      aGen->ReadDensityFile();
    }
  } break;
  case 6: {
    // pencil beam with a realistic target and restgas profile for testing
    tgtfile += "/input/restgasthickness_210421_Normal_IP_with_Cryopump.txt";
    LOG(info) << "Using distributed Beam-Target profile " << tgtfile.Data();
    TObjArray *genList = fGen->GetListOfGenerators();
    for (int i = 0; i < genList->GetEntriesFast(); i++) {
      TObject *obj = genList->At(i);
      if (!obj->InheritsFrom("PndTargetGenerator"))
        continue;
      PndTargetGenerator *aGen = (PndTargetGenerator *)genList->At(i);
      aGen->SetDensityProfile(tgtfile);
      aGen->ReadDensityFile();
    }
  } break;
  case 7: {
    // pencil beam with a flat restgas profile for testing
    tgtfile += "/input/H_flatprofile.txt";
    LOG(info) << "Using distributed Beam-Target profile " << tgtfile.Data();
    TObjArray *genList = fGen->GetListOfGenerators();
    for (int i = 0; i < genList->GetEntriesFast(); i++) {
      TObject *obj = genList->At(i);
      if (!obj->InheritsFrom("PndTargetGenerator"))
        continue;
      PndTargetGenerator *aGen = (PndTargetGenerator *)genList->At(i);
      aGen->SetDensityProfile(tgtfile);
      aGen->ReadDensityFile();
    }
  } break;
  default: LOG(info) << "Unknown target mode - Using no vertex smearing";
  }
}

void PndMasterRunSim::UseBoxGenerator(TString BoxConfig)
{
  // use BOX generator; defaults

  Double_t BoxMomMin = 0.05; // minimum momentum for box generator
  Double_t BoxMomMax = 10.;  // maximum   "       "
  Double_t BoxThtMin = 0.;   // minimum theta for box generator
  Double_t BoxThtMax = 180.; // maximum   "       "
  Double_t BoxPhiMin = 0.;   // minimum phi for box generator
  Double_t BoxPhiMax = 360.; // maximum   "       "
  Bool_t BoxCosTht = false;  // isotropic in cos(theta) instead theta
  Bool_t BoxPt = false;      // is pt given instead of p

  Int_t BoxType = 13;          // default particle muon
  Int_t BoxMult = 1;           // default particle multiplicity
  Double_t type = 0, mult = 0; // ref. parameters for range function

  Double_t BoxOriginX = 0.;
  Double_t BoxOriginY = 0.;
  Double_t BoxOriginZ = 0.;

  BoxConfig.ToLower();

  if (BoxConfig != "box") {
    BoxConfig.ReplaceAll("box", "");
    BoxConfig.ReplaceAll(" ", "");
    BoxConfig += ":";

    while (BoxConfig.Contains(":")) {
      TString curpar = BoxConfig(0, BoxConfig.Index(":"));
      BoxConfig = BoxConfig(BoxConfig.Index(":") + 1, 1000);
      curpar.ReplaceAll("[", "(");
      curpar.ReplaceAll("]", ")");

      if (curpar.BeginsWith("type(")) {
        GetRange(curpar, type, mult);
        BoxType = (Int_t)type;
        BoxMult = (Int_t)mult;
      }
      if (curpar.BeginsWith("p("))
        GetRange(curpar, BoxMomMin, BoxMomMax);
      if (curpar.BeginsWith("pt(")) {
        GetRange(curpar, BoxMomMin, BoxMomMax);
        BoxPt = true;
      }
      if (curpar.BeginsWith("tht("))
        GetRange(curpar, BoxThtMin, BoxThtMax);
      if (curpar.BeginsWith("ctht(")) {
        GetRange(curpar, BoxThtMin, BoxThtMax);
        BoxCosTht = true;
      }
      if (curpar.BeginsWith("phi("))
        GetRange(curpar, BoxPhiMin, BoxPhiMax);
      if (curpar.BeginsWith("xyz("))
        GetCoords(curpar, BoxOriginX, BoxOriginY, BoxOriginZ);
    }
  }

  PndBoxGenerator *boxGen = new PndBoxGenerator(BoxType, BoxMult);
  boxGen->SetDebug(0);

  if (BoxPt == true) {
    boxGen->SetPtRange(BoxMomMin, BoxMomMax);
  } else {
    boxGen->SetPRange(BoxMomMin, BoxMomMax); // GeV/c
  }
  boxGen->SetPhiRange(BoxPhiMin, BoxPhiMax);   // Azimuth angle range [degree]
  boxGen->SetThetaRange(BoxThtMin, BoxThtMax); // Polar angle in lab system range [degree]

  if (BoxCosTht)
    boxGen->SetCosTheta();

  boxGen->SetXYZ(BoxOriginX, BoxOriginY, BoxOriginZ); // cm

  LOG(info) << "Using PndBoxGenerator(" << GetBeamMom() << ", pdg=" << BoxType << " mult=" << BoxMult << " ) generator with range p[" << BoxMomMin << "," << BoxMomMax << "]  tht["
            << BoxThtMin << "," << BoxThtMax << "]" << (BoxCosTht ? "*" : "") << "  phi[" << BoxPhiMin << "," << BoxPhiMax << "] with origin at (" << BoxOriginX << ","
            << BoxOriginY << "," << BoxOriginZ << ")";

  //  cout <<"BOX generator range: p["<<BoxMomMin<<","<<BoxMomMax<<"]  tht["<<BoxThtMin<<","<<BoxThtMax<<"]"<<(BoxCosTht?"*":"")<<"  phi["<<BoxPhiMin<<","<<BoxPhiMax<<"]"<<endl;

  fGen->AddGenerator(boxGen);
}

void PndMasterRunSim::UsePiPiGenerator(TString pipiConfig)
{
  // use BOX generator; defaults

  Double_t cosThetaMin = 0.0;
  Double_t cosThetaMax = 1.0;

  pipiConfig.ToLower();

  if (pipiConfig != "pipi") {
    pipiConfig.ReplaceAll("pipi", "");
    pipiConfig.ReplaceAll(" ", "");
    pipiConfig += ":";

    while (pipiConfig.Contains(":")) {
      TString curpar = pipiConfig(0, pipiConfig.Index(":"));
      pipiConfig = pipiConfig(pipiConfig.Index(":") + 1, 1000);
      curpar.ReplaceAll("[", "(");
      curpar.ReplaceAll("]", ")");

      if (curpar.BeginsWith("cosTheta("))
        GetRange(curpar, cosThetaMin, cosThetaMax);
    }
  }

  PndPiPiGenerator *pipiGen = new PndPiPiGenerator();
  pipiGen->SetBeamMom(GetBeamMom());
  pipiGen->SetCosThetaMin(cosThetaMin);
  pipiGen->SetCosThetaMax(cosThetaMax);

  LOG(info) << "Using PndPiPiGenerator(BeamMom = " << GetBeamMom() << " GeV/c, cosThetaRange = " << cosThetaMin << " : " << cosThetaMax << ")";

  //  cout <<"BOX generator range: p["<<BoxMomMin<<","<<BoxMomMax<<"]  tht["<<BoxThtMin<<","<<BoxThtMax<<"]"<<(BoxCosTht?"*":"")<<"  phi["<<BoxPhiMin<<","<<BoxPhiMax<<"]"<<endl;

  fGen->AddGenerator(pipiGen);
}

void PndMasterRunSim::UseLepLepGenerator(TString leplepConfig)
{
  // use BOX generator; defaults

  Double_t cosThetaMin = 0.0;
  Double_t cosThetaMax = 1.0;
  Double_t pid = -1.0;
  Double_t GeGmRatio = -1.0;
  Double_t dummy = 1.0;

  leplepConfig.ToLower();

  if (leplepConfig != "") {
    leplepConfig.ReplaceAll("leplep", "");
    leplepConfig.ReplaceAll(" ", "");
    leplepConfig += ":";

    while (leplepConfig.Contains(":")) {
      TString curpar = leplepConfig(0, leplepConfig.Index(":"));
      leplepConfig = leplepConfig(leplepConfig.Index(":") + 1, 1000);
      curpar.ReplaceAll("[", "(");
      curpar.ReplaceAll("]", ")");

      if (curpar.BeginsWith("pid("))
        GetRange(curpar, pid, dummy);
      if (curpar.BeginsWith("gegm("))
        GetRange(curpar, GeGmRatio, dummy);
      if (curpar.BeginsWith("cosTheta("))
        GetRange(curpar, cosThetaMin, cosThetaMax);
    }
  }

  PndLepLepGenerator *leplepGen = new PndLepLepGenerator();
  leplepGen->SetBeamMom(GetBeamMom());

  leplepGen->SetCosThetaMin(cosThetaMin);
  leplepGen->SetCosThetaMax(cosThetaMax);

  LOG(info) << "Using PndleplepGenerator(BeamMom = " << GetBeamMom() << " GeV/c, cosThetaRange = " << cosThetaMin << " : " << cosThetaMax << ")";

  //  cout <<"BOX generator range: p["<<BoxMomMin<<","<<BoxMomMax<<"]  tht["<<BoxThtMin<<","<<BoxThtMax<<"]"<<(BoxCosTht?"*":"")<<"  phi["<<BoxPhiMin<<","<<BoxPhiMax<<"]"<<endl;

  fGen->AddGenerator(leplepGen);
}

// -----   SetGenerator   --------------------------------------------------
void PndMasterRunSim::AddGenerator(FairGenerator *aGen)
{
  LOG(info) << "Adding a FairGenerator generator: " << aGen->GetName();
  CreatePrimGen();
  fGen->AddGenerator(aGen);
}

// -----   SetGenerator   --------------------------------------------------
void PndMasterRunSim::SetGenerator(FairGenerator *aGen)
{
  LOG(info) << "Using only the FairGenerator generator:" << aGen->GetName();
  CreatePrimGen();
  // make sure to set the list to only one generator
  TObjArray *genList = fGen->GetListOfGenerators();
  genList->Clear(); // We're not using Delete(), since we don't know what users may still want to do with their generators
  fGen->AddGenerator(aGen);
}

// -----   UseDpmGenerator   -----------------------------------------------
void PndMasterRunSim::UseDpmGenerator()
{
  LOG(info) << "Using PndDpmDirect(" << GetBeamMom() << ", " << fDpmFlag << ") generator";
  PndDpmDirect *Dpm = new PndDpmDirect(GetBeamMom(), fDpmFlag);
  fGen->AddGenerator(Dpm);
}

void PndMasterRunSim::UseAsciiGenerator(TString AsciiFile)
{
  LOG(info) << "Using Asciigenerator";
  FairAsciiGenerator *ascGen = new FairAsciiGenerator(AsciiFile);
  fGen->AddGenerator(ascGen);
}

// -----   UseFtfGenerator   -----------------------------------------------
void PndMasterRunSim::UseFtfGenerator(TString ftfData)
{
  // if ( strncmp(fName,"TGeant4",7 ) == 0 ) LOG(fatal) << "FTF does not run with Geant4 !!!"  ;
  if (ftfData.Contains(".root")) {
    LOG(info) << "Using PndFtfGenerator with input file " << ftfData;
    PndFtfGenerator *Ftf = new PndFtfGenerator(ftfData);
    fGen->AddGenerator(Ftf);
  } else {
    LOG(info) << "Using PndFtfDirect(anti_proton, G4_H, 1, ftfp, " << GetBeamMom() << ", " << gRandom->GetSeed() << ", " << fFtfFlag << ") generator";
    PndFtfDirect *Ftf = new PndFtfDirect("anti_proton", "G4_H", 1, "ftfp", GetBeamMom(), gRandom->GetSeed(), fFtfFlag);
    fGen->AddGenerator(Ftf);
  }
}

// -----   UsePythia8Generator   -------------------------------------------
void PndMasterRunSim::UsePythia8Generator(TString py8Data)
{
  PndPythia8Direct *pygen = new PndPythia8Direct();
  pygen->SetMom(GetBeamMom());
  // clean beginning of the string
  py8Data.ReplaceAll("pythia8:", "");
  py8Data.ReplaceAll("pythia8", "");
  if (py8Data.Contains("=")) // lets make sure the sting is valid
  {
    LOG(info) << "Using user config PndPythia8Direct with beam momentum " << GetBeamMom() << " and the options \"" << py8Data.Data() << "\"";
    //  tokenize
    TString token;
    int from = 0;
    while (py8Data.Tokenize(token, from, ";")) {
      pygen->AddParameter(token.Data());
    }
  } else {
    // this is used as the default process for Panda - may change with newer versions of Pythia8
    LOG(info) << "Using default PndPythia8Direct with beam momentum " << GetBeamMom() << " and the options \"SoftQCD:inelastic = on\"";
    pygen->SetParameters("SoftQCD:inelastic = on");
  }
  fGen->AddGenerator(pygen);
}

// -----   UseEvtGenGenerator   --------------------------------------------
void PndMasterRunSim::UseEvtGenGenerator(TString EvtGenFile)
{

  TString IniRes = "";

  if (EvtGenFile.Contains(":")) // is the initial resonance provide as <decfile>.dec:iniRes ?
  {
    IniRes = EvtGenFile(EvtGenFile.Index(":") + 1, 1000);
    EvtGenFile = EvtGenFile(0, EvtGenFile.Index(":"));
  }

  if (IniRes == "") // we need to search the decay file
  {
    TString fnamepath = fInputDir + EvtGenFile;
    std::ifstream fs(fnamepath.Data());
    char line[250];

    while (fs) {
      fs.getline(line, 249);
      TString s(line);
      s.ReplaceAll("\r", "");
      if (IniRes == "" && s.Contains("Decay ")) {
        if (s.Contains("#"))
          s = s(0, s.Index("#"));
        s.ReplaceAll("Decay ", "");
        s.ReplaceAll(" ", "");
        IniRes = s;
      }
    }
    fs.close();
  }
  /*
  // Looping over the dec file trying to find the first string "Decay", in order to find the initai
  // state as the following string
  FILE *dec = fopen(fInputDir+EvtGenFile,"r");
  if (dec==nullptr) LOG(fatal) << "The EvtGen dec file does not exist!! " << EvtGenFile ;

  char temp[6], particle[20];
  Bool_t found = kFALSE;
  while(fgets(temp, 6, dec) !=nullptr)
    {
      if((strstr(temp, "Decay")) != nullptr)
  {
    fscanf(dec, "%s",particle);
    LOG(info) << "It was found a " << particle << " as initial state." ;
    found = kTRUE;
    break;
  }
    }
  */
  if (IniRes == "")
    LOG(fatal) << "The input file is not a proper .dec!! ";

  //   TString  EvtInput =gSystem->Getenv("VMCWORKDIR");
  //   EvtInput+="/macro/run/psi2s_Jpsi2pi_Jpsi_mumu.dec";
  LOG(info) << "Using PndEvtGenDirect(" << IniRes << ", " << (fInputDir + EvtGenFile).Data() << ", " << GetBeamMom() << ") generator";
  PndEvtGenDirect *EvtGen = new PndEvtGenDirect(IniRes, (fInputDir + EvtGenFile).Data(), GetBeamMom());
  EvtGen->SetStoreTree(kTRUE);
  // --------------------------------------------------------------------------
  fGen->AddGenerator(EvtGen);
}

void PndMasterRunSim::PrintListOfTasks(TList *list, TString prefix)
{
  if (list == nullptr)
    return;
  for (const TObject *obj : *list) {
    cout << prefix.Data() << obj->ClassName() << " (" << obj->GetName() << ")" << endl;
    PrintListOfTasks(((FairTask *)obj)->GetListOfTasks(), "  " + prefix);
  }
}

// -----   Finish   ---------------------------------------------------------
void PndMasterRunSim::Finish()
{
  fRtdb->saveOutput();

  cout << "PndMasterRunSim::Finish(): Tasks that ran just now:" << endl;
  FairSink *sink = fRootManager->GetSink();
  TFile *outfile = nullptr;
  if (sink->GetSinkType() == kFILESINK) {
    outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();

    bool wasopen = outfile->IsOpen();
    if (!wasopen) {
      cout << "file is " << ((wasopen) ? "" : "not ") << "open" << endl;
      cout << "Opening file: " << outfile->GetName() << endl;
      outfile = TFile::Open(outfile->GetName(), "UPDATE");
    }
    outfile->cd();

    // write the summary of event filter to output root file
    if (!strcmp(fGen->ClassName(), "PndFilteredPrimaryGenerator"))
      ((PndFilteredPrimaryGenerator *)fGen)->WriteEvtFilterStatsToRootFile(outfile);
    else
      ((FairFilteredPrimaryGenerator *)fGen)->WriteEvtFilterStatsToRootFile(outfile);

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
  cout << "Parameter ROOT file is\t" << fParamRootFile << endl;
  cout << "Parameter ASCII file is\t" << fParamAsciiFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl;
  cout << "CPU usage " << cpuUsage * 100. << "%" << endl;
  cout << "Max Memory " << maxMemory << " MB" << endl;

  cout << "Macro finished successfully." << endl;
}

// -----Helper function for parameter parsing  ---------------------------------------------------------
// par = string parameter like 'varname(min,max)' of 'varname(value)'

void PndMasterRunSim::GetRange(TString par, double &min, double &max)
{
  par.ReplaceAll(" ", "");
  par = par(par.Index("(") + 1, par.Length() - par.Index("(") - 2);

  TString smin = par, smax = par;

  if (par.Contains(",")) {
    smin = par(0, par.Index(","));
    smax = par(par.Index(",") + 1, 1000);
  }

  min = smin.Atof();
  max = smax.Atof();
}

void PndMasterRunSim::GetCoords(TString par, double &X, double &Y, double &Z)
{
  par = par(par.Index("(") + 1, par.Length() - par.Index("(") - 2);
  if ((par.CountChar(',') == 2)) {
    // Elementwise parsing of Parameters
    X = static_cast<TString>(par(0, par.Index(","))).Atof();
    par = par(par.Index(",") + 1, 1000);
    Y = static_cast<TString>(par(0, par.Index(","))).Atof();
    par = par(par.Index(",") + 1, 1000);
    Z = static_cast<TString>(par).Atof();
  }
}
void PndMasterRunSim::CreatePrimGen()
{
  if (fGen == nullptr) {
    if (fOptions.Contains("pndfiltprim"))
      fGen = new PndFilteredPrimaryGenerator();
    else
      fGen = new FairFilteredPrimaryGenerator();
  }
}

/** @cond CLASSIMP */
ClassImp(PndMasterRunSim);
/** @endcond */
