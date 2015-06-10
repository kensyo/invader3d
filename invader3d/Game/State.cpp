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
		p4[ i ].z = 0.0; //zは0固定
		p4[ i ].w = 1.0; //x,y,zを割る値。wは1固定。
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
	mRobo = new Robo*[mStageID+1]; //mStageID+1の数だけキャラを作る（自キャラも含めて）。
	mRobo[0] = new Robo(0); //自キャラ。番号は0
	mRobo[0]->setPosition(Vector3(0.0, 0.0, 0.0)); //自キャラの初期位置
	mRobo[0]->setAngleY(0.0); //向きの初期値

	Random rand = Random::create();
	for (int i = 1; i <= mStageID + 1; ++i){
		float radius = rand.getFloat(80.f, 120.f); //80～120までの乱数を取得
		mRobo[i] = new Robo(i); //敵の番号は1から
		//敵の初期位置と向き
		mRobo[i]->setPosition(Vector3(radius*Math::sin(static_cast<float>(360.0*(i-1)/(mStageID+1))), 10.0, radius*Math::cos(static_cast<float>(360.0*(i-1)/(mStageID+1)))));
		mRobo[i]->setAngleY(static_cast<float>(180.0+360.0*(i-1)/(mStageID+1)));
	}
	mStage = new Stage(); //ステージの作成

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
	//自キャラの視線方向で一番近い敵キャラの同定
	Vector2 eyeVector2(Math::cos(mRobo[0]->getAngleY()), Math::sin(mRobo[0]->getAngleY())); //zx平面上での自キャラの向き。大きさは1
	Vector3 myPos3 = *mRobo[0]->position();
	mAbleToSeeRoboIndex = 1;
	float ableToSeeRoboDistance = numeric_limits<float>::max();
	for (int i = 1; i <= mStageID + 1; ++i){// 敵の数は mStageID + 1
		if (mRobo[i]->getHitPoint() > 0){
			Vector3 enemyPos3 = *mRobo[i]->position();
			Vector2 enemyPos2;
			enemyPos2.y = enemyPos3.x;
			enemyPos2.x = enemyPos3.z;
			enemyPos2 *= 1.f / enemyPos2.length(); //大きさを1にする
			float dotProduct = eyeVector2.dot(enemyPos2); //大きさ１同士の内積なので、コサインを表す。
			if (Math::cos(25.f) < dotProduct){ //視野角25*2=50°内にあるかチェック。画角は45°なので、少し広め。
				Vector3 dir;
				dir.setSub(enemyPos3, myPos3); //自分と敵の位置の差ベクトル
				if (ableToSeeRoboDistance > dir.length()){ //前の敵キャラよりも近いなら
					ableToSeeRoboDistance = dir.length();
					mAbleToSeeRoboIndex = i;
				}

			}
		}
	}
//	cout << mAbleToSeeRoboIndex << " hoge" << endl;
	if (mAbleToSeeRoboIndex == 1){//視線方向にいなかったら
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

	//0番ロボからカメラ行列ゲット
	Matrix44 pvm;
	pvm.setPerspectiveTransform( 45.f, //画角
		static_cast< float >( f.width() ), //画面横幅
		static_cast< float >( f.height() ), //画面縦幅
		1.f, //ニアクリップ
		10000.f ); //ファークリップ
	Matrix34 vm;
	Vector3 eyePosition; //視点の位置
	mRobo[ 0 ]->getView( &vm, &eyePosition ); //ビュー行列はロボに作ってもらう
	pvm *= vm;

	//描画
	//グローバルなライティング設定
	Graphics::Manager gm = Graphics::Manager::instance();
	gm.setProjectionViewMatrix( pvm );
	gm.setEyePosition( eyePosition );
	gm.setLightingMode( Graphics::LIGHTING_PER_PIXEL );
	gm.setAmbientColor( mAmbient ); //環境光
	gm.setLightColor( 0, Vector3( 0.7f, 0.4f, 0.2f ) );
	gm.enableDepthTest( true ); //zテスト
	gm.enableDepthWrite( true );
	Vector3 lightPos[ 4 ];
	lightPos[ 0 ].set( 100000.f, 100000.f, 100000.f );
	float lightIntensities[ 4 ]; //光強度
	lightIntensities[ 0 ] = lightPos[ 0 ].length(); //長さを強度にすれば、だいたいその距離で1になる
	//ライトは最大４つ置ける
	//残り三つのライトは弾のところに置く。
	//カメラに近い順に3つ選ぶことにする。
	float nearestDistance[ 3 ];
	int nearestIndex[ 3 ];
	for ( int i = 0; i < 3; ++i ){
		nearestDistance[ i ] = numeric_limits< float >::max();
		nearestIndex[ i ] = -1;
	}
	//距離を測りながら最小を三つ取得
	//100はロボあたりの弾の数の最大数。
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
	//ライト設定
	for ( int i = 0; i < 4; ++i ){
		gm.setLightPosition( i, lightPos[ i ] );
		gm.setLightIntensity( i, lightIntensities[ i ] );
	}
	mStage->draw();
	for (int i = 0; i <= mStageID+1; ++i){
		//hpがノンゼロのもののみドロウする
		if (mRobo[i]->getHitPoint() > 0){
			mRobo[ i ]->draw();
		}
	}

	//以下フロントエンド描画
	//モデル描画でいじられたものを元に戻す
	gm.setDiffuseColor( Vector3( 1.f, 1.f, 1.f ) );
	gm.setTransparency( 1.f );
	gm.setLightingMode( Graphics::LIGHTING_NONE );

	//必要な情報を変数に収める
	int hp0 = mRobo[ 0 ]->getHitPoint();
	int e0 = mRobo[ 0 ]->getEnergy();
	bool lockOn0 = mRobo[ 0 ]->getLockOn();

	//Zテスト無用。アルファブレンドON
	mPrimitiveRenderer.enableDepthTest( false );
	mPrimitiveRenderer.enableDepthWrite( false );
	mPrimitiveRenderer.setBlendMode( Graphics::BLEND_LINEAR );

	Vector2 p[ 4 ];
	unsigned c1;
	unsigned c2;
	//体力バー背景
	p[ 0 ].set( -0.9f, 0.95f );
	p[ 1 ].set( -0.9f, 0.87f );
	p[ 2 ].set( 0.1f, 0.95f );
	p[ 3 ].set( 0.1f, 0.87f );
	c1 = c2 = 0xff406080;
	drawRect( p, c1, c2 );
	//体力バー本体
	float length = static_cast< float >( hp0 ) / static_cast< float >( Robo::mMaxHitPoint );
	p[ 0 ].set( -0.9f, 0.95f );
	p[ 1 ].set( -0.9f, 0.87f );
	p[ 2 ].set( -0.9f + length, 0.95f );
	p[ 3 ].set( -0.9f + length, 0.87f );
	c1 = 0xff882244;
	c2 = 0xff88ccff;
	drawRect( p, c1, c2 );
  	drawRect( p, c1, c2 );
	//武器エネルギー
	//背景
	p[ 0 ].set( -0.1f, -0.7f );
	p[ 1 ].set( -0.1f, -0.8f );
	p[ 2 ].set( 0.1f, -0.7f );
	p[ 3 ].set( 0.1f, -0.8f );
	c1 = c2 = 0x80404040;
	drawRect( p, c1, c2 );
	//本体
	mPrimitiveRenderer.setBlendMode( Graphics::BLEND_ADDITIVE ); //加算にしてみようかな
	length = 0.2f * static_cast< float >( e0 ) / static_cast< float >( Robo::mMaxEnergy );
	p[ 0 ].set( -0.1f, -0.7f );
	p[ 1 ].set( -0.1f, -0.8f );
	p[ 2 ].set( -0.1f + length, -0.7f );
	p[ 3 ].set( -0.1f + length, -0.8f );
	c1 = 0x80ff0000;
	c2 = 0x80ffff00;
	drawRect( p, c1, c2 );

	//レーダー
	mPrimitiveRenderer.setBlendMode( Graphics::BLEND_LINEAR );
	//背景
	p[ 0 ].set( 0.7f, 0.7f );
	p[ 1 ].set( 0.7f, 0.9f );
	p[ 2 ].set( 0.9f, 0.7f );
	p[ 3 ].set( 0.9f, 0.9f );
	c1 = c2 = 0x80404040;
	drawRect( p, c1, c2 );
	mPrimitiveRenderer.setBlendMode( Graphics::BLEND_ADDITIVE ); //加算ブレンド
	//自分
	Vector2 t;
	t.set( -mRobo[ 0 ]->position()->x, mRobo[ 0 ]->position()->z ); //x.z
	t *= 0.002f; //+-50のステージを0.2の大きさに縮小するので、0.2/100で0.002
	t += Vector2( 0.8f, 0.8f ); //(0.8,0.8)が中心ゆえ、それを足す
	p[ 0 ].set( t.x - 0.005f, t.y - 0.005f );
	p[ 1 ].set( t.x - 0.005f, t.y + 0.005f );
	p[ 2 ].set( t.x + 0.005f, t.y - 0.005f );
	p[ 3 ].set( t.x + 0.005f, t.y + 0.005f );
	c1 = c2 = 0xcc0080ff;
	drawRect( p, c1, c2 );
	//敵
	for (int i = 1; i <= mStageID + 1; ++i){
		if (mRobo[i]->getHitPoint() > 0){
			t.set(-mRobo[i]->position()->x, mRobo[i]->position()->z); //x.z
			t *= 0.002f; //+-50のステージを0.2の大きさに縮小するので、0.2/100で0.002
			t += Vector2(0.8f, 0.8f); //(0.8,0.8)が中心ゆえ、それを足す
			p[0].set(t.x - 0.005f, t.y - 0.005f);
			p[1].set(t.x - 0.005f, t.y + 0.005f);
			p[2].set(t.x + 0.005f, t.y - 0.005f);
			p[3].set(t.x + 0.005f, t.y + 0.005f);
			c1 = c2 = 0xccff8000;
			drawRect(p, c1, c2);

		}
	}
	//ロックオンマーク
	if ( lockOn0 ){
		mPrimitiveRenderer.setBlendMode( Graphics::BLEND_ADDITIVE ); //加算にする
		//敵ロボを座標変換してスクリーン座標に変換
		Vector3 t = *mRobo[ mAbleToSeeRoboIndex ]->position();
		Vector4 p4;
		pvm.mul( &p4, t );
		//XY座標はwで割れば出る。
		float x = p4[ 0 ] / p4[ 3 ];
		float y = p4[ 1 ] / p4[ 3 ];
		//色は赤
		c1 = c2 = 0x80ff0000;
		//線4本を書く。
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
	//時間制限
	mPrimitiveRenderer.setBlendMode( Graphics::BLEND_LINEAR ); //線形ブレンド
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
	mPrimitiveRenderer.setBlendMode( Graphics::BLEND_ADDITIVE ); //加算
	c1 = 0x80ff8888;
	c2 = 0x8088ffff;
	drawRect( p, c1, c2 );

	mPrimitiveRenderer.draw(); //描画
	//終了判定(マウスで×ボタンが叩かれたか)
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
	//敵が残っていなければ、もしくは時間がたっていればクリア
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
	//hpが0より大きいなら生きている
	int hp0 = mRobo[ 0 ]->getHitPoint();
	if (hp0 <= 0){
		return false;
	}
	return true;
}

