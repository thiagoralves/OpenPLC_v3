//
//  _   _         ______    _ _ _   _             _ _ _
// | \ | |       |  ____|  | (_) | (_)           | | | |
// |  \| | ___   | |__   __| |_| |_ _ _ __   __ _| | | |
// | . ` |/ _ \  |  __| / _` | | __| | '_ \ / _` | | | |
// | |\  | (_) | | |___| (_| | | |_| | | | | (_| |_|_|_|
// |_| \_|\___/  |______\__,_|_|\__|_|_| |_|\__, (_|_|_)
//                                           __/ |
//                                          |___/
// 
// This file is auto-generated. Do not edit manually
// 
// Copyright 2013 Automatak LLC
// 
// Automatak LLC (www.automatak.com) licenses this file
// to you under the the Apache License Version 2.0 (the "License"):
// 
// http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "Group120.h"

#include <openpal/serialization/Serialization.h>
#include "opendnp3/app/parsing/PrefixFields.h"
#include <openpal/serialization/Format.h>
#include <openpal/serialization/Parse.h>

using namespace openpal;

namespace opendnp3 {

// ------- Group120Var1 -------

Group120Var1::Group120Var1() : 
  challengeSeqNum(0), userNum(0), hmacAlgo(HMACType::UNKNOWN), challengeReason(ChallengeReason::UNKNOWN)
{}

Group120Var1::Group120Var1(
  uint32_t challengeSeqNum_,
  uint16_t userNum_,
  HMACType hmacAlgo_,
  ChallengeReason challengeReason_,
  const openpal::RSlice& challengeData_
) : 
  challengeSeqNum(challengeSeqNum_),
  userNum(userNum_),
  hmacAlgo(hmacAlgo_),
  challengeReason(challengeReason_),
  challengeData(challengeData_)
{}

uint32_t Group120Var1::Size() const
{
  return MIN_SIZE + challengeData.Size();
}

bool Group120Var1::Read(const RSlice& buffer)
{
  if(buffer.Size() < Group120Var1::MIN_SIZE)
  {
    return false;
  }

  RSlice copy(buffer); //mutable copy for parsing

  this->challengeSeqNum = UInt32::ReadBuffer(copy);
  this->userNum = UInt16::ReadBuffer(copy);
  this->hmacAlgo = HMACTypeFromType(UInt8::ReadBuffer(copy));
  this->challengeReason = ChallengeReasonFromType(UInt8::ReadBuffer(copy));

  this->challengeData = copy; // whatever is left over
  return true;
}

bool Group120Var1::Write(openpal::WSlice& buffer) const
{
  if(this->Size() > buffer.Size())
  {
    return false;
  }

  UInt32::WriteBuffer(buffer, this->challengeSeqNum);
  UInt16::WriteBuffer(buffer, this->userNum);
  UInt8::WriteBuffer(buffer, HMACTypeToType(this->hmacAlgo));
  UInt8::WriteBuffer(buffer, ChallengeReasonToType(this->challengeReason));

  challengeData.CopyTo(buffer);
  return true;
}

// ------- Group120Var2 -------

Group120Var2::Group120Var2() : 
  challengeSeqNum(0), userNum(0)
{}

Group120Var2::Group120Var2(
  uint32_t challengeSeqNum_,
  uint16_t userNum_,
  const openpal::RSlice& hmacValue_
) : 
  challengeSeqNum(challengeSeqNum_),
  userNum(userNum_),
  hmacValue(hmacValue_)
{}

uint32_t Group120Var2::Size() const
{
  return MIN_SIZE + hmacValue.Size();
}

bool Group120Var2::Read(const RSlice& buffer)
{
  if(buffer.Size() < Group120Var2::MIN_SIZE)
  {
    return false;
  }

  RSlice copy(buffer); //mutable copy for parsing

  this->challengeSeqNum = UInt32::ReadBuffer(copy);
  this->userNum = UInt16::ReadBuffer(copy);

  this->hmacValue = copy; // whatever is left over
  return true;
}

bool Group120Var2::Write(openpal::WSlice& buffer) const
{
  if(this->Size() > buffer.Size())
  {
    return false;
  }

  UInt32::WriteBuffer(buffer, this->challengeSeqNum);
  UInt16::WriteBuffer(buffer, this->userNum);

  hmacValue.CopyTo(buffer);
  return true;
}

// ------- Group120Var3 -------

Group120Var3::Group120Var3() : challengeSeqNum(0), userNum(0)
{}

bool Group120Var3::Read(RSlice& buffer, Group120Var3& output)
{
  return Parse::Many(buffer, output.challengeSeqNum, output.userNum);
}

bool Group120Var3::Write(const Group120Var3& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.challengeSeqNum, arg.userNum);
}

// ------- Group120Var4 -------

Group120Var4::Group120Var4() : userNum(0)
{}

bool Group120Var4::Read(RSlice& buffer, Group120Var4& output)
{
  return Parse::Many(buffer, output.userNum);
}

bool Group120Var4::Write(const Group120Var4& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.userNum);
}

// ------- Group120Var5 -------

Group120Var5::Group120Var5() : 
  keyChangeSeqNum(0), userNum(0), keyWrapAlgo(KeyWrapAlgorithm::UNDEFINED), keyStatus(KeyStatus::UNDEFINED), hmacAlgo(HMACType::UNKNOWN)
{}

Group120Var5::Group120Var5(
  uint32_t keyChangeSeqNum_,
  uint16_t userNum_,
  KeyWrapAlgorithm keyWrapAlgo_,
  KeyStatus keyStatus_,
  HMACType hmacAlgo_,
  const openpal::RSlice& challengeData_,
  const openpal::RSlice& hmacValue_
) : 
  keyChangeSeqNum(keyChangeSeqNum_),
  userNum(userNum_),
  keyWrapAlgo(keyWrapAlgo_),
  keyStatus(keyStatus_),
  hmacAlgo(hmacAlgo_),
  challengeData(challengeData_),
  hmacValue(hmacValue_)
{}

uint32_t Group120Var5::Size() const
{
  return MIN_SIZE + challengeData.Size() + hmacValue.Size();
}

bool Group120Var5::Read(const RSlice& buffer)
{
  if(buffer.Size() < Group120Var5::MIN_SIZE)
  {
    return false;
  }

  RSlice copy(buffer); //mutable copy for parsing

  this->keyChangeSeqNum = UInt32::ReadBuffer(copy);
  this->userNum = UInt16::ReadBuffer(copy);
  this->keyWrapAlgo = KeyWrapAlgorithmFromType(UInt8::ReadBuffer(copy));
  this->keyStatus = KeyStatusFromType(UInt8::ReadBuffer(copy));
  this->hmacAlgo = HMACTypeFromType(UInt8::ReadBuffer(copy));

  if(!PrefixFields::Read(copy, challengeData))
  {
    return false;
  }

  this->hmacValue = copy; // whatever is left over
  return true;
}

bool Group120Var5::Write(openpal::WSlice& buffer) const
{
  if(this->Size() > buffer.Size())
  {
    return false;
  }

  // All of the fields that have a uint16_t length must have the proper size
  if(!PrefixFields::LengthFitsInUInt16(challengeData))
  {
    return false;
  }

  UInt32::WriteBuffer(buffer, this->keyChangeSeqNum);
  UInt16::WriteBuffer(buffer, this->userNum);
  UInt8::WriteBuffer(buffer, KeyWrapAlgorithmToType(this->keyWrapAlgo));
  UInt8::WriteBuffer(buffer, KeyStatusToType(this->keyStatus));
  UInt8::WriteBuffer(buffer, HMACTypeToType(this->hmacAlgo));

  if(!PrefixFields::Write(buffer, challengeData))
  {
    return false;
  }

  hmacValue.CopyTo(buffer);
  return true;
}

// ------- Group120Var6 -------

Group120Var6::Group120Var6() : 
  keyChangeSeqNum(0), userNum(0)
{}

Group120Var6::Group120Var6(
  uint32_t keyChangeSeqNum_,
  uint16_t userNum_,
  const openpal::RSlice& keyWrapData_
) : 
  keyChangeSeqNum(keyChangeSeqNum_),
  userNum(userNum_),
  keyWrapData(keyWrapData_)
{}

uint32_t Group120Var6::Size() const
{
  return MIN_SIZE + keyWrapData.Size();
}

bool Group120Var6::Read(const RSlice& buffer)
{
  if(buffer.Size() < Group120Var6::MIN_SIZE)
  {
    return false;
  }

  RSlice copy(buffer); //mutable copy for parsing

  this->keyChangeSeqNum = UInt32::ReadBuffer(copy);
  this->userNum = UInt16::ReadBuffer(copy);

  this->keyWrapData = copy; // whatever is left over
  return true;
}

bool Group120Var6::Write(openpal::WSlice& buffer) const
{
  if(this->Size() > buffer.Size())
  {
    return false;
  }

  UInt32::WriteBuffer(buffer, this->keyChangeSeqNum);
  UInt16::WriteBuffer(buffer, this->userNum);

  keyWrapData.CopyTo(buffer);
  return true;
}

// ------- Group120Var7 -------

Group120Var7::Group120Var7() : 
  challengeSeqNum(0), userNum(0), assocId(0), errorCode(AuthErrorCode::UNKNOWN), time(0)
{}

Group120Var7::Group120Var7(
  uint32_t challengeSeqNum_,
  uint16_t userNum_,
  uint16_t assocId_,
  AuthErrorCode errorCode_,
  DNPTime time_,
  const openpal::RSlice& errorText_
) : 
  challengeSeqNum(challengeSeqNum_),
  userNum(userNum_),
  assocId(assocId_),
  errorCode(errorCode_),
  time(time_),
  errorText(errorText_)
{}

uint32_t Group120Var7::Size() const
{
  return MIN_SIZE + errorText.Size();
}

bool Group120Var7::Read(const RSlice& buffer)
{
  if(buffer.Size() < Group120Var7::MIN_SIZE)
  {
    return false;
  }

  RSlice copy(buffer); //mutable copy for parsing

  this->challengeSeqNum = UInt32::ReadBuffer(copy);
  this->userNum = UInt16::ReadBuffer(copy);
  this->assocId = UInt16::ReadBuffer(copy);
  this->errorCode = AuthErrorCodeFromType(UInt8::ReadBuffer(copy));
  this->time = UInt48::ReadBuffer(copy);

  this->errorText = copy; // whatever is left over
  return true;
}

bool Group120Var7::Write(openpal::WSlice& buffer) const
{
  if(this->Size() > buffer.Size())
  {
    return false;
  }

  UInt32::WriteBuffer(buffer, this->challengeSeqNum);
  UInt16::WriteBuffer(buffer, this->userNum);
  UInt16::WriteBuffer(buffer, this->assocId);
  UInt8::WriteBuffer(buffer, AuthErrorCodeToType(this->errorCode));
  UInt48::WriteBuffer(buffer, this->time);

  errorText.CopyTo(buffer);
  return true;
}

// ------- Group120Var8 -------

Group120Var8::Group120Var8() : 
  keyChangeMethod(KeyChangeMethod::UNDEFINED), certificateType(CertificateType::UNKNOWN)
{}

Group120Var8::Group120Var8(
  KeyChangeMethod keyChangeMethod_,
  CertificateType certificateType_,
  const openpal::RSlice& certificate_
) : 
  keyChangeMethod(keyChangeMethod_),
  certificateType(certificateType_),
  certificate(certificate_)
{}

uint32_t Group120Var8::Size() const
{
  return MIN_SIZE + certificate.Size();
}

bool Group120Var8::Read(const RSlice& buffer)
{
  if(buffer.Size() < Group120Var8::MIN_SIZE)
  {
    return false;
  }

  RSlice copy(buffer); //mutable copy for parsing

  this->keyChangeMethod = KeyChangeMethodFromType(UInt8::ReadBuffer(copy));
  this->certificateType = CertificateTypeFromType(UInt8::ReadBuffer(copy));

  this->certificate = copy; // whatever is left over
  return true;
}

bool Group120Var8::Write(openpal::WSlice& buffer) const
{
  if(this->Size() > buffer.Size())
  {
    return false;
  }

  UInt8::WriteBuffer(buffer, KeyChangeMethodToType(this->keyChangeMethod));
  UInt8::WriteBuffer(buffer, CertificateTypeToType(this->certificateType));

  certificate.CopyTo(buffer);
  return true;
}

// ------- Group120Var9 -------

Group120Var9::Group120Var9()
{}

Group120Var9::Group120Var9(
  const openpal::RSlice& hmacValue_
) : 
  hmacValue(hmacValue_)
{}

uint32_t Group120Var9::Size() const
{
  return MIN_SIZE + hmacValue.Size();
}

bool Group120Var9::Read(const RSlice& buffer)
{
  this->hmacValue = buffer; // the object is just the remainder field
  return true;
}

bool Group120Var9::Write(openpal::WSlice& buffer) const
{
  if(this->Size() > buffer.Size())
  {
    return false;
  }

  hmacValue.CopyTo(buffer);
  return true;
}

// ------- Group120Var10 -------

Group120Var10::Group120Var10() : 
  keyChangeMethod(KeyChangeMethod::UNDEFINED), userOperation(UserOperation::OP_UNDEFINED), statusChangeSeqNum(0), userRole(0), userRoleExpDays(0)
{}

Group120Var10::Group120Var10(
  KeyChangeMethod keyChangeMethod_,
  UserOperation userOperation_,
  uint32_t statusChangeSeqNum_,
  uint16_t userRole_,
  uint16_t userRoleExpDays_,
  const openpal::RSlice& userName_,
  const openpal::RSlice& userPublicKey_,
  const openpal::RSlice& certificationData_
) : 
  keyChangeMethod(keyChangeMethod_),
  userOperation(userOperation_),
  statusChangeSeqNum(statusChangeSeqNum_),
  userRole(userRole_),
  userRoleExpDays(userRoleExpDays_),
  userName(userName_),
  userPublicKey(userPublicKey_),
  certificationData(certificationData_)
{}

uint32_t Group120Var10::Size() const
{
  return MIN_SIZE + userName.Size() + userPublicKey.Size() + certificationData.Size();
}

bool Group120Var10::Read(const RSlice& buffer)
{
  if(buffer.Size() < Group120Var10::MIN_SIZE)
  {
    return false;
  }

  RSlice copy(buffer); //mutable copy for parsing

  this->keyChangeMethod = KeyChangeMethodFromType(UInt8::ReadBuffer(copy));
  this->userOperation = UserOperationFromType(UInt8::ReadBuffer(copy));
  this->statusChangeSeqNum = UInt32::ReadBuffer(copy);
  this->userRole = UInt16::ReadBuffer(copy);
  this->userRoleExpDays = UInt16::ReadBuffer(copy);

  if(!PrefixFields::Read(copy, userName, userPublicKey, certificationData))
  {
    return false;
  }

  // object does not have a remainder field so it should be fully consumed
  // The header length disagrees with object encoding so abort
  if(copy.IsNotEmpty())
  {
    return false;
  }

  return true;
}

bool Group120Var10::Write(openpal::WSlice& buffer) const
{
  if(this->Size() > buffer.Size())
  {
    return false;
  }

  // All of the fields that have a uint16_t length must have the proper size
  if(!PrefixFields::LengthFitsInUInt16(userName, userPublicKey, certificationData))
  {
    return false;
  }

  UInt8::WriteBuffer(buffer, KeyChangeMethodToType(this->keyChangeMethod));
  UInt8::WriteBuffer(buffer, UserOperationToType(this->userOperation));
  UInt32::WriteBuffer(buffer, this->statusChangeSeqNum);
  UInt16::WriteBuffer(buffer, this->userRole);
  UInt16::WriteBuffer(buffer, this->userRoleExpDays);

  if(!PrefixFields::Write(buffer, userName, userPublicKey, certificationData))
  {
    return false;
  }

  return true;
}

// ------- Group120Var11 -------

Group120Var11::Group120Var11() : 
  keyChangeMethod(KeyChangeMethod::UNDEFINED)
{}

Group120Var11::Group120Var11(
  KeyChangeMethod keyChangeMethod_,
  const openpal::RSlice& userName_,
  const openpal::RSlice& challengeData_
) : 
  keyChangeMethod(keyChangeMethod_),
  userName(userName_),
  challengeData(challengeData_)
{}

uint32_t Group120Var11::Size() const
{
  return MIN_SIZE + userName.Size() + challengeData.Size();
}

bool Group120Var11::Read(const RSlice& buffer)
{
  if(buffer.Size() < Group120Var11::MIN_SIZE)
  {
    return false;
  }

  RSlice copy(buffer); //mutable copy for parsing

  this->keyChangeMethod = KeyChangeMethodFromType(UInt8::ReadBuffer(copy));

  if(!PrefixFields::Read(copy, userName, challengeData))
  {
    return false;
  }

  // object does not have a remainder field so it should be fully consumed
  // The header length disagrees with object encoding so abort
  if(copy.IsNotEmpty())
  {
    return false;
  }

  return true;
}

bool Group120Var11::Write(openpal::WSlice& buffer) const
{
  if(this->Size() > buffer.Size())
  {
    return false;
  }

  // All of the fields that have a uint16_t length must have the proper size
  if(!PrefixFields::LengthFitsInUInt16(userName, challengeData))
  {
    return false;
  }

  UInt8::WriteBuffer(buffer, KeyChangeMethodToType(this->keyChangeMethod));

  if(!PrefixFields::Write(buffer, userName, challengeData))
  {
    return false;
  }

  return true;
}

// ------- Group120Var12 -------

Group120Var12::Group120Var12() : 
  keyChangeSeqNum(0), userNum(0)
{}

Group120Var12::Group120Var12(
  uint32_t keyChangeSeqNum_,
  uint16_t userNum_,
  const openpal::RSlice& challengeData_
) : 
  keyChangeSeqNum(keyChangeSeqNum_),
  userNum(userNum_),
  challengeData(challengeData_)
{}

uint32_t Group120Var12::Size() const
{
  return MIN_SIZE + challengeData.Size();
}

bool Group120Var12::Read(const RSlice& buffer)
{
  if(buffer.Size() < Group120Var12::MIN_SIZE)
  {
    return false;
  }

  RSlice copy(buffer); //mutable copy for parsing

  this->keyChangeSeqNum = UInt32::ReadBuffer(copy);
  this->userNum = UInt16::ReadBuffer(copy);

  if(!PrefixFields::Read(copy, challengeData))
  {
    return false;
  }

  // object does not have a remainder field so it should be fully consumed
  // The header length disagrees with object encoding so abort
  if(copy.IsNotEmpty())
  {
    return false;
  }

  return true;
}

bool Group120Var12::Write(openpal::WSlice& buffer) const
{
  if(this->Size() > buffer.Size())
  {
    return false;
  }

  // All of the fields that have a uint16_t length must have the proper size
  if(!PrefixFields::LengthFitsInUInt16(challengeData))
  {
    return false;
  }

  UInt32::WriteBuffer(buffer, this->keyChangeSeqNum);
  UInt16::WriteBuffer(buffer, this->userNum);

  if(!PrefixFields::Write(buffer, challengeData))
  {
    return false;
  }

  return true;
}

// ------- Group120Var13 -------

Group120Var13::Group120Var13() : 
  keyChangeSeqNum(0), userNum(0)
{}

Group120Var13::Group120Var13(
  uint32_t keyChangeSeqNum_,
  uint16_t userNum_,
  const openpal::RSlice& encryptedUpdateKey_
) : 
  keyChangeSeqNum(keyChangeSeqNum_),
  userNum(userNum_),
  encryptedUpdateKey(encryptedUpdateKey_)
{}

uint32_t Group120Var13::Size() const
{
  return MIN_SIZE + encryptedUpdateKey.Size();
}

bool Group120Var13::Read(const RSlice& buffer)
{
  if(buffer.Size() < Group120Var13::MIN_SIZE)
  {
    return false;
  }

  RSlice copy(buffer); //mutable copy for parsing

  this->keyChangeSeqNum = UInt32::ReadBuffer(copy);
  this->userNum = UInt16::ReadBuffer(copy);

  if(!PrefixFields::Read(copy, encryptedUpdateKey))
  {
    return false;
  }

  // object does not have a remainder field so it should be fully consumed
  // The header length disagrees with object encoding so abort
  if(copy.IsNotEmpty())
  {
    return false;
  }

  return true;
}

bool Group120Var13::Write(openpal::WSlice& buffer) const
{
  if(this->Size() > buffer.Size())
  {
    return false;
  }

  // All of the fields that have a uint16_t length must have the proper size
  if(!PrefixFields::LengthFitsInUInt16(encryptedUpdateKey))
  {
    return false;
  }

  UInt32::WriteBuffer(buffer, this->keyChangeSeqNum);
  UInt16::WriteBuffer(buffer, this->userNum);

  if(!PrefixFields::Write(buffer, encryptedUpdateKey))
  {
    return false;
  }

  return true;
}

// ------- Group120Var14 -------

Group120Var14::Group120Var14()
{}

Group120Var14::Group120Var14(
  const openpal::RSlice& digitalSignature_
) : 
  digitalSignature(digitalSignature_)
{}

uint32_t Group120Var14::Size() const
{
  return MIN_SIZE + digitalSignature.Size();
}

bool Group120Var14::Read(const RSlice& buffer)
{
  this->digitalSignature = buffer; // the object is just the remainder field
  return true;
}

bool Group120Var14::Write(openpal::WSlice& buffer) const
{
  if(this->Size() > buffer.Size())
  {
    return false;
  }

  digitalSignature.CopyTo(buffer);
  return true;
}

// ------- Group120Var15 -------

Group120Var15::Group120Var15()
{}

Group120Var15::Group120Var15(
  const openpal::RSlice& hmacValue_
) : 
  hmacValue(hmacValue_)
{}

uint32_t Group120Var15::Size() const
{
  return MIN_SIZE + hmacValue.Size();
}

bool Group120Var15::Read(const RSlice& buffer)
{
  this->hmacValue = buffer; // the object is just the remainder field
  return true;
}

bool Group120Var15::Write(openpal::WSlice& buffer) const
{
  if(this->Size() > buffer.Size())
  {
    return false;
  }

  hmacValue.CopyTo(buffer);
  return true;
}


}
