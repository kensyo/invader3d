//#include "GameLib/Framework.h"

#include "Game/State.h"
#include <limits>
#include "GameLib/Framework.h"
#include "GameLib/DebugScreen.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Vector4.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Vector2.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Math/Random.h"
#include "GameLib/Scene/PrimitiveRenderer.h"
#include "GameLib/Graphics/Manager.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Graphics/IndexBuffer.h"
#include "GameLib/Graphics/VertexBuffer.h"
#include "GameLib/Graphics/Texture.h"
using namespace GameLib;

using namespace std;

#include "Pad.h"
#include "Game/Robo.h"
#include "Game/Stage.h"

void State::drawRect( Vector2* p, unsigned c1, unsigned c2 ){
	
	Vector4 p4[ 4 ];
	for ( int i = 0; i < 4; ++i ){
		p4[ i ].x = p[ i ].x;
		p4[ i ].y = p[ i ].y;
		p4[ i ].z = 0.0; //z��0�Œ�
		p4[ i ].w = 1.0; //x,y,z������l�Bw��1�Œ�B
	}
	mPrimitiveRenderer.addTransformedTriangle( p4[ 0 ], p4[ 1 ], p4[ 2 ], c1, c1, c2 );
	mPrimitiveRenderer.addTransformedTriangle( p4[ 3 ], p4[ 1 ], p4[ 2 ], c2, c1, c2 );
}

State::State(int stageID) :
mStage(0),
mTime(0),
mTimeLimit(45),
mAmbient(0.2f, 0.3f, 0.4f),
mStageID( stageID ),
mAbleToSeeRoboIndex(1){
	mPrimitiveRenderer = Scene::PrimitiveRenderer::create(1000, 100);
	mRobo = new Robo*[mStageID+1]; //mStageID+1�̐������L���������i���L�������܂߂āj�B
	mRobo[0] = new Robo(0); //���L�����B�ԍ���0
	mRobo[0]->setPosition(Vector3(0.0, 0.0, 0.0)); //���L�����̏����ʒu
	mRobo[0]->setAngleY(0.0); //�����̏����l

	Random rand = Random::create();
	for (int i = 1; i <= mStageID + 1; ++i){
		float radius = rand.getFloat(80.f, 120.f); //80�`120�܂ł̗������擾
		mRobo[i] = new Robo(i); //�G�̔ԍ���1����
		//�G�̏����ʒu�ƌ���
		mRobo[i]->setPosition(Vector3(radius*Math::sin(static_cast<float>(360.0*(i-1)/(mStageID+1))), 10.0, radius*Math::cos(static_cast<float>(360.0*(i-1)/(mStageID+1)))));
		mRobo[i]->setAngleY(static_cast<float>(180.0+360.0*(i-1)/(mStageID+1)));
	}
	mStage = new Stage(); //�X�e�[�W�̍쐬

}

State::~State(){
//	SAFE_DELETE( mImage );
//	SAFE_DELETE_ARRAY( mDynamicObjects );
	for (int i = 0; i <= mStageID + 1; ++i){
		SAFE_DELETE(mRobo[i]);
	}
	SAFE_DELETE(mStage);
}

//void State::draw() const {
//}

void State::update(){
	//���L�����̎��������ň�ԋ߂��G�L�����̓���
	Vector2 eyeVector2(Math::cos(mRobo[0]->getAngleY()), Math::sin(mRobo[0]->getAngleY())); //zx���ʏ�ł̎��L�����̌����B�傫����1
	Vector3 myPos3 = *mRobo[0]->position();
	mAbleToSeeRoboIndex = 1;
	float ableToSeeRoboDistance = numeric_limits<float>::max();
	for (int i = 1; i <= mStageID + 1; ++i){// �G�̐��� mStageID + 1
		if (mRobo[i]->getHitPoint() > 0){
			Vector3 enemyPos3 = *mRobo[i]->position();
			Vector2 enemyPos2;
			enemyPos2.y = enemyPos3.x;
			enemyPos2.x = enemyPos3.z;
			enemyPos2 *= 1.f / enemyPos2.length(); //�傫����1�ɂ���
			float dotProduct = eyeVector2.dot(enemyPos2); //�傫���P���m�̓��ςȂ̂ŁA�R�T�C����\���B
			if (Math::cos(25.f) < dotProduct){ //����p25*2=50�����ɂ��邩�`�F�b�N�B��p��45���Ȃ̂ŁA�����L�߁B
				Vector3 dir;
				dir.setSub(enemyPos3, myPos3); //�����ƓG�̈ʒu�̍��x�N�g��
				if (ableToSeeRoboDistance > dir.length()){ //�O�̓G�L���������߂��Ȃ�
					ableToSeeRoboDistance = dir.length();
					mAbleToSeeRoboIndex = i;
				}

			}
		}
	}
//	cout << mAbleToSeeRoboIndex << " hoge" << endl;
	if (mAbleToSeeRoboIndex == 1){//���������ɂ��Ȃ�������
		for (int i = 1; i <= mStageID+1; ++i){
			if (mRobo[i]->getHitPoint() > 0){
				mAbleToSeeRoboIndex = i;
				break;
			}
		}
	}
//	cout << mAbleToSeeRoboIndex << endl;
		
	mRobo[ 0 ]->update( mRobo[ mAbleToSeeRoboIndex ] );
	for (int i = 1; i <= mStageID + 1; ++i){
		mRobo[ i ]->update( mRobo[ 0 ] );
	}
	++mTime;

	Framework f = Framework::instance();

	//0�ԃ��{����J�����s��Q�b�g
	Matrix44 pvm;
	pvm.setPerspectiveTransform( 45.f, //��p
		static_cast< float >( f.width() ), //��ʉ���
		static_cast< float >( f.height() ), //��ʏc��
		1.f, //�j�A�N���b�v
		10000.f ); //�t�@�[�N���b�v
	Matrix34 vm;
	Vector3 eyePosition; //���_�̈ʒu
	mRobo[ 0 ]->getView( &vm, &eyePosition ); //�r���[�s��̓��{�ɍ���Ă��炤
	pvm *= vm;

	//�`��
	//�O���[�o���ȃ��C�e�B���O�ݒ�
	Graphics::Manager gm = Graphics::Manager::instance();
	gm.setProjectionViewMatrix( pvm );
	gm.setEyePosition( eyePosition );
	gm.setLightingMode( Graphics::LIGHTING_PER_PIXEL );
	gm.setAmbientColor( mAmbient ); //����
	gm.setLightColor( 0, Vector3( 0.7f, 0.4f, 0.2f ) );
	gm.enableDepthTest( true ); //z�e�X�g
	gm.enableDepthWrite( true );
	Vector3 lightPos[ 4 ];
	lightPos[ 0 ].set( 100000.f, 100000.f, 100000.f );
	float lightIntensities[ 4 ]; //�����x
	lightIntensities[ 0 ] = lightPos[ 0 ].length(); //���������x�ɂ���΁A�����������̋�����1�ɂȂ�
	//���C�g�͍ő�S�u����
	//�c��O�̃��C�g�͒e�̂Ƃ���ɒu���B
	//�J�����ɋ߂�����3�I�Ԃ��Ƃɂ���B
	float nearestDistance[ 3 ];
	int nearestIndex[ 3 ];
	for ( int i = 0; i < 3; ++i ){
		nearestDistance[ i ] = numeric_limits< float >::max();
		nearestIndex[ i ] = -1;
	}
	//�����𑪂�Ȃ���ŏ����O�擾
	//100�̓��{������̒e�̐��̍ő吔�B
	for ( int i = 0; i <= mStageID + 1; ++i ){
		for ( int j = 0; j < 100; ++j ){
			const Vector3* p = mRobo[ i ]->getBulletPosition( j );
			if ( p ){
				Vector3 t;
				t.setSub( *p, eyePosition );
				float d = t.squareLength();
				if ( d < nearestDistance[ 0 ] ){
					nearestDistance[ 2 ] = nearestDistance[ 1 ];
					nearestDistance[ 1 ] = nearestDistance[ 0 ];
					nearestDistance[ 0 ] = d;
					nearestIndex[ 2 ] = nearestIndex[ 1 ];
					nearestIndex[ 1 ] = nearestIndex[ 0 ];
					nearestIndex[ 0 ] = i * 100 + j;
				}else if ( d < nearestDistance[ 1 ] ){
					nearestDistance[ 2 ] = nearestDistance[ 1 ];
					nearestDistance[ 1 ] = d;
					nearestIndex[ 2 ] = nearestIndex[ 1 ];
					nearestIndex[ 1 ] = i * 100 + j;
				}else if ( d < nearestDistance[ 2 ] ){
					nearestDistance[ 2 ] = d;
					nearestIndex[ 2 ] = i * 100 + j;
				}
			}
		}
	}
	for ( int i = 0; i < 3; ++i ){
		if ( nearestIndex[ i ] != -1 ){
			int robo = nearestIndex[ i ] / 100;
			int bullet = nearestIndex[ i ] % 100;
			const Vector3* p = mRobo[ robo ]->getBulletPosition( bullet );
			lightPos[ i + 1 ] = *p;
			if ( robo == 0 ){
				gm.setLightColor( i + 1, Vector3( 0.2f, 0.4f, 1.f ) );
			}else{
				gm.setLightColor( i + 1, Vector3( 1.f, 0.2f, 0.4f ) );
			}
			lightIntensities[ i + 1 ] = 10.f;
		}else{
			lightIntensities[ i + 1 ] = 0.f;
		}
	}
	gm.setCullMode( Graphics::CULL_BACK );
	//���C�g�ݒ�
	for ( int i = 0; i < 4; ++i ){
		gm.setLightPosition( i, lightPos[ i ] );
		gm.setLightIntensity( i, lightIntensities[ i ] );
	}
	mStage->draw();
	for (int i = 0; i <= mStageID+1; ++i){
		//hp���m���[���̂��̂̂݃h���E����
		if (mRobo[i]->getHitPoint() > 0){
			mRobo[ i ]->draw();
		}
	}

	//�ȉ��t�����g�G���h�`��
	//���f���`��ł�����ꂽ���̂����ɖ߂�
	gm.setDiffuseColor( Vector3( 1.f, 1.f, 1.f ) );
	gm.setTransparency( 1.f );
	gm.setLightingMode( Graphics::LIGHTING_NONE );

	//�K�v�ȏ���ϐ��Ɏ��߂�
	int hp0 = mRobo[ 0 ]->getHitPoint();
	int e0 = mRobo[ 0 ]->getEnergy();
	bool lockOn0 = mRobo[ 0 ]->getLockOn();

	//Z�e�X�g���p�B�A���t�@�u�����hON
	mPrimitiveRenderer.enableDepthTest( false );
	mPrimitiveRenderer.enableDepthWrite( false );
	mPrimitiveRenderer.setBlendMode( Graphics::BLEND_LINEAR );

	Vector2 p[ 4 ];
	unsigned c1;
	unsigned c2;
	//�̗̓o�[�w�i
	p[ 0 ].set( -0.9f, 0.95f );
	p[ 1 ].set( -0.9f, 0.87f );
	p[ 2 ].set( 0.1f, 0.95f );
	p[ 3 ].set( 0.1f, 0.87f );
	c1 = c2 = 0xff406080;
	drawRect( p, c1, c2 );
	//�̗̓o�[�{��
	float length = static_cast< float >( hp0 ) / static_cast< float >( Robo::mMaxHitPoint );
	p[ 0 ].set( -0.9f, 0.95f );
	p[ 1 ].set( -0.9f, 0.87f );
	p[ 2 ].set( -0.9f + length, 0.95f );
	p[ 3 ].set( -0.9f + length, 0.87f );
	c1 = 0xff882244;
	c2 = 0xff88ccff;
	drawRect( p, c1, c2 );
  	drawRect( p, c1, c2 );
	//����G�l���M�[
	//�w�i
	p[ 0 ].set( -0.1f, -0.7f );
	p[ 1 ].set( -0.1f, -0.8f );
	p[ 2 ].set( 0.1f, -0.7f );
	p[ 3 ].set( 0.1f, -0.8f );
	c1 = c2 = 0x80404040;
	drawRect( p, c1, c2 );
	//�{��
	mPrimitiveRenderer.setBlendMode( Graphics::BLEND_ADDITIVE ); //���Z�ɂ��Ă݂悤����
	length = 0.2f * static_cast< float >( e0 ) / static_cast< float >( Robo::mMaxEnergy );
	p[ 0 ].set( -0.1f, -0.7f );
	p[ 1 ].set( -0.1f, -0.8f );
	p[ 2 ].set( -0.1f + length, -0.7f );
	p[ 3 ].set( -0.1f + length, -0.8f );
	c1 = 0x80ff0000;
	c2 = 0x80ffff00;
	drawRect( p, c1, c2 );

	//���[�_�[
	mPrimitiveRenderer.setBlendMode( Graphics::BLEND_LINEAR );
	//�w�i
	p[ 0 ].set( 0.7f, 0.7f );
	p[ 1 ].set( 0.7f, 0.9f );
	p[ 2 ].set( 0.9f, 0.7f );
	p[ 3 ].set( 0.9f, 0.9f );
	c1 = c2 = 0x80404040;
	drawRect( p, c1, c2 );
	mPrimitiveRenderer.setBlendMode( Graphics::BLEND_ADDITIVE ); //���Z�u�����h
	//����
	Vector2 t;
	t.set( -mRobo[ 0 ]->position()->x, mRobo[ 0 ]->position()->z ); //x.z
	t *= 0.002f; //+-50�̃X�e�[�W��0.2�̑傫���ɏk������̂ŁA0.2/100��0.002
	t += Vector2( 0.8f, 0.8f ); //(0.8,0.8)�����S�䂦�A����𑫂�
	p[ 0 ].set( t.x - 0.005f, t.y - 0.005f );
	p[ 1 ].set( t.x - 0.005f, t.y + 0.005f );
	p[ 2 ].set( t.x + 0.005f, t.y - 0.005f );
	p[ 3 ].set( t.x + 0.005f, t.y + 0.005f );
	c1 = c2 = 0xcc0080ff;
	drawRect( p, c1, c2 );
	//�G
	for (int i = 1; i <= mStageID + 1; ++i){
		if (mRobo[i]->getHitPoint() > 0){
			t.set(-mRobo[i]->position()->x, mRobo[i]->position()->z); //x.z
			t *= 0.002f; //+-50�̃X�e�[�W��0.2�̑傫���ɏk������̂ŁA0.2/100��0.002
			t += Vector2(0.8f, 0.8f); //(0.8,0.8)�����S�䂦�A����𑫂�
			p[0].set(t.x - 0.005f, t.y - 0.005f);
			p[1].set(t.x - 0.005f, t.y + 0.005f);
			p[2].set(t.x + 0.005f, t.y - 0.005f);
			p[3].set(t.x + 0.005f, t.y + 0.005f);
			c1 = c2 = 0xccff8000;
			drawRect(p, c1, c2);

		}
	}
	//���b�N�I���}�[�N
	if ( lockOn0 ){
		mPrimitiveRenderer.setBlendMode( Graphics::BLEND_ADDITIVE ); //���Z�ɂ���
		//�G���{�����W�ϊ����ăX�N���[�����W�ɕϊ�
		Vector3 t = *mRobo[ mAbleToSeeRoboIndex ]->position();
		Vector4 p4;
		pvm.mul( &p4, t );
		//XY���W��w�Ŋ���Ώo��B
		float x = p4[ 0 ] / p4[ 3 ];
		float y = p4[ 1 ] / p4[ 3 ];
		//�F�͐�
		c1 = c2 = 0x80ff0000;
		//��4�{�������B
		p[ 0 ].set( x - 0.01f, y + 0.2f );
		p[ 1 ].set( x - 0.01f, y + 0.1f );
		p[ 2 ].set( x + 0.01f, y + 0.2f );
		p[ 3 ].set( x + 0.01f, y + 0.1f );
		drawRect( p, c1, c2 );
		p[ 0 ].set( x - 0.01f, y - 0.2f );
		p[ 1 ].set( x - 0.01f, y - 0.1f );
		p[ 2 ].set( x + 0.01f, y - 0.2f );
		p[ 3 ].set( x + 0.01f, y - 0.1f );
		drawRect( p, c1, c2 );
		p[ 0 ].set( x - 0.2f, y - 0.01f );
		p[ 1 ].set( x - 0.2f, y + 0.01f );
		p[ 2 ].set( x - 0.1f, y - 0.01f );
		p[ 3 ].set( x - 0.1f, y + 0.01f );
		drawRect( p, c1, c2 );
		p[ 0 ].set( x + 0.2f, y - 0.01f );
		p[ 1 ].set( x + 0.2f, y + 0.01f );
		p[ 2 ].set( x + 0.1f, y - 0.01f );
		p[ 3 ].set( x + 0.1f, y + 0.01f );
		drawRect( p, c1, c2 );
	}
	//���Ԑ���
	mPrimitiveRenderer.setBlendMode( Graphics::BLEND_LINEAR ); //���`�u�����h
	length = 1.9f * static_cast< float >( 60 * mTimeLimit - mTime ) / static_cast< float >( 60 * mTimeLimit );
	p[ 0 ].set( -0.95f, -0.95f );
	p[ 1 ].set( -0.95f, -0.9f );
	p[ 2 ].set( 0.95f, -0.95f );
	p[ 3 ].set( 0.95f, -0.9f );
	c1 = c2 = 0x80404040;
	drawRect( p, c1, c2 );
	p[ 0 ].set( -0.95f, -0.95f );
	p[ 1 ].set( -0.95f, -0.9f );
	p[ 2 ].set( -0.95f + length, -0.95f );
	p[ 3 ].set( -0.95f + length, -0.9f );
	mPrimitiveRenderer.setBlendMode( Graphics::BLEND_ADDITIVE ); //���Z
	c1 = 0x80ff8888;
	c2 = 0x8088ffff;
	drawRect( p, c1, c2 );

	mPrimitiveRenderer.draw(); //�`��
	//�I������(�}�E�X�Ł~�{�^�����@���ꂽ��)
	if ( f.isEndRequested() ){
		if ( mStage ){
			for (int i = 0; i <= mStageID + 1; ++i){
				SAFE_DELETE( mRobo[ i ] );

			}
			SAFE_DELETE( mStage );
		}
	}
	
}


bool State::hasCleared() const {
	//�G���c���Ă��Ȃ���΁A�������͎��Ԃ������Ă���΃N���A
	bool enemyIsAlive = false;
	for (int i = 1; i <= mStageID + 1; ++i){
		int hp = mRobo[ i ]->getHitPoint();
		if (hp > 0){
			enemyIsAlive = true;
			break;
		}
	}
	if (!enemyIsAlive || mTime >= 60 * mTimeLimit){
		return true;
	}
	return false;
}

bool State::isAlive( int playerID ) const {
	//hp��0���傫���Ȃ琶���Ă���
	int hp0 = mRobo[ 0 ]->getHitPoint();
	if (hp0 <= 0){
		return false;
	}
	return true;
}

