#ifndef INCLUDED_SEQUENCE_GAME_FAILURE_H
#define INCLUDED_SEQUENCE_GAME_FAILURE_H

#include "Sequence/Game/Child.h"

namespace Sequence{
namespace Game{
class Parent;

class Failure : public Child{
public:
	Failure();
	~Failure();
	Base* update( Parent* );
private:
	int mCount;
};

} //namespace Game
} //namespace Sequence

#endif