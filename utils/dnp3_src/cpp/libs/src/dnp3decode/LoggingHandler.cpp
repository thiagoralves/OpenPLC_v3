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

#include "LoggingHandler.h"

#include <ctime>
#include <iomanip>

using namespace openpal;

namespace opendnp3
{

LoggingHandler::LoggingHandler(openpal::Logger logger_, IDecoderCallbacks& callbacks_) :
	logger(logger_),
	callbacks(&callbacks_)
{}

void LoggingHandler::OnHeaderResult(const HeaderRecord& record, const IINField& result)
{
	if (result.Any())
	{
		Indent i(*callbacks);
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "Pretty printing not supported for this type");
	}
}

std::string LoggingHandler::ToUTCString(const DNPTime& dnptime)
{
	time_t seconds = static_cast<time_t>(dnptime / 1000);
	uint16_t milliseconds = static_cast<uint16_t>(dnptime % 1000);

#ifdef WIN32
	tm t;
	if (gmtime_s(&t, &seconds))
	{
		return "BAD TIME";
	}
#else
	tm t;
	if (!gmtime_r(&seconds, &t))
	{
		return "BAD TIME";
	}
#endif

	std::ostringstream oss;
	oss << (1900 + t.tm_year);
	oss << "-" << std::setfill('0') << std::setw(2) << (1 + t.tm_mon);
	oss << "-" << std::setfill('0') << std::setw(2) << t.tm_mday;
	oss << " " << std::setfill('0') << std::setw(2) << t.tm_hour;
	oss << ":" << std::setfill('0') << std::setw(2) << t.tm_min;
	oss << ":" << std::setfill('0') << std::setw(2) << t.tm_sec;
	oss << "." << std::setfill('0') << std::setw(3) << milliseconds;
	return oss.str();
}

IINField LoggingHandler::PrintCrob(const ICollection<Indexed<ControlRelayOutputBlock>>& items)
{
	Indent i(*callbacks);
	auto logItem = [this](const Indexed<ControlRelayOutputBlock>& item)
	{
		std::ostringstream oss;
		oss << "[" << item.index << "] - code: 0x" << ToHex(item.value.rawCode) << " (" << ControlCodeToString(item.value.functionCode) << ")";
		oss << " count: " << static_cast<uint32_t>(item.value.count);
		oss << " on-time: " << static_cast<uint32_t>(item.value.onTimeMS);
		oss << " off-time: " << static_cast<uint32_t>(item.value.offTimeMS);
		oss << " status: " << CommandStatusToString(item.value.status);
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());
	};

	items.ForeachItem(logItem);

	return IINField::Empty();
}

IINField LoggingHandler::PrintOctets(const ICollection<Indexed<OctetString>>& items)
{
	Indent i(*callbacks);
	auto logItem = [this](const Indexed<OctetString>& item)
	{
		auto slice = item.value.ToRSlice();
		FORMAT_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "[%u] value: (length = %u)", item.index, slice.Size());
		FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, slice, 18, 18);
	};

	items.ForeachItem(logItem);

	return IINField::Empty();
}

IINField LoggingHandler::PrintTimeAndInterval(const ICollection<Indexed<TimeAndInterval>>& values)
{
	Indent i(*callbacks);
	auto logItem = [this](const Indexed<TimeAndInterval>& item)
	{
		std::ostringstream oss;
		oss << "[" << item.index << "] - startTime: " << ToUTCString(item.value.time);
		oss << " count: " << item.value.interval;
		oss << " units: " << IntervalUnitsToString(item.value.GetUnitsEnum()) << " (" << static_cast<int>(item.value.units) << ")";
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());
	};

	values.ForeachItem(logItem);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const FreeFormatHeader& header, const Group120Var1& value, const openpal::RSlice& object)
{
	Indent i(*callbacks);

	std::ostringstream oss;
	oss << "csq: " << value.challengeSeqNum;
	oss << " user: " << value.userNum;
	oss << " MAC: " << HMACTypeToString(value.hmacAlgo);
	oss << " reason: " << ChallengeReasonToString(value.challengeReason);
	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "challenge data: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.challengeData, 18, 18);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const FreeFormatHeader& header, const Group120Var2& value, const openpal::RSlice& object)
{
	Indent i(*callbacks);

	std::ostringstream oss;
	oss << "csq: " << value.challengeSeqNum;
	oss << " user: " << value.userNum;
	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "MAC value: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.hmacValue, 18, 18);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const FreeFormatHeader& header, const Group120Var5& value, const openpal::RSlice& object)
{
	Indent i(*callbacks);

	std::ostringstream oss;
	oss << "ksq: " << value.keyChangeSeqNum;
	oss << " user: " << value.userNum;
	oss << " KW: " << KeyWrapAlgorithmToString(value.keyWrapAlgo);
	oss << " KS: " << KeyStatusToString(value.keyStatus);
	oss << " MAC: " << HMACTypeToString(value.hmacAlgo);
	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "challenge data: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.challengeData, 18, 18);

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "MAC value: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.hmacValue, 18, 18);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const FreeFormatHeader& header, const Group120Var6& value, const openpal::RSlice& object)
{
	Indent i(*callbacks);

	std::ostringstream oss;
	oss << "ksq: " << value.keyChangeSeqNum;
	oss << " user: " << value.userNum;
	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "key data: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.keyWrapData, 18, 18);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const FreeFormatHeader& header, const Group120Var7& value, const openpal::RSlice& object)
{
	Indent i(*callbacks);

	{
		std::ostringstream oss;
		oss << "seq: " << value.challengeSeqNum;
		oss << " user: " << value.userNum;
		oss << " assoc: " << value.assocId;
		oss << " error: " << AuthErrorCodeToString(value.errorCode);
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());
	}

	{
		std::ostringstream oss;
		oss << "time of error: " << ToUTCString(value.time);
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());
	}

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "error text: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.errorText, 18, 18);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const FreeFormatHeader& header, const Group120Var8& value, const openpal::RSlice& object)
{
	Indent i(*callbacks);

	std::ostringstream oss;
	oss << "kcm: " << KeyChangeMethodToString(value.keyChangeMethod);
	oss << " type: " << CertificateTypeToString(value.certificateType);
	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "certificate: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.certificate, 18, 18);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const FreeFormatHeader& header, const Group120Var9& value, const openpal::RSlice& object)
{
	Indent i(*callbacks);

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "MAC: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.hmacValue, 18, 18);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const FreeFormatHeader& header, const Group120Var10& value, const openpal::RSlice& object)
{
	Indent i(*callbacks);

	{
		std::ostringstream oss;
		oss << "kcm: " << KeyChangeMethodToString(value.keyChangeMethod);
		oss << " op: " << UserOperationToString(value.userOperation);
		oss << " scsn: " << value.statusChangeSeqNum;
		oss << " role: " << value.userRole;
		oss << "exp: " << value.userRoleExpDays;
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());
	}

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "user name: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.userName, 18, 18);

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "pub key: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.userPublicKey, 18, 18);

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "cert data: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.certificationData, 18, 18);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const FreeFormatHeader& header, const Group120Var11& value, const openpal::RSlice& object)
{
	Indent i(*callbacks);

	{
		std::ostringstream oss;
		oss << "kcm: " << KeyChangeMethodToString(value.keyChangeMethod);
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());
	}

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "user name: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.userName, 18, 18);

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "master challenge data: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.challengeData, 18, 18);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const FreeFormatHeader& header, const Group120Var12& value, const openpal::RSlice& object)
{
	Indent i(*callbacks);

	{
		std::ostringstream oss;
		oss << "ksq: " << value.keyChangeSeqNum;
		oss << " user: " << value.userNum;
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());
	}

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "outstation challenge data: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.challengeData, 18, 18);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const FreeFormatHeader& header, const Group120Var13& value, const openpal::RSlice& object)
{
	Indent i(*callbacks);

	{
		std::ostringstream oss;
		oss << "ksq: " << value.keyChangeSeqNum;
		oss << " user: " << value.userNum;
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());
	}

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "encrypted update key: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.encryptedUpdateKey, 18, 18);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const FreeFormatHeader& header, const Group120Var14& value, const openpal::RSlice& object)
{
	Indent i(*callbacks);

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "Digital Signature: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.digitalSignature, 18, 18);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const FreeFormatHeader& header, const Group120Var15& value, const openpal::RSlice& object)
{
	Indent i(*callbacks);

	SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, "MAC value: ");
	FORMAT_HEX_BLOCK(logger, flags::APP_OBJECT_RX, value.hmacValue, 18, 18);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const CountHeader& header, const ICollection<Group50Var1>& values)
{
	return this->PrintTime(values);
}

IINField LoggingHandler::ProcessHeader(const CountHeader& header, const ICollection<Group51Var1>& values)
{
	return this->PrintTime(values);
}

IINField LoggingHandler::ProcessHeader(const CountHeader& header, const ICollection<Group51Var2>& values)
{
	return this->PrintTime(values);
}

IINField LoggingHandler::ProcessHeader(const CountHeader& header, const ICollection<Group52Var1>& values)
{
	return this->PrintTime16(values);
}

IINField LoggingHandler::ProcessHeader(const CountHeader& header, const ICollection<Group52Var2>& values)
{
	return this->PrintTime16(values);
}

IINField LoggingHandler::ProcessHeader(const CountHeader& header, const ICollection<Group120Var3>& values)
{
	Indent i(*callbacks);

	auto print = [this](const Group120Var3 & value)
	{
		std::ostringstream oss;
		oss << "csq: " << value.challengeSeqNum;
		oss << " user: " << value.userNum;
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());
	};

	values.ForeachItem(print);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const CountHeader& header, const ICollection<Group120Var4>& values)
{
	Indent i(*callbacks);

	auto print = [this](const Group120Var4 & value)
	{
		std::ostringstream oss;
		oss << "user: " << value.userNum;
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());
	};

	values.ForeachItem(print);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<IINValue>>& values)
{
	return this->PrintV(values);
}

IINField LoggingHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Binary>>& values)
{
	auto stringify = [](bool value) -> const char*
	{
		return GetStringValue(value);
	};
	return this->PrintVQTStringify(header.enumeration, values, stringify);
}

IINField LoggingHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<DoubleBitBinary>>& values)
{
	auto stringify = [](DoubleBit db) -> const char*
	{
		return DoubleBitToString(db);
	};
	return this->PrintVQTStringify(header.enumeration, values, stringify);
}

IINField LoggingHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<BinaryOutputStatus>>& values)
{
	return this->PrintVQT(header.enumeration, values);
}

IINField LoggingHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Counter>>& values)
{
	return this->PrintVQT(header.enumeration, values);
}

IINField LoggingHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<FrozenCounter>>& values)
{
	return this->PrintVQT(header.enumeration, values);
}

IINField LoggingHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Analog>>& values)
{
	return this->PrintVQT(header.enumeration, values);
}

IINField LoggingHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<AnalogOutputStatus>>& values)
{
	return this->PrintVQT(header.enumeration, values);
}

IINField LoggingHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<OctetString>>& values)
{
	return this->PrintOctets(values);
}

IINField LoggingHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<TimeAndInterval>>& values)
{
	return this->PrintTimeAndInterval(values);
}

IINField LoggingHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Group121Var1>>& values)
{
	return this->PrintSecStat(values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Binary>>& values)
{
	return this->PrintVQT(header.enumeration, values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<BinaryOutputStatus>>& values)
{
	return this->PrintVQT(header.enumeration, values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<DoubleBitBinary>>& values)
{
	auto stringify = [](DoubleBit db) -> const char*
	{
		return DoubleBitToString(db);
	};
	return this->PrintVQTStringify(header.enumeration, values, stringify);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Counter>>& values)
{
	return this->PrintVQT(header.enumeration, values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<FrozenCounter>>& values)
{
	return this->PrintVQT(header.enumeration, values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Analog>>& values)
{
	return this->PrintVQT(header.enumeration, values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputStatus>>& values)
{
	return this->PrintVQT(header.enumeration, values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<OctetString>>& values)
{
	return this->PrintOctets(values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<TimeAndInterval>>& values)
{
	return this->PrintTimeAndInterval(values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<BinaryCommandEvent>>& values)
{
	Indent i(*callbacks);
	const bool HAS_TIME = HasAbsoluteTime(header.enumeration);
	auto logItem = [this, HAS_TIME](const Indexed<BinaryCommandEvent>& item)
	{
		std::ostringstream oss;
		oss << "[" << item.index << "] - value: " << GetStringValue(item.value.value) << "  status: " << CommandStatusToString(item.value.status);
		if (HAS_TIME)
		{
			oss << " time: " << ToUTCString(item.value.time);
		}
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());
	};

	values.ForeachItem(logItem);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogCommandEvent>>& values)
{
	Indent i(*callbacks);
	const bool HAS_TIME = HasAbsoluteTime(header.enumeration);
	auto logItem = [this, HAS_TIME](const Indexed<AnalogCommandEvent>& item)
	{
		std::ostringstream oss;
		oss << "[" << item.index << "] - value: " << item.value.value << "  status: " << CommandStatusToString(item.value.status);
		if (HAS_TIME)
		{
			oss << " time: " << ToUTCString(item.value.time);
		}
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());
	};

	values.ForeachItem(logItem);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Group122Var1>>& values)
{
	return this->PrintSecStat(values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Group122Var2>>& values)
{
	Indent i(*callbacks);
	auto logItem = [this](const Indexed<Group122Var2>& item)
	{
		std::ostringstream oss;
		oss << "[" << item.index << "] - flags: 0x" << ToHex(item.value.flags);
		oss << " assoc: " << item.value.assocId;
		oss << " value: " << item.value.value;
		oss << " time: " << ToUTCString(item.value.time);
		SIMPLE_LOG_BLOCK(logger, flags::APP_OBJECT_RX, oss.str().c_str());
	};

	values.ForeachItem(logItem);

	return IINField::Empty();
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<ControlRelayOutputBlock>>& values)
{
	return PrintCrob(values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputInt16>>& values)
{
	return PrintAO(values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputInt32>>& values)
{
	return PrintAO(values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputFloat32>>& values)
{
	return PrintAO(values);
}

IINField LoggingHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputDouble64>>& values)
{
	return PrintAO(values);
}

}


