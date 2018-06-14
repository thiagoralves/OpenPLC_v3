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

#ifndef OPENDNP3_GROUP120_H
#define OPENDNP3_GROUP120_H

#include "opendnp3/app/GroupVariationID.h"
#include <openpal/container/RSlice.h>
#include <openpal/container/WSlice.h>
#include "opendnp3/app/DNPTime.h"
#include "opendnp3/app/IVariableLength.h"
#include "opendnp3/gen/HMACType.h"
#include "opendnp3/gen/ChallengeReason.h"
#include "opendnp3/gen/KeyWrapAlgorithm.h"
#include "opendnp3/gen/KeyStatus.h"
#include "opendnp3/gen/AuthErrorCode.h"
#include "opendnp3/gen/KeyChangeMethod.h"
#include "opendnp3/gen/CertificateType.h"
#include "opendnp3/gen/UserOperation.h"

namespace opendnp3 {

// Authentication - Challenge
struct Group120Var1 : public IVariableLength
{
  static GroupVariationID ID() { return GroupVariationID(120,1); }

  virtual GroupVariationID InstanceID() const override final { return ID(); }

  Group120Var1();

  Group120Var1(
    uint32_t challengeSeqNum,
    uint16_t userNum,
    HMACType hmacAlgo,
    ChallengeReason challengeReason,
    const openpal::RSlice& challengeData
  );

  virtual uint32_t Size() const override final;
  virtual bool Read(const openpal::RSlice&) override final;
  virtual bool Write(openpal::WSlice&) const override final;

  static const uint32_t MIN_SIZE = 8;

  // member variables
  uint32_t challengeSeqNum;
  uint16_t userNum;
  HMACType hmacAlgo;
  ChallengeReason challengeReason;
  openpal::RSlice challengeData;
};

// Authentication - Reply
struct Group120Var2 : public IVariableLength
{
  static GroupVariationID ID() { return GroupVariationID(120,2); }

  virtual GroupVariationID InstanceID() const override final { return ID(); }

  Group120Var2();

  Group120Var2(
    uint32_t challengeSeqNum,
    uint16_t userNum,
    const openpal::RSlice& hmacValue
  );

  virtual uint32_t Size() const override final;
  virtual bool Read(const openpal::RSlice&) override final;
  virtual bool Write(openpal::WSlice&) const override final;

  static const uint32_t MIN_SIZE = 6;

  // member variables
  uint32_t challengeSeqNum;
  uint16_t userNum;
  openpal::RSlice hmacValue;
};

// Authentication - Aggressive Mode Request
struct Group120Var3
{
  static GroupVariationID ID() { return GroupVariationID(120,3); }

  Group120Var3();

  static uint32_t Size() { return 6; }
  static bool Read(openpal::RSlice&, Group120Var3&);
  static bool Write(const Group120Var3&, openpal::WSlice&);

  uint32_t challengeSeqNum;
  uint16_t userNum;
};

// Authentication - Session Key Status Request
struct Group120Var4
{
  static GroupVariationID ID() { return GroupVariationID(120,4); }

  Group120Var4();

  static uint32_t Size() { return 2; }
  static bool Read(openpal::RSlice&, Group120Var4&);
  static bool Write(const Group120Var4&, openpal::WSlice&);

  uint16_t userNum;
};

// Authentication - Session Key Status
struct Group120Var5 : public IVariableLength
{
  static GroupVariationID ID() { return GroupVariationID(120,5); }

  virtual GroupVariationID InstanceID() const override final { return ID(); }

  Group120Var5();

  Group120Var5(
    uint32_t keyChangeSeqNum,
    uint16_t userNum,
    KeyWrapAlgorithm keyWrapAlgo,
    KeyStatus keyStatus,
    HMACType hmacAlgo,
    const openpal::RSlice& challengeData,
    const openpal::RSlice& hmacValue
  );

  virtual uint32_t Size() const override final;
  virtual bool Read(const openpal::RSlice&) override final;
  virtual bool Write(openpal::WSlice&) const override final;

  static const uint32_t MIN_SIZE = 11;

  // member variables
  uint32_t keyChangeSeqNum;
  uint16_t userNum;
  KeyWrapAlgorithm keyWrapAlgo;
  KeyStatus keyStatus;
  HMACType hmacAlgo;
  openpal::RSlice challengeData;
  openpal::RSlice hmacValue;
};

// Authentication - Session Key Change
struct Group120Var6 : public IVariableLength
{
  static GroupVariationID ID() { return GroupVariationID(120,6); }

  virtual GroupVariationID InstanceID() const override final { return ID(); }

  Group120Var6();

  Group120Var6(
    uint32_t keyChangeSeqNum,
    uint16_t userNum,
    const openpal::RSlice& keyWrapData
  );

  virtual uint32_t Size() const override final;
  virtual bool Read(const openpal::RSlice&) override final;
  virtual bool Write(openpal::WSlice&) const override final;

  static const uint32_t MIN_SIZE = 6;

  // member variables
  uint32_t keyChangeSeqNum;
  uint16_t userNum;
  openpal::RSlice keyWrapData;
};

// Authentication - Error
struct Group120Var7 : public IVariableLength
{
  static GroupVariationID ID() { return GroupVariationID(120,7); }

  virtual GroupVariationID InstanceID() const override final { return ID(); }

  Group120Var7();

  Group120Var7(
    uint32_t challengeSeqNum,
    uint16_t userNum,
    uint16_t assocId,
    AuthErrorCode errorCode,
    DNPTime time,
    const openpal::RSlice& errorText
  );

  virtual uint32_t Size() const override final;
  virtual bool Read(const openpal::RSlice&) override final;
  virtual bool Write(openpal::WSlice&) const override final;

  static const uint32_t MIN_SIZE = 15;

  // member variables
  uint32_t challengeSeqNum;
  uint16_t userNum;
  uint16_t assocId;
  AuthErrorCode errorCode;
  DNPTime time;
  openpal::RSlice errorText;
};

// Authentication - User Certificate
struct Group120Var8 : public IVariableLength
{
  static GroupVariationID ID() { return GroupVariationID(120,8); }

  virtual GroupVariationID InstanceID() const override final { return ID(); }

  Group120Var8();

  Group120Var8(
    KeyChangeMethod keyChangeMethod,
    CertificateType certificateType,
    const openpal::RSlice& certificate
  );

  virtual uint32_t Size() const override final;
  virtual bool Read(const openpal::RSlice&) override final;
  virtual bool Write(openpal::WSlice&) const override final;

  static const uint32_t MIN_SIZE = 2;

  // member variables
  KeyChangeMethod keyChangeMethod;
  CertificateType certificateType;
  openpal::RSlice certificate;
};

// Authentication - HMAC
struct Group120Var9 : public IVariableLength
{
  static GroupVariationID ID() { return GroupVariationID(120,9); }

  virtual GroupVariationID InstanceID() const override final { return ID(); }

  Group120Var9();

  explicit Group120Var9(
    const openpal::RSlice& hmacValue
  );

  virtual uint32_t Size() const override final;
  virtual bool Read(const openpal::RSlice&) override final;
  virtual bool Write(openpal::WSlice&) const override final;

  static const uint32_t MIN_SIZE = 0;

  // member variables
  openpal::RSlice hmacValue;
};

// Authentication - User Status Change
struct Group120Var10 : public IVariableLength
{
  static GroupVariationID ID() { return GroupVariationID(120,10); }

  virtual GroupVariationID InstanceID() const override final { return ID(); }

  Group120Var10();

  Group120Var10(
    KeyChangeMethod keyChangeMethod,
    UserOperation userOperation,
    uint32_t statusChangeSeqNum,
    uint16_t userRole,
    uint16_t userRoleExpDays,
    const openpal::RSlice& userName,
    const openpal::RSlice& userPublicKey,
    const openpal::RSlice& certificationData
  );

  virtual uint32_t Size() const override final;
  virtual bool Read(const openpal::RSlice&) override final;
  virtual bool Write(openpal::WSlice&) const override final;

  static const uint32_t MIN_SIZE = 16;

  // member variables
  KeyChangeMethod keyChangeMethod;
  UserOperation userOperation;
  uint32_t statusChangeSeqNum;
  uint16_t userRole;
  uint16_t userRoleExpDays;
  openpal::RSlice userName;
  openpal::RSlice userPublicKey;
  openpal::RSlice certificationData;
};

// Authentication - Update Key Change Request
struct Group120Var11 : public IVariableLength
{
  static GroupVariationID ID() { return GroupVariationID(120,11); }

  virtual GroupVariationID InstanceID() const override final { return ID(); }

  Group120Var11();

  Group120Var11(
    KeyChangeMethod keyChangeMethod,
    const openpal::RSlice& userName,
    const openpal::RSlice& challengeData
  );

  virtual uint32_t Size() const override final;
  virtual bool Read(const openpal::RSlice&) override final;
  virtual bool Write(openpal::WSlice&) const override final;

  static const uint32_t MIN_SIZE = 5;

  // member variables
  KeyChangeMethod keyChangeMethod;
  openpal::RSlice userName;
  openpal::RSlice challengeData;
};

// Authentication - Update Key Change Reply
struct Group120Var12 : public IVariableLength
{
  static GroupVariationID ID() { return GroupVariationID(120,12); }

  virtual GroupVariationID InstanceID() const override final { return ID(); }

  Group120Var12();

  Group120Var12(
    uint32_t keyChangeSeqNum,
    uint16_t userNum,
    const openpal::RSlice& challengeData
  );

  virtual uint32_t Size() const override final;
  virtual bool Read(const openpal::RSlice&) override final;
  virtual bool Write(openpal::WSlice&) const override final;

  static const uint32_t MIN_SIZE = 8;

  // member variables
  uint32_t keyChangeSeqNum;
  uint16_t userNum;
  openpal::RSlice challengeData;
};

// Authentication - Update Key Change
struct Group120Var13 : public IVariableLength
{
  static GroupVariationID ID() { return GroupVariationID(120,13); }

  virtual GroupVariationID InstanceID() const override final { return ID(); }

  Group120Var13();

  Group120Var13(
    uint32_t keyChangeSeqNum,
    uint16_t userNum,
    const openpal::RSlice& encryptedUpdateKey
  );

  virtual uint32_t Size() const override final;
  virtual bool Read(const openpal::RSlice&) override final;
  virtual bool Write(openpal::WSlice&) const override final;

  static const uint32_t MIN_SIZE = 8;

  // member variables
  uint32_t keyChangeSeqNum;
  uint16_t userNum;
  openpal::RSlice encryptedUpdateKey;
};

// Authentication - Update Key Change Signature
struct Group120Var14 : public IVariableLength
{
  static GroupVariationID ID() { return GroupVariationID(120,14); }

  virtual GroupVariationID InstanceID() const override final { return ID(); }

  Group120Var14();

  explicit Group120Var14(
    const openpal::RSlice& digitalSignature
  );

  virtual uint32_t Size() const override final;
  virtual bool Read(const openpal::RSlice&) override final;
  virtual bool Write(openpal::WSlice&) const override final;

  static const uint32_t MIN_SIZE = 0;

  // member variables
  openpal::RSlice digitalSignature;
};

// Authentication - Update Key Change Confirmation
struct Group120Var15 : public IVariableLength
{
  static GroupVariationID ID() { return GroupVariationID(120,15); }

  virtual GroupVariationID InstanceID() const override final { return ID(); }

  Group120Var15();

  explicit Group120Var15(
    const openpal::RSlice& hmacValue
  );

  virtual uint32_t Size() const override final;
  virtual bool Read(const openpal::RSlice&) override final;
  virtual bool Write(openpal::WSlice&) const override final;

  static const uint32_t MIN_SIZE = 0;

  // member variables
  openpal::RSlice hmacValue;
};


}

#endif
