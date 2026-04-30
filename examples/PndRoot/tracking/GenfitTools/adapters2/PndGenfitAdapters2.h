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

// modified by Elisabetta Prencipe, 19/05/2014

#ifndef PNDGENFITADAPTERS2_H
#define PNDGENFITADAPTERS2_H

class PndTrack;
class PndTrackCand;
namespace genfit {
class Track;
class AbsTrackRep;
class TrackCand;

} // namespace genfit

PndTrackCand *Genfit2TrackCand2PndTrackCand(const genfit::TrackCand *);
genfit::TrackCand *PndTrackCand2Genfit2TrackCand(PndTrackCand *);
PndTrack *Genfit2Track2PndTrack(const genfit::Track *);

#endif
