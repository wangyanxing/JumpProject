//
//  ensMath.h
//  HelloCpp
//
//  Created by apple on 14-6-14.
//
//

#ifndef __HelloCpp__ensMath__
#define __HelloCpp__ensMath__

#include <iostream>
using namespace std;
#include "cocos2d.h"
using namespace cocos2d;
#include "ensDefine.h"
#include "ensSimpleClasses.h"
namespace_ens_begin

float random01();
float randomInRange(float min,float max);
bool catchProbability(float p);
bool isPointEqual(const Point&point1,const Point&point2,float eps);
Point rotateVector2(const Point&vec,float A);
Point rotateVector2(const Point&vec,float cosA,float sinA);
float calculateAngleFromCosAndSin(float cosA,float sinA);
Point calculateCosASinAOfVec1ToVec2(const Point&vec1,const Point&vec2);
float nPow(float a,int n);
float pow2(float x);
float PNDisPointToLine(const Point&point,const Point&start,const Point&end);
bool isPointEqual(const Point&p1,const Point&p2);
Point getIntersectPointOfTwoLine(const Point&p1,const Point&dir1, const Point&p2,const Point&dir2);
float calculateIncircleR2(const Point&v0,const Point&v1,const Point&v2);
vector<Point> makeTriangleSmaller(Point v0,Point v1,Point v2,float d);
Point getRightNormal(const Point&dir);
Point getRightVector(const Point&p1,const Point&p2);
float calculateTriangleArea(const Point&v0,const Point&v1,const Point&v2);
vector<Point> calculateTangentPointsOfPointToCircle(const Point &center, float r,const Point &point);
namespace_ens_end

#endif /* defined(__HelloCpp__ensMath__) */
