#ifndef __PlayerLayer_H__
#define __PlayerLayer_H__

#include <array>
#include "cocos2d.h"
#include "SushiSprite.h"

#define MATRIX_WIDTH 7
#define MATRIX_HEIGHT 9
#define SUSHI_GAP 1

using namespace cocos2d;

class PlayerLayer : public Layer
{
public:
	virtual bool init();
	void initMatrix();
	void createAndDropSushi(int row, int col);
	Vec2 positionOfItem(int row, int col);
	void getColChain(SushiSprite* sushi, std::list<SushiSprite*> &chainList);
	void getRowChain(SushiSprite* sushi, std::list<SushiSprite*> &chainList);
	virtual void update(float delta);
	CREATE_FUNC(PlayerLayer);
private:
	void explodeSushi(SushiSprite* sushi);//当寿司被判断连着的时候被消除
	void checkAndRemoveChain();//检查和移除寿司
	void removeSushis(std::list<SushiSprite*> &sushiList);//移除多个寿司
	void removeSushi(SushiSprite* sushi);//移除单个寿司
	void fillVacancies();//填补空位

	float time = 1.0;
	bool m_isAnimationing;
	int m_width;
	int m_height;
	int m_matrixLeftBottomX;
	int m_matrixLeftBottomY;
	std::array<std::array<SushiSprite*, MATRIX_WIDTH>, MATRIX_HEIGHT> m_matrix;
};

#endif