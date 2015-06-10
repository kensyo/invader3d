#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "GameLib/Scene/StringRenderer.h"
#include "GameLib/Scene/Font.h"
#include <sstream>
using namespace GameLib;

#include "Sequence/Title.h"
#include "Sequence/Parent.h"
#include "Sequence/Game/Parent.h"
#include "Pad.h"

using namespace GameLib::Scene;

namespace Sequence{

Title::Title(){} 


Title::~Title(){
}

Base* Title::update( Parent* parent ){
	Base* next = this;

	if (Pad::instance()->isTriggered(Pad::FIRE, 0)){
		next = new Game::Parent();
	}

	Font Gothic = Font::create("ÑMÇr ÉSÉVÉbÉN", 32, false, false, false, true);
	StringRenderer SR = StringRenderer::create(2048, 128);

	ostringstream oss;
	oss << "Invader3DÅI";
	SR.setFont(Gothic);
	SR.add(200, 120, oss.str().c_str(), 0xffffffff, true);
	oss.str("");
	oss << "Press x!";
	SR.add(220, 320, oss.str().c_str(), 0xff00ffff, true);
	SR.draw();

	return next;
}

} //namespace Sequence