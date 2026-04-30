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

// -------------------------------------------------------------------------
// -----                      PndMultiField source file                -----
// -----                 Created 29/01/07  by M. Al/Turany             -----
// -------------------------------------------------------------------------

#include "PndMultiField.h"
#include "PndConstField.h"
#include "PndFieldMap.h"
#include "PndMapPar.h"
#include "PndMultiFieldPar.h"
#include "PndSolenoidMap.h"
#include "PndTransMap.h"
#include "PndDipoleMap.h"

#include "FairRunSim.h"
#include "FairRuntimeDb.h"

#include "TObjArray.h"

#include <iomanip>
#include <iostream>
#include <fstream>

using namespace std;

// -------------   Default constructor  ----------------------------------
PndMultiField::PndMultiField() : fMaps(new TObjArray(10)), fNoOfMaps(0), fFieldMaps(), fBeamMom(0.)
{
  fType = 5;
}

// -------------   Default constructor  ----------------------------------
PndMultiField::PndMultiField(TString Map, Double_t BeamMom) : fMaps(new TObjArray(10)), fNoOfMaps(0), fFieldMaps(), fBeamMom(BeamMom)
{
  // Here we actually select which field pieces are being used. The available Keywords
  // are "AUTO", "FULL", "HALF", "DIPLOE", "SOLENOID", "SOLENOID_HALF"
  fType = 5;
  if (BeamMom < 0.) {
    FairRunSim *fRun = FairRunSim::Instance();
    if (fRun)
      fBeamMom = fRun->GetBeamMom();
  }

  Map.ToUpper();

  if (Map == "FULL") {

    PndTransMap *map_t = new PndTransMap("TransMap", "R", fBeamMom);
    PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1", "R", fBeamMom);
    PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2", "R", fBeamMom);
    PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap1", "R");
    PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap2", "R");
    PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap3", "R");
    PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap4", "R");

    // The order is important here. First map to cover a point in z will be selected.
    AddField(map_t);
    AddField(map_d1);
    AddField(map_d2);
    AddField(map_s1);
    AddField(map_s2);
    AddField(map_s3);
    AddField(map_s4);

  }

  else if (Map == "COMPACT") {
    PndDipoleMap *map_d1 = new PndDipoleMap("CompactDipoleMap1", "R", fBeamMom);
    PndDipoleMap *map_d2 = new PndDipoleMap("CompactDipoleMap2", "R", fBeamMom);

    PndTransMap *map_t{nullptr};
    PndSolenoidMap *map_s1{nullptr};
    PndSolenoidMap *map_s2{nullptr};
    PndSolenoidMap *map_s3{nullptr};
    PndSolenoidMap *map_s4{nullptr};

    if (fBeamMom >= 3.0) {
      map_t = new PndTransMap("CompactTransMap", "R", fBeamMom);
      map_s1 = new PndSolenoidMap("CompactSolenoidMap1", "R");
      map_s2 = new PndSolenoidMap("CompactSolenoidMap2", "R");
      map_s3 = new PndSolenoidMap("CompactSolenoidMap3", "R");
      map_s4 = new PndSolenoidMap("CompactSolenoidMap4", "R");
    } else {
      map_t = new PndTransMap("CompactTransMap_Half", "R", fBeamMom);
      map_s1 = new PndSolenoidMap("CompactSolenoidMap_Half1", "R");
      map_s2 = new PndSolenoidMap("CompactSolenoidMap_Half2", "R");
      map_s3 = new PndSolenoidMap("CompactSolenoidMap_Half3", "R");
      map_s4 = new PndSolenoidMap("CompactSolenoidMap_Half4", "R");
    }

    // The order is important here. First map to cover a point in z will be selected.
    AddField(map_t);
    AddField(map_d1);
    AddField(map_d2);
    AddField(map_s1);
    AddField(map_s2);
    AddField(map_s3);
    AddField(map_s4);

  }

  else if (Map == "AUTO") {
    PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1", "R", fBeamMom);
    PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2", "R", fBeamMom);

    PndTransMap *map_t;
    PndSolenoidMap *map_s1;
    PndSolenoidMap *map_s2;
    PndSolenoidMap *map_s3;
    PndSolenoidMap *map_s4;

    if (fBeamMom >= 3.0) {
      map_t = new PndTransMap("TransMap", "R", fBeamMom);
      map_s1 = new PndSolenoidMap("SolenoidMap1", "R");
      map_s2 = new PndSolenoidMap("SolenoidMap2", "R");
      map_s3 = new PndSolenoidMap("SolenoidMap3", "R");
      map_s4 = new PndSolenoidMap("SolenoidMap4", "R");
    } else {
      map_t = new PndTransMap("TransMap_Half", "R", fBeamMom);
      map_s1 = new PndSolenoidMap("SolenoidMap_Half1", "R");
      map_s2 = new PndSolenoidMap("SolenoidMap_Half2", "R");
      map_s3 = new PndSolenoidMap("SolenoidMap_Half3", "R");
      map_s4 = new PndSolenoidMap("SolenoidMap_Half4", "R");
    }

    // The order is important here. First map to cover a point in z will be selected.
    AddField(map_t);
    AddField(map_d1);
    AddField(map_d2);
    AddField(map_s1);
    AddField(map_s2);
    AddField(map_s3);
    AddField(map_s4);
  }

  else if (Map == "HALF") {
    PndTransMap *map_t = new PndTransMap("TransMap_Half", "R", fBeamMom);
    PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1", "R", fBeamMom);
    PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2", "R", fBeamMom);
    PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap_Half1", "R");
    PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap_Half2", "R");
    PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap_Half3", "R");
    PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap_Half4", "R");

    // The order is important here. First map to cover a point in z will be selected.
    AddField(map_t);
    AddField(map_d1);
    AddField(map_d2);
    AddField(map_s1);
    AddField(map_s2);
    AddField(map_s3);
    AddField(map_s4);

  }

  else if (Map == "DIPOLE") {
    PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1", "R", fBeamMom);
    PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2", "R", fBeamMom);

    // The order is important here. First map to cover a point in z will be selected.
    AddField(map_d1);
    AddField(map_d2);

  } else if (Map == "SOLENOID") {

    PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap1", "R");
    PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap2", "R");
    PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap3", "R");
    PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap4", "R");

    // The order is important here. First map to cover a point in z will be selected.
    AddField(map_s1);
    AddField(map_s2);
    AddField(map_s3);
    AddField(map_s4);
  } else if (Map == "SOLENOID_HALF") {

    PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap_Half1", "R");
    PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap_Half2", "R");
    PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap_Half3", "R");
    PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap_Half4", "R");

    // The order is important here. First map to cover a point in z will be selected.
    AddField(map_s1);
    AddField(map_s2);
    AddField(map_s3);
    AddField(map_s4);
  }
}

// ------------------------------------------------------------------------

// ------------   Constructor from PndFieldPar   --------------------------
PndMultiField::PndMultiField(PndMultiFieldPar *fieldPar) : fMaps(new TObjArray(10)), fNoOfMaps(0), fFieldMaps(), fBeamMom(0.)
{
  fType = 5;
  TObjArray *fArray = fieldPar->GetParArray();
  if (fArray->IsEmpty())
    fType = -1;
}

// ------------   Copy Constructor ----------------------------------------
PndMultiField::PndMultiField(const PndMultiField &field)
    : FairField()
    , fMaps(nullptr)
    , fNoOfMaps(field.fNoOfMaps)
    , fFieldMaps(field.fFieldMaps)
    , fBeamMom(field.fBeamMom)
{
    fMaps = new TObjArray(field.fMaps->GetSize());
    for (Int_t i = 0; i < field.fMaps->GetEntriesFast(); i++) {
        fMaps->Add(field.fMaps->At(i)); 
    }
    fMaps->SetOwner(kFALSE); 

    fType = field.fType;
}

// ------------   Destructor   --------------------------------------------
PndMultiField::~PndMultiField()
{

  // printf("PndMultiField::~PndMultiField() \n");
  fMaps->Delete();
  delete fMaps;
}

// -----------   Adding fields   ------------------------------------------
void PndMultiField::AddField(FairField *field)
{
    if (field) {
        std::lock_guard<std::mutex> lock(fMapsMutex);
        fMaps->AddLast(field);
        fNoOfMaps++;
    }
}

// -----------   Intialisation   ------------------------------------------
void PndMultiField::Init()
{
    std::unique_lock<std::shared_mutex> lock(fFieldMapsMutex);

    fFieldMaps.clear();
    
    for (Int_t n = 0; n < fNoOfMaps; n++) {
        PndFieldMap   *fieldMap = dynamic_cast<PndFieldMap   *>(fMaps->At(n));
        PndConstField *field    = dynamic_cast<PndConstField *>(fMaps->At(n));

        if (fieldMap) {
            fieldMap->Init();
            fFieldMaps.push_back({
                {fieldMap->GetZmin(), fieldMap->GetZmax()}, fieldMap
            });
        } else if (field) {
            field->Init();
            // Fix: use field->GetZmin/Zmax, not the null fieldMap pointer
            fFieldMaps.push_back({
                {field->GetZmin(), field->GetZmax()}, field
            });
        }
    }
    fInitialized.store(true, std::memory_order_release);
}

// ---------   Screen output   --------------------------------------------
void PndMultiField::Print()
{
  for (Int_t n = 0; n < fNoOfMaps; n++) {
    FairField *fieldMap = dynamic_cast<FairField *>(fMaps->At(n));
    if (fieldMap)
      fieldMap->Print("");
  }
}

// ---------   Fill the parameters   --------------------------------------------
void PndMultiField::FillParContainer()
{
  // for (Int_t n=0; n<=fNoOfMaps; n++){
  //      FairField *fieldMap = dynamic_cast<FairField *>(fMaps->At(n));
  //      if(fieldMap) fieldMap->FillParContainer();
  //  }
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
  // Bool_t kParameterMerged=kTRUE;
  PndMultiFieldPar *Par = (PndMultiFieldPar *)rtdb->getContainer("PndMultiFieldPar");
  Par->SetParameters(this);
  Par->setInputVersion(fRun->GetRunId(), 1);
  Par->setChanged();
}

// -------------------------------------------------------------------------
void PndMultiField::GetFieldValue(const Double_t point[3], Double_t *bField)
{
    if (!fInitialized.load(std::memory_order_acquire)) {
        bField[0] = bField[1] = bField[2] = 0;
        return;
    }

    std::shared_lock<std::shared_mutex> lock(fFieldMapsMutex);

    for (const auto &entry : fFieldMaps) {
        if (point[2] >= entry.first.first && point[2] <= entry.first.second) {
            FairField *field = dynamic_cast<FairField *>(entry.second);
            if (field) {
                field->GetBxyz(point, bField);
                //                LOG(info) << "B(" << point[0] << "," << point[1] << "," << point[2] << ") = (" << bField[0] << "," << bField[1] << "," << bField[2] << ")";
                return;
            }
        }
    }
    bField[0] = bField[1] = bField[2] = 0;
}

Double_t PndMultiField::GetBz(Double_t x, Double_t y, Double_t z)
{
    if (!fInitialized.load(std::memory_order_acquire))
        return 0;

    std::shared_lock<std::shared_mutex> lock(fFieldMapsMutex);

    for (const auto &entry : fFieldMaps) {
        if (z >= entry.first.first && z <= entry.first.second) {
            FairField *field = dynamic_cast<FairField *>(entry.second);
            if (field)
                return field->GetBz(x, y, z);
        }
    }
    return 0;
}

FairField* PndMultiField::CloneField() const
{
    return new PndMultiField(*this);
}

ClassImp(PndMultiField)
