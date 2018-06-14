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
#ifndef OPENDNP3_ITRANSACTABLE_H
#define OPENDNP3_ITRANSACTABLE_H

#include <openpal/util/Uncopyable.h>

namespace opendnp3
{

/**
    Something that can be performed atomically or needing Start()/End() signals
*/
class ITransactable
{
public:

	friend class Transaction;
	virtual ~ITransactable() {}

protected:

	virtual void Start() = 0;
	virtual void End() = 0;
};

/**
  Uses RAII to safely perform a transaction
*/
class Transaction : private openpal::Uncopyable
{
public:

	Transaction(ITransactable& transactable) : pTransactable(&transactable)
	{
		pTransactable->Start();
	}

	Transaction(ITransactable* pTransactable_) : pTransactable(pTransactable_)
	{
		if (pTransactable)
		{
			pTransactable->Start();
		}
	}

	~Transaction()
	{
		if (pTransactable)
		{
			pTransactable->End();
		}
	}

	static void Start(ITransactable* t)
	{
		if (t)
		{
			t->Start();
		}
	}
	static void End(ITransactable* t)
	{
		if (t)
		{
			t->End();
		}
	}

	template <class ReturnType, class TransactionType, class Fun>
	static ReturnType Apply(TransactionType& transactable, const Fun& fun)
	{
		Transaction t(transactable);
		return fun(transactable);
	}

private:
	ITransactable* pTransactable;

};

}

#endif
