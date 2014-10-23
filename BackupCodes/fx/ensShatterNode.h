

#ifndef HelloWorld_ensShatterNode_h
#define HelloWorld_ensShatterNode_h

#include <vector>
#include<iostream>
using namespace std;
#include "cocos2d.h"
using namespace cocos2d;
#include "ensCommon.h"

namespace_ens_begin
namespace shatter{
class Cfrag : public Sprite
{
public:
	int m_randomNumber;
	Cfrag(){
		m_randomNumber=-1;
	}
    
    void _initTex(Texture2D* tex) {
        initWithTexture(tex);
    }
};
}
namespace shatter{
	class CshatterAction;
}
class CshatterSprite : public Sprite
{
public:
	friend class shatter::CshatterAction;
public:
	CshatterSprite(){
		m_gridSideLen=2.7;
        m_initalFrageScale=4.5;
        m_fragBatchNode=NULL;
	}
	virtual~CshatterSprite(){}
	bool init(const string&texFileName);
protected:
	void createShatter();
	void resetShatter();
	void updateShatterAction(float time,float dt,float growSpeedOfTargetR);

protected:
	float m_gridSideLen;//grid side length, also frag size
    float m_initalFrageScale;//we want to make frags a litte bigger at start time.
    vector<vector<shatter::Cfrag*> > m_grid;//hold all the frags
    CCSpriteBatchNode* m_fragBatchNode;//all frags add to this batchNode
};
namespace shatter{

class CshatterAction : public ActionInterval
//ref to CCRotateBy
//why use action: http://user.qzone.qq.com/350479720/blog/1403318648
{
public:
    //creates the action
    static CshatterAction* create(float fDuration);
    //initializes the action
    bool initWithDuration(float fDuration);
    virtual void stop();
    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);
    virtual bool isDone();
    CshatterAction(){initMembers();}
    virtual~CshatterAction(){
	}
    virtual ActionInterval* reverse() const{return nullptr;}
    virtual ActionInterval *clone() const{return nullptr;}
protected:
    void initMembers(){
        m_timeFoe=0;
        m_timeCur=0;
       
        m_growSpeedOfTargetR=50;
    }

protected:
    float m_timeFoe;
    float m_timeCur;
protected:
 
    float m_growSpeedOfTargetR;
    
};
	}
namespace_ens_end
#endif
