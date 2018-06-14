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
#ifndef __MOCK_LOWER_LAYER_H_
#define __MOCK_LOWER_LAYER_H_

#include <queue>
#include <string>

#include <opendnp3/LayerInterfaces.h>

namespace opendnp3
{

class MockLowerLayer : public ILowerLayer, public HasUpperLayer
{
public:

	void SendUp(const openpal::RSlice& arBuffer);
	void SendUp(const std::string&);
	void SendSuccess();
	void SendFailure();
	void ThisLayerUp();
	void ThisLayerDown();

	void EnableAutoSendCallback(bool aIsSuccess);
	void DisableAutoSendCallback();

	bool HasNoData() const;

	size_t NumWrites() const;
	openpal::RSlice PopWrite();
	std::string PopWriteAsHex();

	virtual bool BeginTransmit(const openpal::RSlice& arBuffer) override final;

private:

	std::queue<openpal::RSlice> sendQueue;
};

}

#endif
