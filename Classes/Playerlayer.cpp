#include "PlayerLayer.h"
#include <array>
#include <ctime>

bool PlayerLayer::init()
{
	if (!Layer::init())
		return false;

	srand(std::time(0));//Ϊ������������������

	//������Ϸ����
	auto size = Director::getInstance()->getVisibleSize();
	auto background = Sprite::create("background.png");
	background->setPosition(size.width / 2, size.height / 2);
	this->addChild(background);

	//��ʼ����˾��
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("total.plist");

	//��ʼ������Ŀ�͸�
	m_width = MATRIX_WIDTH;
	m_height = MATRIX_HEIGHT;

	//��ʼ����˾���½ǵĵ�
	m_matrixLeftBottomX = (size.width - SushiSprite::getContentWidth() * m_width + (m_width - 1) * SUSHI_GAP) / 2;
	m_matrixLeftBottomY = (size.height - SushiSprite::getContentWidth() * m_height + (m_height - 1) * SUSHI_GAP) / 2;
	
	//��ʼ����˾����
	initMatrix();

	//ָ��ѭ�����
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

	//������ִ�����䶯��
	auto endPosition = positionOfItem(row, col);
	auto startPosition = positionOfItem(row + MATRIX_HEIGHT, col);//�ѿ�ʼ��ŵ�������
	sushi->setPosition(startPosition);
	this->addChild(sushi);//����˾��ӵ��ò���
	float speed = startPosition.y / size.height;
	sushi->runAction(MoveTo::create(speed, endPosition));

	m_matrix[row][col] = sushi;//��ָ��λ�õ����鸳ֵ
}

Vec2 PlayerLayer::positionOfItem(int row, int col)
{
	float y = m_matrixLeftBottomY + (SushiSprite::getContentWidth() + SUSHI_GAP) * row + SushiSprite::getContentWidth() / 2;
	float x = m_matrixLeftBottomX + (SushiSprite::getContentWidth() + SUSHI_GAP) * col + SushiSprite::getContentWidth() / 2;
	return Vec2(x, y);
}

void PlayerLayer::update(float delta)
{
	//�����Ϸ�������Ƿ���action
	if (m_isAnimationing)
	{
		//��ʼ��m_isAnimationing
		m_isAnimationing = false;
		for (int i = 0; i < m_height; i++)
		{
			for (int j = 0; j < m_width; j++)
			{
				auto sushi = m_matrix[i][j];
				if (sushi && sushi->getNumberOfRunningActions() > 0)//ԭ����������Ǽ̳����ģ��һ�����Ϊʲôû��ά���ж���
				{
					m_isAnimationing = true;//����о����ж���������Ϊtrue
					break;
				}
			}
		}
	}
	//�����û����˾ִ��actionʱ������ɾ������������������˾
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
			if (!m_matrix[i][j])//�����ǰ��˾Ϊ�գ����˳���ǰѭ��
				break;
			getColChain(m_matrix[i][j], rowChainList);//�������Ĳ�С��������˾����
			getRowChain(m_matrix[i][j], colChainList);//������Ĳ�С��������˾����
			if (rowChainList.size() >= 2 || colChainList.size() >= 2)//�ж��Ƿ�Ҫ�Ƴ���˾
			{
				removeSushi(m_matrix[i][j]);
				if (rowChainList.size() >= 2)
					removeSushis(rowChainList);
				if (colChainList.size() >= 2)
					removeSushis(colChainList);
			}
		}
	}
	fillVacancies();//���ȱ
}

void PlayerLayer::getColChain(SushiSprite* sushi, std::list<SushiSprite*> &chainList)
{
	//��ǰ�����ͬͼ��ֵ(ImgIndex)����˾
	int neighborCol = sushi->getCol() - 1;//sushiǰһ����˾��������ֵ
	while (neighborCol >= 0)
	{
		auto neighborSushi = m_matrix[sushi->getRow()][neighborCol];
		if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex()))
		{
			chainList.push_back(neighborSushi);
			neighborCol--;//������ǰ
		}
		else
		{
			break;
		}
	}
	//�������ͬͼ��ֵ(ImgIndex)����˾
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
	//���¼����ͬͼ��ֵ(ImgIndex)����˾
	int neighborRow = sushi->getRow() - 1;//sushiǰһ����˾��������ֵ
	while (neighborRow >= 0)
	{
		auto neighborSushi = m_matrix[neighborRow][sushi->getCol()];
		if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex()))
		{
			chainList.push_back(neighborSushi);
			neighborRow--;//������ǰ
		}
		else
		{
			break;
		}
	}
	//�������ͬͼ��ֵ(ImgIndex)����˾
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
	//����ȥ��sushiList��ֵ
	for (itList = sushiList.begin(); itList != sushiList.end(); itList++)
	{
		auto sushi = *itList;
		removeSushi(sushi);
	}
}

void PlayerLayer::removeSushi(SushiSprite* sushi)
{
	//��m_matrix���Ƴ�sushi
	m_matrix[sushi->getRow()][sushi->getCol()] = nullptr;
	explodeSushi(sushi);
}

void PlayerLayer::explodeSushi(SushiSprite* sushi)
{
	//1.��˾����action-�𽥱�С
	sushi->runAction(Sequence::createWithTwoActions(ScaleTo::create(time, 0.0), CallFunc::create(CC_CALLBACK_0(SushiSprite::removeFromParent, sushi))));
	//2.ԲȦ��Ч
	auto circleSprite = Sprite::create("circle.png");
	this->addChild(circleSprite, 10);
	circleSprite->setPosition(sushi->getPosition());
	circleSprite->setScale(0);
	circleSprite->runAction(Sequence::createWithTwoActions(ScaleTo::create(time, 1.0), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, circleSprite))));//ִ����ScaleTo������circleSprite�Ӹ��ڵ��Ƴ�
	//3.������Ч
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
	
	//1.�ÿ�ȱ���������˾����������
	SushiSprite* sushi = nullptr;
	for (int col = 0; col < m_width; col++)
	{
		int removeSushiOfCol = 0;//��¼һ���п�ȱ�ľ�����
		//�������ϣ�����Կ������˳�����н�����
		for (int row = 0; row < m_height; row++)
		{
			sushi = m_matrix[row][col];
			if (sushi == nullptr)
				removeSushiOfCol++;
			else
			{
				if (removeSushiOfCol > 0)
				{
					m_matrix[row - removeSushiOfCol][col] = sushi;//���������˾�������ƶ�
					m_matrix[row][col] = nullptr;//�ѵ�ǰλ�õ���˾��Ϊ��
					//Ȼ�����������ϵ��ƶ�
					auto startPosition = sushi->getPosition();
					auto endPosition = positionOfItem(row - removeSushiOfCol, col);
					float speed = (startPosition.y - endPosition.y) / size.height;
					sushi->stopAllActions();//must stop pre drop action
					sushi->runAction(MoveTo::create(speed, endPosition));
					//������˾������
					sushi->setRow(row - removeSushiOfCol);
				}
			}
		}
		//��¼col���Ͽ�ȱ��
		colEmptyInfo[col] = removeSushiOfCol;
	}
	
	//2.�����µ���˾���鲢�����䵽�Ϸ���ȱ��
	for (int col = 0; col < m_width; col++)
	{
		for (int row = m_height - colEmptyInfo[col]; row < m_height; row++)
		{
			createAndDropSushi(row, col);
		}
	}
}