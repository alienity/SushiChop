#include "StarsLayer.h"
#include <math.h>

Scene* StarsLayer::createScene()
{
	auto scene = Scene::create();
	auto layer = StarsLayer::create();
	scene->addChild(layer);
	return scene;
}

bool StarsLayer::init()
{
	if (!Layer::init())
		return false;

	//Ϊ������ṩ����
	srand(time(0));
	
	//��ȡ���Ӵ�С
	size = Director::getInstance()->getVisibleSize();

	//��ʼ������
	auto background = Sprite::create("background.png");
	background->setPosition(size.width / 2, size.height / 2);
	this->addChild(background, -1);

	//��ʼ�����Ǳ�
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("total.plist");
	
	//��ʼ�����ǵĿ�
	auto tempStar = StarSprite::createStar(0, 0);
	starWidth = tempStar->getContentSize().width;

	//��ʼ������Ŀ��
	width = STARS_WIDTH;
	height = STARS_HEIGHT;

	//��ʼ���������½ǵĵ�
	m_matrixLeftBottomX = (size.width - starWidth * width + STARS_GAP * (width - 1)) / 2;
	m_matrixLeftBottomY = (size.height - starWidth * height + STARS_GAP * (height - 1)) / 2;
	

	//��ʼ�����Ǿ���
	initMatrix();

	//ѭ�����
	scheduleUpdate();

	//ע�����¼�
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(StarsLayer::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(StarsLayer::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(StarsLayer::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void StarsLayer::initMatrix()
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			createAndDropStar(x, y);
		}
	}
}

void StarsLayer::createAndDropStar(int x, int y)
{
	auto star = StarSprite::createStar(x, y);

	//������ִ�����䶯��
	auto endPosition = positionOf_X_Y(x, y);
	auto startPosition = positionOf_X_Y(x, y + height);
	star->setPosition(startPosition);
	this->addChild(star);
	float speed = startPosition.y / size.height;
	star->runAction(MoveTo::create(speed, endPosition));
	//�Ѵ��������ǵ����ñ�������
	starSprites[x][y] = star;
}

//�����࣬�ܹ���9��7�У������ж���һ����һ�ж�Ӧ�ľ�������
Vec2 StarsLayer::positionOf_X_Y(int x, int y)
{
	float real_x = m_matrixLeftBottomX + (starWidth + STARS_GAP) * x + starWidth / 2;
	float real_y = m_matrixLeftBottomY + (starWidth + STARS_GAP) * y + starWidth / 2;;
	return Vec2(real_x, real_y);
}

//�����࣬�ܹ���9��7�У������ж��ִ����ľ��������Ӧ����һ����һ��
Vec2 StarsLayer::x_y_OfPosition(float real_x, float real_y)
{
	int x = (real_x - m_matrixLeftBottomX) / starWidth;
	int y = (real_y - m_matrixLeftBottomY) / starWidth;
	if ((x >= 0 && x < 7 && y >= 0 && y < 9))
		return Vec2(x, y);
	return Vec2(-1, -1);
}

void StarsLayer::update(float dt)
{
	//1.�����Ϸ�������Ƿ��ж���
	checkAnimation();
	
	//2.���ȼ���Ƿ�����������ƶ�����
	if (m_isTouched)
		return;
	
	//3.�����û������ִ�ж����Լ�û������ڴ�����Ļ��ʱ�򣬼���ɾ��������������������
	if (!m_isAnimationing)
		checkAndRemoveChain();
}

bool StarsLayer::checkAnimation()
{
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				auto star = starSprites[x][y];
				if (star && star->getNumberOfRunningActions() > 0)
				{
					m_isAnimationing = true;
					return m_isAnimationing;
				}
			}
		}
		m_isAnimationing = false;
		return m_isAnimationing;
}

void StarsLayer::checkAndRemoveChain()
{
	//����һ�������Ƿ�Ҫɾ����array
	std::array<std::array<bool, STARS_HEIGHT>, STARS_WIDTH> removedArray;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			removedArray[i][j] = false;
		}
	}

	//���Ҫɾ��������
	std::list<StarSprite*> starList_x;
	std::list<StarSprite*> starList_y;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (removedArray[x][y])//����õ��Ѿ������ɾ���������õ���ж�
				break;
			
			getXChain(x , y, starList_x);
			getYChain(x , y, starList_y);
			
			if (starList_x.size() >= 3)//�Ѳ鵽��������Ҫɾ���ı��б��
				markItemToRemove(starList_x, removedArray);
			if (starList_y.size() >= 3)
				markItemToRemove(starList_y, removedArray);

			starList_x.clear();
			starList_y.clear();
		}
	}

	//ɾ������ǵ�����
	removeStars(removedArray);

	//���ɾ��������
	if (!checkAnimation())
		fillVancances();
}

void StarsLayer::removeStars(std::array<std::array<bool, STARS_HEIGHT>, STARS_WIDTH>& removedArray)
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (removedArray[x][y])//��������Ҫɾ��
			{
				exploseStar(x, y);//��ը֮��ͰѶ���ɾ����
				starSprites[x][y] = nullptr;
			}
		}
	}
}

void StarsLayer::exploseStar(int x, int y)
{
	auto star = starSprites[x][y];
	//1.����Ч��
	auto particleStars = ParticleSystemQuad::create("stars.plist");
	particleStars->setAutoRemoveOnFinish(true);
	particleStars->setBlendAdditive(false);
	particleStars->setPosition(star->getPosition());
	particleStars->setScale(0.3);
	this->addChild(particleStars, 20);
	//2.ԲȦ��Ч
/*	auto circleSprite = Sprite::create("circle.png");
	this->addChild(circleSprite, 10);
	circleSprite->setPosition(star->getPosition());
	circleSprite->setScale(0);
	circleSprite->runAction(Sequence::createWithTwoActions(
		ScaleTo::create(1.5, 1.0),
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, circleSprite))));
*/	//3.��СЧ��
	star->runAction(Sequence::createWithTwoActions(
		ScaleTo::create(1, 0),
		CallFunc::create(CC_CALLBACK_0(StarSprite::removeFromParent, star))));
}

void StarsLayer::fillVancances()
{
	
	//�ƶ����������
	for (int x = 0; x < width; x++)
	{
		int removedNumber = 0;
		for (int y = 0; y < height; y++)
		{
			if (starSprites[x][y] == nullptr)
			{
				removedNumber++;
			}
			else if (removedNumber != 0)
			{
				int speed = removedNumber / 2;
				starSprites[x][y]->runAction(MoveTo::create(speed, positionOf_X_Y(x, y - removedNumber)));
				starSprites[x][y]->setX_Position(x);
				starSprites[x][y]->setY_Position(y - removedNumber);
				starSprites[x][y - removedNumber] = starSprites[x][y];
				starSprites[x][y] = nullptr;
			}
		}
		//��ӵ��������
		for (int i = 0; i < removedNumber; i++)
			createAndDropStar(x, height + i - removedNumber);
	}
}

void StarsLayer::markItemToRemove(std::list<StarSprite*>& starList, std::array<std::array<bool, STARS_HEIGHT>, STARS_WIDTH>& removedArray)
{
	for (auto i = starList.begin(); i != starList.end(); i++)
	{
		auto star = *i;
		removedArray[star->getX_Position()][star->getY_Position()] = true;
	}
}

void StarsLayer::getXChain(int x, int y, std::list<StarSprite*>& starList)//��ȡ�ڵ�(x, y)x�᷽���������������˾
{
	//�Ȱѵ�ǰ������뵽Ҫ�洢������
	starList.push_back(starSprites[x][y]);

	//�����Ҳ鿴
	int neighbor = x + 1;
	while (neighbor < width)
	{
		if (starSprites[x][y]->getImg_Index() == starSprites[neighbor][y]->getImg_Index())
			starList.push_back(starSprites[neighbor][y]);
		else
			break;
		neighbor += 1;
	}

	//����鿴
	neighbor = x - 1;
	while (neighbor >= 0)
	{
		if (starSprites[x][y]->getImg_Index() == starSprites[neighbor][y]->getImg_Index())
			starList.push_back(starSprites[neighbor][y]);
		else
			break;
		neighbor -= 1;
	}
}

void StarsLayer::getYChain(int x, int y, std::list<StarSprite*>& starList)////��ȡ�ڵ�(x, y)x�᷽���������������˾
{
	//�Ȱѵ�ǰ������뵽Ҫ�洢������
	starList.push_back(starSprites[x][y]);

	//�����Ҳ鿴
	int neighbor = y + 1;
	while (neighbor < height)
	{
		if (starSprites[x][y]->getImg_Index() == starSprites[x][neighbor]->getImg_Index())
			starList.push_back(starSprites[x][neighbor]);
		else
			break;
		neighbor += 1;
	}

	//����鿴
	neighbor = y - 1;
	while (neighbor >= 0)
	{
		if (starSprites[x][y]->getImg_Index() == starSprites[x][neighbor]->getImg_Index())
			starList.push_back(starSprites[x][neighbor]);
		else
			break;
		neighbor -= 1;
	}
}

bool StarsLayer::checkRelated(StarSprite* star1, StarSprite* star2)//�ж����������ǲ������ڵ�
{
	if (((std::abs(star1->getX_Position() - star2->getX_Position()) == 1) && (star1->getY_Position() == star2->getY_Position())) ||
		((std::abs(star1->getY_Position() - star2->getY_Position()) == 1) && (star1->getX_Position() == star2->getX_Position())))
		return true;
	return false;
}

void StarsLayer::swapStar(StarSprite* star1, StarSprite* star2)//�����������ڵ�����
{
/*	//1.������ͼ���ϰ��������������
	star1->runAction(MoveTo::create(1, star2->getPosition()));
	star2->runAction(MoveTo::create(1, star1->getPosition()));
*/	
	//2.�������ǽ������������λ������
	int star1_x = star1->getX_Position();
	int star1_y = star1->getY_Position();

	int star2_x = star2->getX_Position();
	int star2_y = star2->getY_Position();

	star1->setX_Position(star2_x);
	star1->setY_Position(star2_y);

	star2->setX_Position(star1_x);
	star2->setY_Position(star1_y);

	//3.����ڼ�¼��Ҳ�ѽ���ʵ��
	starSprites[star1_x][star1_y] = star2;
	starSprites[star2_x][star2_y] = star1;

}

bool StarsLayer::checkGoodSwap(StarSprite* star)
{
	std::list<StarSprite*> starList_x;
	std::list<StarSprite*> starList_y;
	getXChain(star->getX_Position(), star->getY_Position(), starList_x);
	getYChain(star->getX_Position(), star->getY_Position(), starList_y);
	if (starList_x.size() > 2 || starList_y.size() > 2)
		return true;
	return false;
}

bool StarsLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	if (m_isAnimationing)
		return false;

	m_isTouched = true;//�൱��һ����������Ҵ�����Ļ��ʱ���������

	auto location = touch->getLocation();
	auto position = x_y_OfPosition(location.x, location.y);

	if (position.x == -1)//�жϵ����λ���Ƿ��е�λ
	{
		m_isTouched = false;
		return false;
	}

	//Ϊ����������¼������
	if (star_selected_1 == nullptr)
	{
		star_selected_1 = starSprites[position.x][position.y];
		auto selected_1 = Sprite::create("circle.png");
		selected_1->setPosition(positionOf_X_Y(position.x, position.y));
		this->addChild(selected_1, -1, Selected_tag::t1);
	}
	else if (star_selected_2 == nullptr)
	{
		star_selected_2 = starSprites[position.x][position.y];
		auto selected_2 = Sprite::create("circle.png");
		selected_2->setPosition(positionOf_X_Y(position.x, position.y));
		this->addChild(selected_2, -1, Selected_tag::t2);
	}

	//������Ӧ���¼�
	if (star_selected_1 != nullptr && star_selected_2 != nullptr)
	{
		this->removeChildByTag(Selected_tag::t1, true);
		this->removeChildByTag(Selected_tag::t2, true);

		if (star_selected_1 == star_selected_2)
		{
			star_selected_1 = nullptr;
			star_selected_2 = nullptr;

			m_isTouched = false;
			return false;
		}
		else if (checkRelated(star_selected_1, star_selected_2))
		{
			swapStar(star_selected_1, star_selected_2);
			auto action_1 = MoveBy::create(1, star_selected_2->getPosition() - star_selected_1->getPosition());
			auto action_2 = MoveBy::create(1, star_selected_1->getPosition() - star_selected_2->getPosition());
			if (checkGoodSwap(star_selected_1) || checkGoodSwap(star_selected_2))
			{
				star_selected_1->runAction(action_1);
				star_selected_2->runAction(action_2);
			}
			else
			{
				star_selected_1->runAction(Sequence::create(action_1, action_1->reverse(), nullptr));
				star_selected_2->runAction(Sequence::create(action_2, action_2->reverse(), nullptr));
				swapStar(star_selected_1, star_selected_2);
			}
		}

		star_selected_1 = nullptr;
		star_selected_2 = nullptr;
	}

	m_isTouched = false;
	return true;
}

void StarsLayer::onTouchMoved(Touch *touch, Event *unused_event)
{

}

void StarsLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
	
}