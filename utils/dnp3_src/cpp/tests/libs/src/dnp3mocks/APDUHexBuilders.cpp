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
#include "APDUHexBuilders.h"

#include <opendnp3/app/APDURequest.h>
#include <opendnp3/app/APDUResponse.h>
#include <opendnp3/app/APDUBuilders.h>
#include <opendnp3/app/AppConstants.h>

#include <opendnp3/objects/Group12.h>
#include <opendnp3/objects/Group120.h>

#include <openpal/container/Buffer.h>

#include <testlib/HexConversions.h>
#include <testlib/BufferHelpers.h>

using namespace std;
using namespace openpal;
using namespace opendnp3;
using namespace testlib;

namespace hex
{
std::string repeat(uint8_t value, uint16_t count)
{
	Buffer buffer(count);
	buffer.GetWSlice().SetAllTo(value);
	return ToHex(buffer.ToRSlice());
}

std::string ClassTask(FunctionCode fc, uint8_t seq, const ClassField& field)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDURequest request(buffer.GetWSlice());
	opendnp3::build::ClassRequest(request, fc, field, seq);
	return ToHex(request.ToRSlice());
}

std::string IntegrityPoll(uint8_t seq, const ClassField& field)
{
	return ClassTask(FunctionCode::READ, seq, field);
}

std::string ClassPoll(uint8_t seq, PointClass pc)
{
	return ClassTask(FunctionCode::READ, seq, ClassField(pc));
}

std::string EventPoll(uint8_t seq, const ClassField& field)
{
	return ClassTask(FunctionCode::READ, seq, field);
}

std::string ClearRestartIIN(uint8_t seq)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDURequest request(buffer.GetWSlice());
	build::ClearRestartIIN(request, seq);
	return ToHex(request.ToRSlice());
}

std::string MeasureDelay(uint8_t seq)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDURequest request(buffer.GetWSlice());
	build::MeasureDelay(request, seq);
	return ToHex(request.ToRSlice());
}

std::string Control(opendnp3::FunctionCode code, uint8_t seq, const opendnp3::ControlRelayOutputBlock& crob, uint16_t index)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDURequest request(buffer.GetWSlice());

	request.SetControl(AppControlField::Request(seq));
	request.SetFunction(code);

	auto writer = request.GetWriter();
	writer.WriteSingleIndexedValue<UInt16, ControlRelayOutputBlock>(QualifierCode::UINT16_CNT_UINT16_INDEX, Group12Var1::Inst(), crob, index);


	return ToHex(request.ToRSlice());
}

std::string EmptyResponse(uint8_t seq, const opendnp3::IINField& iin)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDUResponse response(buffer.GetWSlice());
	response.SetFunction(FunctionCode::RESPONSE);
	response.SetControl(AppControlField(true, true, false, false, seq));
	response.SetIIN(iin);
	return ToHex(response.ToRSlice());
}

std::string EmptyAuthResponse(uint8_t seq, const opendnp3::IINField& iin)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDUResponse response(buffer.GetWSlice());
	response.SetFunction(FunctionCode::AUTH_RESPONSE);
	response.SetControl(AppControlField(true, true, false, false, seq));
	response.SetIIN(iin);
	return ToHex(response.ToRSlice());
}

std::string NullUnsolicited(uint8_t seq, const IINField& iin)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDUResponse response(buffer.GetWSlice());
	build::NullUnsolicited(response, seq, iin);
	return ToHex(response.ToRSlice());
}

std::string SolicitedConfirm(uint8_t seq)
{
	return Confirm(seq, false);
}

std::string UnsolConfirm(uint8_t seq)
{
	return Confirm(seq, true);
}

std::string Confirm(uint8_t seq, bool unsol)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDURequest apdu(buffer.GetWSlice());
	apdu.SetControl(AppControlField(true, true, false, unsol, seq));
	apdu.SetFunction(FunctionCode::CONFIRM);
	return ToHex(apdu.ToRSlice());
}

// ----------- sec auth -------------

std::string RequestKeyStatus(uint8_t seq, uint16_t user)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDURequest apdu(buffer.GetWSlice());
	apdu.SetControl(AppControlField(true, true, false, false, seq));
	apdu.SetFunction(FunctionCode::AUTH_REQUEST);
	Group120Var4 status;
	status.userNum = user;
	apdu.GetWriter().WriteSingleValue<UInt8>(QualifierCode::UINT8_CNT, status);
	return ToHex(apdu.ToRSlice());
}

std::string AuthErrorResponse(
    opendnp3::IINField iin,
    uint8_t appSeq,
    uint32_t challengeSeqNum,
    uint16_t user,
    uint16_t assocId,
    opendnp3::AuthErrorCode code,
    opendnp3::DNPTime timestamp,
    std::string hexErrorText)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDUResponse apdu(buffer.GetWSlice());

	apdu.SetControl(AppControlField(true, true, false, false, appSeq));
	apdu.SetFunction(FunctionCode::AUTH_RESPONSE);
	apdu.SetIIN(iin);

	HexSequence hexErrorTextBuff(hexErrorText);

	Group120Var7 error(
	    challengeSeqNum,
	    user,
	    assocId,
	    code,
	    timestamp,
	    hexErrorTextBuff.ToRSlice()
	);

	apdu.GetWriter().WriteFreeFormat(error);

	return ToHex(apdu.ToRSlice());
}

std::string ChallengeResponse(
    opendnp3::IINField iin,
    uint8_t seq,
    uint32_t csq,
    uint16_t user,
    HMACType hmacType,
    ChallengeReason reason,
    std::string challengeDataHex
)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDUResponse apdu(buffer.GetWSlice());

	apdu.SetControl(AppControlField(true, true, false, false, seq));
	apdu.SetFunction(FunctionCode::AUTH_RESPONSE);
	apdu.SetIIN(iin);

	HexSequence challengeBuff(challengeDataHex);

	Group120Var1 rsp(
	    csq,
	    user,
	    hmacType,
	    reason,
	    challengeBuff.ToRSlice()
	);

	apdu.GetWriter().WriteFreeFormat(rsp);

	return ToHex(apdu.ToRSlice());
}

std::string ChallengeReply(
    uint8_t appSeq,
    uint32_t challengeSeqNum,
    uint16_t userNum,
    std::string hmacHex
)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDURequest apdu(buffer.GetWSlice());

	apdu.SetControl(AppControlField(true, true, false, false, appSeq));
	apdu.SetFunction(FunctionCode::AUTH_REQUEST);

	HexSequence hmacBuff(hmacHex);

	Group120Var2 rsp(challengeSeqNum, userNum, hmacBuff.ToRSlice());

	apdu.GetWriter().WriteFreeFormat(rsp);

	return ToHex(apdu.ToRSlice());
}

std::string KeyStatusResponse(
    IINField iin,
    uint8_t seq,
    uint32_t ksq,
    uint16_t user,
    KeyWrapAlgorithm keyWrap,
    KeyStatus status,
    HMACType hmacType,
    const std::string& challenge,
    const std::string& hmac
)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDUResponse apdu(buffer.GetWSlice());
	apdu.SetControl(AppControlField(true, true, false, false, seq));
	apdu.SetFunction(FunctionCode::AUTH_RESPONSE);
	apdu.SetIIN(iin);

	HexSequence challengeBuff(challenge);
	HexSequence hmacBuff(hmac);

	Group120Var5 rsp;
	rsp.keyChangeSeqNum = ksq;
	rsp.userNum = user;
	rsp.keyWrapAlgo = keyWrap;
	rsp.keyStatus = status;
	rsp.hmacAlgo = hmacType;
	rsp.challengeData = challengeBuff.ToRSlice();
	rsp.hmacValue = hmacBuff.ToRSlice();

	apdu.GetWriter().WriteFreeFormat(rsp);

	return ToHex(apdu.ToRSlice());
}

std::string KeyChangeRequest(
    uint8_t seq,
    uint32_t ksq,
    uint16_t user,
    const std::string& keyWrapData
)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDURequest apdu(buffer.GetWSlice());
	apdu.SetControl(AppControlField(true, true, false, false, seq));
	apdu.SetFunction(FunctionCode::AUTH_REQUEST);

	HexSequence keyBuffer(keyWrapData);

	Group120Var6 rsp;
	rsp.keyChangeSeqNum = ksq;
	rsp.userNum = user;
	rsp.keyWrapData = keyBuffer.ToRSlice();

	apdu.GetWriter().WriteFreeFormat(rsp);

	return ToHex(apdu.ToRSlice());
}

std::string UserStatusChangeRequest(
    uint8_t seq,
    opendnp3::KeyChangeMethod keyChangeMethod,
    opendnp3::UserOperation userOperation,
    uint32_t statusChangeSeqNum,
    uint16_t userRole,
    uint16_t userRoleExpDays,
    const std::string& userName,
    const std::string& userPublicKeyHex,
    const std::string& certificationDataHex
)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDURequest apdu(buffer.GetWSlice());
	apdu.SetControl(AppControlField(true, true, false, false, seq));
	apdu.SetFunction(FunctionCode::AUTH_REQUEST);

	RSlice name(reinterpret_cast<const uint8_t*>(userName.c_str()), static_cast<uint32_t>(userName.size()));
	HexSequence userPublicKeyBuffer(userPublicKeyHex);
	HexSequence certificationDataBuffer(certificationDataHex);

	Group120Var10 statusChange(
	    keyChangeMethod,
	    userOperation,
	    statusChangeSeqNum,
	    userRole,
	    userRoleExpDays,
	    name,
	    userPublicKeyBuffer.ToRSlice(),
	    certificationDataBuffer.ToRSlice()
	);


	apdu.GetWriter().WriteFreeFormat(statusChange);

	return ToHex(apdu.ToRSlice());
}

std::string BeginUpdateKeyChangeRequest(
    uint8_t seq,
    opendnp3::KeyChangeMethod method,
    const std::string& username,
    const std::string& masterChallenge
)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDURequest apdu(buffer.GetWSlice());
	apdu.SetControl(AppControlField(true, true, false, false, seq));
	apdu.SetFunction(FunctionCode::AUTH_REQUEST);

	RSlice name(reinterpret_cast<const uint8_t*>(username.c_str()), static_cast<uint32_t>(username.size()));
	HexSequence challenge(masterChallenge);

	apdu.GetWriter().WriteFreeFormat(Group120Var11(method, name, challenge));

	return ToHex(apdu.ToRSlice());
}

std::string BeginUpdateKeyChangeResponse(
    uint8_t seq,
    uint32_t ksq,
    uint16_t user,
    const std::string& outstationChallenge
)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDUResponse apdu(buffer.GetWSlice());
	apdu.SetControl(AppControlField(true, true, false, false, seq));
	apdu.SetFunction(FunctionCode::AUTH_RESPONSE);
	apdu.SetIIN(IINBit::DEVICE_RESTART);

	HexSequence challenge(outstationChallenge);

	apdu.GetWriter().WriteFreeFormat(Group120Var12(ksq, user, challenge));

	return ToHex(apdu.ToRSlice());
}

std::string FinishUpdateKeyChangeRequest(
    uint8_t seq,
    uint32_t ksq,
    uint16_t user,
    const std::string& encryptedData,
    const std::string& hmac
)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDURequest apdu(buffer.GetWSlice());
	apdu.SetControl(AppControlField(true, true, false, false, seq));
	apdu.SetFunction(FunctionCode::AUTH_REQUEST);

	HexSequence encryptedBuffer(encryptedData);
	HexSequence hmacBuffer(hmac);

	auto writer = apdu.GetWriter();

	writer.WriteFreeFormat(Group120Var13(ksq, user, encryptedBuffer.ToRSlice()));
	writer.WriteFreeFormat(Group120Var15(hmacBuffer.ToRSlice()));

	return ToHex(apdu.ToRSlice());
}

std::string FinishUpdateKeyChangeResponse(
    uint8_t seq,
    const std::string& hmac
)
{
	Buffer buffer(DEFAULT_MAX_APDU_SIZE);
	APDUResponse apdu(buffer.GetWSlice());
	apdu.SetControl(AppControlField(true, true, false, false, seq));
	apdu.SetFunction(FunctionCode::AUTH_RESPONSE);
	apdu.SetIIN(IINBit::DEVICE_RESTART);

	HexSequence hmacBuffer(hmac);

	apdu.GetWriter().WriteFreeFormat(Group120Var15(hmacBuffer.ToRSlice()));

	return ToHex(apdu.ToRSlice());
}

std::string KeyWrapData(
    uint16_t keyLengthBytes,
    uint8_t keyRepeatValue,
    std::string keyStatusMsg
)
{
	Buffer key(keyLengthBytes);
	key.GetWSlice().SetAllTo(keyRepeatValue);
	auto keyHex = ToHex(key.ToRSlice());
	HexSequence statusBuffer(keyStatusMsg);

	Buffer lengthBuff(2);
	auto lenDest = lengthBuff.GetWSlice();
	UInt16::WriteBuffer(lenDest, keyLengthBytes);
	auto lengthHex = ToHex(lengthBuff.ToRSlice());

	return AppendHex({lengthHex, keyHex, keyHex, keyStatusMsg});
}

}


