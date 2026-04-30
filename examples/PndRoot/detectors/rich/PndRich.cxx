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

#include "PndRich.h"

#include "PndRichPDPoint.h"
#include "PndRichBarPoint.h"
#include "PndRichGeo.h"
#include "PndRichGeoPar.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "PndDetectorList.h"
#include "PndStack.h"
#include "TParticle.h"

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "PndGeoHandling.h"

#include "FairGeoRootBuilder.h"
#include "FairGeoMedia.h"
#include "FairGeoLoader.h"
#include "TGeoCompositeShape.h"

#include "G4NistManager.hh"

#include <iostream>
using std::cout;
using std::endl;

PndRich::PndRich()
  : FairDetector("PndRich", kTRUE, static_cast<int>(DetectorId::kRICH)), fTrackID(-1), fVolumeID(-1), fPos(), fMom(), fTime(-1.), fLength(-1.), fELoss(-1), fGeo(new PndRichGeo()),
    fGeoH(nullptr), fUseProtection(kFALSE), fRunCherenkov(kFALSE), //!  Switch ON/OFF Cherenkov propagation
    fPndRichPDPointCollection(new TClonesArray("PndRichPDPoint")), fPndRichBarPointCollection(new TClonesArray("PndRichBarPoint"))
{
  fListOfSensitives.push_back("Sensor");
  if (fVerboseLevel > 0) {
    std::cout << "-I- PndRich: fListOfSensitives contains:";
    for (size_t k = 0; k < fListOfSensitives.size(); k++)
      std::cout << "\n\t" << fListOfSensitives[k];
    std::cout << std::endl;
  }

  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();
  fGeoVersion = 313; // default geometry
}

PndRich::PndRich(const char *name, Bool_t active)
  : FairDetector(name, active, static_cast<int>(DetectorId::kRICH)), fTrackID(-1), fVolumeID(-1), fPos(), fMom(), fTime(-1.), fLength(-1.), fELoss(-1), fGeo(new PndRichGeo()),
    fGeoH(nullptr), fUseProtection(kFALSE), fRunCherenkov(kFALSE), //!  Switch ON/OFF Cherenkov propagation
    fPndRichPDPointCollection(new TClonesArray("PndRichPDPoint")), fPndRichBarPointCollection(new TClonesArray("PndRichBarPoint"))
{
  fListOfSensitives.push_back("Sensor");
  if (fVerboseLevel > 0) {
    std::cout << "-I- PndRich: fListOfSensitives contains:";
    for (size_t k = 0; k < fListOfSensitives.size(); k++)
      std::cout << "\n\t" << fListOfSensitives[k];
    std::cout << std::endl;
  }

  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();
  fGeoVersion = 313; // default geometry
  LOG(info) << " PndRich:";
}

PndRich::~PndRich()
{
  if (fPndRichPDPointCollection) {
    fPndRichPDPointCollection->Delete();
    delete fPndRichPDPointCollection;
  }
  if (fPndRichBarPointCollection) {
    fPndRichBarPointCollection->Delete();
    delete fPndRichBarPointCollection;
  }
}

void PndRich::Initialize()
{
  FairDetector::Initialize();
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
  PndRichGeoPar *par = (PndRichGeoPar *)(rtdb->getContainer("PndRichGeoPar"));
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(), 1);

  if (0 == gGeoManager)
    cout << "We do not have gGeoManager" << endl;
  else
    cout << "there is gGeoManager" << endl;

  cout << "list of sensitives has " << fListOfSensitives.size() << " entries" << endl;
  fGeoH->CreateUniqueSensorId("", fListOfSensitives);
  if (fVerboseLevel > 0)
    fGeoH->PrintSensorNames();
  trackid.clear();

  if (fRunCherenkov == kFALSE)
    cout << " -I- PndRich: Switching OFF Cherenkov Propagation" << endl;

  // define geometry version from name of geometry file
  DefGeoVersion();

  // average refractive index of aerogel
  fGeo->init(fGeoVersion);
  std::vector<Double_t> nopt = fGeo->nOpt();
  if (nopt.size() > 0) {
    fnOpt = 0;
    for (size_t i = 0; i < nopt.size(); i++)
      fnOpt += nopt.at(i);
    fnOpt /= nopt.size();
    std::cout << "Mid ref. index = " << fnOpt << std::endl;
  }

  // aerogel bar entrance z coordinate
  TVector3 richOffset = fGeo->richOffset();
  TVector3 aerogelOffset = fGeo->aerogelOffset();
  TVector3 aerogelSize = fGeo->aerogelSize();
  fZabar = richOffset.Z() + aerogelOffset.Z();

  // pde_dpc3200_22.dat
  std::string workdir(getenv("VMCWORKDIR"));
  std::string effFileName(workdir + "/detectors/rich/pde_dpc3200_22.dat");
  std::ifstream from(effFileName.c_str());
  Double_t wli, pdei;
  from >> wli >> pdei;
  while (!from.eof()) {
    fWlPhoton.push_back(wli); // nm
    fPDE.push_back(pdei);     // %
    from >> wli >> pdei;
  };
}

Bool_t PndRich::ProcessHits(FairVolume *vol)
{
  /** This method is called from the MC stepping */
  TString nam = vol->GetName();
  // Int_t num = vol->getMCid(); //[R.K. 01/2017] unused variable?

  // Set parameters at entrance of volume. Reset ELoss.
  Int_t fEventID = gMC->CurrentEvent();
  Int_t fPdgCode = gMC->TrackPid();
  fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
  fTime = gMC->TrackTime() * 1.0e09;
  fLength = gMC->TrackLength();

  /*  //Get current material
    //if ( nam.BeginsWith("RichPhDetSi") ) {
    if ( nam.BeginsWith("RichProtection") ) {
    //if ( nam.BeginsWith("RichAerogel") ) {
       Float_t a, z, dens, radl, absl;
       Int_t mat = gMC->CurrentMaterial(a,z,dens,radl,absl);
       cout<< nam << " " << mat << endl;
       cout<<"a = " << a << " z = " << z << " dens = " << dens << " radl = " << radl << " absl = " << absl << endl;

       cout<< ">>>>>>>>>>>>>> " <<gGeoManager->GetVolume(vol->getVolumeId())->GetName() <<endl;

       TGeoMaterial *material = gGeoManager->GetVolume(vol->getVolumeId())->GetMaterial();
       //TGeoMaterial *material = gGeoManager->GetMaterial(mat);
       //TGeoMaterial *material = gGeoManager->GetMaterial("RichProtection");
       //TGeoMaterial *material = gGeoManager->GetMaterial("silicon");
       //TGeoMaterial *material = gGeoManager->GetMaterial("RichAerogel0");
       cout<< material->GetName() << endl;
       Int_t nels = material->GetNelements();
       for(Int_t i=0; i<nels; i++){
          TGeoElement *element = material->GetElement(i);
          cout << element->GetName() << endl;
          Int_t nis = element->GetNisotopes();
          cout << "element->GetNisotopes() = " << nis << endl;
          cout << "element->A() = " << element->A()
             << "  element->Z() = " << element->Z()
                << "  element->N() = " << element->N() << endl;
          for(Int_t j=0; j<nis; j++){
             TGeoIsotope *isotope =  element->GetIsotope(j);
             cout << "Isotope: " << j << " " << isotope->GetName()
                << " a=" << isotope->GetA()
                   << " z=" << isotope->GetZ()
                      << " n=" << isotope->GetN() << endl;
          }
       }
    }
    if ( nam.BeginsWith("RichAerogel") || nam.BeginsWith("RichPhDetSi")) {
       TGeoMaterial *material = gGeoManager->GetVolume(vol->getVolumeId()-1)->GetMaterial();
       //cout << nam << " " << "material->GetName() = " << material->GetName() << endl;
       if ( strncmp( material->GetName(), "RichAerogel", strlen("RichAerogel") ) == 0 ) {
          TObject *cherprop = material->GetCerenkovProperties();
          //cout << material->GetName() << " " << cherprop << endl;
          //cout << "cherprop->GetName() = " << cherprop->GetName() << endl;
       }

       //TList *listOfMedia = gGeoManager->GetListOfMedia();
       //TListIter iter(listOfMedia);
       //TGeoMedium* medium   = nullptr;

       //while( (medium = (TGeoMedium*)iter.Next()) ) {
       //   cout << "medium->GetName() = " << medium->GetName() << endl;
       //}
    }
     //TGeoVolume *RichProtection = gGeoManager->GetVolume("RichProtectionSensor0");
     //cout << "RichProtection = " << RichProtection
     //   << " RichProtection->IsActive() = " << RichProtection->IsActive()
     //      << " nam.BeginsWith() = " << nam.BeginsWith("RichProtection") <<  endl;
  */
  gMC->TrackPosition(fPos);
  gMC->TrackMomentum(fMom);

  // Create PndRichPoint at exit of active volume
  if (fPdgCode == 50000050) {
    // Efficiency of photodetector
    Double_t eff = fGeo->phDetQEff(2 * 3.1415927 * 197.3269602e-9 / fMom.Vect().Mag());
    if (fRunCherenkov == kFALSE) {
      if (fVerboseLevel > 0)
        cout << "Photon killed" << endl;
      gMC->StopTrack();
    } else if (nam.BeginsWith("RichPhDetSi") && (gMC->IsTrackEntering() == 1) && (gRandom->Uniform() < eff)) {
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      fVolumeID = vol->getMCid();
      AddPDPoint(fTrackID, fVolumeID, fPos.Vect(), fMom.Vect(), fTime, fLength, fELoss, fEventID);

      // Increment number of PndRich det points in TParticle
      PndStack *stack = (PndStack *)gMC->GetStack();
      stack->AddPoint(DetectorId::kRICH);
      gMC->StopTrack();
    }
  }

  if ((gMC->TrackCharge() != 0) && (fMom.Beta() > 1.00 / fnOpt) && gMC->IsTrackEntering() == 1 && (fPos.Z() < fZabar + 0.001) && (nam.BeginsWith("RichAerogel")) &&
      (trackid[fTrackID] != 1)) {

    trackid[fTrackID] = 1; // register new track

    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    Double_t fMass = gMC->TrackMass();

    Double_t fThetaC = 1 / fnOpt / fMom.Beta(); // Cerenkov angle
    fThetaC = fMom.Beta();                      // beta

    TParticle *particle = gMC->GetStack()->GetCurrentTrack();
    TVector3 pos0 = TVector3(particle->Vx(), particle->Vy(), particle->Vz());
    TVector3 mom0 = TVector3(particle->Px(), particle->Py(), particle->Pz());

    AddBarPoint(fTrackID, fVolumeID, fPos.Vect(), fMom.Vect(), fTime, fLength, fPdgCode, fThetaC, fEventID, fMass, pos0, mom0);

    //      TParticle *particle = gMC->GetStack()->GetCurrentTrack();
    //      cout << "particle: " << particle->Vx() << " "
    //         << particle->Vy() << " " << particle->Vz() << " "
    //         << particle->T() << " "
    //         << particle->Theta() << " " << particle->Phi() << " "
    //         << particle->Px() << " " << particle->Py() << " "
    //         << particle->Pz() << " " << particle->P() <<
    //         endl;
  }

  return kTRUE;
}

void PndRich::FinishRun()
{
  //  C->Delete();
  //  H->Delete();
  //  B->Delete();
  //  B10->Delete();
  //  B11->Delete();
  //  matRcihProt->Delete();
  //  med->Delete();
}

void PndRich::BeginEvent()
{
  /*   TList *listOfMedia = gGeoManager->GetListOfMedia();
     TListIter iter(listOfMedia);
     TGeoMedium* medium   = nullptr;

     while( (medium = (TGeoMedium*)iter.Next()) ) {
        cout << "medium->GetName() = " << medium->GetName() << endl;
     }

     TList *listOfMaterials = gGeoManager->GetListOfMaterials();
     TListIter iter1(listOfMaterials);
     TGeoMaterial* material   = nullptr;

     while( (material = (TGeoMaterial*)iter1.Next()) ) {
        TObject *obj = material->GetCerenkovProperties();
        cout << "material->GetName() = " << material->GetName() <<
           " " << material->GetDensity() <<
           " " << material->GetFWExtension() << endl;
     }

     TObjArray *listOfVolumes = gGeoManager->GetListOfVolumes();
     TGeoVolume* volume   = nullptr;

     cout << "listOfVolumes->GetSize() = " << listOfVolumes->GetSize() << endl;
     for(Int_t i=0; i<listOfVolumes->GetSize(); i++){
        volume = (TGeoVolume*)listOfVolumes->At(i);
        if (volume) {
           medium = volume->GetMedium();
           material = volume->GetMaterial();
           cout << "volume->GetName() = " << volume->GetName() <<
              " " << medium->GetName() << " " << material->GetName() <<endl;
        }
     }

     size_t nm = G4NistManager::Instance()->GetNumberOfElements();
     cout << "GetNumberOfElements = " << nm << endl;
  */
}

void PndRich::EndOfEvent()
{
  fPndRichPDPointCollection->Clear();
  fPndRichBarPointCollection->Clear();
  trackid.clear();
}

void PndRich::DefGeoVersion()
{
  TString fileName = GetGeometryFileName();
  if (fileName.EndsWith(".root") && (fGeoVersion == 313)) {
    char pat[] = "rich_v";
    size_t ind = fileName.Index(pat) + strlen(pat);
    sscanf(fileName(ind, 5).Data(), "%d", &fGeoVersion);
    std::cout << "GetGeometryFileName() = " << fileName << " " << fGeoVersion << std::endl;
  }
}

void PndRich::Register()
{
  /** This will create a branch in the output tree called
      PndRichPoint, setting the last parameter to kFALSE means:
      this collection will not be written to the file, it will exist
      only during the simulation.
  */

  if (fRunCherenkov == kTRUE)
    FairRootManager::Instance()->Register("RichPDPoint", "PndRich", fPndRichPDPointCollection, kTRUE);
  FairRootManager::Instance()->Register("RichBarPoint", "PndRich", fPndRichBarPointCollection, kTRUE);
}

TClonesArray *PndRich::GetCollection(Int_t iColl) const
{
  if (iColl == 0)
    return fPndRichPDPointCollection;
  if (iColl == 1)
    return fPndRichBarPointCollection;
  return nullptr;
}

void PndRich::Reset()
{
  fPndRichPDPointCollection->Clear();
  fPndRichBarPointCollection->Clear();
}

void PndRich::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if (fileName.EndsWith(".root")) {
    ConstructRootGeometry();

    //-------------------------------------------------------------
    /*     if (fUseProtection) {

            Int_t n, z, ncomponents;   //number of nucleon in a isotope
            TString symbol;
            Double_t abundance, a, density;

            C = new TGeoElement("Carbon", "C", z=6, n=12, a=12.0107);
            H = new TGeoElement("Hydrogen", "H", z=1, n=1,  a=1.00794);

            B10 = new TGeoIsotope("B10", z=5,n=10,a=10.0129370);
            B11 = new TGeoIsotope("B11", z=5,n=11,a=11.0093054);

            B = new TGeoElement ("Boron",symbol="B",ncomponents=2);
            B->AddIsotope(B10,19.8);
            B->AddIsotope(B11,80.2);

            matRcihProt = (TGeoMaterial*) ( new TGeoMixture ("RichProtectionMaterial", 3, density=0.95) );
            ((TGeoMixture*)matRcihProt)->AddElement(C,0.813467);
            ((TGeoMixture*)matRcihProt)->AddElement(H,0.136533);
            ((TGeoMixture*)matRcihProt)->AddElement(B,0.05);

            gGeoManager->AddMaterial(matRcihProt);

            med = new TGeoMedium("RichProtectionMedium",1000,matRcihProt);

            TGeoVolume *alRichBoxAir = gGeoManager->GetVolume("RichAlBoxAir");

            // Polyethilene protection
            std::vector<Double_t> rpWidth;
            rpWidth.push_back(5);
            //rpWidth.push_back(1);
            //rpWidth.push_back(1);
            //rpWidth.push_back(1);
            //rpWidth.push_back(1);
            std::vector<Double_t> rpHoleWidth;
            rpHoleWidth.push_back(10);
            //rpHoleWidth.push_back(40);
            //rpHoleWidth.push_back(30);
            //rpHoleWidth.push_back(20);
            //rpHoleWidth.push_back(10);
            UInt_t numberOfLayers = rpWidth.size();
            TString rpUnity = Form("( ");
            std::vector<TGeoBBox*> lRichProtection(numberOfLayers);
            std::vector<TGeoTranslation*> rpTrans(numberOfLayers);
            std::vector<TGeoCompositeShape*> richProtectionCS(numberOfLayers);
            std::vector<TGeoVolume*> richProtection(numberOfLayers);
            std::vector<TGeoBBox*> lRichProtectionHole(numberOfLayers);

            DefGeoVersion();
            fGeo->init(fGeoVersion);
            TVector3 aerogelOffset            = fGeo->aerogelOffset();
            TVector3 alBoxSize                = fGeo->alBoxSize();
            TVector3 aerogelSize              = fGeo->aerogelSize();
            std::vector<Double_t> phDetY      = fGeo->phDetY();

            Double_t zrp = aerogelOffset.Z()-alBoxSize.Z()/2+70;
            for(UInt_t ia=0; ia<numberOfLayers; ia++) {
               Double_t rpThickness = rpWidth[ia];
               zrp += rpThickness/2;

               TString rpName = Form("rp%d",ia);
               TString rpTransName = Form("rpTrans%d",ia);
               TString rpMedia = Form("RichProtectionMaterial");
               TString rpCSName = Form("RichProtectionCS%d",ia);
               TString richProtectionName = Form("RichProtectionSensor%d",ia);
               TString rpHoleName = Form("richProtectionHole%d",ia);

               lRichProtectionHole.at(ia) = new TGeoBBox(rpHoleName,
                                                         rpHoleWidth.at(ia),
                                                         rpHoleWidth.at(ia),
                                                         alBoxSize.Z()/2 );

               rpUnity = "( " + rpName + ":" + rpTransName + " ) - ( " + rpHoleName + " ) ";

               lRichProtection.at(ia) = new TGeoBBox(rpName,
                                                     aerogelSize.X()/2,
                                                     phDetY[1],
                                                     rpThickness/2 );
               rpTrans.at(ia) = new TGeoTranslation(rpTransName,
                                                    aerogelOffset.X(),
                                                    aerogelOffset.Y(),
                                                    zrp);
               rpTrans.at(ia)->RegisterYourself();
               richProtectionCS.at(ia) = new TGeoCompositeShape(rpCSName,rpUnity);
               richProtection.at(ia) = new TGeoVolume(richProtectionName,
                                                      richProtectionCS.at(ia),
                                                      med);
               richProtection.at(ia)->SetLineColor(kCyan);
               richProtection.at(ia)->SetTransparency(40);
               alRichBoxAir->AddNode(richProtection.at(ia), 1, new TGeoCombiTrans( 0, 0, 0, new TGeoRotation(0) ) );
               AddSensitiveVolume(richProtection.at(ia));

               zrp += rpThickness/2;
            }
         }
    */
    //-------------------------------------------------------------

  } else {
    /** If you are using the standard ASCII input for the geometry
        just copy this and use it for your detector, otherwise you can
        implement here you own way of constructing the geometry. */

    FairGeoLoader *geoLoad = FairGeoLoader::Instance();
    FairGeoInterface *geoFace = geoLoad->getGeoInterface();
    PndRichGeo *Geo = new PndRichGeo();
    Geo->setGeomFile(GetGeometryFileName());
    geoFace->addGeoModule(Geo);

    Bool_t rc = geoFace->readSet(Geo);
    if (rc) {
      Geo->create(geoLoad->getGeoBuilder());
    }
    TList *volList = Geo->getListOfVolumes();

    // store geo parameter
    FairRun *fRun = FairRun::Instance();
    FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
    PndRichGeoPar *par = (PndRichGeoPar *)(rtdb->getContainer("PndRichGeoPar"));
    TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
    TObjArray *fPassNodes = par->GetGeoPassiveNodes();

    TListIter iter(volList);
    FairGeoNode *node = nullptr;
    FairGeoVolume *aVol = nullptr;

    while ((node = (FairGeoNode *)iter.Next())) {
      aVol = dynamic_cast<FairGeoVolume *>(node);
      if (node->isSensitive()) {
        fSensNodes->AddLast(aVol);
      } else {
        fPassNodes->AddLast(aVol);
      }
    }
    par->setChanged();
    par->setInputVersion(fRun->GetRunId(), 1);

    ProcessNodes(volList);
  }
}

// https://www.slac.stanford.edu/grp/eg/minos/dist/dist_aux4/geant4_vmc/examples/E06/src/Ex06DetectorConstruction.cxx
// http://personalpages.to.infn.it/~puccio/htmldoc/src/AliHMPIDv2.cxx.html
void PndRich::ConstructOpGeometry()
{
  cout << " ==================================================== " << endl;
  cout << " =======  Rich::  ConstructOpticalGeometry()  ======== " << endl;

  DefGeoVersion();
  // PndRichGeo *fGeo = new PndRichGeo();
  fGeo->init(fGeoVersion);

  // Aerogel optical properties

  std::vector<Double_t> nOpt = fGeo->nOpt();
  UInt_t nAerogelLayers = nOpt.size();
  if (nOpt.size() > 0) {
    fnOpt = 0;
    for (size_t i = 0; i < nOpt.size(); i++)
      fnOpt += nOpt.at(i);
    fnOpt /= nOpt.size();
  }

  Int_t npoints = 10;

  Double_t ephotonMin = 1.240 * 1.0e-09; // 1000 nm - maximum of pde_dpc3200
  Double_t ephotonMax = 4.428 * 1.0e-09; //  280 nm - minimum of pde_dpc3200
  Double_t ephoton[npoints];             // Value of photon momentum (in GeV)

  Double_t absLen[npoints];                 // absorption length in cm
  Double_t qEff[npoints];                   // Detection efficiency for UV photons
  Double_t refInd[nAerogelLayers][npoints]; // Refraction index

  Double_t k = 2 * 3.1415927 * 197.3269602e-9; // coefficient energy to wavelength (hc)

  for (Int_t i = 0; i < npoints; i++) {

    ephoton[i] = ephotonMin + i * (ephotonMax - ephotonMin) / (npoints - 1);
    Double_t wl = k / ephoton[i]; // wavelength in nm

    for (UInt_t l = 0; l < nAerogelLayers; l++)
      refInd[l][i] = lhcbaerindex(nOpt[l], wl);

    absLen[i] = 4.5 * std::pow(wl / 400, 4.0);
    qEff[i] = 0.;
  }

  cout << "fGeoVersion = " << fGeoVersion << endl;
  cout << "nOpt.size() = " << nOpt.size() << endl;
  for (size_t i = 0; i < nOpt.size(); i++) {

    // Int_t mId = gGeoManager->GetVolume( Form("RichAerogelSensor%d",i) )->GetMedium()->GetId();
    Int_t mId = gMC->MediumId(Form("RichAerogel%d", (int)i));

    gMC->SetCerenkov(mId, npoints, ephoton, absLen, qEff, refInd[i]);
  }

  // optical properties of air, photodetector window, mirror surface
  Int_t npoints_i = 2;

  Double_t ephoton_i[npoints_i];
  ephoton_i[0] = 1.240 * 1.0e-09; // 1 eV
  ephoton_i[1] = 4.428 * 1.0e-09; // 10 eV

  Double_t reflectivity_i[npoints_i];
  reflectivity_i[0] = 0.9;
  reflectivity_i[1] = 0.9;

  Double_t abs_i[npoints_i];
  abs_i[0] = 1000.;
  abs_i[1] = 1000.;

  Double_t pdWindowRefInd[npoints_i];
  pdWindowRefInd[0] = 1.46;
  pdWindowRefInd[1] = 1.46;

  Double_t airRefInd[npoints_i];
  airRefInd[0] = 1.0;
  airRefInd[1] = 1.0;

  Double_t qEff_i[npoints_i];
  qEff_i[0] = 0.;
  qEff_i[1] = 0.;

  gMC->SetCerenkov(gMC->MediumId("RichAir"), npoints_i, ephoton_i, abs_i, qEff_i, airRefInd);
  gMC->SetCerenkov(gMC->MediumId("RichPDWindow"), npoints_i, ephoton_i, abs_i, qEff_i, pdWindowRefInd);

  gMC->DefineOpSurface("RichMirrSurface", kGlisur, kDielectric_metal, kPolished, 0.0);
  gMC->SetMaterialProperty("RichMirrSurface", "REFLECTIVITY", npoints_i, ephoton_i, reflectivity_i);
  gMC->SetBorderSurface("BarRichMirrorSurface", "RichMirror", 1, "RichAlBoxAir", 1, "RichMirrSurface");
  gMC->SetSkinSurface("RichAirMirrorSurface", "RichMirror", "RichMirrSurface");

  //  gMC->SetBorderSurface("BarRichMirrorLeftSurface", "RichMirrorLeft", 1, "RichAlBoxAir", 1, "RichMirrSurface");
  //  gMC->SetSkinSurface("RichAirMirrorLeftSurface", "RichMirrorLeft", "RichMirrSurface");
  //  gMC->SetBorderSurface("BarRichMirrorRightSurface", "RichMirrorRight", 1, "RichAlBoxAir", 1, "RichMirrSurface");
  //  gMC->SetSkinSurface("RichAirMirrorRightSurface", "RichMirrorRight", "RichMirrSurface");

  cout << " =======  RICH::ConstructOpGeometry -> Finished! ====== " << endl;
}

/*Refractive index as function of wavelength
 *     for aerogel of the nominal ref. ind. at 400nm.
 *   Scaled from LHCb data:
 *   T. Bellunato et al., "Refractive index dispersion law of silica aerogel",
 *   Eur. Phys. J. C 52 (2007) 759-764
 * */
Double_t PndRich::lhcbaerindex(Double_t n400, Double_t wl)
{
  const double LHCb_a0 = 0.05639, LHCb_wl0sqr = 83.22 * 83.22;
  double LHCb_RI2m1ref = LHCb_a0 / (1 - LHCb_wl0sqr / (400 * 400)); //(n**2-1) of LHCb aerogel at 400nm
  double ri2m1_lhcb = LHCb_a0 / (1 - LHCb_wl0sqr / (wl * wl));      //(n**2-1) of LHCb aerogel at wl
  return sqrt(1 + (n400 * n400 - 1) / LHCb_RI2m1ref * ri2m1_lhcb);
}

// -----   Public method CheckIfSensitive   --------------------------------------
bool PndRich::CheckIfSensitive(std::string name)
{
  for (size_t i = 0; i < fListOfSensitives.size(); i++) {
    if (name.find(fListOfSensitives[i]) != std::string::npos)
      return true;
  }
  return false;
}

PndRichPDPoint *PndRich::AddPDPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss, UInt_t EventId)
{
  TClonesArray &clref = *fPndRichPDPointCollection;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndRichPDPoint(trackID, detID, pos, mom, time, length, eLoss, EventId);
}

PndRichBarPoint *PndRich::AddBarPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Int_t pdgCode, Double_t thetaC, Int_t eventID,
                                      Double_t mass, TVector3 pos0, TVector3 mom0)
{
  TClonesArray &clref = *fPndRichBarPointCollection;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndRichBarPoint(trackID, detID, pos, mom, time, length, pdgCode, thetaC, eventID, mass, pos0, mom0);
}

ClassImp(PndRich)
