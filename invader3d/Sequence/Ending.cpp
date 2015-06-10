#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "GameLib/Scene/StringRenderer.h"
#include "GameLib/Scene/Font.h"
#include <sstream>
using namespace GameLib;

#include "Sequence/Ending.h"
#include "Sequence/Parent.h"
#include "Sequence/Title.h"

using namespace GameLib::Scene;

namespace Sequence{

Ending::Ending() : 
mCount( 0 ){
}

Ending::~Ending(){
}

Base* Ending::update( Parent* ){
	Base* next = this;
	if ( mCount == 120 ){ //2•b‘Ò‚Â 
		next = new Title;
	}
	Font Gothic = Font::create("„M‚r ƒSƒVƒbƒN", 32, false, false, false, true);
	StringRenderer SR = StringRenderer::create(2048, 128);

	ostringstream oss;
	oss << "CongratulationsI\n";
	oss << "ì¬ŽÒFKensyo";
	SR.setFont(Gothic);
	SR.add(120, 120, oss.str().c_str(), 0xffffffff, true);
	SR.draw();

	++mCount;

	return next;
}

} //namespace Sequence