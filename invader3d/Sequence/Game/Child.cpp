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
	//ASSERT(0);//‚±‚ê‚ª‚ ‚Á‚Ä‚à‘åä•vB‚È‚º‚È‚ç‚±‚ÌŠÖ”‚ÍŒÄ‚Î‚ê‚È‚¢
	return update( parent );
}

} //namespace Game
} //namespace Sequence
