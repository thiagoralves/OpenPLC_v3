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

import java.util.ArrayList;
import java.util.function.Consumer;

/**
 * A CommandHeaders is something that can send some info to a CommandBuilder
 */
public class CommandHeader implements CommandHeaders
{
    private final Consumer<CommandBuilder> action;

    private CommandHeader(Consumer<CommandBuilder> action)
    {
        this.action = action;
    }

    public static CommandHeaders fromCROB(Iterable<IndexedValue<ControlRelayOutputBlock>> commands)
    {
        return new CommandHeader((CommandBuilder b) -> b.addCrob(commands));
    }

    public static CommandHeaders fromAO16(Iterable<IndexedValue<AnalogOutputInt16>> commands)
    {
        return new CommandHeader((CommandBuilder b) -> b.addAO16(commands));
    }

    public static CommandHeader fromAO32(Iterable<IndexedValue<AnalogOutputInt32>> commands)
    {
       return new CommandHeader((CommandBuilder b) -> b.addAO32(commands));
    }

    public static CommandHeader fromAOFloat32(Iterable<IndexedValue<AnalogOutputFloat32>> commands)
    {
        return new CommandHeader((CommandBuilder b) -> b.addAOFloat32(commands));
    }

    public static CommandHeader fromAODouble64(Iterable<IndexedValue<AnalogOutputDouble64>> commands)
    {
        return new CommandHeader((CommandBuilder b) -> b.addAODouble64(commands));
    }

    public static CommandHeaders fromSingleCROB(ControlRelayOutputBlock command, int index)
    {
        return fromCROB(wrap(command, index));
    }

    public static CommandHeaders fromSingleAO16(AnalogOutputInt16 command, int index)
    {
        return fromAO16(wrap(command, index));
    }

    public static CommandHeader fromSingleAO32(AnalogOutputInt32 command, int index)
    {
        return fromAO32(wrap(command, index));
    }

    public static CommandHeader fromSingleAOFloat32(AnalogOutputFloat32 command, int index)
    {
        return fromAOFloat32(wrap(command, index));
    }

    public static CommandHeader fromSingleAODouble64(AnalogOutputDouble64 command, int index)
    {
        return fromAODouble64(wrap(command, index));
    }

    private static <T> Iterable<IndexedValue<T>> wrap(T meas, int index)
    {
        ArrayList<IndexedValue<T>> list = new ArrayList<>(1);
        list.add(new IndexedValue<>(meas, index));
        return list;
    }

    @Override
    public void build(CommandBuilder builder)
    {
        action.accept(builder);
    }
};