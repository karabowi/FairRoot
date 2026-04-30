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
// -----                 PndMvdConvertApvTask header file              -----
// -----                  Created 12/01/09  by L.Ackermann             -----
// -------------------------------------------------------------------------

/** PndMvdConvertApvTask.h
 *@author L.Ackermann <lars.ackermann@physik.tu-dresden.de>,
 *        R.Kliemt <ralf.kliemt@hiskp.uni-bonn.de>
 **
 ** Converting Task from ascci hit file to clones array of PndSdsDigiStrip
 **/

#ifndef PNDMVDCONVERTAPVTASK_H
#define PNDMVDCONVERTAPVTASK_H

// framework includes
#include "FairTask.h"
#include "PndMvdConvertApv.h"
#include "PndMvdBoxMap.h"
#include "PndGeoHandling.h"

#include <vector>
#include <map>

class TClonesArray;
class PndSdsDigiStrip;

/**
 @class PndMvdConvertApvTask : public FairTask
 @brief Convert Task of data

 Task to convert data from real measurement with strip sensors
 @author Lars Ackermann
 @date 11.03.2009
*/
class PndMvdConvertApvTask : public FairTask {
 public:
  /** Default constructor **/
  PndMvdConvertApvTask(PndMvdConvertApv *Apvconvert, PndMvdBoxMap *Apvmapper);

  /** Destructor **/
  ~PndMvdConvertApvTask();

  PndMvdConvertApvTask(const PndMvdConvertApvTask &) = delete;
  PndMvdConvertApvTask &operator=(const PndMvdConvertApvTask &) = delete;

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void SetParContainers();
  /**
  @fn virtual InitStatus Init()
  init task
  */
  virtual InitStatus Init();

  /**
  @fn virtual void Finish()
  finish task
  @return void
  */
  virtual void Finish();

  void SetPersistance(Bool_t p = kTRUE) { fPersistance = p; };
  Bool_t GetPersistance() { return fPersistance; };

 private:
  Bool_t fPersistance; // switch to turn on/off storing the arrays to a file

  /// class which convert the read in and hold the streams
  PndMvdConvertApv *fApvConvert;
  PndMvdBoxMap *fApvMapper;
  /// array of results
  TClonesArray *fStripArray;
  /// Geohandler for name string compression
  PndGeoHandling *fGeoH;
  /// number of strips in this event
  Int_t iStrip;
  TList *fDigiParameterList;
  std::map<TString, Int_t> fBotSides;
  Bool_t IsSingleSided(TString &detpath);
  Int_t CalcBotFakeFE(TString detpath);

  /**
  @fn void Register()
  @return void
  */
  void Register();

  /**
  @fn void Reset()
  @return void
  */
  void Reset();

  ClassDef(PndMvdConvertApvTask, 1);
};

#endif
