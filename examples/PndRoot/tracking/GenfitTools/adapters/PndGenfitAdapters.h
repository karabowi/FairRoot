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

#ifndef PNDGENFITADAPTERS_H
#define PNDGENFITADAPTERS_H

class PndTrack;
class PndTrackCand;
class GFTrack;
class GFAbsTrackRep;
class GFTrackCand;

PndTrackCand *GenfitTrackCand2PndTrackCand(const GFTrackCand *);
GFTrackCand *PndTrackCand2GenfitTrackCand(PndTrackCand *);
PndTrack *GenfitTrack2PndTrack(const GFTrack *);

#endif
