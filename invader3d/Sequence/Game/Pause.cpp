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
		if ( mCursorPosistion < 0 ){ //�}�C�i�X�͍ő�l�Ƀ��[�v
 			mCursorPosistion = 1;
		}
	}else if ( Pad::instance()->isTriggered(Pad::DOWN, 0) ){
		++mCursorPosistion;
		if ( mCursorPosistion > 1 ){ //1���z������0�Ƀ��[�v
			mCursorPosistion = 0;
		}
	}else if ( Pad::instance()->isTriggered(Pad::FIRE, 0) ){
		if ( mCursorPosistion == 0 ){ //�Q�[���ɖ߂�
			next = new Play;
		}else if ( mCursorPosistion == 1 ){ //�^�C�g���֖߂�
			next = new Title;
		}
	}
	//�J�[�\���������̂�Y��Ȃ�
	Font Gothic = Font::create("�M�r �S�V�b�N", 32, false, false, false, true);
	StringRenderer SR = StringRenderer::create(2048, 128);

	ostringstream oss;
	oss << "�|�[�Y�I\n";
	oss << "�Q�[���𑱂���I\n";
	oss << "�^�C�g���ɖ߂�I\n";
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