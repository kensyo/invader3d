#include "GameLib/Framework.h"
using namespace GameLib;

#include "Pad.h"
#include "Sequence/Parent.h"

namespace GameLib{
	void Framework::configure(Configuration* c){
		c->enableVSync(true); //���j�^�̏������������ɍ��킹�ăt���[����i�߂�B�e�B�A�����O���N����Ȃ�
		c->setArchiveNumber(1);//�A�[�J�C�u�f�[�^�̐�
		c->setArchiveName(0, "data.bin");//�A�[�J�C�u�t�@�C��
		c->setLoadMode(LOAD_ARCHIVE_ONLY);//�A�[�J�C�u�t�@�C���݂̂��f�[�^���Ƃ���
	}
	void Framework::update(){
		if (!Sequence::Parent::instance()){
			Sequence::Parent::create();//�V�[�P���X�̑匳�̍쐬
			Pad::create();//�p�b�h�V���O���g���̍쐬
		}
		Sequence::Parent::instance()->update();
		if (isEndRequested()){//�~�{�^���������ꂽ���̑Ώ�
			Sequence::Parent::destroy(); //�V�[�P���X�̍폜
			Pad::destroy(); //�p�b�h�V���O���g���̍폜
		}
	}
}