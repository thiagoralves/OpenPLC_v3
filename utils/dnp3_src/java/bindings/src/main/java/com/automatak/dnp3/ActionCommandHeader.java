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

import java.util.function.Consumer;

/// <summary>
/// Concrete implementation of ICommandHeader
/// </summary>
public class ActionCommandHeader implements CommandHeaders {

    private final Consumer<CommandBuilder> action;

    @Override
    public void build(CommandBuilder builder) {
        action.accept(builder);
    }

    private ActionCommandHeader(Consumer<CommandBuilder> action) {
        this.action = action;
    }

    public static CommandHeaders fromCROB(Iterable<IndexedValue<ControlRelayOutputBlock>> commands) {
        return new ActionCommandHeader((CommandBuilder builder) -> builder.addCrob(commands));
    }

    public static CommandHeaders fromAO16(Iterable<IndexedValue<AnalogOutputInt16>> commands) {
        return new ActionCommandHeader((CommandBuilder builder) -> builder.addAO16(commands));
    }

    public static CommandHeaders fromAO32(Iterable<IndexedValue<AnalogOutputInt32>> commands) {
        return new ActionCommandHeader((CommandBuilder builder) -> builder.addAO32(commands));
    }

    public static CommandHeaders fromAOFloat(Iterable<IndexedValue<AnalogOutputFloat32>> commands) {
        return new ActionCommandHeader((CommandBuilder builder) -> builder.addAOFloat32(commands));
    }

    public static CommandHeaders fromAODouble(Iterable<IndexedValue<AnalogOutputDouble64>> commands) {
        return new ActionCommandHeader((CommandBuilder builder) -> builder.addAODouble64(commands));
    }

}
