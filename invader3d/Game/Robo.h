#ifndef INCLUDED_ROBO_H
#define INCLUDED_ROBO_H

#include "GameLib/Scene/Container.h"
#include "GameLib/Scene/Tree.h"
#include "GameLib/Math/Vector3.h"
namespace GameLib{
	namespace Math{
		class Matrix34;
		class Matrix44;
	}
}
using namespace GameLib::Math;

class Bullet;

class Robo{
public:
	Robo( int id ); //���ʔԍ��ƂƂ��ɃR���X�g���N�g�B0�͎�����1�`�͓G�B
	~Robo();
	void draw() const;
	void update( Robo* robo );
	void setPosition( const Vector3& );
	void setAngleY( float );
	const Vector3* position() const;
	void getView( Matrix34* matrix, Vector3* position ) const;
	void setDamage( int damage );
	int getHitPoint() const;
	int getEnergy() const;
	bool getLockOn() const;
	const Vector3* getBulletPosition( int i ) const;
	float getAngleY() const;

	static const int mJumpUpTime; //�㏸���Ă�������
	static const int mJumpStayTime; //�㏸�㉺�~�܂ł̎���
	static const int mJumpFallTime; //���~�ɂ����鎞��
	static const int mMoveAccelEndCount; //�����n�߂ĉ������I������܂ł̎���
	static const float mMaxMoveSpeed; //�ő�ړ����x
	static const float mJumpHeight; //�ő卂�x
	static const float mCameraDistanceZ; //�����[�g����납��ʂ��̂��H
	static const float mCameraDistanceY; //�����낵�
	static const float mCameraTargetDistanceZ; //�����_�͉����[�g����ɂ��邩�H
	static const float mTurnSpeed; //���񑬓x
	static const int mMaxHitPoint; //�ő�q�b�g�|�C���g
	static const int mMaxEnergy; //����|�C���g�ő�l
	static const int mEnergyPerBullet; //�ꔭ������̏���G�l���M�[
	static const int mEnergyCharge; //���t���[�����܂�G�l���M�[
	static const float mLockOnAngleIn; //���b�N�I������p�x
	static const float mLockOnAngleOut; //���b�N�I������p�x
	static const int mRotationTime; //45�x�J�����񂷂̂ɂ����鎞��
	static const int mLockOnTime; //�G�̂ق��������̂ɂ����鎞��
private:
	void move( bool left, bool right, bool up, bool down );
	void turn( bool left, bool right );
	//�ȈՓI��AI�B�{�^�����͂�Ԃ��B�v���C���[����L�����Ȃ炽�����͂���邾��
	void think( bool* jump, bool* fire, bool* turn, bool* left, bool* right, bool* up, bool* down ,bool* l, bool* r, bool* lock) const;

	Vector3 mPosition;
	float mAngleY;
	int mId;
	GameLib::Scene::Container mSceneContainer;
	GameLib::Scene::Tree mTree;
	
	Bullet* mBullets;
	int mBulletNumber;

	int mCount; //�ړ��J�n�㉽�t���[�����������J�E���g
	Vector3 mVelocity; //���x
	float mAngleVelocityY; //��]���x

	enum Mode{
		MODE_JUMP_UP, //�W�����v�㏸��
		MODE_JUMP_STAY, //�W�����v���Œ�~��
		MODE_JUMP_FALL, //�W�����v�~����
		MODE_ON_LAND, //���n��ԁB�����ʒu���󒆂̏ꍇ�͋󒆂ɕ��V���邱�ƂɂȂ�B
		MODE_ON_ROTATION,// ��]��
		MODE_ON_LOCKING,//�G�̂ق���U�������
	};
	Mode mMode;

	int mHitPoint; //�̗�
	int mEnergy; //��������̂ɕK�v�ȃG�l���M�[
	bool mLockOn; //���b�N�I�����Ă邩�H
};

#endif
