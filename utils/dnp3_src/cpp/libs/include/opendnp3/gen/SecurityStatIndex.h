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

#ifndef OPENDNP3_SECURITYSTATINDEX_H
#define OPENDNP3_SECURITYSTATINDEX_H

#include <cstdint>

namespace opendnp3 {

/**
  Indices of the SA security statistics
*/
enum class SecurityStatIndex : uint8_t
{
  /// The other device has responded with a message that was not the expected next step in the state machine.
  UNEXPECTED_MESSAGES = 0,
  /// The other device has replied with the correct authentication information, so the user is authentic, but the user is not authorized to perform the requested operation.
  AUTHORIZATION_FAILURES = 1,
  /// The other device has provided invalid authentication information such as an incorrect MAC
  AUTHENTICATION_FAILURES = 2,
  /// The other device has not replied within the configured time required
  REPLY_TIMEOUTS = 3,
  /// An Authentication Failure has occurred that causes the master station to change the session keys
  REKEYS_DUE_TO_AUTH_FAILUE = 4,
  /// The device sends an Application Layer fragment
  TOTAL_MESSAGES_TX = 5,
  /// The device receives an Application Layer fragment
  TOTAL_MESSAGES_RX = 6,
  /// The device receives a Challenge message or transmits an Aggressive Mode Request message
  CRITICAL_MESSAGES_TX = 7,
  /// The device transmits a Challenge message or receives an Aggressive Mode Request message
  CRITICAL_MESSAGES_RX = 8,
  /// The device discards a received message
  DISCARED_MESSAGES = 9,
  /// The device has sent a fragment containing an Error object indicating an authentication failure or potential configuration error
  ERROR_MESSAGES_TX = 10,
  /// The device has received an Error object
  ERROR_MESSAGES_RX = 11,
  /// The device successfully authenticates a message
  SUCCESSFUL_AUTHS = 12,
  /// A users session key is successfully changed
  SESSION_KEY_CHANGES = 13,
  /// A failure occurs while changing a session key
  FAILED_SESSION_KEY_CHANGES = 14,
  /// The master and authority change the Update Key for a user
  UPDATE_KEY_CHANGES = 15,
  /// The master and authority fail to change the Update Key for a user
  FAILED_UPDATE_KEY_CHANGES = 16,
  /// Only used by a master. Set to zero in outstations. The master rekeyed the session keys because the outstation restarted
  REKEYS_DUE_TO_RESTART = 17
};


}

#endif
