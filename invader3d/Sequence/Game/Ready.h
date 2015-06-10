#ifndef INCLUDED_SEQUENCE_GAME_READY_H
#define INCLUDED_SEQUENCE_GAME_READY_H

#include "Sequence/Game/Child.h"


namespace Sequence{
namespace Game{
class Parent;

class Ready : public Child{
public:
	Ready();
	~Ready();
	Base* update( Parent* );
private:
	int mCount;
	bool mStarted;
};

} //namespace Game
} //namespace Sequence

#endif