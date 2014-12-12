#ifndef __StarsLayer_H__
#define __StarsLayer_H__

#include "cocos2d.h"
#include "StarSprite.h"
#include <array>

using namespace cocos2d;

#define STARS_WIDTH 7
#define STARS_HEIGHT 9
#define STARS_GAP 1

enum Selected_tag
{
	t1 = 100000,
	t2
};

class StarsLayer : public Layer
{
public:
	static Scene* createScene();
	virtual bool init();

	virtual void update(float dt);

	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);

	CREATE_FUNC(StarsLayer);
private:
	Size size;

	int width;
	int height;

	float m_matrixLeftBottomX;//�������½ǵĿ�ʼ���x����
	float m_matrixLeftBottomY;//�������½ǵĿ�ʼ���y����

	float starWidth;//���ڱ������ǵĿ��(��Ϊ�����������ε�)

	bool m_isAnimationing = true;//�Ƿ��ж���,Ĭ�����ж�����
	bool m_isTouched = false;//�Ƿ����ִ�����Ļ�ƶ�����

	std::array<std::array<StarSprite*, STARS_HEIGHT>, STARS_WIDTH> starSprites;//�����������õ�����
	
	StarSprite* star_selected_1 = nullptr;//��һ����ѡ�е�����
	StarSprite* star_selected_2 = nullptr;//�ڶ�����ѡ�е�����
private:
	Vec2 positionOf_X_Y(int x, int y);
	Vec2 x_y_OfPosition(float real_x, float real_y);

	void initMatrix();
	void createAndDropStar(int x, int y);
	void checkAndRemoveChain();
	bool checkAnimation();
	void markItemToRemove(std::list<StarSprite*>& starList, std::array<std::array<bool, STARS_HEIGHT>, STARS_WIDTH>& removedArray);

	void getXChain(int x, int y, std::list<StarSprite*>& starList);
	void getYChain(int x, int y, std::list<StarSprite*>& starList);

	void fillVancances();
	void removeStars(std::array<std::array<bool, STARS_HEIGHT>, STARS_WIDTH>& removedArray);

	void exploseStar(int x, int y);

	bool checkRelated(StarSprite* star1, StarSprite* star2);
	void swapStar(StarSprite* star1, StarSprite* star2);
	bool checkGoodSwap(StarSprite* star1);
};

#endif