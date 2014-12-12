#include "PlayerLayer.h"
#include <array>
#include <ctime>

bool PlayerLayer::init()
{
	if (!Layer::init())
		return false;

	srand(std::time(0));//为后面的随机数种下种子

	//创建游戏背景
	auto size = Director::getInstance()->getVisibleSize();
	auto background = Sprite::create("background.png");
	background->setPosition(size.width / 2, size.height / 2);
	this->addChild(background);

	//初始化寿司表单
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("total.plist");

	//初始化矩阵的宽和高
	m_width = MATRIX_WIDTH;
	m_height = MATRIX_HEIGHT;

	//初始化寿司左下角的点
	m_matrixLeftBottomX = (size.width - SushiSprite::getContentWidth() * m_width + (m_width - 1) * SUSHI_GAP) / 2;
	m_matrixLeftBottomY = (size.height - SushiSprite::getContentWidth() * m_height + (m_height - 1) * SUSHI_GAP) / 2;
	
	//初始化寿司矩阵
	initMatrix();

	//指定循环检测
	scheduleUpdate();
	return true;
}

void PlayerLayer::initMatrix()
{
	for (int row = 0; row < m_height; row++)
	{
		for (int col = 0; col < m_width; col++)
		{
			createAndDropSushi(row, col);
		}
	}
}

void PlayerLayer::createAndDropSushi(int row, int col)
{
	auto size = Director::getInstance()->getVisibleSize();
	auto sushi = SushiSprite::create(row, col);

	//创建并执行下落动画
	auto endPosition = positionOfItem(row, col);
	auto startPosition = positionOfItem(row + MATRIX_HEIGHT, col);//把开始点放到最上面
	sushi->setPosition(startPosition);
	this->addChild(sushi);//把寿司添加到该层上
	float speed = startPosition.y / size.height;
	sushi->runAction(MoveTo::create(speed, endPosition));

	m_matrix[row][col] = sushi;//给指定位置的数组赋值
}

Vec2 PlayerLayer::positionOfItem(int row, int col)
{
	float y = m_matrixLeftBottomY + (SushiSprite::getContentWidth() + SUSHI_GAP) * row + SushiSprite::getContentWidth() / 2;
	float x = m_matrixLeftBottomX + (SushiSprite::getContentWidth() + SUSHI_GAP) * col + SushiSprite::getContentWidth() / 2;
	return Vec2(x, y);
}

void PlayerLayer::update(float delta)
{
	//检查游戏场景中是否有action
	if (m_isAnimationing)
	{
		//初始化m_isAnimationing
		m_isAnimationing = false;
		for (int i = 0; i < m_height; i++)
		{
			for (int j = 0; j < m_width; j++)
			{
				auto sushi = m_matrix[i][j];
				if (sushi && sushi->getNumberOfRunningActions() > 0)//原来这个方法是继承来的，我还在想为什么没有维护判断呢
				{
					m_isAnimationing = true;//如果有精灵有动作就设置为true
					break;
				}
			}
		}
	}
	//当如果没有寿司执行action时，检测和删除满足消除条件的寿司
	if (!m_isAnimationing)
		checkAndRemoveChain();
}

void PlayerLayer::checkAndRemoveChain()
{
	std::list<SushiSprite*> rowChainList, colChainList;
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			if (!m_matrix[i][j])//如果当前寿司为空，则退出当前循环
				break;
			getColChain(m_matrix[i][j], rowChainList);//检测纵向的不小于三个寿司的数
			getRowChain(m_matrix[i][j], colChainList);//检测横向的不小于三个寿司的数
			if (rowChainList.size() >= 2 || colChainList.size() >= 2)//判断是否要移除寿司
			{
				removeSushi(m_matrix[i][j]);
				if (rowChainList.size() >= 2)
					removeSushis(rowChainList);
				if (colChainList.size() >= 2)
					removeSushis(colChainList);
			}
		}
	}
	fillVacancies();//填补空缺
}

void PlayerLayer::getColChain(SushiSprite* sushi, std::list<SushiSprite*> &chainList)
{
	//向前检测相同图标值(ImgIndex)的寿司
	int neighborCol = sushi->getCol() - 1;//sushi前一列寿司所在列数值
	while (neighborCol >= 0)
	{
		auto neighborSushi = m_matrix[sushi->getRow()][neighborCol];
		if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex()))
		{
			chainList.push_back(neighborSushi);
			neighborCol--;//继续往前
		}
		else
		{
			break;
		}
	}
	//向后检测相同图标值(ImgIndex)的寿司
	neighborCol = sushi->getCol() + 1;
	while (neighborCol < m_width)
	{
		auto neighborSushi = m_matrix[sushi->getRow()][neighborCol];
		if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex()))
		{
			chainList.push_front(neighborSushi);
			++neighborCol;
		}
		else
		{
			break;
		}
	}
}

void PlayerLayer::getRowChain(SushiSprite* sushi, std::list<SushiSprite*> &chainList)
{
	//向下检测相同图标值(ImgIndex)的寿司
	int neighborRow = sushi->getRow() - 1;//sushi前一列寿司所在列数值
	while (neighborRow >= 0)
	{
		auto neighborSushi = m_matrix[neighborRow][sushi->getCol()];
		if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex()))
		{
			chainList.push_back(neighborSushi);
			neighborRow--;//继续往前
		}
		else
		{
			break;
		}
	}
	//向后检测相同图标值(ImgIndex)的寿司
	neighborRow = sushi->getRow() + 1;
	while (neighborRow < m_height)
	{
		auto neighborSushi = m_matrix[neighborRow][sushi->getCol()];
		if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex()))
		{
			chainList.push_front(neighborSushi);
			++neighborRow;
		}
		else
		{
			break;
		}
	}
}

void PlayerLayer::removeSushis(std::list<SushiSprite*> &sushiList)
{
	m_isAnimationing = true;

	std::list<SushiSprite*>::iterator itList;
	//依次去除sushiList的值
	for (itList = sushiList.begin(); itList != sushiList.end(); itList++)
	{
		auto sushi = *itList;
		removeSushi(sushi);
	}
}

void PlayerLayer::removeSushi(SushiSprite* sushi)
{
	//从m_matrix中移除sushi
	m_matrix[sushi->getRow()][sushi->getCol()] = nullptr;
	explodeSushi(sushi);
}

void PlayerLayer::explodeSushi(SushiSprite* sushi)
{
	//1.寿司精灵action-逐渐变小
	sushi->runAction(Sequence::createWithTwoActions(ScaleTo::create(time, 0.0), CallFunc::create(CC_CALLBACK_0(SushiSprite::removeFromParent, sushi))));
	//2.圆圈特效
	auto circleSprite = Sprite::create("circle.png");
	this->addChild(circleSprite, 10);
	circleSprite->setPosition(sushi->getPosition());
	circleSprite->setScale(0);
	circleSprite->runAction(Sequence::createWithTwoActions(ScaleTo::create(time, 1.0), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, circleSprite))));//执行完ScaleTo动作后将circleSprite从父节点移除
	//3.粒子特效
	auto particleStars = ParticleSystemQuad::create("stars.plist");
	particleStars->setAutoRemoveOnFinish(true);
	particleStars->setBlendAdditive(false);
	particleStars->setPosition(sushi->getPosition());
	particleStars->setScale(0.3);
	this->addChild(particleStars, 20);
}

void PlayerLayer::fillVacancies()
{
	std::array<int, MATRIX_WIDTH> colEmptyInfo;
	auto size = Director::getInstance()->getVisibleSize();
	
	//1.让空缺处上面的寿司精灵向下落
	SushiSprite* sushi = nullptr;
	for (int col = 0; col < m_width; col++)
	{
		int removeSushiOfCol = 0;//记录一列中空缺的精灵数
		//从下往上，你可以看到这个顺序是有讲究的
		for (int row = 0; row < m_height; row++)
		{
			sushi = m_matrix[row][col];
			if (sushi == nullptr)
				removeSushiOfCol++;
			else
			{
				if (removeSushiOfCol > 0)
				{
					m_matrix[row - removeSushiOfCol][col] = sushi;//把上面的寿司往下面移动
					m_matrix[row][col] = nullptr;//把当前位置的寿司置为空
					//然后再做界面上的移动
					auto startPosition = sushi->getPosition();
					auto endPosition = positionOfItem(row - removeSushiOfCol, col);
					float speed = (startPosition.y - endPosition.y) / size.height;
					sushi->stopAllActions();//must stop pre drop action
					sushi->runAction(MoveTo::create(speed, endPosition));
					//设置寿司的新行
					sushi->setRow(row - removeSushiOfCol);
				}
			}
		}
		//记录col列上空缺数
		colEmptyInfo[col] = removeSushiOfCol;
	}
	
	//2.创建新的寿司精灵并让它落到上方空缺处
	for (int col = 0; col < m_width; col++)
	{
		for (int row = m_height - colEmptyInfo[col]; row < m_height; row++)
		{
			createAndDropSushi(row, col);
		}
	}
}