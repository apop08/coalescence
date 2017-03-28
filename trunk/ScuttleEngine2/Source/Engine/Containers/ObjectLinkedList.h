/**************************************************************************************************/
/*!
\file	ObjectLinkedList.h
\par	Project: ScuttleEngine 2
\date	5/16/12
\authors
Primary  : Matt Sutherlin
\brief
	Intrusively linked list.  Based on Chris Peter's sample, extended for the needs of the engine.

\copyright 
	“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/

#pragma once

namespace Scuttlemeyer
{
	template<typename type>
	class ObjectLinkedList
	{
	public:
		typedef type* pointer;
		typedef type& reference;

		/// <summary>
		/// List constructor.
		/// </summary>
		/// <returns></returns>
		ObjectLinkedList()
			:First(NULL), Last(NULL), ObjectCount(0)
		{
		}
		
		/// <summary>
		/// Getter for the current list size.
		/// </summary>
		/// <returns>The number of objects currently in the list.</returns>
		int Count()
		{
			return ObjectCount;
		}

		/// <summary>
		/// Removes the object at the front of the list and returns it to the caller.
		/// </summary>
		/// <returns>The object currently at the front of the list.</returns>
		pointer pop_front()
		{
			if(First == NULL)
			{
				return NULL;
			}
			else
			{
				type *first = First;
				First = first->Next;
			
				if(First)
				{
					First->Prev = NULL;
				}
				else
				{
					Last = NULL;
				}

				--ObjectCount;

				return first;
			}
		}

		/// <summary>
		/// Sorted insert.  Uses operator< for sorting logic.
		/// </summary>
		/// <param name="object">The object to be inserted to the list.</param>
		/// <returns></returns>
		void insert(pointer object)
		{
			++ObjectCount;

			pointer prev = NULL;
			pointer next = First;

			while(next && *next < *object)
			{
				prev = next;
				next = next->Next;
			}

			object->Prev = prev;
			object->Next = next;

			if(object->Next == NULL)
			{
				Last = object;
			}
			else
			{
				object->Next->Prev = object;
			}

			if(object->Prev == NULL)
			{
				First = object;
			}
			else
			{
				object->Prev->Next = object;
			}
		}

		/// <summary>
		/// Adds an object to the back of the list.
		/// </summary>
		/// <param name="object">The object to be added.</param>
		/// <returns></returns>
		void push_back(pointer object)
		{
			++ObjectCount;

			if(Last == NULL)
			{
				First = object;
				Last = object;
				object->Next = NULL;
				object->Prev = NULL;
			}
			else
			{
				object->Prev = Last;
				object->Next = NULL;
				Last->Next = object;
				Last = object;
			}
		}

		/// <summary>
		/// Removes the given object from the list and updates pointers around it.
		/// </summary>
		/// <param name="object">The object to be removed.</param>
		/// <returns></returns>
		void erase(pointer object)
		{
			--ObjectCount;

			if(object->Next == NULL && object->Prev == NULL)
			{
				Last = NULL;
				First = NULL;
			}
			else if(object->Next == NULL)
			{
				Last = object->Prev;
				if(Last)
				{
					Last->Next = NULL;
				}
			}
			else if(object->Prev == NULL)
			{
				First = object->Next;
				if(First)
				{
					First->Prev = NULL;
				}
			}
			else
			{
				object->Prev->Next = object->Next;
				object->Next->Prev = object->Prev;
			}
		}

		class iterator
		{
		public:
			friend class ObjectLinkedList;
			iterator(){};
			iterator(pointer p) : item(p) {}
			void operator--() { item = item->Prev; }
			void operator++() { item = item->Next; }
			reference operator*() { return *item; }
			pointer operator->() { return item; }
			bool operator==(const iterator& i) { return item == i.item; }
			bool operator!=(const iterator& i) { return item != i.item; }
			operator bool() { return item != NULL; }
			operator pointer() { return item; }
			pointer GetPtr() { return item; }
			
		private:
			pointer item;
		};

		iterator begin() { return First; }
		iterator end() { return NULL;}
		pointer last() { return Last; }
	private:
		pointer First;
		pointer Last;
		unsigned ObjectCount;
	};
}