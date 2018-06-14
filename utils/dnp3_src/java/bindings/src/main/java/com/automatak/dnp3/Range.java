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

public class Range {

    private static final int MIN_INDEX = 0;
    private static final int MAX_INDEX = 65535;

    public static Range from(int start, int stop) {
        return new Range(start, stop);
    }

    public static Range all() {
        return new Range(MIN_INDEX, MAX_INDEX);
    }

    public boolean isDefined()
    {
        return (start >= MIN_INDEX) && (stop <= MAX_INDEX) && (start <= stop);
    }

    public boolean isAllObjects() {
        return start == MIN_INDEX && stop == MAX_INDEX;
    }

    Range(int start, int stop) {
        this.start = start;
        this.stop = stop;
    }

    public final int start;
    public final int stop;
}
