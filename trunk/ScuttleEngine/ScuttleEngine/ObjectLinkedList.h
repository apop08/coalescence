/******************************************************************************/
/*!
\file   ObjectLinkedList.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   7/25/2011
\brief  
Intrusively Linked List with sorted insert.  Based upon Chris' sample.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#define NULL 0

namespace Scuttlemeyer
{
	template<typename type>
	class ObjectLinkedList
	{
	public:
		typedef type * pointer;
		typedef type& reference;

		ObjectLinkedList()
			:First(NULL),Last(NULL),ObjectCount(0)
		{
		}
		
		int Count()
		{
			return ObjectCount;
		}

		pointer pop_front()
		{
			if(First==NULL)
			{
				return NULL;
			}
			else
			{
				type * first = First;
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

		void push_back(pointer object)
		{
			++ObjectCount;

			if( Last == NULL )
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

		void erase(pointer object)
		{
			--ObjectCount;

			if( object->Next == NULL && object->Prev == NULL)
			{
				Last = NULL;
				First = NULL;
			}
			else if( object->Next == NULL )
			{
				Last = object->Prev;
				if( Last ) Last->Next = NULL;
			}
			else if( object->Prev == NULL )
			{
				First = object->Next;
				if( First ) First->Prev = NULL;
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
			void operator--(){ item = item->Prev; }
			void operator++(){ item = item->Next; }
			reference operator*(){ return *item; }
			pointer operator->(){ return item; }
			bool operator==(const iterator& i){ return item == i.item; }
			bool operator!=(const iterator& i){ return item != i.item; }
			operator bool(){ return item != NULL; }
			operator pointer(){ return item; }
			pointer GetPtr(){ return item; }
			
		private:
			pointer item;
		};

		iterator begin(){ return First; }
		iterator end(){ return NULL;}
		pointer last(){ return Last; }
	private:
		pointer First;
		pointer Last;
		unsigned ObjectCount;
	};
}