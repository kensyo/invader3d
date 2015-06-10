#ifndef INCLUDED_GAME_STATE_H
#define INCLUDED_GAME_STATE_H

#include "GameLib/Scene/PrimitiveRenderer.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Vector2.h"

//�錾��������B�C���N���[�h�͂��Ȃ��Ă悢�B
class Robo;
class Stage;

class State{
public:
	State( int stageID );
	~State();
	void update();
//	void draw() const;
	//�O��������
	bool hasCleared() const; //�N���A�������H
	bool isAlive( int playerID ) const; //�����Ă��邩�H
	void drawRect(GameLib::Math::Vector2* p, unsigned c1, unsigned c2);
private:
	int mAbleToSeeRoboIndex;//���L�����Ɉ�ԋ߂��G
	Robo** mRobo;
	Stage* mStage;
	GameLib::Scene::PrimitiveRenderer mPrimitiveRenderer; //�����`���������߂ɕK�v�B��Ƀt�����g�G���h�i�g�o�o�[�Ȃǁj�Ɏg�p�B
	const GameLib::Math::Vector3 mAmbient; //�����B�萔�B
	int mTime; //�߂����t���[��
	const int mTimeLimit; //�^�C�����~�b�g�܂ł̎��ԁB���̃^�C�����~�b�g�ŃN���A�[�ƂȂ�B

	int mStageID; //�X�e�[�W�̔ԍ��B0����n�܂�X�e�[�W�N���A���Ƃ�1�Â�����B�G�̐�������Ɉˑ��imStageID+1 �C)
};

#endif
