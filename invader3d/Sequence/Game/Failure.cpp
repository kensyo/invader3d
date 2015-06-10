#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "GameLib/Scene/StringRenderer.h"
#include "GameLib/Scene/Font.h"
#include <sstream>
using namespace GameLib;

#include "Sequence/Game/Failure.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/Game/Ready.h"
#include "Sequence/GameOver.h"

using namespace GameLib::Scene;

namespace Sequence{
namespace Game{

Failure::Failure() : 
mCount( 0 ){
}

Failure::~Failure(){
}

Base* Failure::update( Parent* parent ){
	Base* next = this;
	if ( mCount == 60 ){ //1ïbë“Ç¬ 
		if ( parent->lifeNumber() == 0 ){
			next = new GameOver;
		}else{
			next = new Ready;
		}
	}

	Font Gothic = Font::create("ÑMÇr ÉSÉVÉbÉN", 32, false, false, false, true);
	StringRenderer SR = StringRenderer::create(2048, 128);

	ostringstream oss;
	oss << "ïâÇØÅI";
	SR.setFont(Gothic);
	SR.add(120, 120, oss.str().c_str(), 0xffffffff, true);
	SR.draw();

	++mCount;

	return next;
}

} //namespace Game
} //namespace Sequence
