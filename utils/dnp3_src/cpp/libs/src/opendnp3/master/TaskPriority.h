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
#ifndef OPENDNP3_TASKPRIORITY_H
#define OPENDNP3_TASKPRIORITY_H

namespace opendnp3
{
namespace priority
{
const int USER_STATUS_CHANGE = 20;

const int UPDATE_KEY_CHANGE = 40;

const int SESSION_KEY = 50;

const int COMMAND = 100;

const int USER_REQUEST = 110;

const int CLEAR_RESTART = 120;

const int DISABLE_UNSOLICITED = 130;

const int ASSIGN_CLASS = 140;

const int INTEGRITY_POLL = 150;

const int TIME_SYNC = 160;

const int ENABLE_UNSOLICITED = 170;

const int EVENT_SCAN = 180;

const int USER_POLL = 190;
}

}

#endif
