#include "runFairMQDevice.h"

// MQRunSim
#include "FairMQRunSim.h"
#include "FairRuntimeDb.h"
#include "FairModule.h"
#include "FairCave.h"
#include "Pixel.h"
#include "FairPrimaryGenerator.h"
#include "FairBoxGenerator.h"
#include "FairParRootFileIo.h"
  
#include "TROOT.h"
#include "TRint.h"
#include "TVirtualMC.h"
#include "TVirtualMCApplication.h"


namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description& options)
{
}

FairMQDevicePtr getDevice(const FairMQProgOptions& config)
{
  gROOT->Print();
  TVirtualMC::GetMC();
  //  gMC->Print();

  Int_t nEvents = 10;
  TString mcEngine = "TGeant3";
  Int_t fileId = 0;

  TString dir = getenv("VMCWORKDIR");
  TString tutdir = dir + "/MQ/9-PixelDetector";

  TString tut_geomdir = dir + "/common/geometry";
  gSystem->Setenv("GEOMPATH",tut_geomdir.Data());

  TString tut_configdir = dir + "/common/gconfig";
  gSystem->Setenv("CONFIG_DIR",tut_configdir.Data());

  TString partName[] = {"pions","eplus","proton"};
  Int_t   partPdgC[] = {    211,     11,    2212};
  Int_t chosenPart  = 0;

  TString outDir = "./";
  TString outFile;
  if ( fileId == 0 )  outFile = Form("%s/pixel_%s.mc.root",
				     outDir.Data(),
				     mcEngine.Data());
  else                outFile = Form("%s/pixel_%s.mc.f%d.root",
				     outDir.Data(),
				     mcEngine.Data(),
				     fileId);
 
  TString parFile = Form("%s/pixel_%s.params.root",
			 outDir.Data(),
			 mcEngine.Data());

  FairMQRunSim* run = new FairMQRunSim();

  run->GetFairRunSimPointer()->SetName(mcEngine);              // Transport engine
  run->GetFairRunSimPointer()->SetOutputFile(outFile);          // Output file
  FairRuntimeDb* rtdb = run->GetFairRunSimPointer()->GetRuntimeDb();
  // ------------------------------------------------------------------------
  
  // -----   Create media   -------------------------------------------------
  run->GetFairRunSimPointer()->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------
  
  // -----   Create geometry   ----------------------------------------------

  FairModule* cave= new FairCave("CAVE");
  cave->SetGeometryFileName("cave_vacuum.geo");
  run->GetFairRunSimPointer()->AddModule(cave);

  Pixel*  det = new Pixel("Tut9", kTRUE);
  det->SetGeometryFileName("pixel.geo");
//  det->SetMisalignDetector(kTRUE);
  run->GetFairRunSimPointer()->AddModule(det);
  // ------------------------------------------------------------------------

  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  FairBoxGenerator* boxGen = new FairBoxGenerator(partPdgC[chosenPart], 5);
  boxGen->SetPRange(1,2);
  boxGen->SetThetaRange(0,40);
  boxGen->SetPhiRange(0,360);
  //  boxGen->SetDebug(kTRUE);

  primGen->AddGenerator(boxGen);

  
  run->GetFairRunSimPointer()->SetGenerator(primGen);
  // ------------------------------------------------------------------------

  run->GetFairRunSimPointer()->SetStoreTraj(kFALSE);
   
  // -----   Initialize simulation run   ------------------------------------
  run->GetFairRunSimPointer()->Init();
  // ------------------------------------------------------------------------

  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------
  run->GetFairRunSimPointer()->CreateGeometryFile("geofile_full.root");

  return run;



}
