#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "GameLib/Scene/StringRenderer.h"
#include "GameLib/Scene/Font.h"
#include <sstream>
using namespace GameLib;

#include "Sequence/Game/Clear.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/Game/Ready.h"
#include "Sequence/Ending.h"

using namespace GameLib::Scene;

namespace Sequence{
namespace Game{

Clear::Clear() : 
mCount( 0 ){
}

Clear::~Clear(){
}

Base* Clear::update( Parent* parent ){
	Base* next = this;
	if ( mCount == 60 ){ //1秒待つ 
		if ( parent->hasFinalStageCleared() ){ //最終面をクリアしたならエンディングへ
			next = new Ending;
		}else{
			next = new Ready;
		}
	}
	Font Gothic = Font::create("МＳ ゴシック", 32, false, false, false, true);
	StringRenderer SR = StringRenderer::create(2048, 128);

	ostringstream oss;
	oss << "勝ち！";
	SR.setFont(Gothic);
	SR.add(120, 120, oss.str().c_str(), 0xffffffff, true);
	SR.draw();

	++mCount;

	return next;
}

} //namespace Game
} //namespace Sequence
