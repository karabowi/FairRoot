/********************************************************************************
 * Copyright (C) 2014-2022 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#include <TStopwatch.h>
#include <TString.h>
#include <TSystem.h>
#include <memory>

void run_simdigi(int nEvents = 10, string mcEngine = "TGeant3", bool runMT = false)
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    string dir = getenv("VMCWORKDIR");
    string tutdir = dir + "/simulation/Tutorial1";

    string tut_geomdir = dir + "/common/geometry";
    gSystem->Setenv("GEOMPATH", tut_geomdir.c_str());

    string tut_configdir = dir + "/common/gconfig";
    gSystem->Setenv("CONFIG_DIR", tut_configdir.c_str());

    // create Instance of Run Manager class
    auto run = std::make_unique<FairRunSim>();
    run->SetIsMT(runMT);
    run->SetUseFairLinks(true);
    //  FairLinkManager::Instance()->AddIncludeType(0);
    // set the MC version used
    // ------------------------

    run->SetName(mcEngine.c_str());

    string outFile = "data/data_simdigi_" + mcEngine + ".root";
    string geoFile = "data/geo_simdigi_" + mcEngine + ".root";
    string parFile = "data/par_simdigi_" + mcEngine + ".root";

    run->SetSink(std::make_unique<FairRootFileSink>(outFile));

    // -----   Magnetic field   -------------------------------------------
    // Constant Field
    FairConstField *fMagField = new FairConstField();
    fMagField->SetField(0., 10., 0.);                        // values are in kG
    fMagField->SetFieldRegion(-50, 50, -50, 50, 350, 450);   // values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)
    run->SetField(fMagField);
    // --------------------------------------------------------------------

    // Set Material file Name
    //-----------------------
    run->SetMaterials("media.geo");

    // Create and add detectors
    //-------------------------
    FairModule *Cave = new FairCave("CAVE");
    Cave->SetGeometryFileName("cave.geo");
    run->AddModule(Cave);

    FairModule *Magnet = new FairMagnet("MAGNET");
    Magnet->SetGeometryFileName("magnet.geo");
    run->AddModule(Magnet);

    FairDetector *Torino = new FairTestDetector("TORINO", kTRUE);
    Torino->SetGeometryFileName("torino.geo");
    run->AddModule(Torino);

    // Add digitizer
    FairTestDetectorDigiTask *digiTask = new FairTestDetectorDigiTask();
    run->AddTask(digiTask);

    // Create and Set Event Generator
    //-------------------------------

    FairPrimaryGenerator *primGen = new FairPrimaryGenerator();
    run->SetGenerator(primGen);

    auto nofPart = 10;

    // Box Generator
    FairBoxGenerator *boxGen = new FairBoxGenerator(2212, nofPart);   // 13 = muon; 1 = multipl.
    boxGen->SetPRange(2., 2.);                                        // GeV/c //setPRange vs setPtRange
    boxGen->SetPhiRange(0, 360);                                      // Azimuth angle range [degree]
    boxGen->SetThetaRange(0, 0);                                      // Polar angle in lab system range [degree]
    boxGen->SetCosTheta();   // uniform generation on all the solid angle(default)

    // boxGen->SetXYZ(0., 0.37, 0.);
    primGen->AddGenerator(boxGen);

    run->SetStoreTraj(false);

    run->Init();

    // Fill the Parameter containers for this run
    //-------------------------------------------

    FairRuntimeDb *rtdb = run->GetRuntimeDb();
    auto kParameterMerged = true;
    FairParRootFileIo *parOut = new FairParRootFileIo(kParameterMerged);
    parOut->open(parFile.c_str());
    rtdb->setOutput(parOut);

    rtdb->saveOutput();
    rtdb->print();

    // Transport nEvents
    // -----------------

    //  Int_t nEvents = 1;
    run->Run(nEvents);

    run->CreateGeometryFile(geoFile.c_str());

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
    if (run->WasMT()) {
        cout << endl << "MT mode, reading two files: t1 and t2" << endl;
        chain.Add((outFile.replace(outFile.find(".root"), 5, "_t1.root")).c_str());
        chain.Add((outFile.replace(outFile.find("_t1.root"), 8, "_t2.root")).c_str());
    } else {
        cout << endl << "Read from " << outFile << endl;
        chain.Add(outFile.c_str());
    }

    auto nofE = chain.GetEntries();
    auto nofP = chain.Draw("FairTestDetectorPoint.fZ", "", "goff");
    auto nofD = chain.Draw("FairTestDetectorDigi.fZ", "", "goff");
    cout << "Output chain has " << nofE << " events, " << nofP << " points, " << nofD << " digis." << endl;
    if (nofE < nEvents) {
        std::cerr << "Not enough events (" << nofE << " < " << nEvents << ") in the output chain." << endl;
        return 1;
    }
    if (nofP < nEvents * nofPart * 3) {
        std::cerr << "Not enough points (" << nofP << " < " << nEvents * nofPart << ") in the output chain." << endl;
        return 1;
    }
    if (nofD < nEvents * nofPart * 3) {
        std::cerr << "Not enough digis (" << nofD << " < " << nEvents * nofPart << ") in the output chain." << endl;
        return 1;
    }
    cout << "Simulation successful." << endl;
}
