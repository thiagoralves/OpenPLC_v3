/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
 * more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 * Green Energy Corp licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project was forked on 01/01/2013 by Automatak, LLC and modifications
 * may have been made to this file. Automatak, LLC licenses these modifications
 * to you under the terms of the License.
 */
#ifndef OPENDNP3_TRANSPORTLAYER_H
#define OPENDNP3_TRANSPORTLAYER_H

#include "TransportRx.h"
#include "TransportTx.h"

#include <openpal/executor/IExecutor.h>
#include <openpal/logging/Logger.h>

#include "opendnp3/LayerInterfaces.h"
#include "opendnp3/StackStatistics.h"
#include "opendnp3/link/ILinkLayer.h"

namespace opendnp3
{

/**
	Implements the DNP3 transport layer
*/
class TransportLayer : public IUpperLayer, public ILowerLayer
{

public:

	TransportLayer(const openpal::Logger& logger, uint32_t maxRxFragSize);

	/// ILowerLayer

	virtual bool BeginTransmit(const openpal::RSlice&) override final;

	/// IUpperLayer

	virtual bool OnReceive(const openpal::RSlice&) override final;
	virtual bool OnLowerLayerUp() override final;
	virtual bool OnLowerLayerDown() override final;
	virtual bool OnSendResult(bool isSuccess) override final;

	void SetAppLayer(IUpperLayer& upperLayer);

	void SetLinkLayer(ILinkLayer& linkLayer);

	StackStatistics::Transport GetStatistics() const;

private:

	openpal::Logger logger;

	IUpperLayer* upper = nullptr;
	ILinkLayer* lower = nullptr;

	// ---- state ----
	bool isOnline = false;
	bool isSending = false;

	// ----- Transmitter and Receiver Classes ------
	TransportRx receiver;
	TransportTx transmitter;
};

}

#endif

