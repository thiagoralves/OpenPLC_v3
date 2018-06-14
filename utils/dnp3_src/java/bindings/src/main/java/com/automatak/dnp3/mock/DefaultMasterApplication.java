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
package com.automatak.dnp3.mock;

import com.automatak.dnp3.*;
import com.automatak.dnp3.enums.LinkStatus;
import com.automatak.dnp3.enums.MasterTaskType;


import java.util.Collections;

public class DefaultMasterApplication implements MasterApplication
{
    private static DefaultMasterApplication instance = new DefaultMasterApplication();

    public static MasterApplication getInstance() {
        return instance;
    }

    private DefaultMasterApplication() {}

    @Override
    public long getMillisecondsSinceEpoch()
    {
        return System.currentTimeMillis();
    }

    @Override
    public void onReceiveIIN(IINField iin)
    {}

    @Override
    public void onTaskStart(MasterTaskType type, TaskId userId)
    {}

    @Override
    public void onTaskComplete(TaskInfo info)
    {}

    @Override
    public void onOpen(){}

    @Override
    public void onClose(){}

    @Override
    public boolean assignClassDuringStartup()
    {
        return false;
    }

    @Override
    public Iterable<ClassAssignment> getClassAssignments()
    {
        return Collections.emptyList();
    }

    @Override
    public void onStateChange(LinkStatus value){}

    @Override
    public void onKeepAliveInitiated(){}

    @Override
    public void onKeepAliveFailure(){}

    @Override
    public void onKeepAliveSuccess(){}

}
