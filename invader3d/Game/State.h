#ifndef INCLUDED_GAME_STATE_H
#define INCLUDED_GAME_STATE_H

#include "GameLib/Scene/PrimitiveRenderer.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Vector2.h"

//宣言だけする。インクルードはしなくてよい。
class Robo;
class Stage;

class State{
public:
	State( int stageID );
	~State();
	void update();
//	void draw() const;
	//外から取る情報
	bool hasCleared() const; //クリアしたか？
	bool isAlive( int playerID ) const; //生きているか？
	void drawRect(GameLib::Math::Vector2* p, unsigned c1, unsigned c2);
private:
	int mAbleToSeeRoboIndex;//自キャラに一番近い敵
	Robo** mRobo;
	Stage* mStage;
	GameLib::Scene::PrimitiveRenderer mPrimitiveRenderer; //長方形を書くために必要。主にフロントエンド（ＨＰバーなど）に使用。
	const GameLib::Math::Vector3 mAmbient; //環境光。定数。
	int mTime; //過ぎたフレーム
	const int mTimeLimit; //タイムリミットまでの時間。このタイムリミットでクリアーとなる。

	int mStageID; //ステージの番号。0から始まりステージクリアごとに1づつ増える。敵の数もこれに依存（mStageID+1 匹)
};

#endif
