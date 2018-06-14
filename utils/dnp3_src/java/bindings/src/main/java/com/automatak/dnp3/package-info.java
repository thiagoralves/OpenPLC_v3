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
/**
 * A package for all of the interfaces and pure java classes
 * used in the opendnp3 java bindings.
 *
 * The root interface is DNP3Manager and is the starting point of
 * all applications.
 *
 * You need the opendnp3-bindings jar with the package com.automatak.dnp3.impl
 * package to obtain a concrete reference to a DNP3Manager, i.e. :
 *
 * <pre>
 * {@code
 * DNP3Manager manager = DNP3ManagerFactory.createManager(Runtime.getRuntime().availableProcessors());
 * }
 * </pre>
 */
package com.automatak.dnp3;