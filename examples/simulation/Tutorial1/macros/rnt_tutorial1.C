/********************************************************************************
 * Copyright (C) 2014-2024 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#include <TChain.h>
#include <TRandom3.h>
#include <TStopwatch.h>
#include <TString.h>
#include <TSystem.h>
#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

using std::cout;
using std::endl;
using std::string;

int Read(string fileName, int nEvents, int nofPart);

int rnt_tutorial1(int nEvents = 10, string mcEngine = "TGeant4", bool isMT = true, bool loadPostInitConfig = false)
{
    string dir = getenv("VMCWORKDIR");
    string tutdir = dir + "/simulation/Tutorial1";

    string tut_geomdir = dir + "/common/geometry";
    gSystem->Setenv("GEOMPATH", tut_geomdir.c_str());

    string tut_configdir = dir + "/common/gconfig";
    gSystem->Setenv("CONFIG_DIR", tut_configdir.c_str());

    string partName[] = {"pions", "eplus", "proton"};
    int partPdgC[] = {211, 11, 2212};
    auto chosenPart = 0;

    auto momentum = 2.;

    auto theta = 0.;

    auto nofPart = 1;

    string outPrefix = "./rntuple1_";
    outPrefix += mcEngine;
    if (isMT) {
        outPrefix += "MT";
    }
    if (loadPostInitConfig) {
        outPrefix += "_postinit";
    }
    outPrefix += '_';

    // Output file name
    string outFile = outPrefix + partName[chosenPart] + ".mc_p" + std::to_string(momentum) + "_t"
                     + std::to_string(theta) + "_n" + std::to_string(nEvents) + ".root";

    // Parameter file name
    string parFile = outPrefix + partName[chosenPart] + ".params_p" + std::to_string(momentum) + "_t"
                     + std::to_string(theta) + "_n" + std::to_string(nEvents) + ".root";

    auto geoFile = outPrefix + "_geofile_full.root";

    // In general, the following parts need not be touched
    // ========================================================================

    // ----    Debug option   -------------------------------------------------
    gDebug = 0;
    // ------------------------------------------------------------------------

    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // ------------------------------------------------------------------------

    // -----   Create simulation run   ----------------------------------------
    auto run = std::make_unique<FairRunSim>();
    run->SetName(mcEngine.c_str());   // Transport engine
    auto config = std::make_unique<FairGenericVMCConfig>();
    if (loadPostInitConfig)
        config->UsePostInitConfig();
    run->SetSimulationConfig(std::move(config));
    run->SetIsMT(isMT);   // Multi-threading mode (Geant4 only)
    run->SetSink(std::make_unique<FairRNTupleSink>(outFile));
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    // ------------------------------------------------------------------------

    // -----   Create media   -------------------------------------------------
    run->SetMaterials("media.geo");   // Materials
    // ------------------------------------------------------------------------

    // -----   Create geometry   ----------------------------------------------

    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName("cave_vacuum.geo");
    run->AddModule(cave);

    FairTutorialDet1* tutdet = new FairTutorialDet1("TUTDET", kTRUE);
    tutdet->SetGeometryFileName("double_sector.geo");
    tutdet->SetVectorStorage(true);
    run->AddModule(tutdet);
    // ------------------------------------------------------------------------

    // -----   Create PrimaryGenerator   --------------------------------------
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    FairBoxGenerator* boxGen = new FairBoxGenerator(partPdgC[chosenPart], nofPart);

    boxGen->SetThetaRange(theta, theta + 0.01);
    boxGen->SetPRange(momentum, momentum + 0.01);
    boxGen->SetPhiRange(0., 360.);
    boxGen->SetDebug(kTRUE);

    primGen->AddGenerator(boxGen);

    run->SetGenerator(primGen);
    // ------------------------------------------------------------------------

    // -----   Initialize simulation run   ------------------------------------
    UInt_t randomSeed = 123456;
    TRandom3 random(randomSeed);
    gRandom = &random;

    run->Init();
    // ------------------------------------------------------------------------

    // -----   Runtime database   ---------------------------------------------

    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
    parOut->open(parFile.c_str());
    rtdb->setOutput(parOut);
    rtdb->saveOutput();
    rtdb->print();
    // ------------------------------------------------------------------------

    // -----   Start run   ----------------------------------------------------
    run->Run(nEvents);
    run->CreateGeometryFile(geoFile.c_str());
    // ------------------------------------------------------------------------

    // -----   Finish   -------------------------------------------------------

    cout << endl << endl;

    // Extract the maximal used memory an add is as Dart measurement
    // This line is filtered by CTest and the value send to CDash
    FairSystemInfo sysInfo;
    Float_t maxMemory = sysInfo.GetMaxMemory();
    cout << "<DartMeasurement name=\"MaxMemory\" type=\"numeric/double\">";
    cout << maxMemory;
    cout << "</DartMeasurement>" << endl;

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();

    Float_t cpuUsage = ctime / rtime;
    cout << "<DartMeasurement name=\"CpuLoad\" type=\"numeric/double\">";
    cout << cpuUsage;
    cout << "</DartMeasurement>" << endl;

    cout << endl << endl;
    cout << "Output file is " << outFile << endl;
    cout << "Parameter file is " << parFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
    cout << "Macro finished successfully." << endl;

    // ------------------------------------------------------------------------

    return Read(outFile, nEvents, nofPart);
}


// For all of the events, histogram only one of the written vectors
int Read(string fileName, int nEvents, int nofPart)
{
   // Get a unique pointer to an empty RNTuple model
   auto model = ROOT::RNTupleModel::Create();
   
   auto tracks = model->MakeField<std::vector<FairMCTrack>>("MCTrack");
   auto points = model->MakeField<std::vector<FairTutorialDet1Point>>("TutorialDetPoint");

   // Create an ntuple without imposing a specific data model.  We could generate the data model from the ntuple
   // but here we prefer the view because we only want to access a single field
   auto reader = ROOT::RNTupleReader::Open(std::move(model), "fairdata", fileName);

   //   reader->PrintInfo();

   //   std::cout << "Entry number 5 in JSON format:" << std::endl;
   //   reader->Show(4);

   int nofE = 0;
   int nofT = 0; 
   int nofP = 0;
  
   for (auto entryId : *reader) {
       nofE++;
       reader->LoadEntry(entryId);
       nofT += tracks->size();
       nofP += points->size();
   }
   
   cout << "\nOutput chain has " << nofE << " events, " << nofT << " tracks, " << nofP << " points.\n";
    if (nofE < nEvents) {
        std::cerr << "Not enough events (" << nofE << " < " << nEvents << ") in the output chain." << endl;
        return 1;
    }
    if (nofT < nEvents * nofPart) {
        std::cerr << "Not enough tracks (" << nofT << " < " << nEvents * nofPart << ") in the output chain." << endl;
        return 1;
    }
    if (nofP < nEvents * nofPart) {
        std::cerr << "Not enough points (" << nofP << " < " << nEvents * nofPart << ") in the output chain." << endl;
        return 1;
    }
    cout << "Simulation successful." << endl;

    return 0;
}
