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
package com.automatak.dnp3.impl.mocks;

import com.automatak.dnp3.ChannelListener;
import com.automatak.dnp3.enums.ChannelState;

import java.time.Duration;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;


public class BlockingChannelListener implements ChannelListener {

    BlockingQueue<ChannelState> states = new LinkedBlockingQueue<>();

    public void waitFor(ChannelState state, Duration duration) {
        try {
            ChannelState cs = states.poll(duration.toMillis(), TimeUnit.MILLISECONDS);
            if(cs == null) throw new RuntimeException("Timeout waiting for state: " + state);
            if(cs != state) waitFor(state, duration);
        }
        catch(InterruptedException ex)
        {
            throw new RuntimeException(ex);
        }
    }

    @Override
    public void onStateChange(ChannelState state)
    {
        states.add(state);
    }

}
