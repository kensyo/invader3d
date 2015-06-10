#include "GameLib/GameLib.h"
#include "Sequence/Child.h"
#include "Sequence/Parent.h"

namespace Sequence{

Child::~Child(){
}

Base* Child::update( Base* p ){
	Parent* parent = dynamic_cast< Parent* >( p );
	ASSERT( parent );
	//ASSERT(0);//���ꂪ�����Ă����v�B�Ȃ��Ȃ炱�̊֐��͌Ă΂�Ȃ�
	return update( parent );
}

} //namespace Sequence
