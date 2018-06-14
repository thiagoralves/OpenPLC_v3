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
#ifndef __APDU_HEX_BUILDERS_H_
#define __APDU_HEX_BUILDERS_H_

#include <string>
#include <cstdint>

#include <opendnp3/app/DNPTime.h>

#include <opendnp3/app/ClassField.h>
#include <opendnp3/app/IINField.h>
#include <opendnp3/app/ControlRelayOutputBlock.h>

#include <opendnp3/gen/PointClass.h>
#include <opendnp3/gen/FunctionCode.h>
#include <opendnp3/gen/KeyWrapAlgorithm.h>
#include <opendnp3/gen/KeyStatus.h>
#include <opendnp3/gen/HMACType.h>
#include <opendnp3/gen/ChallengeReason.h>
#include <opendnp3/gen/AuthErrorCode.h>
#include <opendnp3/gen/KeyChangeMethod.h>
#include <opendnp3/gen/UserOperation.h>
#include <opendnp3/gen/UserRole.h>


namespace hex
{
// ----------- simple helpers --------

std::string repeat(uint8_t value, uint16_t count);

// ----------- requests --------------

std::string IntegrityPoll(uint8_t seq, const opendnp3::ClassField& field = opendnp3::ClassField::AllClasses());

std::string ClassTask(opendnp3::FunctionCode fc, uint8_t seq, const opendnp3::ClassField& field = opendnp3::ClassField::AllClasses());

std::string ClassPoll(uint8_t seq, opendnp3::PointClass pc);

std::string EventPoll(uint8_t seq, const opendnp3::ClassField& field = opendnp3::ClassField::AllEventClasses());

std::string ClearRestartIIN(uint8_t seq);

std::string MeasureDelay(uint8_t seq);

std::string Control(opendnp3::FunctionCode code, uint8_t seq, const opendnp3::ControlRelayOutputBlock& crob, uint16_t index);

// ----------- responses --------------

std::string EmptyResponse(uint8_t seq, const opendnp3::IINField& iin = opendnp3::IINField::Empty());

std::string EmptyAuthResponse(uint8_t seq, const opendnp3::IINField& iin = opendnp3::IINField::Empty());

std::string NullUnsolicited(uint8_t seq, const opendnp3::IINField& iin = opendnp3::IINField(opendnp3::IINBit::DEVICE_RESTART));

// ----------- confirms --------------

std::string SolicitedConfirm(uint8_t seq);

std::string UnsolConfirm(uint8_t seq);

std::string Confirm(uint8_t seq, bool unsol);

// ----------- sec auth -------------

std::string RequestKeyStatus(uint8_t seq, uint16_t user);

std::string AuthErrorResponse(
    opendnp3::IINField iin,
    uint8_t appSeq,
    uint32_t challengeSeqNum,
    uint16_t user,
    uint16_t assocId,
    opendnp3::AuthErrorCode code,
    opendnp3::DNPTime timestamp,
    std::string hexText);

std::string ChallengeResponse(
    opendnp3::IINField iin,
    uint8_t seq,
    uint32_t csq,
    uint16_t user,
    opendnp3::HMACType hmacType,
    opendnp3::ChallengeReason reason,
    std::string challengeDataHex
);

std::string ChallengeReply(
    uint8_t appSeq,
    uint32_t challengeSeqNum,
    uint16_t userNum,
    std::string hmacHex
);

std::string KeyStatusResponse(
    opendnp3::IINField iin,
    uint8_t seq,
    uint32_t ksq,
    uint16_t user,
    opendnp3::KeyWrapAlgorithm keyWrap,
    opendnp3::KeyStatus status,
    opendnp3::HMACType,
    const std::string& challenge,
    const std::string& hmac
);

std::string KeyChangeRequest(
    uint8_t seq,
    uint32_t ksq,
    uint16_t user,
    const std::string& keyWrapData
);

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
);

std::string BeginUpdateKeyChangeRequest(
    uint8_t seq,
    opendnp3::KeyChangeMethod keyChangeMethod,
    const std::string& username,
    const std::string& masterChallenge
);

std::string BeginUpdateKeyChangeResponse(
    uint8_t seq,
    uint32_t ksq,
    uint16_t user,
    const std::string& outstationChallenge
);

std::string FinishUpdateKeyChangeRequest(
    uint8_t seq,
    uint32_t ksq,
    uint16_t user,
    const std::string& encryptedData,
    const std::string& hmac
);

std::string FinishUpdateKeyChangeResponse(
    uint8_t seq,
    const std::string& hmac
);

std::string KeyWrapData(
    uint16_t keyLengthBytes,
    uint8_t keyRepeatValue,
    std::string keyStatusMsg
);


}

#endif

