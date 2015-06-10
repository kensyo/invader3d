#ifndef INCLUDED_SEQUENCE_GAME_CLEAR_H
#define INCLUDED_SEQUENCE_GAME_CLEAR_H

#include "Sequence/Game/Child.h"

namespace Sequence{
namespace Game{
class Parent;

class Clear : public Child{
public:
	Clear();
	~Clear();
	Base* update( Parent* );
private:
	int mCount;
};

} //namespace Game
} //namespace Sequence

#endif