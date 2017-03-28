/******************************************************************************/
/*!
\file   ObjectAllocator.cpp
\author Kevin Verholtz
\par    email: kevin.verholtz\@digipen.edu
\par    DigiPen login: kevin.verholtz
\par    Course: GAM200
\date   12/8/2011
\brief  
Manages Memory for specific classes that overload opperator new.
Can be applied to more but is not neccessary at this point of the game.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/
#include "Precompiled.h"
#include "ObjectAllocator.h"
#include "Core.h"

namespace Scuttlemeyer
{
	ObjectAllocator* OA = NULL;

	/******************************************************************************/
	/*!
	  \brief
		Initializes private data and Allocate a Page if using our MemManager

	  \param ObjectSize
		Size of Objects in our MemManager.

	  \param config
	  Configeration details for our MemManager

	*/
	/******************************************************************************/
	ObjectAllocator::ObjectAllocator(unsigned ObjectSize, const OAConfig& config) throw(OAException)//headers: check  debug: check
	{
		//initialize variables
		Config_ = config;
		OAStats_.ObjectSize_ = ObjectSize;

		//set poiters to NULL
		PList = 0;
		FList = 0;

		if(Config_.UseCPPMemManager_ == false)
		{
			//Set Alignment
			Alignment();

			//Calculate PageSize
			int padSize = (Config_.PadBytes_ * 2) * Config_.ObjectsPerPage_;
			int headerSize = Config_.HeaderBlocks_ * Config_.ObjectsPerPage_;
			int interSize = Config_.InterAlignSize_ * (Config_.ObjectsPerPage_ - 1);
			int ObjSize = (Config_.ObjectsPerPage_ * OAStats_.ObjectSize_);

			OAStats_.PageSize_ = padSize + headerSize + sizeof(int) + Config_.LeftAlignSize_ + interSize + ObjSize;

			//Allocates a Page
			AllocateNewPage();
			AllocateNewPage();
		}
	}
	/******************************************************************************/
	/*!
	  \brief
		If using our MemManager delete all the pages

	*/
	/******************************************************************************/
	ObjectAllocator::~ObjectAllocator(void) throw()//headers: check  debug: check
	{
		if(Config_.UseCPPMemManager_ == false)
		{
			while(PList != NULL)
			{
				char * pageFree = reinterpret_cast<char *>(PList);
				PList = PList->Next;
				delete [] pageFree;
			}
		}
	}
	/******************************************************************************/
	/*!
	  \brief
		Give the client memory

	  \return
		A pointer for the client to the memory allocated

	*/
	/******************************************************************************/
	void * ObjectAllocator::Allocate(void) throw(OAException) //headers: check  debug: check
	{
		//If ture use new and delete
		if(Config_.UseCPPMemManager_ == true)
		{
			//Update Counters
			++(OAStats_.ObjectsInUse_);
			++(OAStats_.Allocations_);
			if(OAStats_.ObjectsInUse_ > OAStats_.MostObjects_)
			{
				OAStats_.MostObjects_ = OAStats_.ObjectsInUse_;
			}

			try
			{
				return new char[OAStats_.ObjectSize_];
			}
			catch (std::bad_alloc &)
			{
			    throw OAException(OAException::E_NO_MEMORY, "allocate_new_page: No system memory available.");
			}
		}
		else
		{
			//If there are no more free spots make a new page
			if(FList == NULL)
			{
				AllocateNewPage();
			}

			//Update Counters
			--(OAStats_.FreeObjects_);
			++(OAStats_.ObjectsInUse_);
			++(OAStats_.Allocations_);
			if(OAStats_.ObjectsInUse_ > OAStats_.MostObjects_)
			{
				OAStats_.MostObjects_ = OAStats_.ObjectsInUse_;
			}

			//Store Object and pop front free list
			GenericObject * object;
			object = FList;
			FList = object->Next;


			char * allocObject = (reinterpret_cast<char *>(object));

			//if true set memory
			if(Config_.DebugOn_ == true)
			{
				//Sets all bytes of object to OxBB
				std::memset(allocObject, ALLOCATED_PATTERN, OAStats_.ObjectSize_);
			}
			//Header flag set to 1
			if(Config_.HeaderBlocks_ > 0)
			{
				*(allocObject - Config_.PadBytes_ - 1) = 1;
			}

			//return the object
			return object;
		}
	}
	/******************************************************************************/
	/*!
	  \brief
		Calls new and allocates another page cause one is used up or none are made.

	*/
	/******************************************************************************/
	void ObjectAllocator::AllocateNewPage() throw(OAException)//headers: check  debug: check
	{
		//Check if there is enough memory
		if(Config_.MaxPages_ == OAStats_.PagesInUse_ && Config_.MaxPages_ != 0)
		{
			Printf();
			throw(OAException(OAException::E_NO_PAGES, "Exception thrown from AllocateNewPage: E_NO_PAGES"));
		}
		
		//Update Counters
		++(OAStats_.PagesInUse_);
		OAStats_.FreeObjects_ += Config_.ObjectsPerPage_;

		//Allocate a new page

		char * page = new (std::nothrow) char[(OAStats_.PageSize_)];

		if(!page)
			throw OAException(OAException::E_NO_MEMORY, "allocate_new_page: No system memory available.");


		//Sets the first Align, Header, PAD memory space
		if(Config_.DebugOn_ == true)
		{
			std::memset(page + sizeof(int), ALIGN_PATTERN, Config_.LeftAlignSize_);
			std::memset(page + sizeof(int) + Config_.LeftAlignSize_, 0x00, Config_.HeaderBlocks_);
			std::memset(page + sizeof(int) + Config_.LeftAlignSize_ + Config_.HeaderBlocks_, PAD_PATTERN, Config_.PadBytes_);
		}
		if(Config_.HeaderBlocks_ > 0)
		{
			*(page + sizeof(int) + Config_.LeftAlignSize_) = 0;
		}

		//Push front the Page on the Page List
		GenericObject * pageTemp;
		pageTemp = PList;
		PList = reinterpret_cast<GenericObject *>(page);
		PList->Next = pageTemp;

		//Adds to the free list
		for(unsigned i = 0; i < Config_.ObjectsPerPage_; ++i)
		{
			GenericObject * temp;
			temp = FList;
			FList = reinterpret_cast<GenericObject *>(page + sizeof(int)+Config_.LeftAlignSize_
														   + Config_.HeaderBlocks_+Config_.PadBytes_
														   + ((OAStats_.ObjectSize_*i)+(Config_.PadBytes_ * 2 * i)
														   + (Config_.InterAlignSize_ * i) 
 														   + (Config_.HeaderBlocks_ * i)));
			FList->Next = temp;

			char * object = (reinterpret_cast<char *>(FList));
			//Memsets the stuff in between the Object
			if(Config_.DebugOn_ == true)
			{
				std::memset(object + sizeof(int), UNALLOCATED_PATTERN, OAStats_.ObjectSize_ - sizeof(int));
				std::memset(object + OAStats_.ObjectSize_, PAD_PATTERN, Config_.PadBytes_);
				if(i < Config_.ObjectsPerPage_ - 1)
				{
					std::memset(object + OAStats_.ObjectSize_ + Config_.PadBytes_, ALIGN_PATTERN, Config_.InterAlignSize_);
					std::memset(object + OAStats_.ObjectSize_ + Config_.PadBytes_ + Config_.InterAlignSize_, 0x00, Config_.HeaderBlocks_);
					std::memset(object + OAStats_.ObjectSize_ + Config_.PadBytes_ + Config_.InterAlignSize_ + Config_.HeaderBlocks_, PAD_PATTERN, Config_.PadBytes_);
				}
			}
			if(Config_.HeaderBlocks_ > 0 && i < Config_.ObjectsPerPage_ - 1)
			{
				*(object + OAStats_.ObjectSize_ + Config_.PadBytes_ + Config_.InterAlignSize_) = 0;
			}
		}
	}
	/******************************************************************************/
	/*!
	  \brief
		Helper fn sets up Left and Inter Alignment

	*/
	/******************************************************************************/
	void ObjectAllocator::Alignment()//headers: check  debug: check
	{
		//Sets up LeftAlign and InterAlign sizes
		if(Config_.Alignment_ > 0)
		{
			Config_.LeftAlignSize_ = (Config_.PadBytes_+Config_.HeaderBlocks_+sizeof(int));
			Config_.InterAlignSize_ = ((Config_.PadBytes_* 2) + Config_.HeaderBlocks_ + OAStats_.ObjectSize_);

			unsigned temp = Config_.Alignment_;
			unsigned temp2 = Config_.Alignment_;
			while(temp < Config_.LeftAlignSize_)
			{
				temp += Config_.Alignment_;
			}
			while(temp2 < Config_.InterAlignSize_)
			{
				temp2 += Config_.Alignment_;
			}
			Config_.LeftAlignSize_ = temp - Config_.LeftAlignSize_;
			Config_.InterAlignSize_ = temp2 - Config_.InterAlignSize_;
		}
	}
	/******************************************************************************/
	/*!
	  \brief
		Calls new and allocates another page cause one is used up or none are made.

	  \param Object
		Object to be put back on the free list or delete if not useing MemManager

	*/
	/******************************************************************************/
	void ObjectAllocator::Free(void *Object) throw(OAException)//headers: 1 debug: 1
	{
		if(Config_.UseCPPMemManager_ == true)
		{
			//Update Counters
			--(OAStats_.ObjectsInUse_);
			++(OAStats_.Deallocations_);

			//delete Object
			delete [] reinterpret_cast<char *>(Object);
		}
		else
		{
			//Dont do debug code
			if(Config_.DebugOn_ == false)
			{
				//Move pointer to next object on free list
				GenericObject * object;
				object = FList;
				FList = reinterpret_cast<GenericObject *>(Object);
				FList->Next = object;

				
				if(Config_.HeaderBlocks_ > 0)
				{
					char * header =  reinterpret_cast<char *>(FList);
					*(header - Config_.PadBytes_ - 1) = 0;
				}

				//Update counters
				++(OAStats_.FreeObjects_);
				--(OAStats_.ObjectsInUse_);
				++(OAStats_.Deallocations_);
			}
			else
			{
				//Adds the Object back into the free list
				GenericObject * freeListBackUp, * freeListWalker;
				freeListBackUp = FList;
				freeListWalker = freeListBackUp;
				FList = reinterpret_cast<GenericObject *>(Object);
				
				try
				{
					validate_object();
				}
				catch (const OAException& e)
				{
					FList = freeListBackUp;
					throw(e);
				}


				//Just checks if the header is set to 0
				if(Config_.HeaderBlocks_ > 0)
				{
					//Already freed
					if(*(reinterpret_cast<char *>(FList) - Config_.PadBytes_ - 1) == 0)
					{
						FList = freeListBackUp;
						Printf();
						throw(OAException(OAException::E_MULTIPLE_FREE, "FreeObject: Object has already been freed."));							
					}
				}
				else
				{
					//Already freed
					if(*(reinterpret_cast<char *>(FList) + sizeof(int)) == UNALLOCATED_PATTERN || *(reinterpret_cast<char *>(FList) + sizeof(int)) == FREED_PATTERN)
					{
						FList = freeListBackUp;
						Printf();
						throw(OAException(OAException::E_MULTIPLE_FREE, "FreeObject: Object has already been freed."));							
					}
				}

				//finaly if no errors sets the list next to the old free list
				FList->Next = freeListBackUp;

				//sets the memory of Free objects
				char * pointer = (reinterpret_cast<char *>(FList));
				std::memset(pointer + sizeof(int), FREED_PATTERN, OAStats_.ObjectSize_ - sizeof(int));
				std::memset(pointer - Config_.PadBytes_ - Config_.HeaderBlocks_, 0x00, Config_.HeaderBlocks_);

				//Update Counters cause nothing went wrong
				++(OAStats_.FreeObjects_);
				--(OAStats_.ObjectsInUse_);
				++(OAStats_.Deallocations_);

				//check the pad byes to see if anything was corrupted
				check_padbytes();
			}
		}
	}
	//this was for testing
	void ObjectAllocator::Printf(void) const
	{
		GenericObject * temp;
		temp = PList;
		while(temp != NULL)
		{
		char * p = reinterpret_cast<char *>(temp);
			for(unsigned j = 0; j < OAStats_.PageSize_; j++)
			{
				Message* q = new Message(Mid::DebugCNSL, 2, "%8X", *(p+j));
				CORE->EnqueueMessage(q);
			}
	
			temp = temp->Next;
		}
	}
	/******************************************************************************/
	/*!
	  \brief
		Calls the callback fn for each block still in use.

	  \param fn
		callback fn for each block

	  \return
	  how many blocks that are in use

	*/
	/******************************************************************************/
	unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const//headers: check  debug: check  ++
	{
		//get pointer to Page List
		GenericObject * pageWalker;
		pageWalker = PList;

		//How big the front of the page is to the first object
		int front = Config_.LeftAlignSize_+Config_.HeaderBlocks_+Config_.PadBytes_+sizeof(int);

		//How many blocks are in use
		int count = 0;

		//Walks through all the pages
		while(pageWalker != NULL)
		{
			//pointer to the cur page
			unsigned char * curPage = reinterpret_cast<unsigned char *>(pageWalker);

			//Just check header blocks if there is any check if Allocated
			if(Config_.HeaderBlocks_ > 0)
			{
				//moves to each object
				for(unsigned i = 0; i < Config_.ObjectsPerPage_; ++i)
				{
					int object = (OAStats_.ObjectSize_+(Config_.PadBytes_*2)+
								Config_.InterAlignSize_+Config_.HeaderBlocks_) * i;

					if(*(curPage+front+object - Config_.PadBytes_ -1) == 1)
					{
						count++;
						fn(curPage+front+object, OAStats_.ObjectSize_);
					}
				}
			}
			else if(Config_.DebugOn_ == true)
			{
				//DEBUG WAY
				//moves to each object
				for(unsigned i = 0; i < Config_.ObjectsPerPage_; ++i)
				{

					int object = (OAStats_.ObjectSize_+(Config_.PadBytes_*2)+
					Config_.InterAlignSize_+Config_.HeaderBlocks_) * i;
					//If is Allocated pattern then count it and callback fn()
					//if(*(curPage+front+object) == ALLOCATED_PATTERN)
					if(*(curPage+front+object+sizeof(int)) != UNALLOCATED_PATTERN && *(curPage+front+object+sizeof(int)) != FREED_PATTERN)
					{
						count++;
						fn(curPage+front+object, OAStats_.ObjectSize_);
					}
				}
			}
			else
			{
				///Other way would be to check if its not on the free list:  implies that it is in use

				for(unsigned i = 0; i < Config_.ObjectsPerPage_; ++i)
				{
					int object = (OAStats_.ObjectSize_+(Config_.PadBytes_*2)+
								Config_.InterAlignSize_+Config_.HeaderBlocks_) * i;

					//If the free list is null then all object on the rest of the pages are in use.
					//might as well just call the call back fn() on them until this becomes not true
					if(FList == NULL)
					{
						count++;
						fn(curPage+front+object, OAStats_.ObjectSize_);
					}
					else
					{
						GenericObject * freeWalker;
						freeWalker = FList;
							
						//if curObject is on the freeList
						while(freeWalker != NULL)
						{
							//if curObject is on the freeList
							if((curPage+front+object) == reinterpret_cast<unsigned char *>(freeWalker))
							{
								break;
							}
							freeWalker = freeWalker->Next;
						}
						//true if it doesn't break
						if(freeWalker == NULL)
						{
							count++;
							fn(curPage+front+object, OAStats_.ObjectSize_);
						}
					}
				}
			}
			pageWalker = pageWalker->Next;
		}

		//how many blocks that are in use
		return count;
	}
	/******************************************************************************/
	/*!
	  \brief
		Checks the padbytes for overruns.

	*/
	/******************************************************************************/
	void ObjectAllocator::check_padbytes() throw(OAException)//headers: check  debug: check
	{
		//checks pad bytes on both sides of the free list object to see if any byes where corrupted
		unsigned char * freeList = reinterpret_cast<unsigned char *>(FList);

		//checks padbytes of cur freed object
		for(unsigned i = 0; i < Config_.PadBytes_; ++i)
		{
			if(*(freeList-i-1) != PAD_PATTERN)
			{
				Printf();
				throw(OAException(OAException::E_CORRUPTED_BLOCK, "check_padbytes: Memory corrupted before block."));
			}
			if(*(freeList+OAStats_.ObjectSize_+i) != PAD_PATTERN)
			{
				Printf();
				throw(OAException(OAException::E_CORRUPTED_BLOCK, "check_padbytes: Memory corrupted after block."));
			}
		}
	}
	/******************************************************************************/
	/*!
	  \brief
		Calls the callback fn for each block that is potentially corrupted.

	  \param fn
	  callback fn for each block corrupted

	  \return
	  return total corruptions
	*/
	/******************************************************************************/
	unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const//headers: check  debug: check
	{
		//page list walker walking to check if any of the pages are corrupted
		GenericObject * pageWalker;
		pageWalker = PList;

		//Counters for corrupted bytes
		int beforeBlock = 0, afterBlock = 0;

		//Walks through all pages
		while(pageWalker != NULL)
		{
			//Gets pointer to PageList
			unsigned char * pageList = reinterpret_cast<unsigned char *>(pageWalker);
			//front befor padbytes, back first pad bytes plus object size
			int front = sizeof(int) + Config_.LeftAlignSize_ + Config_.HeaderBlocks_;
			int back = Config_.PadBytes_ + OAStats_.ObjectSize_;

			//moves to the first address of pad bytes
			pageList += front;

			for(unsigned j = 0; j < Config_.ObjectsPerPage_; ++j)
			{
				//moves to the next address of pad bytes
				int rest = ((Config_.PadBytes_ * 2) + OAStats_.ObjectSize_ 
						   + Config_.InterAlignSize_ + Config_.HeaderBlocks_) * j;

				//checks pad befor object if corrupted count how many bytes
				for(unsigned i = 0; i < Config_.PadBytes_; ++i)
				{
					if(*(pageList + i + rest) != PAD_PATTERN)
					{
						//before block corrupted
						//just stores the begining of the pad not where its corrupted
						//posCorrupt = (int)(pageList+rest+Config_.PadBytes_);
						fn(pageList+rest+Config_.PadBytes_, OAStats_.ObjectSize_);
						beforeBlock++;
						break;
					}
					if(*(pageList + back + i + rest) != PAD_PATTERN)
					{
						//after block corrupted
						//just stores the begining of the pad not where its corrupted
						//pos2Corrupt = (int)(pageList+rest+Config_.PadBytes_);
						fn(pageList+rest+Config_.PadBytes_, OAStats_.ObjectSize_);
						afterBlock++;
						break;
					}
				}
			}
			//walks the page list
			pageWalker = pageWalker->Next;
		}
		//return total corruptions
		return beforeBlock+afterBlock;
	}
	/******************************************************************************/
	/*!
	  \brief
		Frees all empty pages.

	  \return
	  how many pages where freed
	*/
	/******************************************************************************/
	unsigned ObjectAllocator::FreeEmptyPages(void)//headers: check  debug: check   ++
	{
		if(PList == NULL)
		{
			return 0;
		}
		GenericObject * curPage;
		curPage = PList; 

		//if its the last page
		if(curPage->Next == NULL)
		{
			//check if any object are in use, if none free
			if(OAStats_.ObjectsInUse_ == 0)
			{
				//free page
				char * freePage = reinterpret_cast<char *>(curPage);

				//Set poiter to NULL
				PList = NULL;
				FList = NULL;
				
				//delete last of everything
				delete [] freePage;

				//Update Counters
				--(OAStats_.PagesInUse_);
				OAStats_.FreeObjects_ -= Config_.ObjectsPerPage_;

				//return number of pages freed
				return 1;
			}
			else
			{
				//return number of pages freed
				return 0;
			}
		}


		//total pages freed counter
		int totalcount = 0;

		//store the curPage
		GenericObject * prevPage;
		prevPage = curPage;

		//Walk page list
		while(curPage != NULL)
		{
			//page to check to be freed
			unsigned char * freePage = reinterpret_cast<unsigned char *>(curPage);

			/////check page to be freed
			//if the number of objects in page where free = objects per page then free page
			if(check_empty_page(freePage))
			{
				/////Free page
				//if same page your at the head of the page list just move down
				if(curPage == prevPage)
				{
					//next page
					PList = PList->Next;
					prevPage = prevPage->Next;
					curPage  = curPage->Next;
				}
				else
				{
					//make what the prev page point past the cur page
					prevPage->Next = curPage->Next;
					curPage = curPage->Next;
				}

				fix_free_list(freePage);

				//delete page to be freed
				delete [] freePage;

				//count how many pages where freed
				totalcount++;

				//Update Counter
				--(OAStats_.PagesInUse_);
				OAStats_.FreeObjects_ -= Config_.ObjectsPerPage_;
			}
			else //this page should not be freed
			{
				//if they are pointing at same page only move curPage to make prevPage actually the prevPage
				if(curPage == prevPage)
				{
					curPage = curPage->Next;
				}
				else //else just move both down the list
				{
					prevPage = curPage;
					curPage = curPage->Next;
				}
			}
		}

		//How many pages where freed
		return totalcount;
	}
	/******************************************************************************/
	/*!
	  \brief
		checks the pased in page to see if it is empty.

	  \param freePage
		Page to check if empty

	  \return
	  true if the page is empty else flase
	*/
	/******************************************************************************/
	bool ObjectAllocator::check_empty_page(unsigned char * freePage)
	{
		//counts to see how many object where free in this page
		unsigned objectCount = 0;
		int front = Config_.LeftAlignSize_+Config_.HeaderBlocks_+Config_.PadBytes_+sizeof(int);

		//Header check instead of debug check
		if(Config_.HeaderBlocks_ > 0)
		{
			//for each object in page check info
			for(unsigned i = 0; i < Config_.ObjectsPerPage_; ++i)
			{
				int object = (OAStats_.ObjectSize_+(Config_.PadBytes_*2)+
					Config_.InterAlignSize_+Config_.HeaderBlocks_) *i;
				//if anywhere theres not a free object break and move on.
				if(*(freePage+object+front-Config_.PadBytes_-1) == 0x01)
				{
					break;
				}
				else
				{
					objectCount++;
				}
			}
		}
		else if(Config_.DebugOn_ == true)
		{
			//for each object in page check debug info
			for(unsigned i = 0; i < Config_.ObjectsPerPage_; ++i)
			{
				int object = (OAStats_.ObjectSize_+(Config_.PadBytes_*2)+
							Config_.InterAlignSize_+Config_.HeaderBlocks_) *i;

				//if anywhere theres not a free object break and move on.
				if(*(freePage+object+front+sizeof(int)) != UNALLOCATED_PATTERN && *(freePage+object+front+sizeof(int)) != FREED_PATTERN)
				{
					break;
				}
				else
				{
					objectCount++;
				}
			}
		}
		else
		{
			//for each object in page other check
			for(unsigned i = 0; i < Config_.ObjectsPerPage_; ++i)
			{
				int object = (OAStats_.ObjectSize_+(Config_.PadBytes_*2)+
							Config_.InterAlignSize_+Config_.HeaderBlocks_) *i;
		
				GenericObject * freeWalker;
				freeWalker = FList;
					
				//if curObject is on the freeList
				while(freeWalker != NULL)
				{
					//if curObject is on the freeList
					if((freePage+front+object) == reinterpret_cast<unsigned char *>(freeWalker))
					{
						break;
					}
					freeWalker = freeWalker->Next;
				}
				//true if it doesn't break
				if(freeWalker == NULL)
				{
					//object in use on this page.
					//not all object on this page are on free list
					break;
				}
				else
				{
					objectCount++;
				}
			}
		}
		if(objectCount == Config_.ObjectsPerPage_)
		{
			return true;
		}
		return false;
	}
	/******************************************************************************/
	/*!
	  \brief
		removes object on the free list that are on this page.

	  \param freePage
		Page that will be deleted
	*/
	/******************************************************************************/
	void ObjectAllocator::fix_free_list(unsigned char * freePage)
	{
		//walk through free list until find object in the free list thats on the page
		while(reinterpret_cast<unsigned char *>(FList) > freePage && reinterpret_cast<unsigned char *>(FList) < freePage+OAStats_.PageSize_)
		{
			FList = FList->Next;
		}

		//make sure its not null which means it found nothing
		//free list is null already
		if(FList != NULL)
		{
			//set curFree to the next object on the free list
			GenericObject * curFree, * prevFree;
			curFree = FList->Next;
			prevFree = FList;
			while(curFree != NULL)
			{
				if(reinterpret_cast<unsigned char *>(curFree) > freePage && reinterpret_cast<unsigned char *>(curFree) < freePage+OAStats_.PageSize_)
				{
					prevFree->Next = curFree->Next;
					curFree = curFree->Next;
				}
				else
				{
					curFree = curFree->Next;
					prevFree = prevFree->Next;
				}
			}
		}
	}
	/******************************************************************************/
	/*!
	  \brief
		Checks the freed object for errors.

	*/
	/******************************************************************************/
	void ObjectAllocator::validate_object() throw(OAException)
	{
		//Adds the Object back into the free list
		GenericObject * pageLoop;
		pageLoop = PList;
		
		//Goes through all the pages to find page that includes object
		while(pageLoop != NULL)
		{
			//checks to see if the object is in the bounds of the current page
			if(reinterpret_cast<unsigned char *>(FList) < reinterpret_cast<unsigned char *>(pageLoop) + OAStats_.PageSize_ && reinterpret_cast<unsigned char *>(FList) > reinterpret_cast<unsigned char *>(pageLoop))
			{
				//Used to find if the object is on the alignment boundrys
				int Align;
				if(Config_.Alignment_ == 0)
				{
					Align = (OAStats_.ObjectSize_+
							 Config_.InterAlignSize_+
							 Config_.HeaderBlocks_+
							 (Config_.PadBytes_*2));
				}
				else
				{
					Align = Config_.Alignment_;
				}

				//checks if its on the right offset
				int sizeOfFront = ((sizeof(int) + Config_.LeftAlignSize_ + Config_.HeaderBlocks_ + Config_.PadBytes_));
				int pageOffset = reinterpret_cast<char *>(FList) - (reinterpret_cast<char *>(pageLoop) + sizeOfFront);
				if((pageOffset)% Align != 0)
				{	
					Printf();
					throw(OAException(OAException::E_BAD_BOUNDARY, "validate_object: Object on bad boundary in page."));
				}

				return;
			}
			pageLoop = pageLoop->Next;
		}
		//object wasn't in any page
		Printf();
		throw(OAException(OAException::E_BAD_ADDRESS, "validate_object: Object not on a page."));	
	}
	/******************************************************************************/
	/*!
	  \brief
		Returns true if FreeEmptyPages and alignments are implemented.

	  \return
		true I got this

	*/
	/******************************************************************************/
	bool ObjectAllocator::ImplementedExtraCredit(void)
	{
		return true;
	}
	/******************************************************************************/
	/*!
	  \brief
		Sets the State of Debug so you can change it

	  \param State
		to turn Debug on and off

	*/
	/******************************************************************************/
	void ObjectAllocator::SetDebugState(bool State)
	{
		Config_.DebugOn_ = State;	
	}
	/******************************************************************************/
	/*!
	  \brief
		returns a pointer to the free list

	  \return
		the Free List

	*/
	/******************************************************************************/
	const void * ObjectAllocator::GetFreeList(void) const
	{
		return FList;
	}
	/******************************************************************************/
	/*!
	  \brief
		returns a pointer to the page list

	  \return
		the Page List

	*/
	/******************************************************************************/
	const void * ObjectAllocator::GetPageList(void) const
	{ 
		return PList;
	}
	/******************************************************************************/
	/*!
	  \brief
		returns the Configeration of the Memory manager

	  \return
		returns Config_

	*/
	/******************************************************************************/
	OAConfig ObjectAllocator::GetConfig(void) const
	{
		return Config_;
	}
	/******************************************************************************/
	/*!
	  \brief
		returns the Stats of the Memory manager

	  \return
		returns Config_

	*/
	/******************************************************************************/
	OAStats ObjectAllocator::GetStats(void) const
	{
		return OAStats_;
	}
}