/********************************************************************************
 * Copyright (C) 2015-2017 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#include <FairMQDevice.h>
#include <FairMQLogger.h>

namespace fair
{
namespace mq
{
namespace test
{

class Rep : public FairMQDevice
{
  protected:
    auto Init() -> void override
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    auto Reset() -> void override
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto Run() -> void override
    {
        auto request1 = FairMQMessagePtr{NewMessage()};
        if (Receive(request1, "data") >= 0)
        {
            LOG(INFO) << "Received request 1";
            auto reply = FairMQMessagePtr{NewMessage()};
            Send(reply, "data");
        }
        auto request2 = FairMQMessagePtr{NewMessage()};
        if (Receive(request2, "data") >= 0)
        {
            LOG(INFO) << "Received request 2";
            auto reply = FairMQMessagePtr{NewMessage()};
            Send(reply, "data");
        }

        LOG(INFO) << "REQ-REP test successfull";
    };
};

} // namespace test
} // namespace mq
} // namespace fair
