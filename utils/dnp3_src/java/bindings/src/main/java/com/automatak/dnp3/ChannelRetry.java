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

import java.time.Duration;


/**
 * Constrains the time range used for exponential backoff from some minimum to a maximum
 */
public class ChannelRetry
{
    /**
     * Construct a channel retry class
     * @param minRetryDelay minimum retry delay
     * @param maxRetryDelay maximum retry delay
     */
    public ChannelRetry(Duration minRetryDelay, Duration maxRetryDelay)
    {
        this.minRetryDelay = minRetryDelay;
        this.maxRetryDelay = maxRetryDelay;
    }

    /// <summary>
    /// Default configuration from 1 second to 1 minute
    /// </summary>
    public static ChannelRetry getDefault()
    {
        return new ChannelRetry(Duration.ofSeconds(1), Duration.ofMinutes(1));
    }

    public final Duration minRetryDelay;
    public final Duration maxRetryDelay;
}
