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

	float m_matrixLeftBottomX;//保存左下角的开始点的x坐标
	float m_matrixLeftBottomY;//保存左下角的开始点的y坐标

	float starWidth;//用于保存星星的宽度(因为星星是正方形的)

	bool m_isAnimationing = true;//是否还有动作,默认是有动画的
	bool m_isTouched = false;//是否用手触碰屏幕移动星星

	std::array<std::array<StarSprite*, STARS_HEIGHT>, STARS_WIDTH> starSprites;//保存星星引用的容器
	
	StarSprite* star_selected_1 = nullptr;//第一个被选中的星星
	StarSprite* star_selected_2 = nullptr;//第二个被选中的星星
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