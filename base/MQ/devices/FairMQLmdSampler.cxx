/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
 /* 
 * File:   FairMQLmdSampler.cxx
 * Author: winckler
 *
 * Created on October 27, 2015, 2:07 PM
 */


#include "FairMQLogger.h"
#include "FairMQLmdSampler.h"


FairMQLmdSampler::FairMQLmdSampler() : 
	fCurrentFile(0),
	fNEvent(0),
    fCurrentEvent(0),
    fFileNames(),
    fxInputChannel(nullptr),
    fxEvent(nullptr),
    fxBuffer(nullptr),
    fxEventData(nullptr),
    fxSubEvent(nullptr),
    fxInfoHeader(nullptr),
    stop(false),
    fMsgCounter(0)
{
}

FairMQLmdSampler::~FairMQLmdSampler() 
{
}



//______________________________________________________________________________
void FairMQLmdSampler::AddSubEvtKey(short type, short subType, short procid, short subCrate, short control, const std::string& channelName)
{
	SubEvtKey key(type, subType, procid, subCrate, control);
	if(!fSubEventChanMap.count(key))
		fSubEventChanMap[key] = channelName;
	else
	{
		LOG(WARN) 	<< "FairMQLmdSampler : subevent header key '(" 
					<< type
					<< "," 
					<< subType
					<< ","
					<< procid
					<< ","
					<< subCrate
					<< ","
 					<< control
 					<< ")' has already been defined with channel name '"
					<< fSubEventChanMap.at(key)
					<< "'. it will be overwritten with new channel name = "
					<< channelName;
		fSubEventChanMap[key] = channelName;
	}
}


//______________________________________________________________________________
void FairMQLmdSampler::InitTask()
{
  
	if(fFileNames.size() == 0) 
	{
		//return false;
	}

	std::string name = fFileNames.at(fCurrentFile);
	if(! OpenNextFile(name)) 
	{
		//return false;
	}

	fCurrentFile += 1;

	// Init Counters
	fNEvent=0;
	fCurrentEvent=0;

	//return true;
}

void FairMQLmdSampler::Run()
{
	
	while (CheckCurrentState(RUNNING) && !stop)
	{	
		ReadEvent();
	}
	LOG(INFO)<<"Sent "<<fMsgCounter<<" messages.";

}


void free_buffer (void *data, void *hint)
{
    LOG(TRACE) << "empty deleter";
}

//______________________________________________________________________________
int FairMQLmdSampler::ReadEvent()
{
	void* evtptr = &fxEvent;
	void* buffptr = &fxBuffer;
	
	/*1+ C Main ****************+******************************************/
	/*+ Module      : f_evt_get_event                                     */
	/*--------------------------------------------------------------------*/
	/*+ CALLING     : f_evt_get_event(s_evt_channel &s_chan, long **ppl_buffer, long **ppl_goobuf) */
	/*--------------------------------------------------------------------*/
	/*+ PURPOSE     : f_evt_get_event  returnes address of event          */
	/*+ ARGUMENTS   :                                                     */
	/*+   s_chan    : Input channel from open.                            */
	/*+   ppl_buffer: Address of pointer. Returns address of event.       */
	/*+   ppl_goobuf: Address of pointer. Returns address of buffer.      */
	/*+ Return type : int.                                                */
	/*+ Status codes:                                                     */
	/*-               GETEVT__SUCCESS=0 : success.                        */
	/*-               GETEVT__FRAGMENT=1: Event fragment found.           */
	/*-               GETEVT__NOMORE=3  : No more events.                 */
	/*-               GETEVT__RDERR=6   : read server or file error       */
	/*-               GETEVT__TIMEOUT=9 : when enabled by f_evt_timeout   */
	/*+ Declaration :                                                     */
	/*                INTS4 f_evt_get_event(  */
	/*                     s_evt_channel *, INTS4 **, INTS4 **); */
	/*+ FUNCTION    : Get next event and returnes pointer. The pointer    */
	/*                may point to the event in the buffer or internal    */
	/*                event buffer (spanned events). The content of the   */
	/*                pointer may be destroyed by next call.              */
	/*1- C Main ****************+******************************************/
	//INTS4 f_evt_get_event(s_evt_channel*, INTS4**, INTS4**); // -> in f_evt.h
	
	int status = f_evt_get_event(fxInputChannel, (INTS4**)evtptr, (INTS4**)buffptr);
	//int fuEventCounter = fxEvent->l_count;
	//int fCurrentMbsEventNo = fuEventCounter;
	
	LOG(DEBUG) << "STATUS = "<<status;
	if(GETEVT__SUCCESS != status) // if f_evt_get_event not successfull close if nomore evt or look to another file and start again
	{

		LOG(DEBUG) << "FairMQLmdSampler::ReadEvent()";

		CHARS* sErrorString = NULL;
		f_evt_error(status, sErrorString , 0);

		if(GETEVT__NOMORE == status) 
		{
			Close();
		}


		// fCurrentFile incremented in InitTask once
		if(fCurrentFile >= fFileNames.size()) 
		{
			stop=true;
			return 1;
		}

		std::string name = fFileNames.at(fCurrentFile);
		if(! OpenNextFile(name) ) 
		{
			return 1;
		} 
		else 
		{
			fCurrentFile += 1;
			return ReadEvent();
		}
	}

		//Store Start Times
	//if (fCurrentEvent==0 ) 
	//  	Unpack((int*)fxBuffer, sizeof(s_bufhe), -4, -4, -4, -4, -4);


	// Decode event header
	//bool result = false;
	/*bool result = */
	//Unpack((int*)fxEvent, sizeof(s_ve10_1), -2, -2, -2, -2, -2);

	/*****************+***********+****************************************/
	/*1+ C Procedure *************+****************************************/
	/*+ Module      : f_evt_get_subevent                                  */
	/*--------------------------------------------------------------------*/
	/*+ CALLING     : l_sts = f_evt_get_subevent(ve10_1 *,subevent,**head,**data,*lwords) */
	/*--------------------------------------------------------------------*/
	/*+ PURPOSE     : get subevent pointer                                */
	/*+ ARGUMENTS   :                                                     */
	/*+   ve10_1    : (s_ve10_1 *) event header pointer                   */
	/*+  subevent   : subevent number (1,2,3...)                          */
	/*                If = 0, f_evt_get_subevent returns the number of    */
	/*                subevents. In this case the following arguments     */
	/*                might be NULL.                                      */
	/*+   head      : Address of s_ves10_1 subevent header pointer        */
	/*+   data      : Address of INTS4 event data pointer                 */
	/*+   lwords    : Address of INTS4 to get number of data longwords    */
	/*+ Return type : int                                                 */
	/*-               GETEVT__SUCCESS   : Found subevent.                 */
	/*-               GETEVT__NOMORE    : No more subevents.              */
	/*+ Declaration :                                                     */
	/*                INTS4 f_evt_get_subevent( */
	/*                   s_ve10_1 *, INTS4, INTS4 **, INTS4 **, INTS4 *); */
	/*1- C Procedure *************+****************************************/
	int nrSubEvts = f_evt_get_subevent(fxEvent, 0, NULL, NULL, NULL);
	int sebuflength;
	short setype;
	short sesubtype;
	short seprocid;
	short sesubcrate;
	short secontrol;

	LOG(TRACE) 	<< "FairMQLmdSampler::ReadEvent => Found " 
				<< nrSubEvts 
				<< " Sub-event ";
	//if (fCurrentEvent%10000==0)
	//cout << " -I- LMD_ANA:  evt# " <<  fCurrentEvent << "  n_subevt# " << nrSubEvts << " evt processed# " << fNEvent <<  " : " << fxEvent->l_count << endl;


	//  int* SubEventDataPtr = new int;
	for(int i = 1; i <= nrSubEvts; i++) 
	{
		void* SubEvtptr = &fxSubEvent;
		void* EvtDataptr = &fxEventData;
		int nrlongwords;
		status = f_evt_get_subevent(fxEvent, i, (int**)SubEvtptr, (int**)EvtDataptr, &nrlongwords);

		if(status) 
		{
			return 1;
		}

		sebuflength = nrlongwords;
		setype = fxSubEvent->i_type;
		sesubtype = fxSubEvent->i_subtype;
		seprocid = fxSubEvent->i_procid;
		sesubcrate = fxSubEvent->h_subcrate;
		secontrol = fxSubEvent->h_control;

		

		//cout << setype << "  " << sesubtype << "  " << seprocid << "  " << sesubcrate << "  " << secontrol << endl;
		
		// Data to send : fxEventData


		SubEvtKey key(setype, sesubtype, seprocid, sesubcrate, secontrol);

		
		if(!fSubEventChanMap.count(key))
		{
			LOG(ERROR)<<" key not registered!";
			//TODO : throw something
		}
		LOG(TRACE)<<"array size = "<<sebuflength;
		LOG(TRACE)<<"fxEventData = "<<*fxEventData;

		std::string chanName=fSubEventChanMap.at(key);
		LOG(TRACE)<<"chanName="<<chanName;



		// send header
		//std::unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage(fxSubEvent, sizeof(fxSubEvent), free_buffer, nullptr));
		//fChannels.at(chanName).at(0).SendPart(header);

		int* arraySize = new int(sebuflength);

        std::unique_ptr<FairMQMessage> msgSize(fTransportFactory->CreateMessage(arraySize, sizeof(int)));
        fChannels.at(chanName).at(0).SendPart(msgSize);
		// send data
		std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage(fxEventData, sebuflength, free_buffer, nullptr));
		fChannels.at(chanName).at(0).Send(msg);
		fMsgCounter++;
		/*
		if(Unpack(fxEventData, sebuflength,
		          setype, sesubtype,
		          seprocid, sesubcrate, secontrol)) 
		{
	    	result = true;
		}
		*/
	}

	// Increment evt counters.
	fNEvent++;
	fCurrentEvent++;

	/*if(! result)
	{
		return 2;
	}*/

	return 0;
}

//______________________________________________________________________________
bool FairMQLmdSampler::OpenNextFile(const std::string& fileName)
{
	int inputMode = GETEVT__FILE;
	fxInputChannel = new s_evt_channel;
	void* headptr = &fxInfoHeader;
	INTS4 status;

	LOG(INFO) 	<< "File " 
				<< fileName 
				<< " will be opened.";

	status = f_evt_get_open(inputMode,
	                      const_cast<char*>(fileName.c_str()),
	                      fxInputChannel,
	                      (char**)headptr,
	                      1,
	                      1);

	if(status) 
	{
		LOG(ERROR) 	<< "File " 
					<< fileName 
					<< " opening failed.";

		return false;
	}

	LOG(INFO) 	<< "File " 
				<< fileName 
				<< " opened.";

	// Decode File Header
	//bool result = Unpack((int*)fxInfoHeader, sizeof(s_filhe), -4, -4, -4, -4, -4);

	return true;
}

//______________________________________________________________________________
void FairMQLmdSampler::AddDir(const std::string& dir)
{
	path directory = dir;

	if (fs::exists(directory))
	{
		fs::directory_iterator end_itr;

	    for (fs::directory_iterator itr(directory); itr != end_itr; ++itr)
	    {
	        if (fs::is_regular_file(itr->path())) {
	            std::string current_file = itr->path().string();
	            AddFile(current_file);
	        }
	    }
    }
	else
	{
	    LOG(WARN)<<"FairMQLmdSampler: directory '"<< directory.string() <<"' not found";
	}
}

//______________________________________________________________________________
void FairMQLmdSampler::AddFile(const std::string& fileName)
{
    path filepath = fileName;
	if (fs::exists(filepath))
	{
	    fFileNames.push_back(fileName);
	}
	else
	{
	    LOG(WARN)<<"FairMQLmdSampler: file '"<< fileName <<"' not found";
	}
}

//______________________________________________________________________________
void FairMQLmdSampler::Close()
{
	f_evt_get_close(fxInputChannel);
	//Unpack((Int_t*)fxBuffer, sizeof(s_bufhe), -4, -4, -4, -4, -4);  
	fCurrentEvent=0;
}