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
#include "ClassBasedRequestHandler.h"

namespace opendnp3
{

IINField ClassBasedRequestHandler::ProcessHeader(const AllObjectsHeader& header)
{
	switch (header.enumeration)
	{
	case(GroupVariation::Group60Var2) :
		classField.Set(PointClass::Class1);
		return IINField::Empty();
	case(GroupVariation::Group60Var3) :
		classField.Set(PointClass::Class2);
		return IINField::Empty();
	case(GroupVariation::Group60Var4) :
		classField.Set(PointClass::Class3);
		return IINField::Empty();
	default:
		return IINField(IINBit::FUNC_NOT_SUPPORTED);
	}
}

}

