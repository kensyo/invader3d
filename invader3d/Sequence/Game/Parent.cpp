#include "GameLib/GameLib.h"
#include "Sequence/Parent.h"
#include "Sequence/Game/Parent.h"
#include "Sequence/Game/Ready.h"
#include "Sequence/Title.h"
#include "Sequence/GameOver.h"
#include "Sequence/Ending.h"
#include "Game/State.h"
//#include "File.h"
#include <sstream>

namespace Sequence{
namespace Game{

Parent::Parent( ) : 
mState( 0 ),
mStageID( 0 ),
mLife( INITIALI_LIFE_NUMBER ),
mChild( 0 ){
	//Å‰‚ÍReady
	mChild = new Ready();
}

Parent::~Parent(){
	SAFE_DELETE( mState );
	SAFE_DELETE( mChild );
}

Base* Parent::update( GrandParent* ){
	Base* next = this;
	Base* nextChild = mChild->update( this );
	//‘JˆÚ”»’è
	if ( nextChild != mChild ){
		Game::Child* casted = dynamic_cast< Game::Child* >( nextChild );
		if ( casted ){
			SAFE_DELETE( mChild );
			mChild = casted;
		}else{
			next = nextChild;
		}
		casted = 0;
	}
	nextChild = 0; //”O‚Ì‚½‚ß

	return next;
}

State* Parent::state(){
	return mState;
}

bool Parent::hasFinalStageCleared() const {
	return ( mStageID > FINAL_STAGE );
}

int Parent::lifeNumber() const {
	return mLife;
}

void Parent::startLoading(){
	SAFE_DELETE( mState );
	mState = new State( mStageID );
}

void Parent::drawState() const {
	//mState->draw();
}

void Parent::goToNextStage(){
	++mStageID;
}

void Parent::reduceLife(){
	--mLife;
}

} //namespace Game
} //namespace Sequence