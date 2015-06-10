#ifndef INCLUDED_SEQUENCE_GAME_OVER_H
#define INCLUDED_SEQUENCE_GAME_OVER_H

#include "Sequence/Child.h"

namespace Sequence{
class Parent;

class GameOver : public Child{
public:
	GameOver();
	~GameOver();
	Base* update( Parent* );
private:
	int mCount;
};

} //namespace Sequence

#endif
