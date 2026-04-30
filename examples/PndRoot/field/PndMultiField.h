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
// -----                      PndMultiField header file                  -----
// -----                 Created 29/01/07  by M. Al/Turany               -----
// -------------------------------------------------------------------------
/** PndMultiField.h
 ** @author M.Al/Turany <m.al-turany@gsi.de>
 ** @since 29.01.2007
 ** @version1.0
 **
 ** Multiple Magnetic field maps
 **/

#ifndef PNDMULTIMAP_H
#define PNDMULTIMAP_H 1

#include "FairField.h"
#include "TObjArray.h"
#include <map>

#include <shared_mutex>
#include <atomic>

class PndMultiFieldPar;
class PndMultiField : public FairField {

 public:
  /** Default constructor **/
  PndMultiField();

  PndMultiField(TString Map, Double_t BeamMom = -1.);

  /** Constructor from PndFieldPar **/
  PndMultiField(PndMultiFieldPar *fieldPar);

  /** Destructor **/
  virtual ~PndMultiField();

  /** Initialisation (read map from file) **/
  void Init();

  /**Adding a field to the collection*/

  void AddField(FairField *field);

  TObjArray *GetFieldList() { return fMaps; }

  void FillParContainer();
  /** Screen output **/
  virtual void Print();
  ClassDef(PndMultiField, 2)

  Double_t GetBz(Double_t x, Double_t y, Double_t z);

  /** Get magnetic field. For use of GEANT3
   ** @param point            Coordinates [cm]
   ** @param bField (return)  Field components [kG]
   **/
  void GetFieldValue(const Double_t point[3], Double_t *bField);

    FairField* CloneField() const override;

    
 private:
  PndMultiField(const PndMultiField &field);
  PndMultiField &operator=(const PndMultiField &) { return *this; };

    mutable std::shared_mutex fFieldMapsMutex;
    std::atomic<bool>         fInitialized{false};
    mutable std::mutex fMapsMutex;   // guards fMaps and fNoOfMaps

 protected:
  TObjArray *fMaps;
  Int_t fNoOfMaps;
  std::vector<std::pair<std::pair<double, double>, FairField *>> fFieldMaps; //!
  Double_t fBeamMom;
};

#endif
