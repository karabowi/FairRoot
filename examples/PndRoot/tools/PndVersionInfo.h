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
 * @class PndVersionInfo
 *
 * @brief Handles Git and Version information
 * @details
 * Fetches git information from cmake-generated <build>/PandaVersion.h
 *
 * @author Ralf Kliemt <r.kliemt@gsi.de>
 */


#include <Rtypes.h>
#include <string>

class PndVersionInfo {
 public:
  PndVersionInfo();
  ~PndVersionInfo() {}
  void Print();

  std::string GetRev() { return fGitRev; }
  std::string GetTag() { return fGitTag; }
  std::string GetBranch() { return fGitBranch; }

 protected:
  std::string fGitRev;    // commit hash, ends with "+" if there are local changes
  std::string fGitTag;    // commit Tag
  std::string fGitBranch; // current branch

  // ClassDef(PndVersionInfo, 1)
};
