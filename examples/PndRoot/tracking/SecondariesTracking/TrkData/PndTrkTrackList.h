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

/** PndTrkTrackList
 ** Class for pattern recognition TrackList
 ** @author Lia Lavezzi
 **
 **
 **/

#ifndef PNDTRKTRACKLIST_H
#define PNDTRKTRACKLIST_H 1

#include "TVector3.h"
#include "PndTrkTrack.h"

class TClonesArray;
class PndTrkTrackList : public TObject {

 public:
  PndTrkTrackList();
  // copy ctor
  PndTrkTrackList(const PndTrkTrackList &tlist);
  ~PndTrkTrackList();
  PndTrkTrackList &operator=(const PndTrkTrackList &tlist);

  void AddTrack(PndTrkTrack *track);
  void DeleteTrack(Int_t index);
  void Reset();
  void Clear(Option_t *opt = "");

  inline Int_t GetNofTracks() { return fTrackList.GetEntriesFast(); }

  PndTrkTrack *GetTrack(Int_t index) { return (PndTrkTrack *)fTrackList.At(index); }

  /*   void ReplaceTrack(Int_t index, PndTrkTrack *track); */

 protected:
  TClonesArray fTrackList;
  // std::vector< PndTrkTrack > tracklist;

  ClassDef(PndTrkTrackList, 1);
};

#endif
