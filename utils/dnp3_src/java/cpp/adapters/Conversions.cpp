/**
 * Copyright 2013 Automatak, LLC
 *
 * Licensed to Automatak, LLC (www.automatak.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership. Automatak, LLC
 * licenses this file to you under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include "Conversions.h"

LocalRef<jobject> Conversions::ConvertStackStatistics(JNIEnv* env, const opendnp3::StackStatistics& stats)
{
	auto link = jni::JCache::LinkLayerStatistics.init4(
	                env,
	                stats.link.numUnexpectedFrame,
	                stats.link.numBadMasterBit,
	                stats.link.numUnknownDestination,
	                stats.link.numUnknownSource
	            );

	auto transport = jni::JCache::TransportStatistics.init6(
	                     env,
	                     stats.transport.rx.numTransportRx,
	                     stats.transport.tx.numTransportTx,
	                     stats.transport.rx.numTransportErrorRx,
	                     stats.transport.rx.numTransportBufferOverflow,
	                     stats.transport.rx.numTransportDiscard,
	                     stats.transport.rx.numTransportIgnore
	                 );

	return jni::JCache::StackStatistics.init2(env, link, transport);
}


