#include "StarSprite.h"

static const char* starNormal[TOTAL_STSR] = {
	"sushi_1n.png",
	"sushi_2n.png",
	"sushi_3n.png",
	"sushi_4n.png",
	"sushi_5n.png",
	"sushi_6n.png"
};

StarSprite* StarSprite::createStar(Vec2 vec)
{
	return createStar(vec.x, vec.y);
}

StarSprite* StarSprite::createStar(int x, int y)
{
	auto starSprite = StarSprite::create();
	if (starSprite && starSprite->init())
	{
		starSprite->setX_Position(x);
		starSprite->setY_Position(y);
		starSprite->setImg_Index(rand() % TOTAL_STSR);
		starSprite->initWithSpriteFrameName(starNormal[starSprite->getImg_Index()]);
		return starSprite;
	}
	else
	{
		delete starSprite;
		starSprite = nullptr;
		return nullptr;
	}
}

bool StarSprite::init()
{
	return true;
}

