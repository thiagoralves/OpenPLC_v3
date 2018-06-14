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


import com.automatak.dnp3.*;
import com.automatak.dnp3.enums.ChannelState;
import com.automatak.dnp3.enums.DoubleBit;
import com.automatak.dnp3.enums.EventMode;
import com.automatak.dnp3.mock.DefaultMasterApplication;
import com.automatak.dnp3.mock.DefaultOutstationApplication;
import com.automatak.dnp3.mock.SuccessCommandHandler;

import java.time.Duration;
import java.util.ArrayDeque;
import java.util.List;
import java.util.Queue;
import java.util.Random;
import java.util.concurrent.TimeUnit;

public class StackPair {

    public static final int LEVELS = LogLevels.INFO;

    final int NUM_POINTS_PER_TYPE;
    final int EVENTS_PER_ITERATION;

    static OutstationStackConfig getOutstationConfig(int numPointsPerType, int eventBufferSize)
    {
        OutstationStackConfig config = new OutstationStackConfig(
                DatabaseConfig.allValues(numPointsPerType), EventBufferConfig.allTypes(eventBufferSize)
        );

        config.outstationConfig.unsolRetryTimeout = Duration.ofSeconds(1);

        config.outstationConfig.allowUnsolicited = true;

        return config;
    }

    static MasterStackConfig getMasterStackConfig()
    {
        MasterStackConfig config = new MasterStackConfig();

        config.master.responseTimeout = Duration.ofSeconds(1);
        config.master.taskRetryPeriod = Duration.ofSeconds(1);


        config.master.disableUnsolOnStartup = false;
        config.master.startupIntegrityClassMask = ClassField.none();
        config.master.unsolClassMask = ClassField.allEventClasses();

        return config;
    }

    public StackPair(DNP3Manager manager, int port, int numPointsPerType, int eventsPerIteration)
    {
        this.NUM_POINTS_PER_TYPE = numPointsPerType;
        this.EVENTS_PER_ITERATION = eventsPerIteration;

        try {

            Channel client = manager.addTCPClient(
                    String.format("client:%d", port),
                    LEVELS,
                    ChannelRetry.getDefault(),
                    "127.0.0.1",
                    "127.0.0.1",
                    port,
                    clientListener);

            Channel server = manager.addTCPServer(
                    String.format("server:%d", port),
                    LEVELS | LogLevels.APP_HEADER_TX | LogLevels.APP_OBJECT_TX,
                    ChannelRetry.getDefault(),
                    "127.0.0.1",
                    port,
                    serverListener);

            this.master = client.addMaster(
                    String.format("master:%d", port),
                    this.soeHandler,
                    DefaultMasterApplication.getInstance(),
                    getMasterStackConfig());

            this.outstation = server.addOutstation(
                    String.format("outstation:%d", port),
                    SuccessCommandHandler.getInstance(),
                    DefaultOutstationApplication.getInstance(),
                    getOutstationConfig(numPointsPerType, 2*eventsPerIteration));
        }
        catch(DNP3Exception ex)
        {
            throw new RuntimeException(ex);
        }

        this.outstation.enable();
        this.master.enable();
    }

    public void waitForChannelsOpen(Duration duration)
    {
        this.clientListener.waitFor(ChannelState.OPEN, duration);
        this.serverListener.waitFor(ChannelState.OPEN, duration);
    }

    // transmit some random values via the outstation
    public int sendRandomValues()
    {
        final OutstationChangeSet set = new OutstationChangeSet();

        for(int i = 0; i < this.EVENTS_PER_ITERATION; ++i)
        {

           ExpectedValue value = this.addRandomValue(set);
           this.sentValues.add(value);
        }

        this.outstation.apply(set);

        return this.EVENTS_PER_ITERATION;
    }

    public void awaitSentValues(Duration duration)
    {
        final int total = sentValues.size();

        List<ExpectedValue> receivedValues = soeHandler.waitForValues(duration);

        if(receivedValues == null)
        {
            throw new RuntimeException("No values received within timeout");
        }

        if(receivedValues.size() != sentValues.size())
        {
            throw new RuntimeException(String.format("# sent (%d) != # received (%d)", total, receivedValues.size()));
        }

        int numValidated = 0;

        for(ExpectedValue received : receivedValues)
        {
            ExpectedValue expected = sentValues.poll();

            if(!expected.isEqual(received))
            {
                throw new RuntimeException(String.format("received %s != expected %s w/ num validated %d", received, expected, numValidated));
            }

            ++numValidated;
        }

    }

    public ExpectedValue addRandomValue(OutstationChangeSet set)
    {
        final int index = random.nextInt(NUM_POINTS_PER_TYPE);

        final ExpectedValue.Type type = getRandomElement(ExpectedValue.ALL_TYPES);

        // Note: It's important to use EventMode force here, because otherwise changes might not happen b/c of random value collisions

        switch(type)
        {
            case BinaryType: {
                BinaryInput v = new BinaryInput(random.nextBoolean(), (byte) 0x01, 0);
                set.update(v, index, EventMode.Force);
                return new ExpectedValue(v, index);
            }
            case DoubleBinaryType: {
                DoubleBitBinaryInput v = new DoubleBitBinaryInput(getRandomElement(DoubleBit.values()), (byte) 0x01, 0);
                set.update(v, index, EventMode.Force);
                return new ExpectedValue(v, index);
            }
            case CounterType: {
                Counter v = new Counter(random.nextInt(65535), (byte) 0x01, 0);
                set.update(v, index, EventMode.Force);
                return new ExpectedValue(v, index);
            }
            case FrozenCounterType: {
                FrozenCounter v = new FrozenCounter(random.nextInt(65535), (byte) 0x01, 0);
                set.update(v, index, EventMode.Force);
                return new ExpectedValue(v, index);
            }
            case AnalogType: {
                AnalogInput v = new AnalogInput(random.nextInt(65535), (byte) 0x01, 0);
                set.update(v, index, EventMode.Force);
                return new ExpectedValue(v, index);
            }
            case BOStatusType: {
                BinaryOutputStatus v = new BinaryOutputStatus(random.nextBoolean(), (byte) 0x01, 0);
                set.update(v, index, EventMode.Force);
                return new ExpectedValue(v, index);
            }
            case AOStatusType: {
                AnalogOutputStatus v = new AnalogOutputStatus(random.nextInt(65535), (byte) 0x01, 0);
                set.update(v, index, EventMode.Force);
                return new ExpectedValue(v, index);
            }
            default:
                throw new RuntimeException("unknown random type: " + type);
        }

    }

    public <T> T getRandomElement(T[] items)
    {
        return items[random.nextInt(items.length)];
    }

    final BlockingChannelListener clientListener = new BlockingChannelListener();
    final BlockingChannelListener serverListener = new BlockingChannelListener();
    final QueuedSOEHandler soeHandler = new QueuedSOEHandler();
    final Queue<ExpectedValue> sentValues = new ArrayDeque<>();
    final Random random = new Random(0);

    final Master master;
    final Outstation outstation;

}
