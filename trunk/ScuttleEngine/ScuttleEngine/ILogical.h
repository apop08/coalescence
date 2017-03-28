#pragma once

#include "Component.h"

namespace Scuttlemeyer
{
	class ILogical : public GameComponent
	{
	public:
		virtual void Unload() {};
		virtual void Initialize() {};
		//virtual int ChangeState(State nState) {return 0;};
		virtual void Activate() {};
		virtual void Deactivate() {};
		virtual void Triggered() {};
		virtual Mid::MessageIdType SendMessage(Message *) { return Mid::None; }
		virtual void Serialize(TextLoader& stream) {};
		virtual void Update(float dt) {};
	public:
		ILogical* Next;
		ILogical* Prev;
		enum eAnimModes {PLAYLOOPED, PLAYONCE, STOP, FORWARD, REVERSE};
	protected:
		virtual ~ILogical() {};
	protected:
		std::string mName;
		bool mBIsActivated;
	};
}