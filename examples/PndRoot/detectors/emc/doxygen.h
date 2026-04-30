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
 * @defgroup PndEmc PndEmc
 * @brief Electromagnetic calorimeter
 *
 * @section Simulation
 * Simulation is handled by PndEmc. The energy deposit creates an PndEmcPoint, from those the
 * task PndEmcHitProducer creates PndEmcHit%s.
 *
 * @section Geometry
 * Geometry is handled by the PndEmcMapper. The class PndEmcTwoCoordIndex is used to store the crystal
 * coordinates and check for neighbors.
 *
 * @section Tasks
 * The standard way to create PndEmcRecoHit from detector hits (PndEmcPoint) involves the following tasks:
 * * PndEmcHitProducer: creates PndEmcHit%s from PndEmcPoint%s
 * * PndEmcHitsToWaveform: creates PndEmcWaveform%s from PndEmcHit%s
 * * PndEmcWaveformToDigi: creates PndEmcDigi%s from PndEmcWaveform%s
 * * PndEmcMakeCluster: creates PndEmcCluster%s from PndEmcDigi%s
 * * PndEmcMakeBump: creates PndEmcBump%s from PndEmcCluster%s
 * * PndEmcMakeRecoHit: creates PndEmcRecoHit%s from PndEmcBump%s
 *
 * As an alternative, one can use PndEmcMakeDigi to create PndEmcDigi%s directly from PndEmcHit%s,
 * bypassing the waveform generation and pulse shape analysis.
 *
 * @section Waveforms
 * To simulate the full detector readout chain, the waveform of the detector and the following pulse
 * shape analysis are simulated. The task PndEmcHitsToWaveform uses the pulseshape PndEmcAsicPulseshape
 * for barrel, FwEndcap and BwEndcap and PndEmcCRRCPulseshape for the shashlyk. The task PndEmcWaveformToDigi
 * then uses PndEmcPSAMatchedDigiFilter for the pulse shape analysis of barrel, FwEndcap and BwEndcap and
 * PndEmcPSAParabolic for the shashlyk.
 * @subsection tbwf Time-based waveforms
 * There are two ways running the waveform generation and pulse shape analysis in time-based simulation.
 * One was implemented inside the PndEmcHitsToWaveform and PndEmcWaveformToDigi classes, the other one uses
 * its own classes prefixed (in parts) with PndEmcFWEndcap. This does not mean that they are only for the
 * FwEndcap, but they were developed for the FwEndcap first and can now be used for the whole EMC.
 *
 * @subsubsection tbwfA Variant A
 * The variant using PndEmcWaveformToDigi creates the waveforms in the same way as for event-based
 * simulation, but uses a different buffer PndEmcWaveformWriteoutBuffer. In
 * PndEmcWaveformWriteoutBuffer::Modify() the waveforms are added in the case of pileup via the newly
 * implemented operator PndEmcWaveform::operator+=(). As the noise is generated for the individual
 * waveforms, the noise adds up quadratically in this case. The waveforms have to be sorted before
 * digitisation (PndEmcWaveformSorterTask).
 *
 * Pulse shape analysis is done in PndEmcWaveformToDigi, using the branch EmcSortedWaveform instead
 * of EmcWaveform.
 *
 * @subsubsection tbwfB Variant B
 * The variant using PndEmcFWEndcapTimebasedWaveforms uses a more modular approach with a simulator
 * (inheriting PndEmcAbsWaveformSimulator) and modifier (inheriting PndEmcAbsWaveformModifier) to
 * create waveforms. In case of pileup, the waveforms are first joined and afterwards the noise is
 * added (by waveform modifier PndEmcShapingNoiseAdder). This variant also uses a custom writeout buffer
 * (PndEmcWaveformBuffer).
 *
 * Pulse shape analysis is done in PndEmcFWEndcapDigi. The digis have to be sorted by PndEmcDigiSorterTask.
 * Due to limitations in PndEmcMakeCluster, the task PndEmcClusterRemoveDuplCrys is needed to remove
 * double hits in one crystal for one event.
 *
 */
