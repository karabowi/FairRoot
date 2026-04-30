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

// --------------------------------------------------------
// ----     PndMvdConvertApv header file                   ---
// ----     Created 06.01.09 by Lars Ackermann          ---
// ----     on the basic of the ana tool of dts         ---
// --------------------------------------------------------

/** PndMvdConvertApv.h
 *@author L.Ackermann <lars.ackermann@physik.tu-dresden.de>
 **/

#ifndef PNDMVDCONVERTAPV_H
#define PNDMVDCONVERTAPV_H

#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TString.h"
#include "TObject.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include "PndMvdApvHit.h"
#include "PndGeoHandling.h"

// class PndMvdApv;
#include "PndSdsDigiStrip.h"

#include "PndMvdTsEvent.h"

/**
 @class PndMvdConvertApv
 @brief Convert meassured data into PndSdsDigiStrip

 Load calibration parameter and keeps strean to meassured data. Event wise read in and converting to PndSdsDigiStrip
 @author Lars Ackermann
 @date 11.03.2009
*/
class PndMvdConvertApv {
 public:
  /** default constructor **/
  PndMvdConvertApv()
    : fCalibPars(), fNofEvents(0), fEvent(-1), fLastEvent(0), fNoCalib(kFALSE), fHitFileName(""), fDataFile(), fhitlist(), fTopModuleID(0), fBottomModuleID(0), fFake(kFALSE),
      f(nullptr), t(nullptr), tsEv(nullptr), arr(nullptr), fGeoH(nullptr){};

  /**
  main constructor, call all function to be ready for converting hits from hitfile
  @param CalibFileName name of file where the calibration is stored
  @param HitFileName name of hitfile
  */
  PndMvdConvertApv(const TString &CalibFileName, const TString &HitFileName);

  /** Destructor **/
  virtual ~PndMvdConvertApv() { fDataFile.close(); };

  PndMvdConvertApv(const PndMvdConvertApv &) = delete;

  PndMvdConvertApv &operator=(const PndMvdConvertApv &) = delete;

  /**
  @fn long int GetNofEvents()

  @return long integer of last eventID from hitfile
  */
  long int GetNofEvents();

  /**
  @fn std::vector<PndSdsDigiStrip> ReadAll()

  read all events from hitfile
  @return vector of PndSdsDigiStrip of the event
  */
  std::vector<PndSdsDigiStrip> ReadAll();

  /**
  @fn std::vector<PndSdsDigiStrip> ReadNext()

  read the next event from hitfile
  @return vector of PndSdsDigiStrip of the event
  */
  std::vector<PndSdsDigiStrip> ReadNext();

  /**
  @fn Bool_t Init()
  Initialize global geometry
  @return success
  */
  Bool_t Init();

  /**
  @fn void SetFakePair(Int_t TopModuleID, Int_t BottomModuleID)
  Set two moduleIDs to merge them as one double sided sensor
  @param TopModuleID moduleID for top side of fake sensor
  @param BottomModuleID moduleID for bottom side of fake sensor
  @return void
  */
  void SetFakePair(Int_t TopModuleID, Int_t BottomModuleID);

 private:
  /**
  @fn void ModulChecker(Int_t moduleID, std::vector<Int_t>& modules)

  check the vector of modules if the moduleID is known, if not mind it
  @param moduleID moduleID which could be unknown
  @param modules vector of moduleIDs of modules in the hitfile
  @return void
  */

  /**
  @fn void LoadCalibration(TString CalibFileName, std::vector<Int_t> modules)

  Read the calibration of the modules
  @param CalibFileName Filename where the calibration of the modules are listed
  @param modules vector of moduleIDs of modules in the hitfile
  @return void
  @Todo load calibration from database
  */
  // void LoadCalibration(TString CalibFileName, std::vector<Int_t> modules);
  void LoadCalibration(TString CalibFileName);
  /**
  @fn std::vector<PndSdsDigiStrip> Calc(std::vector<PndMvdApvHit> hitlist)

  Convert the readed hit and store them in the PndSdsDigiStrip
  @param hitlist vector of PndMvdApvHit hits of one event from hitfile
  @return list of converted PndSdsDigiStrip
  */
  std::vector<PndSdsDigiStrip> Calc(std::vector<PndMvdApvHit> hitlist);

  /// Calib Parameters: <moduleID<FE<channel> > >
  // 	std::vector<std::map<Int_t, std::vector<double> > > fCalibPars;
  std::map<Int_t, std::map<Int_t, double>> fCalibPars;

  /// stored last eventID of the hitfile
  long int fNofEvents;

  /// stored current eventID
  long int fEvent;

  /// last eventID
  long int fLastEvent;

  /// true if calib was succesfull loaded
  bool fNoCalib;

  /// name of hitfile
  TString fHitFileName;

  /// fstream of hitfile
  std::ifstream fDataFile;

  /// stored readed events
  std::vector<PndMvdApvHit> fhitlist;

  /// stored fake module top side
  Int_t fTopModuleID;

  /// stored fake module bottom side
  Int_t fBottomModuleID;

  /// knows if fake is allowed
  Bool_t fFake;

  TFile *f;
  TTree *t;
  PndMvdTsEvent *tsEv;

  TClonesArray *arr;

  PndGeoHandling *fGeoH; //! Gives Access to the Path info of a hit

  ClassDef(PndMvdConvertApv, 1);
};

#endif
