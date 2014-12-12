#ifndef __StarSprite_H__
#define __StarSprite_H__

#include "cocos2d.h"

using namespace cocos2d;

#define TOTAL_STSR 6

class StarSprite : public Sprite
{
public:
	static StarSprite* createStar(int x, int y);
	static StarSprite* createStar(Vec2 vec);
	virtual bool init();
	CREATE_FUNC(StarSprite);
	CC_SYNTHESIZE(int, x_pos, X_Position);
	CC_SYNTHESIZE(int, y_pos, Y_Position);
	CC_SYNTHESIZE(int, m_img_index, Img_Index);//设置使用的是哪张纹理
};

#endif