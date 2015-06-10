#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "GameLib/Scene/StringRenderer.h"
#include "GameLib/Scene/Font.h"
#include <sstream>
using namespace GameLib;

#include "Sequence/GameOver.h"
#include "Sequence/Parent.h"
#include "Sequence/Title.h"

using namespace GameLib::Scene;

namespace Sequence{

GameOver::GameOver() : 
mCount( 0 ){
}

GameOver::~GameOver(){
}

Base* GameOver::update( Parent* ){
	Base* next = this;
	if ( mCount == 60 ){ //1•b‘Ò‚Â 
		next = new Title;
	}
	//•`‰æ
	Font Gothic = Font::create("„M‚r ƒSƒVƒbƒN", 32, false, false, false, true);
	StringRenderer SR = StringRenderer::create(2048, 128);

	ostringstream oss;
	oss << "ƒQ[ƒ€ƒI[ƒo[I";
	SR.setFont(Gothic);
	SR.add(120, 120, oss.str().c_str(), 0xffffffff, true);
	SR.draw();

	++mCount;

	return next;
}

} //namespace Sequence