//
//  TimeEvent.cpp
//  JumpEdt
//
//  Created by Yongfeng on 11/17/14.
//
//

#include "TimeEvent.h"
#include "Events.h"

void TimeEvent::TimeEventPoint::trigger(){
	if (mEvents.empty()) return;

	for (std::vector<std::string>::iterator it = mEvents.begin(); it != mEvents.end(); ++it){
		Events::callEvent((*it).c_str(), NULL);
	}
}

void TimeEvent::reset(){
	mEnalbe = true;
	mCurEventIndex = 0;
	mWaitingTimer = 0.0f;
	mEventWaitingTimer = mInitDelay;

}

TimeEvent::TimeEvent(){
}

TimeEvent::~TimeEvent(){
#if 0
	for (std::vector<TimeEventPoint>::iterator it = mEventPoints.begin(); it != mEventPoints.end(); ++it){
		delete &(*it);
	}
#endif
	mEventPoints.clear();
}

void TimeEvent::update(float dt){
	auto eventPonitNum = mEventPoints.size();
	if (mCurEventIndex >= eventPonitNum && !mLoop){
		mEnalbe = false;
	}
	if (mEventPoints.empty() || !mEnalbe) return;

    if (mWaitingTimer > mEventWaitingTimer){
		mEventPoints[mCurEventIndex].trigger();
		mEventWaitingTimer = mEventPoints[mCurEventIndex].waitTime;
		mWaitingTimer = 0.0f;
		mCurEventIndex++;

		if (mCurEventIndex >= eventPonitNum){
			if (mLoop) mCurEventIndex = 0;
			else mEnalbe = false;
		}
    }else{
        mWaitingTimer += dt;
    }
}