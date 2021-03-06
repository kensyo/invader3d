#include "GameLib/GameLib.h"
#include "Sequence/Child.h"
#include "Sequence/Parent.h"

namespace Sequence{

Child::~Child(){
}

Base* Child::update( Base* p ){
	Parent* parent = dynamic_cast< Parent* >( p );
	ASSERT( parent );
	//ASSERT(0);//これがあっても大丈夫。なぜならこの関数は呼ばれない
	return update( parent );
}

} //namespace Sequence
