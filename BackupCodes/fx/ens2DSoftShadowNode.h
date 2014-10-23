//
//  ens2DSoftShadowNode.h
//  HelloCpp
//
//  Created by yang chao (wantnon) on 13-12-16.
//
//

#ifndef HelloWorld_ens2DSoftShadowNode_h
#define HelloWorld_ens2DSoftShadowNode_h

#include <vector>
#include<iostream>
using namespace std;
#include "cocos2d.h"
using namespace cocos2d;
#include "ensCommon.h"

namespace_ens_begin
//this effect was inspired by:
//http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/dynamic-2d-soft-shadows-r2032

class Cedge{
public:
    Point m_start;
    Point m_end;
    Cedge(){}
    Cedge(const Point&start,const Point&end){
        m_start=start;
        m_end=end;
    }
};
class ClightNode:public Node{
public:
    ClightNode(){
        m_r=0;
    }
    bool init(float r){
        m_r=r;
        this->Node::init();
        return true;
    }
    float getR()const{return m_r;}
    
    void onDraw(const Mat4 &transform, uint32_t flags) {
        Director* director = Director::getInstance();
        director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
        
        glLineWidth(2);
        DrawPrimitives::setDrawColor4F(0, 0, 1, 1);
        DrawPrimitives::drawCircle(Point(0,0), m_r, 360, 30, false, 1, 1);
        
        director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    }
    
    void draw(Renderer *renderer, const Mat4& transform, uint32_t flags){
        if(!m_isDrawDebug)
            return;
        
        static CustomCommand _customCommand;
        _customCommand.init(_globalZOrder);
        _customCommand.func = CC_CALLBACK_0(ClightNode::onDraw, this, transform, flags);
       
        renderer->addCommand(&_customCommand);
    }
    
    void setIsDrawDebug(bool value){m_isDrawDebug=value;}
    bool getIsDrawDebug()const{return m_isDrawDebug;}
protected:
    float m_r;
    bool m_isDrawDebug;
};
class CPointType{
public:
    bool m_isLeftUmbraPoint;
    Point m_leftUmbraT;
    bool m_isRightUmbraPoint;
    Point m_rightUmbraT;
    bool m_isLeftPenumbraPoint;
    Point m_leftPenumbraT;
    bool m_isRightPenumbraPoint;
    Point m_rightPenumbraT;
    CPointType(){
         m_isLeftUmbraPoint=false;
         m_isRightUmbraPoint=false;
         m_isLeftPenumbraPoint=false;
         m_isRightPenumbraPoint=false;
    }
};

enum eSideType{
    eLeftSide=0,
    eRightSide,
    eOn,
};
class C2DSoftShadowObj:public Sprite
{
public:
    C2DSoftShadowObj(){
        m_light=NULL;
        m_mesh=NULL;
        m_finTexture=NULL;
        m_indexVBO=NULL;
        m_program=NULL;
        m_shadowLength=1500;
        m_leftPenumbraLine.resize(3);
        m_rightPenumbraLine.resize(3);
        m_leftUmbraLine.resize(3);
        m_rightUmbraLine.resize(3);
        m_leftPenumbraPointID=-1;
        m_rightPenumbraPointID=-1;
        m_leftUmbraPointID=-1;
        m_rightUmbraPointID=-1;
        m_oneDivObjCount=1.0;
        m_isDrawDebug=false;
        m_isDrawNonDebug=true;
        m_isUpdateShadowSucc=false;
    }
    virtual~C2DSoftShadowObj(){
        if(m_mesh)m_mesh->release();
        if(m_finTexture)m_finTexture->release();
        if(m_indexVBO)m_indexVBO->release();
        if(m_program)m_program->release();
    }
    bool init(const Cpolygon&polygon);
    void update(float dt);
    bool updateShadow();
    void onDraw(const Mat4 &transform, uint32_t flags);
    void draw(Renderer* renderer, const Mat4 &transform, uint32_t flags);
    void setLight(ClightNode*light);
    void setOneDivObjCount(float value){m_oneDivObjCount=value;}
    float getOneDivObjCount()const{return m_oneDivObjCount;}
    void setIsDrawDebug(bool value){m_isDrawDebug=value;}
    bool getIsDrawDebug()const{return m_isDrawDebug;}
    void setIsDrawNonDebug(bool value){m_isDrawNonDebug=value;}
    bool getIsDrawNonDebug()const{return m_isDrawNonDebug;}
protected:
    Point getLightPosLocal();
    void makeFullWindowRectMesh();
    void updateMesh();
    void submit(GLenum usage);
protected:
    Cpolygon m_polygon;
    vector<CPointType> m_pointTypeList;
    vector<Point> m_leftPenumbraLine;//m_leftPenumbraLine have three elements, [0] is start point, [1] is mid point, [2] is end point
    vector<Point> m_rightPenumbraLine;
    vector<Point> m_leftUmbraLine;
    vector<Point> m_rightUmbraLine;
    int m_leftPenumbraPointID;
    int m_rightPenumbraPointID;
    int m_leftUmbraPointID;
    int m_rightUmbraPointID;
    Point m_intersectPoint;
    ClightNode *m_light;
    float m_shadowLength;
    Texture2D*m_finTexture;
    Cmesh*m_mesh;
    CindexVBO*m_indexVBO;
    CGLProgramWithUnifos*m_program;
    CustomCommand _customCommand;
    float m_oneDivObjCount;
    bool m_isDrawDebug;
    bool m_isDrawNonDebug;
    bool m_isUpdateShadowSucc;
};


class C2DSoftShadowRoot : public Node
{
public:
    C2DSoftShadowRoot(){
        m_shadowRT=NULL;
        m_program=NULL;
        m_light=NULL;
        m_isDrawDebug=false;
        m_shadowDarkness=0.5;
    }
    virtual~C2DSoftShadowRoot(){
        if(m_shadowRT)m_shadowRT->release();
        if(m_program)m_program->release();
    }
    bool init();
    void setLight(ClightNode*light);
    void addObj(C2DSoftShadowObj*obj);
    void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);
    void setIsDrawDebug(bool value){m_isDrawDebug=value;}
    bool getIsDrawDebug()const{return m_isDrawDebug;}
    void setShadowDarkness(float value){m_shadowDarkness=value;}
    float getShadowDarkness()const{return m_shadowDarkness;}
    RenderTexture* getShadowRT(){return m_shadowRT;}
protected:
    RenderTexture* m_shadowRT;
    CGLProgramWithUnifos*m_program;
    vector<C2DSoftShadowObj*> m_objList;
    ClightNode*m_light;
    bool m_isDrawDebug;
    float m_shadowDarkness;
};
namespace_ens_end
#endif
