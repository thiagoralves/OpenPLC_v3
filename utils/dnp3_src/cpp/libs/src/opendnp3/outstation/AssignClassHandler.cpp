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
#include "AssignClassHandler.h"

#include "opendnp3/app/parsing/IAPDUHandler.h"
#include "opendnp3/outstation/IOutstationApplication.h"

#include <openpal/logging/Logger.h>

namespace opendnp3
{

AssignClassHandler::AssignClassHandler(openpal::IExecutor& executor, IOutstationApplication& application, IClassAssigner& assigner) :
	classHeader(-1),
	clazz(PointClass::Class0),
	pExecutor(&executor),
	pApplication(&application),
	pAssigner(&assigner)
{

}

IINField AssignClassHandler::ProcessHeader(const AllObjectsHeader& header)
{
	if (IsExpectingAssignment())
	{
		switch (header.enumeration)
		{
		case(GroupVariation::Group1Var0) :
			return this->ProcessAssignAll(AssignClassType::BinaryInput, clazz);
		case(GroupVariation::Group3Var0) :
			return this->ProcessAssignAll(AssignClassType::DoubleBinaryInput, clazz);
		case(GroupVariation::Group10Var0) :
			return this->ProcessAssignAll(AssignClassType::BinaryOutputStatus, clazz);
		case(GroupVariation::Group20Var0) :
			return this->ProcessAssignAll(AssignClassType::Counter, clazz);
		case(GroupVariation::Group21Var0) :
			return this->ProcessAssignAll(AssignClassType::FrozenCounter, clazz);
		case(GroupVariation::Group30Var0) :
			return this->ProcessAssignAll(AssignClassType::AnalogInput, clazz);
		case(GroupVariation::Group40Var0) :
			return this->ProcessAssignAll(AssignClassType::AnalogOutputStatus, clazz);
		default:
			return IINBit::FUNC_NOT_SUPPORTED;
		}
	}
	else
	{
		return this->RecordClass(header.enumeration);
	}
}



IINField AssignClassHandler::ProcessHeader(const RangeHeader& header)
{
	if (IsExpectingAssignment())
	{
		switch (header.enumeration)
		{

		case(GroupVariation::Group1Var0) :
			return ProcessAssignRange(AssignClassType::BinaryInput, clazz, header.range);
		case(GroupVariation::Group3Var0) :
			return ProcessAssignRange(AssignClassType::DoubleBinaryInput, clazz, header.range);
		case(GroupVariation::Group10Var0) :
			return ProcessAssignRange(AssignClassType::BinaryOutputStatus, clazz, header.range);
		case(GroupVariation::Group20Var0) :
			return ProcessAssignRange(AssignClassType::Counter, clazz, header.range);
		case(GroupVariation::Group21Var0) :
			return ProcessAssignRange(AssignClassType::FrozenCounter, clazz, header.range);
		case(GroupVariation::Group30Var0) :
			return ProcessAssignRange(AssignClassType::AnalogInput, clazz, header.range);
		case(GroupVariation::Group40Var0) :
			return ProcessAssignRange(AssignClassType::AnalogOutputStatus, clazz, header.range);
		default:
			return IINBit::FUNC_NOT_SUPPORTED;
		}
	}
	else
	{
		return IINBit::PARAM_ERROR;
	}
}

bool AssignClassHandler::IsExpectingAssignment()
{
	int32_t current = static_cast<int32_t>(this->GetCurrentHeader());

	if (current > 0 && ((current - 1) == this->classHeader))
	{
		this->classHeader = -1;
		return true;
	}
	else
	{
		return false;
	}
}

IINField AssignClassHandler::ProcessAssignRange(AssignClassType type, PointClass clazz, const Range& range)
{
	auto actual  = pAssigner->AssignClassToRange(type, clazz, range);

	this->NotifyApplicationOfAssignment(type, clazz, actual);

	// if the range was clipped or invalid return parameter error
	return actual.Equals(range) ? IINField() : IINBit::PARAM_ERROR;
}

IINField AssignClassHandler::ProcessAssignAll(AssignClassType type, PointClass clazz)
{
	auto full = pAssigner->AssignClassToAll(type, clazz);

	this->NotifyApplicationOfAssignment(type, clazz, full);

	return full.IsValid() ? IINField() : IINBit::PARAM_ERROR;
}

void AssignClassHandler::NotifyApplicationOfAssignment(AssignClassType type, PointClass clazz, const Range& range)
{
	if (range.IsValid() && pApplication)
	{
		auto pApplication = this->pApplication;
		auto callback = [pApplication, range, clazz, type]()
		{
			pApplication->RecordClassAssignment(type, clazz, range.start, range.stop);
		};

		pExecutor->Post(callback);
	}
}

IINField AssignClassHandler::RecordClass(GroupVariation gv)
{
	classHeader = this->GetCurrentHeader();

	switch (gv)
	{
	case(GroupVariation::Group60Var1) :
		clazz = PointClass::Class0;
		return IINField();
	case(GroupVariation::Group60Var2) :
		clazz = PointClass::Class1;
		return IINField();
	case(GroupVariation::Group60Var3) :
		clazz = PointClass::Class2;
		return IINField();
	case(GroupVariation::Group60Var4) :
		clazz = PointClass::Class3;
		return IINField();
	default:
		classHeader = -1;
		return IINBit::PARAM_ERROR;
	}
}

}

