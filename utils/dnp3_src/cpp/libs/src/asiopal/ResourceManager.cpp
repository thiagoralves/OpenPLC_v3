/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
 * more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 * Green Energy Corp licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project was forked on 01/01/2013 by Automatak, LLC and modifications
 * may have been made to this file. Automatak, LLC licenses these modifications
 * to you under the terms of the License.
 */

#include "asiopal/ResourceManager.h"

namespace asiopal
{

void ResourceManager::Detach(const std::shared_ptr<IResource>& resource)
{
	std::lock_guard <std::mutex> lock(this->mutex);
	this->resources.erase(resource);
}

void ResourceManager::Shutdown()
{
	std::set<std::shared_ptr<asiopal::IResource>> copy;

	{
		std::lock_guard <std::mutex> lock(this->mutex);
		this->is_shutting_down = true;
		for (auto& resource : this->resources)
		{
			copy.insert(resource);
		}
		resources.clear();
	}

	for (auto& resource : copy)
	{
		resource->Shutdown();
	}
}

}

