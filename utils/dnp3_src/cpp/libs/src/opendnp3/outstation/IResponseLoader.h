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
#ifndef OPENDNP3_IRESPONSELOADER_H
#define OPENDNP3_IRESPONSELOADER_H

#include "opendnp3/app/HeaderWriter.h"

namespace opendnp3
{

class IResponseLoader
{
public:

	/**
	* @ return True if there is any data selected for reporting
	*/
	virtual bool HasAnySelection() const = 0;

	/**
	* Loads static data into a response APDU
	*
	* @param writer The HeaderWriter instance used to place data in the APDU
	* @return True if all data was loaded, false if there is no more space
	*/
	virtual bool Load(HeaderWriter& writer) = 0;
};

}



#endif

