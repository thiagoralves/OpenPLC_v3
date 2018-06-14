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
#ifndef ASIOPAL_RESOURCEMANAGER_H
#define ASIOPAL_RESOURCEMANAGER_H

#include "IResourceManager.h"

#include <set>
#include <mutex>
#include <memory>

namespace asiopal
{

class ResourceManager final : public IResourceManager
{

public:

	static std::shared_ptr<ResourceManager> Create()
	{
		return std::make_shared<ResourceManager>();
	}

	virtual void Detach(const std::shared_ptr<IResource>& resource)  override;

	void Shutdown();

	template <class R, class T>
	std::shared_ptr<R> Bind(const T& create)
	{
		std::lock_guard <std::mutex> lock(this->mutex);

		if (this->is_shutting_down)
		{
			return nullptr;
		}
		else
		{
			auto item = create();
			if (item)
			{
				this->resources.insert(item);
			}
			return item;
		}
	}

private:

	std::mutex mutex;
	bool is_shutting_down = false;
	std::set<std::shared_ptr<asiopal::IResource>> resources;

};

}

#endif
