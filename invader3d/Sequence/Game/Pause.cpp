#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "GameLib/Scene/StringRenderer.h"
#include "GameLib/Scene/Font.h"
#include <sstream>
using namespace GameLib;

#include "Pad.h"
#include "Sequence/Game/Pause.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/Game/Play.h"
#include "Sequence/Title.h"

using namespace GameLib::Scene;

namespace Sequence{
namespace Game{

Pause::Pause() : 
mCursorPosistion( 0 ){
}

Pause::~Pause(){
}

Base* Pause::update( Parent* parent ){
	Base* next = this;
	if (Pad::instance()->isTriggered(Pad::UP, 0)){
		--mCursorPosistion;
		if ( mCursorPosistion < 0 ){ //マイナスは最大値にループ
 			mCursorPosistion = 1;
		}
	}else if ( Pad::instance()->isTriggered(Pad::DOWN, 0) ){
		++mCursorPosistion;
		if ( mCursorPosistion > 1 ){ //1を越えたら0にループ
			mCursorPosistion = 0;
		}
	}else if ( Pad::instance()->isTriggered(Pad::FIRE, 0) ){
		if ( mCursorPosistion == 0 ){ //ゲームに戻る
			next = new Play;
		}else if ( mCursorPosistion == 1 ){ //タイトルへ戻る
			next = new Title;
		}
	}
	//カーソルを書くのを忘れない
	Font Gothic = Font::create("МＳ ゴシック", 32, false, false, false, true);
	StringRenderer SR = StringRenderer::create(2048, 128);

	ostringstream oss;
	oss << "ポーズ！\n";
	oss << "ゲームを続ける！\n";
	oss << "タイトルに戻る！\n";
	SR.setFont(Gothic);
	SR.add(120, 120, oss.str().c_str(), 0xffffffff, true);

	oss.str("");
	oss << "> ";
	SR.add(100, 152 + mCursorPosistion * 32, oss.str().c_str(), 0xffffffff, true);

	SR.draw();

	return next;
}



} //namespace Game
} //namespace Sequence