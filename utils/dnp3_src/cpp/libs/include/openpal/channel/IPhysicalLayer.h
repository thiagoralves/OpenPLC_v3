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
#ifndef OPENPAL_IPHYSICALLAYER_H
#define OPENPAL_IPHYSICALLAYER_H

#include "ChannelStatistics.h"

#include "openpal/container/RSlice.h"
#include "openpal/container/WSlice.h"

namespace openpal
{

class IPhysicalLayerCallbacks;

class IChannelState
{

public:

	virtual bool CanOpen() const = 0;

	virtual bool CanClose() const = 0;

	virtual bool CanRead() const = 0;

	virtual bool CanWrite() const = 0;

	/** @return True if the layer is performing an async read */
	virtual bool IsReading() const = 0;

	/** @return True if the layer is performing an async write */
	virtual bool IsWriting() const = 0;

	/** @return True if the layer is performing an asynchronously closing */
	virtual bool IsClosing() const = 0;

	/** @return True if the layer is in the closed with no other activity */
	virtual bool IsClosed() const = 0;

	/** @return True if the layer is performing an asynchronously opening */
	virtual bool IsOpening() const = 0;

	/** @return True if the layer is open for read/write */
	virtual bool IsOpen() const = 0;
};


/**
 * Defines an asynchronous interface for serial/tcp/?
 */
class IPhysicalLayer : public IChannelState
{
public:

	IPhysicalLayer() : pChannelStatistics(nullptr)
	{}

	virtual ~IPhysicalLayer() {}

	/**
	* Tell the layer where to update its channel statistics
	*/
	void SetChannelStatistics(ChannelStatistics* pChannelStatistics_)
	{
		pChannelStatistics = pChannelStatistics_;
	}

	/**
	 * Starts an open operation.
	 *
	 * Callback is either IHandler::OnLowerLayerUp or
	 * IHandler::OnOpenFailure.
	 */
	virtual void BeginOpen() = 0;

	/**
	 * Starts a close operation.
	 *
	 * Callback is IHandler::OnLowerLayerDown.  Callback occurs
	 * after all hronous operations have occured.  If the user
	 * code has an outstanding read or write, those handlers will not
	 * be called.
	 */
	virtual void BeginClose() = 0;

	/**
	 * Starts a send operation.
	 *
	 * Callback is IHandler::OnSendSuccess or a failure will
	 * result in the layer closing.
	 *
	 * @param arBuffer		The buffer from which the write operation
	 * 						will write data. The underlying buffer must
	 *						remain available until the write callback or
	 *                      close occurs.
	 */
	virtual void BeginWrite(const RSlice& arBuffer) = 0;

	/**
	 * Starts a read operation.
	 *
	 * Use SetHandler to provide a callback that is called by
	 * OnReceive(const RSlice&) or a failure will
	 * result in the layer closing.
	 *
	 * @param arBuffer		Read into the underlying buffer
	 *                      defined by the wrapper.  The underlying buffer
	 *                      must remain available until the read callback
	 */
	virtual void BeginRead(WSlice& arBuffer) = 0;

	/**
	 * Set the handler interface for callbacks. A read interface has
	 * been added so the max size is no longer required.
	 *
	 * @param apHandler		Class that will process asynchronous
	 * 						callbacks
	 */
	virtual void SetHandler(IPhysicalLayerCallbacks* apHandler) = 0;

protected:

	ChannelStatistics* pChannelStatistics;

};

};



#endif
