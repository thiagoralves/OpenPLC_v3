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
#ifndef OPENPAL_LINKEDLIST_H
#define OPENPAL_LINKEDLIST_H

#include "ArrayView.h"

#include "openpal/container/Array.h"

namespace openpal
{

template <class ValueType>
class ListNode
{
public:
	ListNode() : value(), prev(nullptr), next(nullptr)
	{}

	ValueType value;

private:
	ListNode* prev;
	ListNode* next;

	template <class T, class U>
	friend class LinkedList;

	template <class T>
	friend class LinkedListIterator;
};


template <class ValueType>
class LinkedListIterator
{
public:
	static LinkedListIterator<ValueType> Undefined()
	{
		return LinkedListIterator(nullptr);
	}

	static LinkedListIterator<ValueType> From(ListNode< ValueType>* pStart)
	{
		return LinkedListIterator(pStart);
	}



	bool HasNext() const
	{
		return (pCurrent != nullptr);
	}

	ListNode< ValueType>* Next()
	{
		if (pCurrent == nullptr)
		{
			return nullptr;
		}
		else
		{
			auto pRet = pCurrent;
			pCurrent = pCurrent->next;
			return pRet;
		}
	}

	ListNode< ValueType>* Current()
	{
		return pCurrent;
	}

	ValueType* CurrentValue()
	{
		if (pCurrent)
		{
			return &pCurrent->value;
		}
		else
		{
			return nullptr;
		}
	}

private:

	LinkedListIterator(ListNode< ValueType>* pStart) : pCurrent(pStart)
	{}

	ListNode<ValueType>* pCurrent;
};


// A container adapter for a -linked list
template <class ValueType, class IndexType>
class LinkedList : public HasSize<IndexType>
{
public:

	typedef LinkedListIterator<ValueType> Iterator;

	LinkedList(IndexType maxSize) :
		HasSize<IndexType>(0),
		pHead(nullptr),
		pTail(nullptr),
		pFree(nullptr),
		underlying(maxSize)
	{
		Initialize();
	}

	IndexType Capacity() const
	{
		return underlying.Size();
	}

	void Clear()
	{
		if (this->IsNotEmpty())
		{
			// link the remaining free list to the end of active list
			this->Link(pTail, pFree);
			// set the free pointer to the head
			pFree = pHead;
			pHead = pTail = nullptr;
			this->size = 0;
		}
	}

	inline ListNode<ValueType>* Head()
	{
		return pHead;
	}

	Iterator Iterate() const
	{
		return Iterator::From(pHead);
	}

	template <class Selector>
	ListNode<ValueType>* FindFirst(Selector select);

	ListNode<ValueType>* Add(const ValueType& value);

	template <class Selector>
	void While(Selector select)
	{
		auto iter = this->Iterate();
		bool result = true;
		while (result && iter.HasNext())
		{
			result = select(iter.Next()->value);
		}
	}

	template <class Selector>
	void Foreach(Selector select)
	{
		auto iter = this->Iterate();
		while (iter.HasNext())
		{
			select(iter.Next()->value);
		}
	}

	template <class Selector>
	ListNode<ValueType>* RemoveFirst(Selector select)
	{
		auto pNode = this->FindFirst(select);
		if (pNode)
		{
			this->Remove(pNode);
		}
		return pNode;
	}

	template <class Selector>
	IndexType RemoveAll(Selector match)
	{
		IndexType count = 0;

		auto iter = this->Iterate();
		auto pCurrent = iter.Next();
		while (pCurrent)
		{
			if (match(pCurrent->value))
			{
				auto pRemoved = pCurrent;
				pCurrent = iter.Next();
				this->Remove(pRemoved);
				++count;
			}
			else
			{
				pCurrent = iter.Next();
			}
		}

		return count;
	}

	bool Remove(const ValueType& value)
	{
		auto iter = this->Iterate();
		while (iter.HasNext())
		{
			auto pNode = iter.Next();
			if (pNode->value == value)
			{
				this->Remove(pNode);
				return true;
			}
		}
		return false;
	}

	template <class LessThan>
	ListNode<ValueType>* Insert(const ValueType& value, LessThan lt);

	void Remove(ListNode<ValueType>* apNode);

	inline bool IsFull() const;

private:

	ListNode<ValueType>* pHead;
	ListNode<ValueType>* pTail;
	ListNode<ValueType>* pFree;

	Array<ListNode<ValueType>, IndexType> underlying;

	ListNode<ValueType>* Insert(const ValueType& value, ListNode<ValueType>* left, ListNode<ValueType>* right);

	inline static void Link(ListNode<ValueType>* prev, ListNode<ValueType>* next);

	void Initialize();
};

template <class ValueType, class IndexType>
ListNode<ValueType>* LinkedList<ValueType, IndexType>::Add(const ValueType& value)
{
	return this->Insert(value, pTail, nullptr);
}


template <class ValueType, class IndexType>
template <class LessThan>
ListNode<ValueType>* LinkedList<ValueType, IndexType>::Insert(const ValueType& value, LessThan lt)
{
	if (pFree == nullptr)
	{
		return nullptr;
	}
	else
	{
		auto query = [lt, value](const ValueType & v)
		{
			return lt(value, v);
		};
		auto pResult = this->FindFirst(query);
		if (pResult)
		{
			return Insert(value, pResult->prev, pResult);
		}
		else
		{
			return Insert(value, pTail, nullptr);
		}
	}
}

template <class ValueType, class IndexType>
ListNode<ValueType>* LinkedList<ValueType, IndexType>::Insert(const ValueType& value, ListNode<ValueType>* pLeft, ListNode<ValueType>* pRight)
{
	if (pFree == nullptr)
	{
		return nullptr;
	}
	else
	{
		// initialize the new node, and increment the size
		auto pNode = pFree;
		pFree = pFree->next;
		pNode->value = value;
		++(this->size);

		this->Link(pLeft, pNode);
		this->Link(pNode, pRight);

		// change of head
		if (pLeft == nullptr)
		{
			pHead = pNode;
		}

		// change of tail
		if (pRight == nullptr)
		{
			pTail = pNode;
		}

		return pNode;
	}
}

template <class ValueType, class IndexType>
template <class Selector>
ListNode<ValueType>* LinkedList<ValueType, IndexType>::FindFirst(Selector select)
{
	auto iter = this->Iterate();
	while (iter.HasNext())
	{
		auto pNode = iter.Next();
		if (select(pNode->value))
		{
			return pNode;
		}
	}
	return nullptr;
}

template <class ValueType, class IndexType>
void LinkedList<ValueType, IndexType>::Remove(ListNode<ValueType>* apNode)
{
	if(apNode->prev == nullptr) // it's the head
	{
		if (apNode->next == nullptr)
		{
			pHead = pTail = nullptr; // list is now empty
		}
		else
		{
			pHead = apNode->next; // head but not tail
		}
	}
	else
	{
		if(apNode->next == nullptr) pTail = apNode->prev; // was only the tail
	}

	// attach the adjacent nodes to eachother if they exist
	Link(apNode->prev, apNode->next);

	// Now that the data list is complete, attach the freed node to the front of the free list
	apNode->next = pFree;
	if(pFree != nullptr) pFree->prev = apNode;
	apNode->prev = nullptr; // it's the head now
	pFree = apNode;
	--(this->size);
}

template <class ValueType, class IndexType>
bool LinkedList<ValueType, IndexType>::IsFull() const
{
	return (pFree == nullptr);
}



template <class ValueType, class IndexType>
void LinkedList<ValueType, IndexType>::Link(ListNode<ValueType>* first, ListNode<ValueType>* second)
{
	if(first) first->next = second;
	if(second) second->prev = first;
}

template <class ValueType, class IndexType>
void LinkedList<ValueType, IndexType>::Initialize()
{
	if(underlying.IsNotEmpty())
	{
		pFree = &underlying[0];
		for(IndexType i = 1; i < underlying.Size(); ++i)
		{
			Link(&underlying[i - 1], &underlying[i]);
		}
	}
}

}

#endif
