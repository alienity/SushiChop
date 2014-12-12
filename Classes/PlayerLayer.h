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
	void explodeSushi(SushiSprite* sushi);//����˾���ж����ŵ�ʱ������
	void checkAndRemoveChain();//�����Ƴ���˾
	void removeSushis(std::list<SushiSprite*> &sushiList);//�Ƴ������˾
	void removeSushi(SushiSprite* sushi);//�Ƴ�������˾
	void fillVacancies();//���λ

	float time = 1.0;
	bool m_isAnimationing;
	int m_width;
	int m_height;
	int m_matrixLeftBottomX;
	int m_matrixLeftBottomY;
	std::array<std::array<SushiSprite*, MATRIX_WIDTH>, MATRIX_HEIGHT> m_matrix;
};

#endif