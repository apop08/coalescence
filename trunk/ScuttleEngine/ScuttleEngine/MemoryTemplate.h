/******************************************************************************/
/*!
\file   MemoryTemplate.h
\author Kevin Verholtz
\par    email: kevin.verholtz\@digipen.edu
\par    DigiPen login: kevin.verholtz
\par    Course: GAM200
\date   12/8/2011
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/
//Credit to Killian Koenig for initial implementation
#pragma once

#include <cstdlib>
#include <iostream>
#include "ObjectAllocator.h"

namespace Scuttlemeyer
{

#define MAX_CUSTOM_ALLOCATOR_SIZE 256
#define MIN_CUSTOM_ALLOCATOR_SIZE 4
 
#define UseCustomAllocator(T, DebugOn)                                     \
static const bool USING_DEBUG_ALLOCATOR = DebugOn;                         \
static void* operator new(unsigned)                                        \
{                                                                          \
   /*std::cout << #T << "::new(" << sizeof(T) << ")" << std::endl;*/            \
                                                                            \
   typedef typename select_allocator<sizeof(T), DebugOn>::type alloc_type;  \
   return alloc_type::malloc();                                             \
}                                                                          \
                                                                            \
static void operator delete(void* data)                                    \
{                                                                          \
   /*std::cout << #T << "::delete(" << sizeof(T) << ")" << std::endl;*/         \
                                                                            \
   typedef typename select_allocator<sizeof(T), DebugOn>::type alloc_type;  \
   alloc_type::free(data);                                                  \
}
 
// Metafunctions
namespace mp
{
 
// Select type based on boolean condition
template <bool IF, typename THEN, typename ELSE>
struct conditional
{
   typedef THEN type;
};
 
template <typename THEN, typename ELSE>
struct conditional<false, THEN, ELSE>
{
   typedef ELSE type;
};
 
template <int N>
struct find_next_power_of_2
{
private:
   static const int n0 = N - 1;
   static const int n1 = n0 | n0 >> 1;
   static const int n2 = n1 | n1 >> 2;
   static const int n3 = n2 | n2 >> 4;
   static const int n4 = n3 | n3 >> 8;
   static const int n5 = n4 | n4 >> 16;
 
public:
   static const int value = n5 + 1;
};
 
}
 
template <int S>
struct default_allocator
{
   static void* malloc(void);
   static void  free(void* data);
};
 
// Replaces malloc/free with our object allocator
template <int S, bool DebugOn>
struct custom_allocator
{
   static void* malloc(void);
   static void  free(void* data);
 
   static ObjectAllocator Allocator_;
};
 
template <int S, bool DebugOn>
struct select_allocator
{
private:
   static const int size = mp::find_next_power_of_2<S>::value;
 
 
public:
 
   // Select the template that will provide the correct allocator functionality
   // if the size is not within the specified range use default C++ new
   // else use custom allocator
   typedef typename
     mp::conditional< MAX_CUSTOM_ALLOCATOR_SIZE != 0 &&
                     (size > MAX_CUSTOM_ALLOCATOR_SIZE ||
                      size < MIN_CUSTOM_ALLOCATOR_SIZE),
                      default_allocator<S>, custom_allocator<size,  
DebugOn> >::type type;
};
 
template <int S>
inline void* default_allocator<S>::malloc(void)
{
   std::cout << "Default C++ allocating " << S << " bytes" << std::endl;
   return ::malloc(S);
}
 
template <int S>
inline void default_allocator<S>::free(void* data)
{
   std::cout << "Default C++ freeing " << S << " bytes" << std::endl;
   ::free(data);
}
 
template <int S, bool DebugOn>
ObjectAllocator custom_allocator<S, DebugOn>::Allocator_( S, OAConfig(false, DEFAULT_OBJECTS_PER_PAGE, DEFAULT_MAX_PAGES, DebugOn));
 
 
template <int S, bool DebugOn>
void* custom_allocator<S, DebugOn>::malloc(void)
{
   //std::cout << "Custom allocating " << S << " bytes" << std::endl;
   return Allocator_.Allocate();
}
 
template <int S, bool DebugOn>
void custom_allocator<S, DebugOn>::free(void* data)
{
   //std::cout << "Custom freeing " << S << " bytes" << std::endl;
   Allocator_.Free(data);
} 
}