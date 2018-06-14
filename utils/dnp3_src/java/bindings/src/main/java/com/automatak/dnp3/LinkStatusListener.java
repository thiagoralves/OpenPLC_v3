/**
 * Copyright 2013-2016 Automatak, LLC
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
package com.automatak.dnp3;

import com.automatak.dnp3.enums.LinkStatus;

/**
 * Various informational callbacks that can be received from the link layer
 */
public interface LinkStatusListener
{
    /**
     * Called when a the reset/unreset status of the link layer changes
     * @param value the current state of the link layer
     */
    void onStateChange(LinkStatus value);

    /**
     * Called when a keep alive message is transmitted
     */
    void onKeepAliveInitiated();

    /**
     * Called when a keep alive message (request link status) receives no response
     */
    void onKeepAliveFailure();

    /**
     * Called when a keep alive message receives a valid response
     */
    void onKeepAliveSuccess();
}
