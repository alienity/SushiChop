#ifndef __SushiSprite_H__
#define __SushiSprite_H__

#include "cocos2d.h"

using namespace cocos2d;

class SushiSprite : public Sprite
{
public:
	static SushiSprite* create(int row, int col);
	static float getContentWidth();
	virtual bool init();
	CC_SYNTHESIZE(int, m_row, Row);
	CC_SYNTHESIZE(int, m_col, Col);
	CC_SYNTHESIZE(int, m_imgIndex, ImgIndex);
	CREATE_FUNC(SushiSprite);
};

#endif