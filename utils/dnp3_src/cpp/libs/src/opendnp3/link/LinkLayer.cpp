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
#include "LinkLayer.h"

using namespace openpal;

namespace opendnp3
{

LinkLayer::LinkLayer(const openpal::Logger& logger, const std::shared_ptr<openpal::IExecutor>& executor, const std::shared_ptr<IUpperLayer>& upper, const std::shared_ptr<opendnp3::ILinkListener>& listener, const LinkConfig& config) :
	ctx(logger, executor, upper, listener, *this, config)
{}

const StackStatistics::Link& LinkLayer::GetStatistics() const
{
	return this->ctx.statistics;
}

void LinkLayer::SetRouter(ILinkTx& router)
{
	assert(ctx.linktx == nullptr);
	ctx.linktx = &router;
}

////////////////////////////////
// ILowerLayer
////////////////////////////////

bool LinkLayer::Send(ITransportSegment& segments)
{
	if (!ctx.isOnline) return false;

	if (ctx.SetTxSegment(segments))
	{
		ctx.TryStartTransmission();
	}

	return true;
}

////////////////////////////////
// ILinkSession
////////////////////////////////

bool LinkLayer::OnLowerLayerUp()
{
	return ctx.OnLowerLayerUp();
}

bool LinkLayer::OnLowerLayerDown()
{
	return ctx.OnLowerLayerDown();
}

bool LinkLayer::OnTransmitResult(bool success)
{
	auto ret = ctx.OnTransmitResult(success);

	if (ret)
	{
		ctx.TryStartTransmission();
	}

	return true;
}

////////////////////////////////
// IFrameSink
////////////////////////////////

bool LinkLayer::OnFrame(const LinkHeaderFields& header, const openpal::RSlice& userdata)
{
	auto ret = this->ctx.OnFrame(header, userdata);

	if (ret)
	{
		this->ctx.TryStartTransmission();
	}

	return ret;
}

}

