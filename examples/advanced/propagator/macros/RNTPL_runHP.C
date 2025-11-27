/********************************************************************************
 *    Copyright (C) 2020 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
int RNTPL_runHP()
{
    TString dir = getenv("VMCWORKDIR");

    TString tut_geomdir = dir + "/common/geometry";
    gSystem->Setenv("GEOMPATH", tut_geomdir.Data());

    TString tut_configdir = dir + "/common/gconfig";
    gSystem->Setenv("CONFIG_DIR", tut_configdir.Data());

    // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
    Int_t iVerbose = 0;   // just forget about it, for the moment

    // Input file (MC events)
    TString inFile = "rntpl_prop.mc.root";

    // Parameter file
    TString parFile = "rntpl_prop.par.root";

    // Output file
    TString outFile = "rntpl_prop.r_hits.root";

    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;

    // -----   Reconstruction run   -------------------------------------------
    FairRunAna run{};
    run.SetSource(new FairRNTupleSource(inFile));
    run.SetSink(std::make_unique<FairRNTupleSink>(outFile));

    FairRuntimeDb* rtdb = run.GetRuntimeDb();
    FairParRootFileIo* parInput1 = new FairParRootFileIo();
    parInput1->open(parFile.Data());

    rtdb->setFirstInput(parInput1);

    FairTutPropHitProducer* hpTask = new FairTutPropHitProducer();
    run.AddTask(hpTask);

    FairTutPropHitProducer* hpTask2 = new FairTutPropHitProducer();
    hpTask2->SetPointsArrayName("FairTutPropPoint2");
    hpTask2->SetHitsArrayName("FairTutPropHits2");
    run.AddTask(hpTask2);

    run.Init();

    timer.Start();
    run.Run();

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
    return 0;
}
