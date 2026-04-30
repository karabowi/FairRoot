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

/**
 * @class PndMasterRunAna
 * @brief Class for the master reconstruction chain
 * @details
 * # Master Tasks Class
 * This class is the basic for all the reconstruction steps, digi, reco, pid and so on.
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 1, 2016
 **
 **/

#ifndef PNDMASTERRUNANA_H
#define PNDMASTERRUNANA_H

#include "FairRunAna.h"
#include "FairTask.h"
#include "TStopwatch.h"
#include <vector>

class PndMasterRunAna : public FairRunAna {
 public:
  /**
   * @brief Default constructor
   */
  PndMasterRunAna();

  /**
   * @brief Default destructor
   */
  virtual ~PndMasterRunAna();

  /**
   * @brief Initial setup
   * @details
   * This command set the source files, load the proper parameters,
   * and set the relevant flags. If something fails, it returns
   * a kFALSE value.
   */
  Bool_t Setup(TString outprefix = "");

  /**
   * @brief Final diagnostics
   * @details
   * It prints CPU time, memory usage, used parameters, and eventually
   * send the information to CDash
   */
  void Finish();

  /**
   * @brief Add digitization and local reconstruction tasks
   * @details
   * @param pers Persistency of the TCAs, used only to switch OFF
   * It calls PndMasterDigiTask, adding all the standard digitization tasks
   */
  void AddDigiTasks(Bool_t pers = kTRUE);

  /**
   * @brief Add digitization tasks without reconstruction
   * @details
   * @param pers Persistency of the TCAs, used only to switch OFF
   * It calls PndMasterDigiTask, adding all the standard digitization tasks
   */
  void AddDigiOnlyTasks(Bool_t pers = kTRUE);

  /**
   * @brief Add reconstruction tasks
   * @details
   * @param pers Persistency of the TCAs, used only to switch OFF
   * It calls PndMasterRecoTask, adding all the standard reconstruction tasks
   */
  void AddRecoTasks(Bool_t pers = kTRUE);

  /**
   * @brief Add local reconstruction tasks
   * @details
   * @param pers Persistency of the TCAs, used only to switch OFF
   * It calls PndMasterLocalRecoTask, adding all the standard reconstruction tasks
   */
  void AddLocalRecoTasks(Bool_t pers = kTRUE);

  /**
   * @brief Add ideal pattern reconstruction tasks
   * @details
   * @param pers Persistency of the TCAs, used only to switch OFF
   * It calls PndMasterRecoTask, adding all the standard reconstruction tasks
   */
  void AddRecoIdealTasks(Bool_t pers = kTRUE);

  /**
   * @brief Add pid tasks
   * @details
   * @param pers Persistency of the TCAs, used only to switch OFF
   * It calls PndMasterPidTask, adding all the standard pid tasks
   */
  void AddPidTasks(Bool_t pers = kTRUE);

  /**
   * @brief Input of the macro
   */
  void SetInput(TString par) { fInput = par; }

  /**
   * @brief Tag of the output file of the macro
   */
  void SetOutput(TString par) { fOutFile = par; }

  /**
   * @brief Setter of the parameter root file
   */
  void SetParamRootFile(TString par) { fParamRootFile = par; }

  /**
   * @brief Setter of the parameter ascii file
   */
  void SetParamAsciiFile(TString par) { fParamAsciiFile = par; }

  /**
   * @brief Setter of friend root files
   */
  void AddFriend(TString par)
  {
    if (par != "")
      fFriendFiles.push_back(par);
  }

  /**
   * @brief Setter of the reconstruction options
   * @details
   * @param par = ""     -> default settings
   * @param par = "day1" -> Setup for day1 experimentent, no GEM
   * @param par = "day1+GEM" -> Setup for day1 experimentent, 3 GEM planes
   */
  void SetOptions(TString par)
  {
    fOptions = par;
    fOptions.ToLower();
  }
  TString GetOptions() { return fOptions; }

  void PrintListOfTasks(TList *list, TString prefix = " - ");
  void PrintListOfTasks() { PrintListOfTasks(GetMainTask()->GetListOfTasks()); }

  /**
   * @brief Setter of the event counter rate
   */
  void SetEventCounterRate(Int_t par) { fEventCounterRate = par; }

  void GenerateRunInfo(Bool_t val) { fGenerateRunInfo = val; }

  void UseFairLinks(Bool_t val) { fUseFairLinks = val; }

 private:
  TString fInput;                    ///< Name of the input for the simulation
  TString fOutFile;                  ///< Name of the output file
  TString fParamRootFile;            ///< Name of the parameter root file
  TString fParamAsciiFile;           ///< Name of the parameter ascii file
  std::vector<TString> fFriendFiles; ///< Name of friend files
  TString fOptions;                  ///< Options parsed to the reconstruction

  Int_t fEventCounterRate; ///< After how many events the counter will print
  Bool_t fNoGeane;         ///< Protect GEANE from being loaded twice
  Bool_t fGenerateRunInfo; ///< Switches on/off the generation of Run info
  Bool_t fUseFairLinks;    ///< Switches on/off the usage of FairLinks

  TStopwatch fTimer; ///< Timer

  /** @cond CLASSIMP */
  ClassDef(PndMasterRunAna, 2); ///< 1st Implementation -> 1; day options -> 2
                                /** @endcond */
};

#endif /* PNDMASTERRUNANA_H */
