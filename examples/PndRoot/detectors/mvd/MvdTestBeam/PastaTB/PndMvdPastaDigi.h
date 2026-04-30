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

/*
 * PndMvdPastaDigi.h
 *
 *  Created on: 23.09.2017
 *      Author: Stockmanns
 */

#ifndef MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTADIGIFULL_H_
#define MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTADIGIFULL_H_

#include "Rtypes.h"
#include "TObject.h"

#include <ostream>

class RunSummary : public TObject {
 public:
  RunSummary();
  ~RunSummary(){};
  int fCrcMatchCount;                       ///< count of all frames with correct CRC
  int fCrcErrorCount;                       ///< count of all frames with wrong CRC
  int fSingleWordFrames;                    ///< count of all frames with just one word. This is an error a frame has at least two words
  std::vector<ULong64_t> fAllCountedFrames; ///< counts all frames within a partial reset
  int fAllPartialResets;                    ///< count of all partial resets detected

  int fWrongHitCount;   ///< count of all frames where the hits in the header do not match the data
  int fWrongFrameCount; ///< count of all frames where the expected frame ID did not match the frameID in the header
  int fMissingFrames;   ///< count of all missing frames (not very reliable)
  int fSuperFrameCount; ///< count of frame counter overflow (should not happen because the frame counter is HUGE

  ClassDef(RunSummary, 1);
};

class ThresholdDataFullMode : public TObject {
 public:
  ThresholdDataFullMode();
  ~ThresholdDataFullMode(){};
  int t_coarse;
  int t_soc;
  int t_eoc;
  int channelId;
  int tacId;

  friend std::ostream &operator<<(std::ostream &out, const ThresholdDataFullMode &data)
  {
    out << "t_coarse: " << data.t_coarse << " t_soc " << data.t_soc << " t_eoc " << data.t_eoc << " channelId " << data.channelId << " tacId " << data.tacId;
    return out;
  }
  ClassDef(ThresholdDataFullMode, 1);
};

class FrameHeader : public TObject {
 public:
  FrameHeader();
  ~FrameHeader(){};
  ULong64_t frameId;
  int nEvents;
  ClassDef(FrameHeader, 1);
};

class PndMvdPastaDigi : public TObject {
 public:
  enum branchId { TIME, ENERGY, UNDEFINED };

  PndMvdPastaDigi();
  PndMvdPastaDigi(FrameHeader header, ThresholdDataFullMode time, ThresholdDataFullMode energy) : fFrameHeader(header), fTimeBranch(time), fEnergyBranch(energy)
  {
    fToT = CalcTot();
    fTimeStamp = CalcTimeStamp();
  };

  virtual ~PndMvdPastaDigi();

  void SetTimeBranch(ThresholdDataFullMode time) { fTimeBranch = time; }
  void SetEnergyBranch(ThresholdDataFullMode energy) { fEnergyBranch = energy; }
  void SetHeader(FrameHeader header) { fFrameHeader = header; }

  ULong64_t CalcTimeStamp()
  {
    ULong64_t timeStamp = fFrameHeader.frameId * 1024 + fTimeBranch.t_coarse;
    fTimeStamp = timeStamp;
    return timeStamp;
  }

  int CalcTot()
  {
    int tot = fEnergyBranch.t_coarse - fTimeBranch.t_coarse;

    if (tot < 0) {
      tot += 1024;
    }
    fToT = tot;
    return tot;
  }

  int GetTot() { return fToT; }

  ULong64_t GetTimeStamp() { return fTimeStamp; };
  int GetChannelId(branchId id);

  friend std::ostream &operator<<(std::ostream &out, const PndMvdPastaDigi &digi)
  {
    out << std::dec << " Header: nHits " << digi.fFrameHeader.nEvents << " frameId: " << digi.fFrameHeader.frameId << std::endl;
    out << "TimeBranch: " << digi.fTimeBranch << std::endl;
    out << "EnergyBranch: " << digi.fEnergyBranch << std::endl;
    return out;
  }

 private:
  ThresholdDataFullMode fTimeBranch;
  ThresholdDataFullMode fEnergyBranch;
  FrameHeader fFrameHeader;
  ULong64_t fTimeStamp;
  int fToT;

  ClassDef(PndMvdPastaDigi, 1);
};

#endif /* MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTADIGIFULL_H_ */
