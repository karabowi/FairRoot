/********************************************************************************
 * Copyright (C) 2014-2023 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#include <TStopwatch.h>
#include <TSystem.h>
#include <memory>

using std::cout;
using std::endl;
using std::string;

int run_tutorial1_pythia8(int nEvents = 10, string mcEngine = "TGeant3")
{
    string dir = getenv("VMCWORKDIR");
    string tutdir = dir + "/simulation/Tutorial1";

    string tut_geomdir = dir + "/common/geometry";
    gSystem->Setenv("GEOMPATH", tut_geomdir.c_str());

    string tut_configdir = dir + "/common/gconfig";
    gSystem->Setenv("CONFIG_DIR", tut_configdir.c_str());

    string pythia8Config = tut_configdir + "/DecayConfigPythia8.C";

    // 2023.11.23: Introduce and choose D0(421) particle
    // which apparently is not decayed by Geant3.
    // Points present in the input are there thanks to
    // external decayer, Pythia8.
    string partName[] = {"pions", "eplus", "proton", "D0"};
    int partPdgC[] = {221, 11, 2212, 421};
    auto chosenPart = 3;

    auto momentum = 2.;

    auto theta = 0.;

    auto nofPart = 1;

    string outDir = "./";

    // Output file name
    string outFile = outDir + "/tutorial1_pythia8_" + mcEngine + "_" + partName[chosenPart] + ".mc_p" + std::to_string(momentum)
                     + "_t" + std::to_string(theta) + "_n" + std::to_string(nEvents) + ".root";

    // Parameter file name
    string parFile = outDir + "/tutorial1_pythia8_" + mcEngine + "_" + partName[chosenPart] + ".params_p"
                     + std::to_string(momentum) + "_t" + std::to_string(theta) + "_n" + std::to_string(nEvents)
                     + ".root";

    auto geoFile = "geofile_" + mcEngine + "_full.root";

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
    FairRunSim run{};
    run.SetName(mcEngine.c_str());   // Transport engine
    run.SetSink(std::make_unique<FairRootFileSink>(outFile));
    run.SetPythiaDecayer(pythia8Config);   // Define Pythia8 as decayer

    FairRuntimeDb* rtdb = run.GetRuntimeDb();
    // ------------------------------------------------------------------------

    // -----   Create media   -------------------------------------------------
    run.SetMaterials("media.geo");   // Materials
    // ------------------------------------------------------------------------

    // -----   Create geometry   ----------------------------------------------

    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName("cave_vacuum.geo");
    run.AddModule(cave);

    FairDetector* tutdet = new FairTutorialDet1("TUTDET", kTRUE);
    tutdet->SetGeometryFileName("double_sector.geo");
    run.AddModule(tutdet);
    // ------------------------------------------------------------------------

    // -----   Create PrimaryGenerator   --------------------------------------
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    FairBoxGenerator* boxGen = new FairBoxGenerator(partPdgC[chosenPart], 1);

    boxGen->SetThetaRange(theta, theta + 0.01);
    boxGen->SetPRange(momentum, momentum + 0.01);
    boxGen->SetPhiRange(0., 360.);
    boxGen->SetDebug(kTRUE);

    primGen->AddGenerator(boxGen);

    run.SetGenerator(primGen);
    // ------------------------------------------------------------------------

    // -----   Initialize simulation run   ------------------------------------
    run.Init();
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
    run.Run(nEvents);
    run.CreateGeometryFile(geoFile.c_str());
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

    TChain chain("cbmsim");
    chain.Add(outFile.c_str());

    auto nofE = chain.GetEntries();
    auto nofT = chain.Draw("MCTrack.fPx", "", "goff");
    auto nofP = chain.Draw("TutorialDetPoint.fX", "", "goff");
    auto nofB = (chain.GetListOfBranches())->GetEntries();
    auto nofL = (chain.GetListOfLeaves())->GetEntries();
    cout << "Output chain has " << nofE << " events, " << nofT << " tracks, " << nofP << " points in " << nofB
         << " branches, " << nofL << " leaves." << endl;
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
    if (nofB != 3) {
        std::cerr << "Wrong number of branches (" << nofB << " instead of 3) in the output chain." << endl;
        return 1;
    }
    if (nofL != 46) {
        std::cerr << "Wrong number of leaves (" << nofL << " instead of 46) in the output chain." << endl;
        return 1;
    }
    cout << "Simulation successful." << endl;

    return 0;
}
