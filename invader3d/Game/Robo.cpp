#include <sstream>
#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Functions.h"

#include "GameLib/Scene/Animation.h"
#include "GameLib/Scene/Tree.h"

#include "Robo.h"
#include "Bullet.h"
#include "Pad.h"
using namespace GameLib;
using namespace GameLib::Scene;

const int Robo::mJumpUpTime = 20; //�㏸���Ă�������
const int Robo::mJumpStayTime = 60; //�㏸�㉺�~�܂ł̎���
const int Robo::mJumpFallTime = 40; //���~�ɂ����鎞��
const int Robo::mMoveAccelEndCount = 30; //�����n�߂ĉ������I������܂ł̎���
const float Robo::mMaxMoveSpeed = 0.5; //�ő�ړ����x
const float Robo::mJumpHeight = 20.0; //�ő卂�x
const float Robo::mCameraDistanceZ = 10.0; //�����[�g����납��ʂ��H
const float Robo::mCameraDistanceY = 4.0; //�����낵�
const float Robo::mCameraTargetDistanceZ = 20.0; //�����_�͉����[�g����H
const float Robo::mTurnSpeed = 1.0; //���񑬓x
const int Robo::mMaxHitPoint = 20; //�ő�q�b�g�b�|�C���g
const int Robo::mMaxEnergy = 100; //�ő啐��|�C���g
const int Robo::mEnergyPerBullet = 27; //����̏���G�l���M�[
const int Robo::mEnergyCharge = 1; //���t���[�����܂�G�l���M�[�̗�
const float Robo::mLockOnAngleIn = 20.0; //���b�N�I������p�x
const float Robo::mLockOnAngleOut = 30.0; //���b�N�I�����O���p�x
const int Robo::mRotationTime = 3; //45�x�J�����񂷂̂ɂ�����t���[����
const int Robo::mLockOnTime = 4; //�G�̂ق����ނ��̂ɂ����鎞��

Robo::Robo( int id ) : 
mPosition( 0.0, 0.0, 0.0 ),
mAngleY( 0.0 ),
mId( id ),
mBullets( 0 ),
mBulletNumber( 100 ), //���߂���
mCount( 0 ),
mVelocity( 0.0, 0.0, 0.0 ),
mMode( MODE_ON_LAND ),
mHitPoint( mMaxHitPoint ),
mEnergy( mMaxEnergy ),
mLockOn( true ){ //���������Ďn�܂�̂ōŏ��̓��b�N�I���B
	mSceneContainer = Container::create( "data/robo.txt" );
	while ( !mSceneContainer.isReady() ){
		;
	}
//	mModel = mSceneContainer.createModel( "robo" );
	mTree = mSceneContainer.createTree("robo");
//	mTree.setAnimation(mSceneContainer.animation("walking"));
	
	mBullets = new Bullet[ mBulletNumber ];
	if (mId != 0){
		mHitPoint = mMaxHitPoint / 4;
	}
}

Robo::~Robo(){
	SAFE_DELETE_ARRAY( mBullets );
}

void Robo::setPosition( const Vector3& a ){
	mPosition = a;
}

void Robo::setAngleY( float a ){
	mAngleY = a;
}

const Vector3* Robo::getBulletPosition( int i ) const {
	if ( mBullets[ i ].isEmpty() ){
		return 0;
	}else{
		return mBullets[ i ].position();
	}
}

void Robo::update( Robo* enemy ){
	//����ł�
	if ( mHitPoint <= 0 ){
		return;
	}
	//AI�̎v�l�B�v���C���[�̏ꍇ�͓��͂��擾���ĕԂ�����
	bool iJump;
	bool iFire;
	bool iTurn;
	bool iLeft; 
	bool iRight; 
	bool iUp;
	bool iDown; 
	bool iL;
	bool iR;
	bool iLock;
	think( &iJump, &iFire, &iTurn, &iLeft, &iRight, &iUp, &iDown, &iL, &iR , &iLock);
	//�ȉ�����������͂��g���čs��
	const Vector3& enemyPos = *enemy->position();
	++mCount;
	float t;
	//�I�[�g�}�g�����̋L�q
	switch ( mMode ){
		case MODE_JUMP_UP:
			move( iLeft, iRight, iUp, iDown ); //�R�[�h�������Ȃ�̂Ŋ֐��ɔ�΂�
			//�㏸
			t = mJumpHeight / static_cast< float >( mJumpUpTime );
			mVelocity.y = t;
			if ( !iJump ){ //�W�����v���͂��Ȃ��̂ŉ��~�ɕω�
				mMode = MODE_JUMP_FALL;
				mCount = 0;
			}else if ( mCount >= mJumpUpTime ){ //�㏸�I��
				mMode = MODE_JUMP_STAY; 
				mCount = 0;
			}
			break;
		case MODE_JUMP_STAY:
			move( iLeft, iRight, iUp, iDown ); //�R�[�h�������Ȃ�̂Ŋ֐��ɔ�΂�
			mVelocity.y = 0.0;
			if ( !iJump ){ //�W�����v���͂��Ȃ��̂ŉ��~�ɕω�
				mMode = MODE_JUMP_FALL;
				mCount = 0;
			}else if ( mCount >= mJumpStayTime ){ //���~��
				mMode = MODE_JUMP_FALL;
				mCount = 0;
			}
			break;
		case MODE_JUMP_FALL:
			move( iLeft, iRight, iUp, iDown ); //�R�[�h�������Ȃ�̂Ŋ֐��ɔ�΂�
			//���~
			t = mJumpHeight / static_cast< float >( mJumpFallTime );
			mVelocity.y = -t;
			break;
		case MODE_ON_ROTATION:
			mAngleY += mAngleVelocityY;
			if (mCount >= mRotationTime){
				mMode = MODE_ON_LAND;
			}
			break;
		case MODE_ON_LOCKING:
			move( iLeft, iRight, iUp, iDown ); //�R�[�h�������Ȃ�̂Ŋ֐��ɔ�΂�
			mAngleY += mAngleVelocityY;
			if (mCount >= mLockOnTime){
				mMode = MODE_ON_LAND;
			}

			break;
		case MODE_ON_LAND:
			if ( iJump ){
				mMode = MODE_JUMP_UP;
				mCount = 0;
			}
			else if ( iL || iR ){
				mMode = MODE_ON_ROTATION;
				if (iL){
					mAngleVelocityY = 45.f / mRotationTime;
				}
				else{
					mAngleVelocityY = -45.f / mRotationTime;
				}
				mCount = 0;
			}
			else if ( iLock ){
				mMode = MODE_ON_LOCKING;
				mCount = 0;

				//�G�̕��Ɍ�����B
				Vector3 dir;
				dir.setSub( enemyPos, mPosition ); //��������G�ւ̃x�N�g��
				//Y���p�x��atan2( x, z )�B
				t = Math::atan2( dir.x, dir.z );
				//180�x�ȏ㍷�������+-360�x���ċt�ɉ�
				if ( t - mAngleY > 180.f ){
					t -= 360.f;
				}else if ( mAngleY - t > 180.f ){
					t += 360.f;
				}
				mAngleVelocityY = ( t - mAngleY ) / static_cast< float >( mLockOnTime );
			}
			else if (iTurn){
				turn(iLeft, iRight); //�R�[�h�������Ȃ�̂Ŋ֐��ɔ�΂�
			}
			else{
				move( iLeft, iRight, iUp, iDown ); //�R�[�h�������Ȃ�̂Ŋ֐��ɔ�΂�
			}
			mVelocity.y = 0.f;
			break;
	}
	//�������牺�͏Փˏ���������Ƃ��̌�ɂȂ�B
	if (mId == 0){
		//�X�e�[�W�O�ɏo�Ȃ��悤�ɂ���
		mPosition += mVelocity;
		if (mPosition.x >= 50.f){
			mPosition.x = 50.f;
		}
		if (mPosition.x <= -50.f){
			mPosition.x = -50.f;
		}
		if (mPosition.z >= 50.f){
			mPosition.z = 50.f;
		}
		if (mPosition.z <= -50.f){
			mPosition.z = -50.f;
		}
	}
	else{
		Vector3 t;
		t.setMul( mVelocity, 0.25f );//�G��0.25�{�̑����ɂ���
		mPosition += t;
	}
	if ( mPosition.y < 0.f ){
		mPosition.y = 0.f;
		mMode = MODE_ON_LAND;
	}
	//���퐶��
	if ( iFire ){
		//�G�l���M�[����Ă�H
		if ( mEnergy >= mEnergyPerBullet ){
			//�󂫕����T��
			for ( int i = 0; i < mBulletNumber; ++i ){
				if ( mBullets[ i ].isEmpty() ){
					const char* name = ( mId == 0 ) ? "bulletB" : "bulletR";
					mBullets[ i ].create(
						mSceneContainer, 
						name, 
						mPosition,
						15.f,
						mAngleY,
						mLockOn );
					mEnergy -= mEnergyPerBullet;
					break;
				}
			}
		}
	}
	//����X�V
	for ( int i = 0; i < mBulletNumber; ++i ){
		if ( !mBullets[ i ].isEmpty() ){
			mBullets[ i ].update( enemyPos );
			//�Փˏ���
			Vector3 t;
			t.setSub( *mBullets[ i ].position(), enemyPos );
			if ( t.squareLength() < 4.f ){
				enemy->setDamage( 1 ); //�_���[�W���P���炷�B
				mBullets[ i ].die(); //�e�����B
			}
		}
	}
	//����G�l���M�[�`���[�W
	mEnergy += mEnergyCharge;
	if ( mEnergy > mMaxEnergy ){
		mEnergy = mMaxEnergy;
	}
	//���b�N�I������
//	
//
	//�܂��͊p�x�𑪂�B
	Vector3 toEnemy;
	toEnemy.setSub( enemyPos, mPosition );
	Vector3 myDir( 0.f, 0.f, 1.f );
	Matrix34 m;
	m.setRotationY( mAngleY );
	m.mul( &myDir, myDir );
	toEnemy *= 1.f / toEnemy.length(); //������1��
	float dotProduct = toEnemy.dot( myDir );
	if ( dotProduct > 1.f ){ //�덷�̂�����1���z���邩������Ȃ��B
		dotProduct = 1.f;
	}else if ( dotProduct < -1.f ){
		dotProduct = -1.f;
	}
	//�p�x�ɒ���
	float angle = Math::acos( dotProduct );
	if ( mLockOn ){
		//���b�N�I�����Ă�Ȃ�O��邩�ǂ������ׂ�
		if ( angle > mLockOnAngleOut ){
			mLockOn = false;
		}
	}else{
		//���邩�ǂ������ׂ�
		if ( angle < mLockOnAngleIn ){
			mLockOn = true;
		}
	}
//	cout << "lockon? " << mLockOn << endl;
	//�c���[�Ɉʒu�����Z�b�g
	mTree.setAngle( Vector3( 0.f, mAngleY, 0.f ) );
	mTree.setPosition( mPosition );
}

void Robo::think(
bool* jump,
bool* fire,
bool* turn,
bool* left,
bool* right,
bool* up,
bool* down,
bool* l,
bool* r,
bool* lock
) const {
	Pad* pad = Pad::instance();
	if ( mId == 0 ){ //�v���C���[
		*jump = pad->isOn( Pad::JUMP, mId );
		*fire = pad->isTriggered( Pad::FIRE, mId );
		*turn = pad->isOn( Pad::TURN, mId );
		*left = pad->isOn( Pad::LEFT, mId );
		*right = pad->isOn( Pad::RIGHT, mId );
		*up = pad->isOn( Pad::UP, mId );
		*down = pad->isOn( Pad::DOWN, mId );
		*l = pad->isTriggered( Pad::L, mId);
		*r = pad->isTriggered( Pad::R, mId);
		*lock = pad->isTriggered(Pad::LOCK, mId);
	}else{ //AI
		if (mPosition.length() >= 30.f){ //������x�߂Â����獶�ɉ��
			*up = true;
			*left = false;
		}
		else{
			*left = true;
			*up = false;
		}
		*lock = true;// ��Ƀ��b�N�I��
		//�e�͌��Ă邾�����B
		*fire = true;
		//����͂��Ȃ�
		*turn = false;
		//�ق��̓����͂��Ȃ�
		*right = *down = *l = *r = *jump = false;//�W�����v�͂��Ȃ��̂ŗ���ɍ~��Ă��邱�Ƃ͂Ȃ����Ƃɒ��ӂ���
	}
}

void Robo::turn( bool left, bool right ){
	if ( left ){
		mAngleY += mTurnSpeed;
		if ( mAngleY > 180.f ){ //-PI����PI�ɂ����߂�
			mAngleY -= 360.f;
		}
	}
	if ( right ){
		mAngleY -= mTurnSpeed;
		if ( mAngleY < -180.f ){ //-PI����PI�ɂ����߂�
			mAngleY += 360.0;
		}
	}
}

void Robo::move( bool left, bool right, bool up, bool down ){
	//�ړ������B�܂����_���l�����Ȃ������x���o��
	Vector3 move( 0.f, 0.f, 0.f );
	if ( up ){
		move.z = -1.f;
	}
	if ( down ){
		move.z = 1.f;
	}
	if ( left ){
		move.x = -1.f;
	}
	if ( right ){
		move.x = 1.f;
	}
	//�����������������ĉ�]
	Matrix34 m;
	m.setRotationY( mAngleY + 180.f );
	m.mul( &move, move );

	//���~�܂��Ă���Ȃ�A�K���ɉ���������
	if ( mVelocity.x == 0.f && mVelocity.z == 0.f ){
		//�����ɂ����鎞�Ԃōő呬�x�������1�t���[��������̉����x
		float accel = mMaxMoveSpeed / static_cast< float >( mMoveAccelEndCount );
		mVelocity.setMul( move, accel );
	}else{ //���łɓ����Ă���ꍇ
		if ( move.x == 0.f && move.z == 0.f ){ //�ړ����[��
			mVelocity.set( 0.f, 0.f, 0.f ); //�ړ��͂Ƃ܂�B
		}else{
			//���̑��x�ƕ���������Ȃ������������[�������蒼���B

			//90�x�ȏ�̃^�[���Ȃ��U���x��0�ɂ���B
			//90�x�ȏ�̃^�[���Ȃ猻���x�Ɖ����̓��ς��}�C�i�X
			float dp = mVelocity.dot( move );
			if ( dp <= 0.f ){
				mVelocity.set( 0.f, 0.f, 0.f );
			}else{ //90�x����
				//���݂̈ړ����x�Ɛ����Ȑ����̂ݎ��o��
				//���������́A�ړ������P�ʃx�N�^�Ƃ̓��ς��A�ړ������P�ʃx�N�^�ɂ�����΂悢�B
				//�ړ��P�ʃx�N�^E�A�����x�x�N�^V�Ƃ��āA�V�������x�x�N�^V'���Ȃ킿���s������
				//V' = dot(V,E) * E
				//���̎��AE�͈ړ��x�N�^M���g����E=M/|M|�Ə����邩��A
				//V' = dot(V,M) * M / ( |M|^2 )
				//�ƂȂ�B
				float moveSquareLength = move.x * move.x + move.z * move.z;
				float dp = mVelocity.dot( move );
				mVelocity.setMul( move, dp / moveSquareLength );
			}
			//������������B
			//�ړ����x�͍ő呬�x/�������ԁB
			float accel = mMaxMoveSpeed / static_cast< float >( mMoveAccelEndCount );
			mVelocity.madd( move, accel );
			//�ő呬�x�ŃX�g�b�v
			float speed = mVelocity.length();
			if ( speed > mMaxMoveSpeed ){
				mVelocity *= mMaxMoveSpeed / speed;
			}
		}
	}
}

void Robo::draw() const {
	//�`��
//	mModel.draw();
	mTree.draw();
	//����
	for ( int i = 0; i < mBulletNumber; ++i ){
		if ( !mBullets[ i ].isEmpty() ){
			mBullets[ i ].draw();
		}
	}
}

const Vector3* Robo::position() const {
	return &mPosition;
}

void Robo::setDamage( int d ){
	mHitPoint -= d;
}

int Robo::getHitPoint() const {
	return mHitPoint;
}

int Robo::getEnergy() const {
	return mEnergy;
}

float Robo::getAngleY() const{
	return mAngleY;
}

bool Robo::getLockOn() const {
	return mLockOn;
}

void Robo::getView( Matrix34* vm, Vector3* pos ) const {
	//�܂����ʕ����x�N�g�������
	Vector3 d( 0.f, 0.f, 1.f );
	Matrix34 m;
	m.setRotationY( mAngleY );
	m.mul( &d, d );
	//�����O����mCameraTargetDistanceZ�����L�΂�
	Vector3 t;
	t.setMul( d, mCameraTargetDistanceZ );
	//���{�������Ƃ���ɂ���Ȃ炿����Ɖ������邱�Ƃɂ���
	t.y -= mPosition.y * 0.12f;
	//����������mCameraDistacneZ�����L�΂�
	Vector3 p;
	p.setMul( d, -mCameraDistanceZ );
	//Y��mCameraDistanceY���v���X
	p.y += mCameraDistanceY;
	//���{�������Ƃ���ɂ���Ȃ炿����ƍ��ڂɂ��ĉ������邱�Ƃɂ���
	p.y += mPosition.y * 0.12f;
	//���{���݈ʒu���v���X
	t += mPosition;
	p += mPosition;
	//�r���[�s��쐬
	if ( Input::Manager().keyboard().isOn( ' ' ) ){
		Vector3 q = mPosition;
		q.y += mCameraDistanceY;
		p.y += 0.4f*mCameraDistanceY;
		vm->setViewTransform( q, p, Vector3( 0.f, 1.f, 0.f ) );
	}
	else{
		vm->setViewTransform( p, t, Vector3( 0.f, 1.f, 0.f ) );
	}
	*pos = p;
}
