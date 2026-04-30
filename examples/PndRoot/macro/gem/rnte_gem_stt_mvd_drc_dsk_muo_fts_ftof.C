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

Int_t rnte_gem_stt_mvd_drc_dsk_muo_fts_ftof(bool isMT = true, Int_t nStations = 3, Int_t nparts = 10, Int_t nEvents = 1000, Int_t pdgC = 211, int verboseLevel = 0)
{
  if (nStations != 3 && nStations != 4) {
    cout << "WRONG number of stations, only 3 or 4 allowed." << endl;
    return 0;
  }

  gRandom->SetSeed(1234);
  
  TString tut_configdir = "/Users/karabowi/panda/pandaroot/dev_2603/PandaRoot/gconfig";
  gSystem->Setenv("CONFIG_DIR", tut_configdir.Data());

  TString tut_geomdir = "/Users/karabowi/panda/pandaroot/dev_2603/PandaRoot/geometry";
  gSystem->Setenv("GEOMPATH", tut_geomdir.Data());
    
  TStopwatch timer;
  timer.Start();
  gDebug = 0;

  // FileNames
  TString OutputFile;
  OutputFile.Form("rnte_gRtG4_n%d_gsmddmff",nEvents);
  if (isMT)
      OutputFile += "_mt2";
      
  TString ParOutputFile = OutputFile;
  OutputFile += ".root";
  ParOutputFile += "_par.root";

  TString SimEngine = "TGeant4";
  Double_t BeamMomentum = 15.0;
  TString MediaFile = "media_pnd.geo";
  gDebug = 0;
  //------------------------------------------------------------------

  // Create the Simulation run manager--------------------------------
  FairRunSim *fRun = new FairRunSim();
  fRun->SetName(SimEngine.Data());
  //  fRun->SetOutputFile(OutputFile.Data());
  fRun->SetSink(std::make_unique<FairRNTupleSink>(OutputFile.Data()));
  fRun->SetBeamMom(BeamMomentum);
  fRun->SetMaterials(MediaFile.Data());
  fRun->SetIsMT(isMT);   // Multi-threading mode (Geant4 only)

  FairRuntimeDb *rtdb = fRun->GetRuntimeDb();

  //---------------------Set Parameter output      ----------
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo *output = new FairParRootFileIo(kParameterMerged);
  output->open(ParOutputFile.Data());
  rtdb->setOutput(output);

  // Create and add detectors
  //-------------------------

  FairModule *Cave = new PndCave("CAVE");
  Cave->SetGeometryFileName("pndvacuumcave.geo");
  fRun->AddModule(Cave);

  FairModule *Pipe = new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);

  //  FairModule *Magnet= new PndMagnet("MAGNET");
  //  Magnet->SetGeometryFileName("FullSolenoid.root");
  //  fRun->AddModule(Magnet);

  
  FairModule *dipole = new PndMagnet("MAGNET");
  dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(dipole);

  FairDetector *Stt = new PndStt2("STT2", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  fRun->AddModule(Stt);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  fRun->AddModule(Mvd);

  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName(Form("gem_%dStations_Tube.root", nStations));
  Gem->SetVerboseLevel(0);
  fRun->AddModule(Gem);
  
  //  FairDetector *SciT = new PndSciT("SCIT", kTRUE);
  //  SciT->SetGeometryFileName("SciTil_201601.root");
  //  fRun->AddModule(SciT);
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetGeometryFileName("dirc_e3_b3_l6_m40.root");
  Drc->SetRunCherenkov(0);
  fRun->AddModule(Drc);
  
      PndDsk *Dsk = new PndDsk("DSK", kTRUE);
    Dsk->SetStoreCerenkovs(0);
    Dsk->SetStoreTrackPoints(1);
    fRun->AddModule(Dsk);
    PndMdt *Muo = new PndMdt("MDT", kTRUE);
    Muo->SetBarrel("fast");
    Muo->SetEndcap("fast");
    Muo->SetMuonFilter("fast");
    Muo->SetForward("fast");
    Muo->SetMdtMagnet(1);
    Muo->SetMdtCoil(1);
    Muo->SetMdtMFIron(1);
    fRun->AddModule(Muo);

    FairDetector *Fts = new PndFts("FTS", kTRUE);
    Fts->SetGeometryFileName("fts.geo");
    fRun->AddModule(Fts);
        FairDetector *FTof = new PndFtof("FTOF", kTRUE);
    FTof->SetGeometryFileName("ftofwall.root");
    fRun->AddModule(FTof);

  // Event generator
  FairPrimaryGenerator *primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  FairBoxGenerator *boxGen = new FairBoxGenerator(pdgC, nparts);
  boxGen->SetThetaRange(0, 100);
  boxGen->SetPhiRange(0., 360.);
  boxGen->SetPRange(2., 2.);
  primGen->AddGenerator(boxGen);

  //---------------------Create and Set the Field(s)----------
  //  PndMultiField *fField = new PndMultiField("FULL");
  //  fRun->SetField(fField);
  //-----------end of Bfield stuff

  // support event display?
  fRun->SetStoreTraj(kFALSE);

  fRun->SetRadLenRegister(kFALSE);

  fRun->Init();

  // Transport nEvents
  // -----------------

  fRun->Run(nEvents);

  rtdb->saveOutput();
  rtdb->print();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

  cout << OutputFile.Data() << endl;

  return 1;
}
