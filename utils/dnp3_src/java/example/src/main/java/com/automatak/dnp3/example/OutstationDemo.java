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
package com.automatak.dnp3.example;

import com.automatak.dnp3.*;
import com.automatak.dnp3.impl.DNP3ManagerFactory;
import com.automatak.dnp3.mock.DefaultOutstationApplication;
import com.automatak.dnp3.mock.PrintingChannelListener;
import com.automatak.dnp3.mock.PrintingLogHandler;
import com.automatak.dnp3.mock.SuccessCommandHandler;

import java.io.BufferedReader;
import java.io.InputStreamReader;

/**
 * Example master than can be run against the example outstation
 */
public class OutstationDemo {

    public static void main(String[] args) throws Exception {

        // create the root class with a thread pool size of 1
        DNP3Manager manager = DNP3ManagerFactory.createManager(1, PrintingLogHandler.getInstance());

        try {
            run(manager);
        }
        catch(Exception ex)
        {
            System.out.println("Exception: " + ex.getMessage());
        }
        finally {
            // This call is needed b/c the thread-pool will stop the application from exiting
            // and the finalizer isn't guaranteed to run b/c the GC might not be collected during main() exit
            manager.shutdown();
        }
    }

    public static void run(DNP3Manager manager) throws Exception {

        // Create a tcp channel class that will connect to the loopback
        Channel channel = manager.addTCPServer(
                "client",
                LogMasks.NORMAL | LogMasks.APP_COMMS,
                ChannelRetry.getDefault(),
                "127.0.0.1",
                20000,
                PrintingChannelListener.getInstance()
        );

        // Create the default outstation configuration
        OutstationStackConfig config = new OutstationStackConfig(DatabaseConfig.allValues(5), EventBufferConfig.allTypes(50));


        // Create an Outstation instance, pass in a simple a command handler that responds successfully to everything
        Outstation outstation = channel.addOutstation(
                "outstation",
                SuccessCommandHandler.getInstance(),
                DefaultOutstationApplication.getInstance(),
                config
        );

        outstation.enable();

        // all this stuff just to read a line of text in Java. Oh the humanity.
        String line = "";
        InputStreamReader converter = new InputStreamReader(System.in);
        BufferedReader in = new BufferedReader(converter);


        int i = 0;
        while (true) {
            System.out.println("Enter something to update a counter or type <quit> to exit");
            line = in.readLine();
            if(line.equals("quit")) break;
            else {
                OutstationChangeSet set = new OutstationChangeSet();
                set.update(new Counter(i,(byte) 0x01, 0), 0);
                outstation.apply(set);
                ++i;
            }
        }
    }
}
