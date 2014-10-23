//
//  ens2DSoftShadowNode.cpp
//  HelloCpp
//
//  Created by yang chao (wantnon) on 13-12-16.
//
//

#include "VisibleRect.h"
#include "ens2DSoftShadowNode.h"
namespace_ens_begin

bool C2DSoftShadowObj::init(const Cpolygon&polygon){
    m_polygon=polygon;
    //init this sprite
    this->CCSprite::init();
    //start update
    this->scheduleUpdate();
    //create mesh
    m_mesh=new Cmesh();
    m_mesh->autorelease();
    m_mesh->retain();
    //create indexVBO
    m_indexVBO=new CindexVBO();
    m_indexVBO->autorelease();
    m_indexVBO->retain();
    //finTexture
    m_finTexture=CCTextureCache::sharedTextureCache()->addImage("images/lightFin.png");
    m_finTexture->retain();
    //----create and set shader program
    {
        GLchar * fragSource = (GLchar*) CCString::createWithContentsOfFile(CCFileUtils::sharedFileUtils()->fullPathForFilename("shaders/2DSoftShadow.fsh").c_str())->getCString();
        CGLProgramWithUnifos* program = new CGLProgramWithUnifos();
        program->autorelease();
        program->initWithVertexShaderByteArray(ccPositionTextureColor_vert, fragSource);
        //bind attribute
        program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        program->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        //link  (must after bindAttribute)
        program->link();
        //get cocos2d-x build-in uniforms
        program->updateUniforms();
        //get my own uniforms
        program->attachUniform("u_oneDivObjCount");
        //set program
        m_program=program;
        m_program->retain();
        //check gl error
        CHECK_GL_ERROR_DEBUG();
    }
    
    
    return true;
}
void C2DSoftShadowObj::update(float dt){
    m_isUpdateShadowSucc=updateShadow();
    if(m_isUpdateShadowSucc==false){
        makeFullWindowRectMesh();
        submit(GL_DYNAMIC_DRAW);
        return;
    }
    updateMesh();
    submit(GL_DYNAMIC_DRAW);
}

bool C2DSoftShadowObj::updateShadow(){
    if(m_light==NULL)return false;
    Point lightPosLocal=getLightPosLocal();
    m_pointTypeList.clear();
    int nPoint=(int)m_polygon.m_pointList.size();
    m_pointTypeList.resize(nPoint);
    for(int i=0;i<nPoint;i++){
        const Point&P=m_polygon.m_pointList[i];
        CPointType&pointType=m_pointTypeList[i];
        vector<Point> LRT=calculateTangentPointsOfPointToCircle(lightPosLocal, m_light->getR(), P);
        if(LRT.empty()){//P in light circle, return
            return false;
        }
        Point LT=LRT[0];
        Point RT=LRT[1];
        Point PLT=LT-P;
        Point PRT=RT-P;
        //for each point, determine it is on which side of PLT and PRT
        vector<eSideType> sideTypeList_PLT;
        vector<eSideType> sideTypeList_PRT;
        for(int j=0;j<nPoint;j++){
            const Point&p=m_polygon.m_pointList[j];
            Point Pp=p-P;
            if(j==i){
                sideTypeList_PLT.push_back(eOn);
                sideTypeList_PRT.push_back(eOn);
            }else{
                //sideTypeList_PLT
                {
                    float cross = PLT.cross(Pp);
                    if(cross==0){
                        sideTypeList_PLT.push_back(eOn);
                    }else if(cross>0){
                        sideTypeList_PLT.push_back(eLeftSide);
                    }else{
                        sideTypeList_PLT.push_back(eRightSide);
                    }
                }
                //sideTypeList_PRT
                {
                    float cross=ccpCross(PRT, Pp);
                    if(cross==0){
                        sideTypeList_PRT.push_back(eOn);
                    }else if(cross>0){
                        sideTypeList_PRT.push_back(eLeftSide);
                    }else{
                        sideTypeList_PRT.push_back(eRightSide);
                    }
                    
                }
                
            }
        }//got sideTypeList_PLT and sideTypeList_PRT
        
        
        //see whether P is left umbra point
        //namely see whether all points are on the left side of (or on) PRT
        {
            bool allOnLeftOrOn=true;
            for(int j=0;j<nPoint;j++){
                eSideType sideType=sideTypeList_PRT[j];
                if(sideType==eLeftSide||sideType==eOn){
                }else{
                    allOnLeftOrOn=false;
                    break;
                }
            }//got allOnLeftOrOn
            if(allOnLeftOrOn){
                pointType.m_isLeftUmbraPoint=true;
                pointType.m_leftUmbraT=RT;
            }
        }
        //see whether P is right umbra point
        //namely see whether all points are on the right side of (or on) PLT
        {
            bool allOnRightOrOn=true;
            for(int j=0;j<nPoint;j++){
                eSideType sideType=sideTypeList_PLT[j];
                if(sideType==eRightSide||sideType==eOn){
                }else{
                    allOnRightOrOn=false;
                    break;
                }
            }//got allOnRightOrOn
            if(allOnRightOrOn){
                pointType.m_isRightUmbraPoint=true;
                pointType.m_rightUmbraT=LT;
            }
        }
        //see whether P is left penumbra point
        //namely see whether all points are on the left side of (or on) PLT
        {
            bool allOnLeftOrOn=true;
            for(int j=0;j<nPoint;j++){
                eSideType sideType=sideTypeList_PLT[j];
                if(sideType==eLeftSide||sideType==eOn){
                }else{
                    allOnLeftOrOn=false;
                    break;
                }
            }//got allOnLeftOrOn
            if(allOnLeftOrOn){
                pointType.m_isLeftPenumbraPoint=true;
                pointType.m_leftPenumbraT=LT;
            }
        }
        //see whether P is right penumbra point
        //namely see whether all points are on the right side of (or on) PRT
        {
            bool allOnRightOrOn=true;
            for(int j=0;j<nPoint;j++){
                eSideType sideType=sideTypeList_PRT[j];
                if(sideType==eRightSide||sideType==eOn){
                }else{
                    allOnRightOrOn=false;
                    break;
                }
            }//got allOnRightOrOn
            if(allOnRightOrOn){
                pointType.m_isRightPenumbraPoint=true;
                pointType.m_rightPenumbraT=RT;
            }
        }
        
    }//got m_pointTypeList
    //let there is only one left umbra point, only one right umbra point, only one left penumbra point, only one right penumbra point
    int nPointType=(int)m_pointTypeList.size();
    for(int i=0;i<nPointType;i++){
        CPointType&pointType=m_pointTypeList[i];
        CPointType&pointTypen=m_pointTypeList[(i+1)%nPointType];
        if(pointType.m_isLeftPenumbraPoint&&pointTypen.m_isLeftPenumbraPoint){
            pointType.m_isLeftPenumbraPoint=false;
        }
        if(pointType.m_isRightPenumbraPoint&&pointTypen.m_isRightPenumbraPoint){
            pointType.m_isRightPenumbraPoint=false;
        }
        if(pointType.m_isLeftUmbraPoint&&pointTypen.m_isLeftUmbraPoint){
            pointType.m_isLeftUmbraPoint=false;
        }
        if(pointType.m_isRightUmbraPoint&&pointTypen.m_isRightUmbraPoint){
            pointType.m_isRightUmbraPoint=false;
        }
    }
    //make
    //m_leftPenumbraLine
    //m_rightPenumbraLine
    //m_leftUmbraLine
    //m_rightUmbraLine
    //and
    //m_leftPenumbraPointID
    //m_rightPenumbraPointID
    //m_leftUmbraPointID
    //m_rightUmbraPointID
    m_leftPenumbraPointID=-1;
    m_rightPenumbraPointID=-1;
    m_leftUmbraPointID=-1;
    m_rightUmbraPointID=-1;
    for(int i=0;i<nPointType;i++){
        const Point&point=m_polygon.m_pointList[i];
        const CPointType&pointType=m_pointTypeList[i];
        if(pointType.m_isLeftPenumbraPoint){
            m_leftPenumbraLine[1]=point;
            m_leftPenumbraLine[0]=pointType.m_leftPenumbraT;
            m_leftPenumbraLine[2]=m_leftPenumbraLine[1]+ccpMult(ccpNormalize(m_leftPenumbraLine[1]-m_leftPenumbraLine[0]), m_shadowLength);
            m_leftPenumbraPointID=i;
        }
        if(pointType.m_isRightPenumbraPoint){
            m_rightPenumbraLine[1]=point;
            m_rightPenumbraLine[0]=pointType.m_rightPenumbraT;
            m_rightPenumbraLine[2]=m_rightPenumbraLine[1]+ccpMult(ccpNormalize(m_rightPenumbraLine[1]-m_rightPenumbraLine[0]), m_shadowLength);
            m_rightPenumbraPointID=i;
        }
        if(pointType.m_isLeftUmbraPoint){
            m_leftUmbraLine[1]=point;
            m_leftUmbraLine[0]=pointType.m_leftUmbraT;
            m_leftUmbraLine[2]=m_leftUmbraLine[1]+ccpMult(ccpNormalize(m_leftUmbraLine[1]-m_leftUmbraLine[0]), m_shadowLength);
            m_leftUmbraPointID=i;
        }
        if(pointType.m_isRightUmbraPoint){
            m_rightUmbraLine[1]=point;
            m_rightUmbraLine[0]=pointType.m_rightUmbraT;
            m_rightUmbraLine[2]=m_rightUmbraLine[1]+ccpMult(ccpNormalize(m_rightUmbraLine[1]-m_rightUmbraLine[0]), m_shadowLength);
            m_rightUmbraPointID=i;
        }
    }
    
    if(m_leftPenumbraPointID==-1)return false;
    if(m_rightPenumbraPointID==-1)return false;
    if(m_leftUmbraPointID==-1)return false;
    if(m_rightUmbraPointID==-1)return false;
    
    //calculate intersectPoint of m_leftUmbraLine and m_rightUmbraLine
    m_intersectPoint=getIntersectPointOfTwoLine(m_leftUmbraLine[0], ccpNormalize(m_leftUmbraLine[1]-m_leftUmbraLine[0]),
                                                m_rightUmbraLine[0], ccpNormalize(m_rightUmbraLine[1]-m_rightUmbraLine[0]));
    
    return true;
}

void C2DSoftShadowObj::onDraw(const Mat4 &transform, uint32_t flags) {
    
    Director* director = Director::getInstance();
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
    
    //----draw mesh
    if(m_isDrawNonDebug)
    {
        {
            //----change shader
            BlendFunc blendFunc={GL_SRC_ALPHA,GL_ONE};//{GL_SRC_ALPHA, GL_DST_ALPHA};
            this->setBlendFunc(blendFunc);
            
            GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX );
            GL::blendFunc( _blendFunc.src, _blendFunc.dst );
            
            //ccGLEnable(m_eGLServerState);
            //pass values for cocos2d-x build-in uniforms
            this->setGLProgram(m_program);
            CGLProgramWithUnifos*program=(CGLProgramWithUnifos*)this->getGLProgram();
            program->use();
            program->setUniformsForBuiltins();
            //pass values for my own uniforms
            program->passUnifoValue1f("u_oneDivObjCount", m_oneDivObjCount);
            
            //enable attributes
            /*
             bool isAttribPositionOn=CindexVBO::isEnabledAttribArray_position();
             bool isAttribColorOn=CindexVBO::isEnabledAttribArray_color();
             bool isAttribTexCoordOn=CindexVBO::isEnabledAttribArray_texCoord();
             
             CindexVBO::enableAttribArray_position(true);
             CindexVBO::enableAttribArray_color(true);
             CindexVBO::enableAttribArray_texCoord(true);
             */
            
            //bind texture
            GL::bindTexture2D( m_finTexture->getName());
            
            //draw m_indexVBO
            m_indexVBO->setPointer_position();
            m_indexVBO->setPointer_texCoord();
            m_indexVBO->setPointer_color();
            
            m_indexVBO->draw(GL_TRIANGLES);
            
            //unbind texture
            GL::bindTexture2D(0);
            //disable attributes
            //CindexVBO::enableAttribArray_position(isAttribPositionOn);
            //CindexVBO::enableAttribArray_color(isAttribColorOn);
            //CindexVBO::enableAttribArray_texCoord(isAttribTexCoordOn);
            
        }
        
    }
    if(m_isDrawDebug){
        if(m_isUpdateShadowSucc){
            ccBlendFunc blendFunc={GL_ONE, GL_ONE_MINUS_SRC_ALPHA};
            this->setBlendFunc(blendFunc);
            ccGLBlendFunc( _blendFunc.src, _blendFunc.dst );
            //----draw wire
            glLineWidth(2);
            ccDrawColor4F(0, 0, 1, 1);
            ccDrawPoly(&m_polygon.m_pointList.front(), (int)m_polygon.m_pointList.size(), true);
            //
            //light pos local
            ccPointSize(4);
            ccDrawColor4F(0, 0, 1, 1);
            ccDrawPoint(getLightPosLocal());
            
            //m_intersectPoint
            if(isPointEqual(m_intersectPoint, Point(INFINITY,INFINITY), 0)==false)
            {
                ccPointSize(4);
                ccDrawColor4F(0, 0, 1, 1);
                ccDrawPoint(m_intersectPoint);
                
                ccDrawLine(m_intersectPoint, m_leftUmbraLine[0]);
                ccDrawLine(m_intersectPoint, m_rightUmbraLine[0]);
            }
            
            //m_leftPenumbraLine
            {
                ccDrawColor4F(1, 0, 0, 1);
                glLineWidth(1);
                ccDrawLine(m_leftPenumbraLine[0], m_leftPenumbraLine[2]);
                ccDrawCircle(m_leftPenumbraLine[1], 5, 360, 10, false, 1, 1);
            }
            //m_rightPenumbraLine
            {
                ccDrawColor4F(0, 1, 0, 1);
                glLineWidth(1);
                ccDrawLine(m_rightPenumbraLine[0], m_rightPenumbraLine[2]);
                ccDrawCircle(m_rightPenumbraLine[1], 5, 360, 10, false, 1, 1);
            }
            //m_leftUmbraLine
            {
                ccDrawColor4F(1, 0, 0, 1);
                glLineWidth(1);
                ccDrawLine(m_leftUmbraLine[0], m_leftUmbraLine[2]);
                ccPointSize(5);
                ccDrawPoint(m_leftUmbraLine[1]);
            }
            //m_rightUmbraLine
            {
                ccDrawColor4F(0, 1, 0, 1);
                glLineWidth(1);
                ccDrawLine(m_rightUmbraLine[0], m_rightUmbraLine[2]);
                ccPointSize(5);
                ccDrawPoint(m_rightUmbraLine[1]);
            }
        }
    }

    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void C2DSoftShadowObj::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags){
    _customCommand.init(_globalZOrder);
    _customCommand.func = CC_CALLBACK_0(C2DSoftShadowObj::onDraw, this, transform, flags);
    renderer->addCommand(&_customCommand);
}

void C2DSoftShadowObj::setLight(ClightNode*light){
    if(m_light==NULL){
        m_light=light;
        m_light->retain();
    }else{
        m_light->release();
        m_light=light;
        m_light->retain();
    }
    
}

Point C2DSoftShadowObj::getLightPosLocal(){
    Point lightPosWorld=m_light->convertToWorldSpaceAR(ccp(0,0));
    Point lightPosLocal=this->convertToNodeSpace(lightPosWorld);
    return lightPosLocal;
}

void C2DSoftShadowObj::makeFullWindowRectMesh(){
    m_mesh->clear();
    Size winSize = VisibleRect::getVisibleRect().size;
    
    const Point p0World=Point(winSize.width,winSize.height);//RU
    const Point p1World=Point(0,winSize.height);//LU
    const Point p2World=Point(0,0);//LD
    const Point p3World=Point(winSize.width,0);//RD
    const Point p0=convertToNodeSpace(p0World);
    const Point p1=convertToNodeSpace(p1World);
    const Point p2=convertToNodeSpace(p2World);
    const Point p3=convertToNodeSpace(p3World);
    //v0
    Cv2 pos0=ccpTov2(p0);
    Cv2 texCoord0=Cv2(0,0);
    Cv4 color0=Cv4(1,1,0,1);
    //v1
    Cv2 pos1=ccpTov2(p1);
    Cv2 texCoord1=Cv2(0,0);
    Cv4 color1=Cv4(1,1,0,1);
    //v2
    Cv2 pos2=ccpTov2(p2);
    Cv2 texCoord2=Cv2(0,0);
    Cv4 color2=Cv4(1,1,0,1);
    //v3
    Cv2 pos3=ccpTov2(p3);
    Cv2 texCoord3=Cv2(0,0);
    Cv4 color3=Cv4(1,1,0,1);
    //add v0
    m_mesh->vlist.push_back(pos0);
    m_mesh->texCoordList.push_back(texCoord0);
    m_mesh->colorList.push_back(color0);
    int ID0=(int)m_mesh->vlist.size()-1;
    //add v1
    m_mesh->vlist.push_back(pos1);
    m_mesh->texCoordList.push_back(texCoord1);
    m_mesh->colorList.push_back(color1);
    int ID1=(int)m_mesh->vlist.size()-1;
    //add v2
    m_mesh->vlist.push_back(pos2);
    m_mesh->texCoordList.push_back(texCoord2);
    m_mesh->colorList.push_back(color2);
    int ID2=(int)m_mesh->vlist.size()-1;
    //add v3
    m_mesh->vlist.push_back(pos3);
    m_mesh->texCoordList.push_back(texCoord3);
    m_mesh->colorList.push_back(color3);
    int ID3=(int)m_mesh->vlist.size()-1;
    //add IDtri
    m_mesh->IDtriList.push_back(CIDTriangle(ID0,ID1,ID2));
    m_mesh->IDtriList.push_back(CIDTriangle(ID0,ID2,ID3));
}

void C2DSoftShadowObj::updateMesh(){
    m_mesh->clear();
    Point lightPosLocal=getLightPosLocal();
    
    //----right penumbra mesh
    {
        vector<Cedge> edgeList;
        int nPoint=(int)m_polygon.m_pointList.size();
        if(m_rightPenumbraPointID==m_rightUmbraPointID){
            edgeList.push_back(Cedge(m_rightPenumbraLine[1],m_rightPenumbraLine[2]));
            edgeList.push_back(Cedge(m_rightUmbraLine[1],m_rightUmbraLine[2]));
        }else{
            edgeList.push_back(Cedge(m_rightPenumbraLine[1],m_rightPenumbraLine[2]));
            int index=(m_rightPenumbraPointID+1)%nPoint;
            while(1){
                const Point&p=m_polygon.m_pointList[index];
                const Point&pf=m_polygon.m_pointList[index-1>=0?index-1:index-1+nPoint];
                Cedge edge;
                edge.m_start=pf;
                edge.m_end=edge.m_start+ccpMult(ccpNormalize(p-pf), m_shadowLength);
                edgeList.push_back(edge);
                if(index==m_rightUmbraPointID)break;
                index=(index+1)%nPoint;
            }
            edgeList.push_back(Cedge(m_rightUmbraLine[1],m_rightUmbraLine[2]));
        }//got edgeList
        vector<float> brightList;
        int nEdge=(int)edgeList.size();
        for(int i=0;i<nEdge;i++){
            const Cedge&edge=edgeList[i];
            //calculate area of the light circle on the right side of the edge
            float PND=PNDisPointToLine(lightPosLocal, edge.m_start, edge.m_end);
            float area=m_light->getR()-PND;
            brightList.push_back(area/(2*m_light->getR()));
        }
        //convert edgeList to mesh
        for(int i=0;i<nEdge-1;i++){
            const Cedge&edge=edgeList[i];
            const Cedge&edgen=edgeList[i+1];
            const Point&p0=edge.m_end;
            const Point&p1=edgen.m_end;
            const Point&p2=edgen.m_start;
            float bright=brightList[i];
            float brightn=brightList[i+1];
            
            //v0
            Cv2 pos0=ccpTov2(p0);
            Cv2 texCoord0=Cv2(0,0);
            Cv4 color0=Cv4(bright,brightn,0,1);
            //v1
            Cv2 pos1=ccpTov2(p1);
            Cv2 texCoord1=Cv2(1,0);
            Cv4 color1=Cv4(bright,brightn,0,1);
            //v2
            Cv2 pos2=ccpTov2(p2);
            Cv2 texCoord2=Cv2(0,1);
            Cv4 color2=Cv4(bright,brightn,0,1);
            //add v0
            m_mesh->vlist.push_back(pos0);
            m_mesh->texCoordList.push_back(texCoord0);
            m_mesh->colorList.push_back(color0);
            int ID0=(int)m_mesh->vlist.size()-1;
            //add v1
            m_mesh->vlist.push_back(pos1);
            m_mesh->texCoordList.push_back(texCoord1);
            m_mesh->colorList.push_back(color1);
            int ID1=(int)m_mesh->vlist.size()-1;
            //add v2
            m_mesh->vlist.push_back(pos2);
            m_mesh->texCoordList.push_back(texCoord2);
            m_mesh->colorList.push_back(color2);
            int ID2=(int)m_mesh->vlist.size()-1;
            //add IDtri
            m_mesh->IDtriList.push_back(CIDTriangle(ID0,ID1,ID2));
        }
    }
    
    //----left penumbra mesh
    {
        vector<Cedge> edgeList;
        int nPoint=(int)m_polygon.m_pointList.size();
        if(m_leftPenumbraPointID==m_leftUmbraPointID){
            edgeList.push_back(Cedge(m_leftPenumbraLine[1],m_leftPenumbraLine[2]));
            edgeList.push_back(Cedge(m_leftUmbraLine[1],m_leftUmbraLine[2]));
        }else{
            edgeList.push_back(Cedge(m_leftPenumbraLine[1],m_leftPenumbraLine[2]));
            int index=(m_leftPenumbraPointID-1>=0?m_leftPenumbraPointID-1:m_leftPenumbraPointID-1+nPoint);
            while(1){
                const Point&p=m_polygon.m_pointList[index];
                const Point&pf=m_polygon.m_pointList[(index+1)%nPoint];
                Cedge edge;
                edge.m_start=pf;
                edge.m_end=edge.m_start+ccpMult(ccpNormalize(p-pf), m_shadowLength);
                edgeList.push_back(edge);
                if(index==m_leftUmbraPointID)break;
                index=(index-1>=0?index-1:index-1+nPoint);
                
            }
            edgeList.push_back(Cedge(m_leftUmbraLine[1],m_leftUmbraLine[2]));
        }//got edgeList
        vector<float> brightList;
        int nEdge=(int)edgeList.size();
        for(int i=0;i<nEdge;i++){
            const Cedge&edge=edgeList[i];
            //calculate area of the light circle on the right side of the edge
            float PND=PNDisPointToLine(lightPosLocal, edge.m_start, edge.m_end);
            float area=m_light->getR()+PND;
            brightList.push_back(area/(2*m_light->getR()));
        }
        
        //convert edgeList to mesh
        for(int i=0;i<nEdge-1;i++){
            const Cedge&edge=edgeList[i];
            const Cedge&edgen=edgeList[i+1];
            const Point&p0=edge.m_end;
            const Point&p1=edgen.m_start;
            const Point&p2=edgen.m_end;
            float bright=brightList[i];
            float brightn=brightList[i+1];
            //v0
            Cv2 pos0=ccpTov2(p0);
            Cv2 texCoord0=Cv2(0,0);
            Cv4 color0=Cv4(bright,brightn,0,1);
            //v1
            Cv2 pos1=ccpTov2(p1);
            Cv2 texCoord1=Cv2(0,1);
            Cv4 color1=Cv4(bright,brightn,0,1);
            //v2
            Cv2 pos2=ccpTov2(p2);
            Cv2 texCoord2=Cv2(1,0);
            Cv4 color2=Cv4(bright,brightn,0,1);
            //add v0
            m_mesh->vlist.push_back(pos0);
            m_mesh->texCoordList.push_back(texCoord0);
            m_mesh->colorList.push_back(color0);
            int ID0=(int)m_mesh->vlist.size()-1;
            //add v1
            m_mesh->vlist.push_back(pos1);
            m_mesh->texCoordList.push_back(texCoord1);
            m_mesh->colorList.push_back(color1);
            int ID1=(int)m_mesh->vlist.size()-1;
            //add v2
            m_mesh->vlist.push_back(pos2);
            m_mesh->texCoordList.push_back(texCoord2);
            m_mesh->colorList.push_back(color2);
            int ID2=(int)m_mesh->vlist.size()-1;
            //add IDtri
            m_mesh->IDtriList.push_back(CIDTriangle(ID0,ID1,ID2));
            
        }
    }
    //----umbra mesh
    {
        
        if(isPointEqual(m_intersectPoint, Point(INFINITY,INFINITY), 0)){//parallel
            vector<Cedge> edgeList;
            edgeList.push_back(Cedge(m_rightUmbraLine[1],m_rightUmbraLine[2]));
            Point dir=ccpNormalize(m_rightUmbraLine[2]-m_rightUmbraLine[1]);
            int nPoint=(int)m_polygon.m_pointList.size();
            int index=(m_rightUmbraPointID+1)%nPoint;
            assert(m_rightUmbraPointID!=m_leftUmbraPointID);
            while(1){
                if(index==m_leftUmbraPointID)break;
                const Point&point=m_polygon.m_pointList[index];
                Cedge edge;
                edge.m_start=point;
                edge.m_end=edge.m_start+ccpMult(dir, m_shadowLength);
                edgeList.push_back(edge);
                index=(index+1)%nPoint;
            }
            edgeList.push_back(Cedge(m_leftUmbraLine[1],m_leftUmbraLine[2]));
            //convert edgeList to mesh
            int nEdge=(int)edgeList.size();
            for(int i=0;i<nEdge-1;i++){
                const Cedge&edge=edgeList[i];
                const Cedge&edgen=edgeList[i+1];
                const Point&p0=edge.m_start;//RU
                const Point&p1=edge.m_end;//LU
                const Point&p2=edgen.m_end;//LD
                const Point&p3=edgen.m_start;//RD
                //v0
                Cv2 pos0=ccpTov2(p0);
                Cv2 texCoord0=Cv2(1,1);
                Cv4 color0=Cv4(0,0,0,1);
                //v1
                Cv2 pos1=ccpTov2(p1);
                Cv2 texCoord1=Cv2(1,1);
                Cv4 color1=Cv4(0,0,0,1);
                //v2
                Cv2 pos2=ccpTov2(p2);
                Cv2 texCoord2=Cv2(1,1);
                Cv4 color2=Cv4(0,0,0,1);
                //v3
                Cv2 pos3=ccpTov2(p3);
                Cv2 texCoord3=Cv2(1,1);
                Cv4 color3=Cv4(0,0,0,1);
                //add v0
                m_mesh->vlist.push_back(pos0);
                m_mesh->texCoordList.push_back(texCoord0);
                m_mesh->colorList.push_back(color0);
                int ID0=(int)m_mesh->vlist.size()-1;
                //add v1
                m_mesh->vlist.push_back(pos1);
                m_mesh->texCoordList.push_back(texCoord1);
                m_mesh->colorList.push_back(color1);
                int ID1=(int)m_mesh->vlist.size()-1;
                //add v2
                m_mesh->vlist.push_back(pos2);
                m_mesh->texCoordList.push_back(texCoord2);
                m_mesh->colorList.push_back(color2);
                int ID2=(int)m_mesh->vlist.size()-1;
                //add v3
                m_mesh->vlist.push_back(pos3);
                m_mesh->texCoordList.push_back(texCoord3);
                m_mesh->colorList.push_back(color3);
                int ID3=(int)m_mesh->vlist.size()-1;
                //add IDtri
                m_mesh->IDtriList.push_back(CIDTriangle(ID0,ID1,ID2));
                m_mesh->IDtriList.push_back(CIDTriangle(ID0,ID2,ID3));
                
            }
            
        }else{//not parallel
            //see intersectP on which side of light
            if(Vec2(m_leftUmbraLine[1]-m_leftUmbraLine[0]).dot(m_leftUmbraLine[1]-m_intersectPoint)>0){//intersectP on back side of light
                vector<Cedge> edgeList;
                edgeList.push_back(Cedge(m_rightUmbraLine[1],m_rightUmbraLine[2]));
                int nPoint=(int)m_polygon.m_pointList.size();
                int index=(m_rightUmbraPointID+1)%nPoint;
                assert(m_rightUmbraPointID!=m_leftUmbraPointID);
                while(1){
                    if(index==m_leftUmbraPointID)break;
                    const Point&point=m_polygon.m_pointList[index];
                    Cedge edge;
                    edge.m_start=point;
                    edge.m_end=edge.m_start+ (Vec2(point-m_intersectPoint).getNormalized() * m_shadowLength);
                    edgeList.push_back(edge);
                    index=(index+1)%nPoint;
                }
                edgeList.push_back(Cedge(m_leftUmbraLine[1],m_leftUmbraLine[2]));
                //convert edgeList to mesh
                int nEdge=(int)edgeList.size();
                for(int i=0;i<nEdge-1;i++){
                    const Cedge&edge=edgeList[i];
                    const Cedge&edgen=edgeList[i+1];
                    const Point&p0=edge.m_start;//RU
                    const Point&p1=edge.m_end;//LU
                    const Point&p2=edgen.m_end;//LD
                    const Point&p3=edgen.m_start;//RD
                    //v0
                    Cv2 pos0=ccpTov2(p0);
                    Cv2 texCoord0=Cv2(1,1);
                    Cv4 color0=Cv4(0,0,0,1);
                    //v1
                    Cv2 pos1=ccpTov2(p1);
                    Cv2 texCoord1=Cv2(1,1);
                    Cv4 color1=Cv4(0,0,0,1);
                    //v2
                    Cv2 pos2=ccpTov2(p2);
                    Cv2 texCoord2=Cv2(1,1);
                    Cv4 color2=Cv4(0,0,0,1);
                    //v3
                    Cv2 pos3=ccpTov2(p3);
                    Cv2 texCoord3=Cv2(1,1);
                    Cv4 color3=Cv4(0,0,0,1);
                    //add v0
                    m_mesh->vlist.push_back(pos0);
                    m_mesh->texCoordList.push_back(texCoord0);
                    m_mesh->colorList.push_back(color0);
                    int ID0=(int)m_mesh->vlist.size()-1;
                    //add v1
                    m_mesh->vlist.push_back(pos1);
                    m_mesh->texCoordList.push_back(texCoord1);
                    m_mesh->colorList.push_back(color1);
                    int ID1=(int)m_mesh->vlist.size()-1;
                    //add v2
                    m_mesh->vlist.push_back(pos2);
                    m_mesh->texCoordList.push_back(texCoord2);
                    m_mesh->colorList.push_back(color2);
                    int ID2=(int)m_mesh->vlist.size()-1;
                    //add v3
                    m_mesh->vlist.push_back(pos3);
                    m_mesh->texCoordList.push_back(texCoord3);
                    m_mesh->colorList.push_back(color3);
                    int ID3=(int)m_mesh->vlist.size()-1;
                    //add IDtri
                    m_mesh->IDtriList.push_back(CIDTriangle(ID0,ID1,ID2));
                    m_mesh->IDtriList.push_back(CIDTriangle(ID0,ID2,ID3));
                    
                }
                
            }else{//intersectP on front side of light
                vector<Cedge> edgeList;
                edgeList.push_back(Cedge(m_rightUmbraLine[1],m_intersectPoint));
                int nPoint=(int)m_polygon.m_pointList.size();
                int index=(m_rightUmbraPointID+1)%nPoint;
                assert(m_rightUmbraPointID!=m_leftUmbraPointID);
                while(1){
                    if(index==m_leftUmbraPointID)break;
                    const Point&point=m_polygon.m_pointList[index];
                    Cedge edge;
                    edge.m_start=point;
                    edge.m_end=m_intersectPoint;
                    edgeList.push_back(edge);
                    index=(index+1)%nPoint;
                }
                edgeList.push_back(Cedge(m_leftUmbraLine[1],m_intersectPoint));
                //convert edgeList to mesh
                int nEdge=(int)edgeList.size();
                for(int i=0;i<nEdge-1;i++){
                    const Cedge&edge=edgeList[i];
                    const Cedge&edgen=edgeList[i+1];
                    const Point&p0=edge.m_start;
                    const Point&p1=edge.m_end;
                    const Point&p2=edgen.m_start;
                    //v0
                    Cv2 pos0=ccpTov2(p0);
                    Cv2 texCoord0=Cv2(1,1);
                    Cv4 color0=Cv4(0,0,0,1);
                    //v1
                    Cv2 pos1=ccpTov2(p1);
                    Cv2 texCoord1=Cv2(1,1);
                    Cv4 color1=Cv4(0,0,0,1);
                    //v2
                    Cv2 pos2=ccpTov2(p2);
                    Cv2 texCoord2=Cv2(1,1);
                    Cv4 color2=Cv4(0,0,0,1);
                    //add v0
                    m_mesh->vlist.push_back(pos0);
                    m_mesh->texCoordList.push_back(texCoord0);
                    m_mesh->colorList.push_back(color0);
                    int ID0=(int)m_mesh->vlist.size()-1;
                    //add v1
                    m_mesh->vlist.push_back(pos1);
                    m_mesh->texCoordList.push_back(texCoord1);
                    m_mesh->colorList.push_back(color1);
                    int ID1=(int)m_mesh->vlist.size()-1;
                    //add v2
                    m_mesh->vlist.push_back(pos2);
                    m_mesh->texCoordList.push_back(texCoord2);
                    m_mesh->colorList.push_back(color2);
                    int ID2=(int)m_mesh->vlist.size()-1;
                    //add IDtri
                    m_mesh->IDtriList.push_back(CIDTriangle(ID0,ID1,ID2));
                    
                }
                
            }
        }
    }
    //make the full light space mesh
    {
        Point intersectOfPenumbraLines=getIntersectPointOfTwoLine(
                                                                  m_leftPenumbraLine[0], Vec2(m_leftPenumbraLine[1]-m_leftPenumbraLine[0]).getNormalized(),
                                                                    m_rightPenumbraLine[0], Vec2(m_rightPenumbraLine[1]-m_rightPenumbraLine[0]).getNormalized());
        //space between m_leftPenumbraLine, m_rightPenumbraLine and this polygon
        if(m_leftPenumbraPointID==m_rightPenumbraPointID){
            
        }else{//m_leftPenumbraPointID!=m_rightPenumbraPointID
            
            
            int nPoint=(int)m_polygon.m_pointList.size();
            int index=m_leftPenumbraPointID;
            while(1){
                const Point&p=m_polygon.m_pointList[index];
                const Point&pn=m_polygon.m_pointList[(index+1)%nPoint];
                //
                const Point&p0=intersectOfPenumbraLines;
                const Point&p1=pn;
                const Point&p2=p;
                //v0
                Cv2 pos0=ccpTov2(p0);
                Cv2 texCoord0=Cv2(0,0);
                Cv4 color0=Cv4(1,1,0,1);
                //v1
                Cv2 pos1=ccpTov2(p1);
                Cv2 texCoord1=Cv2(0,0);
                Cv4 color1=Cv4(1,1,0,1);
                //v2
                Cv2 pos2=ccpTov2(p2);
                Cv2 texCoord2=Cv2(0,0);
                Cv4 color2=Cv4(1,1,0,1);
                //add v0
                m_mesh->vlist.push_back(pos0);
                m_mesh->texCoordList.push_back(texCoord0);
                m_mesh->colorList.push_back(color0);
                int ID0=(int)m_mesh->vlist.size()-1;
                //add v1
                m_mesh->vlist.push_back(pos1);
                m_mesh->texCoordList.push_back(texCoord1);
                m_mesh->colorList.push_back(color1);
                int ID1=(int)m_mesh->vlist.size()-1;
                //add v2
                m_mesh->vlist.push_back(pos2);
                m_mesh->texCoordList.push_back(texCoord2);
                m_mesh->colorList.push_back(color2);
                int ID2=(int)m_mesh->vlist.size()-1;
                //add IDtri
                m_mesh->IDtriList.push_back(CIDTriangle(ID0,ID1,ID2));
                index=(index+1)%nPoint;
                if(index==m_rightPenumbraPointID)break;
                
            }
            
        }
        //down fan
        {
            const float dA=45;
            const Point startDir=ccpNormalize(m_leftPenumbraLine[2]-intersectOfPenumbraLines);
            const Point endDir=ccpNormalize(m_rightPenumbraLine[0]-m_rightPenumbraLine[1]);
            Point dir=startDir;
            Point dirfoe;
            while(1){
                dirfoe=dir;
                dir=rotateVector2(dir, dA);
                bool isFinishCurLoopAndStop=false;
                if(ccpCross(dir, endDir)<0//surpass endDir
                   ){
                    dir=endDir;
                    isFinishCurLoopAndStop=true;
                }
                //make triangle between dirfoe and dir
                const Point&p0=intersectOfPenumbraLines;
                const Point&p1=intersectOfPenumbraLines+ccpMult(dirfoe, m_shadowLength);
                const Point&p2=intersectOfPenumbraLines+ccpMult(dir, m_shadowLength);
                //v0
                Cv2 pos0=ccpTov2(p0);
                Cv2 texCoord0=Cv2(0,0);
                Cv4 color0=Cv4(1,1,0,1);
                //v1
                Cv2 pos1=ccpTov2(p1);
                Cv2 texCoord1=Cv2(0,0);
                Cv4 color1=Cv4(1,1,0,1);
                //v2
                Cv2 pos2=ccpTov2(p2);
                Cv2 texCoord2=Cv2(0,0);
                Cv4 color2=Cv4(1,1,0,1);
                //add v0
                m_mesh->vlist.push_back(pos0);
                m_mesh->texCoordList.push_back(texCoord0);
                m_mesh->colorList.push_back(color0);
                int ID0=(int)m_mesh->vlist.size()-1;
                //add v1
                m_mesh->vlist.push_back(pos1);
                m_mesh->texCoordList.push_back(texCoord1);
                m_mesh->colorList.push_back(color1);
                int ID1=(int)m_mesh->vlist.size()-1;
                //add v2
                m_mesh->vlist.push_back(pos2);
                m_mesh->texCoordList.push_back(texCoord2);
                m_mesh->colorList.push_back(color2);
                int ID2=(int)m_mesh->vlist.size()-1;
                //add IDtri
                m_mesh->IDtriList.push_back(CIDTriangle(ID0,ID1,ID2));
                //stop
                if(isFinishCurLoopAndStop)break;
            }
        }
        //up fan
        {
            const float dA=45;
            const Point startDir=ccpNormalize(m_leftPenumbraLine[0]-m_leftPenumbraLine[1]);
            const Point endDir=ccpNormalize(m_rightPenumbraLine[2]-m_rightPenumbraLine[1]);//ccpNormalize(m_rightPenumbraLine[0]-m_rightPenumbraLine[1]);//ccpNormalize(m_rightPenumbraLine[2]-intersectOfPenumbraLines);
            Point dir=startDir;
            Point dirfoe;
            while(1){
                dirfoe=dir;
                dir=rotateVector2(dir, dA);
                bool isFinishCurLoopAndStop=false;
                if(ccpCross(dir, endDir)<0//surpass endDir
                   ){
                    dir=endDir;
                    isFinishCurLoopAndStop=true;
                }
                //make triangle between dirfoe and dir
                const Point&p0=intersectOfPenumbraLines;
                const Point&p1=intersectOfPenumbraLines+ccpMult(dirfoe, m_shadowLength);
                const Point&p2=intersectOfPenumbraLines+ccpMult(dir, m_shadowLength);
                //v0
                Cv2 pos0=ccpTov2(p0);
                Cv2 texCoord0=Cv2(0,0);
                Cv4 color0=Cv4(1,1,0,1);
                //v1
                Cv2 pos1=ccpTov2(p1);
                Cv2 texCoord1=Cv2(0,0);
                Cv4 color1=Cv4(1,1,0,1);
                //v2
                Cv2 pos2=ccpTov2(p2);
                Cv2 texCoord2=Cv2(0,0);
                Cv4 color2=Cv4(1,1,0,1);
                //add v0
                m_mesh->vlist.push_back(pos0);
                m_mesh->texCoordList.push_back(texCoord0);
                m_mesh->colorList.push_back(color0);
                int ID0=(int)m_mesh->vlist.size()-1;
                //add v1
                m_mesh->vlist.push_back(pos1);
                m_mesh->texCoordList.push_back(texCoord1);
                m_mesh->colorList.push_back(color1);
                int ID1=(int)m_mesh->vlist.size()-1;
                //add v2
                m_mesh->vlist.push_back(pos2);
                m_mesh->texCoordList.push_back(texCoord2);
                m_mesh->colorList.push_back(color2);
                int ID2=(int)m_mesh->vlist.size()-1;
                //add IDtri
                m_mesh->IDtriList.push_back(CIDTriangle(ID0,ID1,ID2));
                //stop?
                if(isFinishCurLoopAndStop)break;
            }
        }
        //right fan
        {
            const float dA=45;
            const Point startDir = Vec2(m_rightPenumbraLine[0]-m_rightPenumbraLine[1]).getNormalized();
            const Point endDir = Vec2(m_leftPenumbraLine[0]-m_leftPenumbraLine[1]).getNormalized();
            Point dir=startDir;
            Point dirfoe;
            while(1){
                dirfoe=dir;
                dir=rotateVector2(dir, dA);
                bool isFinishCurLoopAndStop=false;
                if(dir.cross(endDir)<0//surpass endDir
                   ){
                    dir=endDir;
                    isFinishCurLoopAndStop=true;
                }
                //make triangle between dirfoe and dir
                const Point&p0=intersectOfPenumbraLines;
                const Point&p1=intersectOfPenumbraLines+ dirfoe * m_shadowLength;
                const Point&p2=intersectOfPenumbraLines+ dir * m_shadowLength;
                //v0
                Cv2 pos0=ccpTov2(p0);
                Cv2 texCoord0=Cv2(0,0);
                Cv4 color0=Cv4(1,1,0,1);
                //v1
                Cv2 pos1=ccpTov2(p1);
                Cv2 texCoord1=Cv2(0,0);
                Cv4 color1=Cv4(1,1,0,1);
                //v2
                Cv2 pos2=ccpTov2(p2);
                Cv2 texCoord2=Cv2(0,0);
                Cv4 color2=Cv4(1,1,0,1);
                //add v0
                m_mesh->vlist.push_back(pos0);
                m_mesh->texCoordList.push_back(texCoord0);
                m_mesh->colorList.push_back(color0);
                int ID0=(int)m_mesh->vlist.size()-1;
                //add v1
                m_mesh->vlist.push_back(pos1);
                m_mesh->texCoordList.push_back(texCoord1);
                m_mesh->colorList.push_back(color1);
                int ID1=(int)m_mesh->vlist.size()-1;
                //add v2
                m_mesh->vlist.push_back(pos2);
                m_mesh->texCoordList.push_back(texCoord2);
                m_mesh->colorList.push_back(color2);
                int ID2=(int)m_mesh->vlist.size()-1;
                //add IDtri
                m_mesh->IDtriList.push_back(CIDTriangle(ID0,ID1,ID2));
                //stop?
                if(isFinishCurLoopAndStop)break;
            }
        }
    }
}

void C2DSoftShadowObj::submit(GLenum usage){
    //submit mesh
    GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX );
    m_indexVBO->submitPos(m_mesh->vlist, usage);
    m_indexVBO->submitTexCoord(m_mesh->texCoordList, usage);
    m_indexVBO->submitColor(m_mesh->colorList, usage);
    m_indexVBO->submitIndex(m_mesh->IDtriList, usage);
}
//---------------------------------------------

bool C2DSoftShadowRoot::init(){
    Size winSize = VisibleRect::getVisibleRect().size;
    m_shadowRT=CCRenderTexture::create(winSize.width, winSize.height);
    m_shadowRT->retain();
    m_shadowRT->setPosition(Vec2(winSize.width/2,winSize.height/2));
    //----create and set shader program
    {
        GLchar * fragSource = (GLchar*) CCString::createWithContentsOfFile(
            FileUtils::getInstance()->fullPathForFilename("shaders/2DSoftShadow_renderShadow.fsh").c_str())->getCString();
        CGLProgramWithUnifos* program = new CGLProgramWithUnifos();
        program->autorelease();
        program->initWithByteArrays(ccPositionTextureColor_vert, fragSource);
        //bind attribute
        program->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
        program->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
        program->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORD);
        //link  (must after bindAttribute)
        program->link();
        //get cocos2d-x build-in uniforms
        program->updateUniforms();
        //get my own uniforms
        program->attachUniform("u_objCount");
        program->attachUniform("u_shadowDarkness");
        //set program
        m_program=program;
        m_program->retain();
        //check gl error
        CHECK_GL_ERROR_DEBUG();
    }
    //change shader for m_shadowRT
    m_shadowRT->getSprite()->setGLProgram(m_program);
    //set blendFunc for m_shadowRT
    BlendFunc blendFunc={GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA};
    m_shadowRT->getSprite()->setBlendFunc(blendFunc);
    return true;
}
void C2DSoftShadowRoot::setLight(ClightNode*light){
    assert(light);
    if(m_light==NULL){
        m_light=light;
        addChild(m_light);
    }else{
        m_light->removeFromParentAndCleanup(true);
        m_light=light;
        addChild(m_light);
    }
}

void C2DSoftShadowRoot::addObj(C2DSoftShadowObj*obj){
    assert(obj);
    m_objList.push_back(obj);
    addChild(obj);
}

void C2DSoftShadowRoot::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags){
    //push matrix
    Director* director = Director::getInstance();
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

    //transform
    this->transform(parentTransform);
    {
        //set objs' isDrawDebug
        int nObj=(int)m_objList.size();
        for(int i=0;i<nObj;i++){
            C2DSoftShadowObj*obj=m_objList[i];
            obj->setIsDrawDebug(m_isDrawDebug);
        }
        //set light's isDrawDebug
        m_light->setIsDrawDebug(m_isDrawDebug);
        
        //render to shadowRT
        m_shadowRT->beginWithClear(0, 0, 0, 1);
        {
            int nObj=(int)m_objList.size();
            float oneDivObjCount=1.0/nObj;
            for(int i=0;i<nObj;i++) {
                C2DSoftShadowObj*obj=m_objList[i];
                obj->setOneDivObjCount(oneDivObjCount);
                bool isDrawDebugOld=obj->getIsDrawDebug();
                obj->setIsDrawDebug(false);
                obj->visit(renderer,parentTransform,parentFlags);
                obj->setIsDrawDebug(isDrawDebugOld);
            }
            
        }
        m_shadowRT->end();
        
        //pass uniform value for m_shadowRT's program
        CGLProgramWithUnifos*program=(CGLProgramWithUnifos*)m_shadowRT->getSprite()->getGLProgram();
        program->use();//must call this!
        program->passUnifoValue1i("u_objCount", nObj);
        program->passUnifoValue1f("u_shadowDarkness", m_shadowDarkness);
        
        //visit shadowRT
        m_shadowRT->visit(renderer,parentTransform,parentFlags);
        
#if 0
        //draw objs' debug
        for(int i=0;i<nObj;i++) {
            C2DSoftShadowObj*obj=m_objList[i];
            bool isDrawNonDebugOld=obj->getIsDrawNonDebug();
            obj->setIsDrawNonDebug(false);
            obj->visit(renderer,parentTransform,parentFlags);
            obj->setIsDrawNonDebug(isDrawNonDebugOld);
        }
#endif
         
        //draw light' debug
        m_light->visit(renderer,parentTransform,parentFlags);
    }

    //pop matrix
    Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}


namespace_ens_end