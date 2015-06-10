#include "GameLib/GameLib.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
using namespace GameLib;

#include "Sequence/Game/Play.h"
#include "Sequence/Game/Clear.h"
#include "Sequence/Game/Failure.h"
#include "Sequence/Game/Judge.h"
#include "Sequence/Game/Pause.h"
#include "Sequence/Game/Parent.h"
#include "Game/State.h"
#include "Pad.h"

namespace Sequence{
namespace Game{

Play::Play(){ //�������Ȃ�
}

Play::~Play(){ //�������Ȃ�
}

//�Q�[���{��
Base* Play::update( Parent* parent ){
	Base* next = this;

	State* state = parent->state();

	bool cleared = state->hasCleared();
	bool die1P = !state->isAlive( 0 );
	//�N���A�������`�F�b�N
		if ( cleared && !die1P ){
			parent->goToNextStage();
			next = new Clear;
		}else if ( die1P ){
			parent->reduceLife();
			next = new Failure;
		}
	if ( ( next == this ) && Pad::instance()->isTriggered(Pad::SELECT, 0) ){ //q�{�^���Ń|�[�Y�֍s���B�������܂��ڂ��ĂȂ�������
		next = new Pause;
	}
	//�X�V����ѕ`��
	state->update();
	//�`��
//	state->draw();

	return next;
}

} //namespace Game
} //namespace Sequence
