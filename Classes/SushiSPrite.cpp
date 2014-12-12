#include "SushiSprite.h"
#include <iostream>

#define TOTAL_SUSHI 6

static const char *sushiNormal[TOTAL_SUSHI] = {
	"sushi_1n.png",
	"sushi_2n.png",
	"sushi_3n.png",
	"sushi_4n.png",
	"sushi_5n.png",
	"sushi_6n.png"
};

float SushiSprite::getContentWidth()
{
	/*
	static float itemWidth = 0;
	if (0 == itemWidth)
	{
		auto sprite = Sprite::createWithSpriteFrameName(sushiNormal[0]);
		itemWidth = sprite->getContentSize().width;
	}
	return itemWidth;
	*/
	return 90;
}

SushiSprite* SushiSprite::create(int row, int col)
{
	auto sushi = SushiSprite::create();
	sushi->m_row = row;
	sushi->m_col = col;
	sushi->m_imgIndex = rand() % TOTAL_SUSHI;
	sushi->initWithSpriteFrameName(sushiNormal[sushi->m_imgIndex]);
	return sushi;
}

bool SushiSprite::init()
{
	if (!Sprite::init())
		return false;
	return true;
}