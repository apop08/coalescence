#pragma once

#include "ILogical.h"

namespace Scuttlemeyer
{
	//class Transform;

	class Button : public ILogical
	{
	public:
		Button();
		virtual void Initialize();
		virtual bool isMouseOn();
		virtual bool isTriggered();
		virtual void SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~Button();
	private:
		bool trigger;
	};
}