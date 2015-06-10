#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "GameLib/Scene/StringRenderer.h"
#include "GameLib/Scene/Font.h"
#include <sstream>
using namespace GameLib;

#include "Sequence/Game/Ready.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/Game/Play.h"

using namespace GameLib::Scene;

namespace Sequence{
namespace Game{

Ready::Ready() : 
mCount( 0 ), 
mStarted( false ){
}

Ready::~Ready(){
}

Base* Ready::update( Parent* parent ){
	Base* next = this;
	if ( !mStarted ){
		parent->startLoading();
		mStarted = true;
	}
	Font Gothic = Font::create("„M‚r ƒSƒVƒbƒN", 32, false, false, false, true);
	StringRenderer SR = StringRenderer::create(2048, 128);

	ostringstream oss;
	if ( mCount >= 120 ){ //2•bŒo‚Á‚½‚çPlay‚Ö
		next = new Play;
	}else if ( mCount >= 60 ){ //Go‚ð•\Ž¦
	oss << "GOI";
	SR.setFont(Gothic);
	SR.add(120, 120, oss.str().c_str(), 0xffffffff, true);
	}else{ //Ready...‚ð•\Ž¦
	oss << "READYI";
	SR.setFont(Gothic);
	SR.add(120, 120, oss.str().c_str(), 0xffffffff, true);
	}
	SR.draw();

	++mCount;

	return next;
}

} //namespace Game
} //namespace Sequence