/******************************************************************************/
/*!
\file   Message.cpp
\author Kevin Verholtz (Base implementation)
\author Matt Sutherlin (Variadic list)
\par    email: kevin.verholtz\@digipen.edu
\par    DigiPen login: kevin.verholtz
\par    Course: GAM200
\date   12/8/2011
\brief  
Internal messaging system.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "Message.h"

namespace Scuttlemeyer
{
	unsigned int Message::smNumberOfMessages = 0;

	Message::Message(Scuttlemeyer::Mid::MessageIdType id, int count, ...)
	{
		mMessageId = id;
		mParamCount = count;

		va_list ap;

		va_start(ap, count); //Requires the last fixed parameter (to get the address)

		for(int i = 0; i < count; i++)
		{
			mMessageData.push_back(va_arg(ap, void*));
		}

		va_end(ap);

		++smNumberOfMessages;
	}

	Message::~Message()
	{
		for(std::vector<void*>::iterator it = mMessageData.begin(); it != mMessageData.end(); ++it)
		{
			//delete [] reinterpret_cast<char*>(*it);
			//delete (std::string*)(*it);
		}
		--smNumberOfMessages;
	}
}