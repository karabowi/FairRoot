#include "runFairMQDevice.h"

// MQRunSim
#include "FairMQSimManager.h"
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
#include "TSystem.h"


namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description& options)
{
}

FairMQDevicePtr getDevice(const FairMQProgOptions& config)
{
  TString dir = getenv("VMCWORKDIR");
  TString tutdir = dir + "/MQ/9-PixelDetector";

  TString tut_geomdir = dir + "/common/geometry";
  gSystem->Setenv("GEOMPATH",tut_geomdir.Data());

  TString tut_configdir = dir + "/common/gconfig";
  gSystem->Setenv("CONFIG_DIR",tut_configdir.Data());

  FairMQSimManager* run = new FairMQSimManager();
  
  run->SetNofEvents       (10000);
  run->SetTransportName   ("TGeant3");
  run->SetMaterials       ("media.geo");

  TObjArray* detArray = new TObjArray();
  FairModule* cave= new FairCave("CAVE");
  cave->SetGeometryFileName("cave_vacuum.geo");
  detArray->Add(cave);
  Pixel*  det = new Pixel("Tut9", kTRUE);
  det->SetGeometryFileName("pixel.geo");
  detArray->Add(det);
  run->SetDetectorArray   (detArray);

  TString partName[] = {"pions","eplus","proton"};
  Int_t   partPdgC[] = {    211,     11,    2212};
  Int_t chosenPart  = 0;

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  FairBoxGenerator* boxGen = new FairBoxGenerator(partPdgC[chosenPart], 5);
  boxGen->SetPRange(1,2);
  boxGen->SetThetaRange(0,40);
  boxGen->SetPhiRange(0,360);
  primGen->AddGenerator(boxGen);
  run->SetGenerator       (primGen);

  run->SetStoreTraj       (false);


  return run;



}
