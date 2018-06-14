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
package com.automatak.dnp3.impl;

import com.automatak.dnp3.DNP3Exception;
import com.automatak.dnp3.DNP3Manager;
import com.automatak.dnp3.impl.mocks.NullLogHandler;
import com.automatak.dnp3.impl.mocks.StackPair;
import junit.framework.*;
import org.junit.Ignore;
import org.junit.Test;


import java.time.Duration;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Consumer;

@Ignore
public class DNP3ManagerIntegrationTest extends TestCase {

    static final int NUM_STACKS = 10;
    static final int NUM_POINTS_PER_EVENT_TYPE = 50;
    static final int NUM_ITERATIONS = 10;
    static final int EVENTS_PER_ITERATION = 50;
    static final int START_PORT = 20000;

    static final int NUM_THREADS_IN_POOL = 4;
    static final Duration TIMEOUT = Duration.ofSeconds(10);

    static void withManager(int numThreads, Consumer<DNP3Manager> func)
    {
        DNP3Manager manager= null;
        try {
            manager = DNP3ManagerFactory.createManager(numThreads, new NullLogHandler());
            func.accept(manager);
        }
        catch(DNP3Exception ex)
        {
            throw new RuntimeException(ex);
        }
        finally {
            if(manager != null) {
                manager.shutdown();
            }

        }
    }
    
    @Test
    public void testEventOrdering() {

        List<StackPair> stacks = new ArrayList<>();

        withManager(NUM_THREADS_IN_POOL, manager ->  {

            for(int i = 0; i < NUM_STACKS; ++i) {
                StackPair pair = new StackPair(manager, START_PORT+i, NUM_POINTS_PER_EVENT_TYPE, EVENTS_PER_ITERATION);
                stacks.add(pair);
            }

            final long start = System.currentTimeMillis();

            stacks.forEach(pair -> pair.waitForChannelsOpen(TIMEOUT));

            for(int i = 0; i < NUM_ITERATIONS; ++i) {

                stacks.forEach(pair -> pair.sendRandomValues());
                stacks.forEach(pair -> pair.awaitSentValues(TIMEOUT));
            }

            final long ELASPED_MS = System.currentTimeMillis() - start;
            final long TOTAL_EVENTS = NUM_STACKS*NUM_ITERATIONS*EVENTS_PER_ITERATION;
            final long RATE = (TOTAL_EVENTS * 1000)/ ELASPED_MS;

            System.out.println(String.format("%d events in %d ms == %d events/sec", TOTAL_EVENTS, ELASPED_MS, RATE));
        });

    }
}
