#include "GameLib/Framework.h"
using namespace GameLib;

#include "Pad.h"
#include "Sequence/Parent.h"

namespace GameLib{
	void Framework::configure(Configuration* c){
		c->enableVSync(true); //モニタの書き換え周期に合わせてフレームを進める。ティアリングが起こらない
		c->setArchiveNumber(1);//アーカイブデータの数
		c->setArchiveName(0, "data.bin");//アーカイブファイル
		c->setLoadMode(LOAD_ARCHIVE_ONLY);//アーカイブファイルのみをデータ源とする
	}
	void Framework::update(){
		if (!Sequence::Parent::instance()){
			Sequence::Parent::create();//シーケンスの大元の作成
			Pad::create();//パッドシングルトンの作成
		}
		Sequence::Parent::instance()->update();
		if (isEndRequested()){//×ボタンが押された時の対処
			Sequence::Parent::destroy(); //シーケンスの削除
			Pad::destroy(); //パッドシングルトンの削除
		}
	}
}