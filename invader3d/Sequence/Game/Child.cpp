#include "GameLib/GameLib.h"
#include "Sequence/Game/Child.h"
#include "Sequence/Game/Parent.h"

namespace Sequence{
namespace Game{

Child::~Child(){
}

Base* Child::update( Base* p ){
	Parent* parent = dynamic_cast< Parent* >( p );
	ASSERT( parent );
	//ASSERT(0);//これがあっても大丈夫。なぜならこの関数は呼ばれない
	return update( parent );
}

} //namespace Game
} //namespace Sequence
