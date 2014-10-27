#include "PopLayer.h"




PopLayer::PopLayer():
	m__pMenu(NULL)
	, m_contentPadding(0)
	, m_contentPaddingTop(0)
	, m_callbackListener(NULL)
	, m_callback(NULL)
	, m__sfBackGround(NULL)
	, m__s9BackGround(NULL)
	, m__ltContentText(NULL)
	, m__ltTitle(NULL)
	, m__pTitle(NULL)
{

}

PopLayer::~PopLayer()
{
	CC_SAFE_RELEASE(m__pMenu);
	CC_SAFE_RELEASE(m__sfBackGround);
	CC_SAFE_RELEASE(m__ltContentText);
	CC_SAFE_RELEASE(m__ltTitle);
	CC_SAFE_RELEASE(m__s9BackGround);

	CC_SAFE_RELEASE(m__pTitle);
}

bool PopLayer::init()
{
	bool bRef = false;
	do{
		CC_BREAK_IF(!CCLayer::init());
		this->setContentSize(Size::ZERO);

		// 初始化需要的 Menu
		Menu* menu = CCMenu::create();
		menu->setPosition(Vec2::ZERO);
		setMenuButton(menu);

		//屏蔽下方的触摸
		EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();		
		listener->setSwallowTouches(true);
		listener->onTouchBegan = [](Touch *touch, Event *event){
			return true;
		};
		this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	
		bRef = true;
	}while(0);
	return bRef;
}

PopLayer* PopLayer::create(const char* backgroundImage)
{
	PopLayer* ml = PopLayer::create();
	ml->setSpriteBackGround(Sprite::create(backgroundImage));
	ml->setSprite9BackGround(Scale9Sprite::create(backgroundImage));
	return ml;
}

#ifdef USE_TEXT
void PopLayer::setTitle(const char* title, int fontsize /*= 20*/)
{
	LabelTTF* ltfTitle = LabelTTF::create(title, "", fontsize);
	ltfTitle->setColor(Color3B(128, 128, 105));
	setLabelTitle(ltfTitle);
}
#endif

#ifdef USE_IMAGE
void PopLayer::setTitle(const char *fileName)
{
	Sprite *imgTitle = Sprite::create(fileName);
	setImgTitle(imgTitle);
}
#endif
void PopLayer::setContentText(const char* text, int fontsize /*= 20*/, int padding /*= 50*/, int paddingTop /*= 100*/)
{

	LabelTTF* ltf = LabelTTF::create(text, "", fontsize);
	//LabelTTF* ltf = LabelTTF::create("longggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg", "", fontsize);
	ltf->setColor(Color3B::WHITE);
	setLabelContentText(ltf);
	m_contentPadding = padding;
	m_contentPaddingTop = paddingTop;
}

void PopLayer::setCallbackFunc(Node* target, SEL_CallFuncN callfun)
{
	m_callbackListener = target;
	m_callback = callfun; 
}

bool PopLayer::addButton(const char* normalImage, const char* selectedImage, const char* title, int tag /*= 0*/)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 pCenter(visibleSize.width / 2, visibleSize.height / 2);

	// 创建图片菜单按钮
	MenuItemImage* menuImage = MenuItemImage::create(normalImage, selectedImage, CC_CALLBACK_1(PopLayer::buttonCallback, this));
	menuImage->setTag(tag);
	//menuImage->setPosition(pCenter);

	// 添加文字说明并设置位置
	if(title != NULL)
	{
		Size imenu = menuImage->getContentSize();
		LabelTTF* ttf = LabelTTF::create(title, "", 20);
		ttf->setColor(Color3B(0, 0, 0));
		ttf->setPosition(Vec2(imenu.width / 2, imenu.height / 2));
		menuImage->addChild(ttf);
	}
	

	getMenuButton()->addChild(menuImage);
	return true;
}

void PopLayer::onEnter()
{
	Layer::onEnter();

	Size winSize = Director::getInstance()->getVisibleSize();
	Vec2 pCenter(winSize.width / 2, winSize.height / 2);

	Size contentSize;
	// 设定好参数，在运行时加载
	if (getContentSize().equals(Size::ZERO)) {
		getSpriteBackGround()->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
		this->addChild(getSpriteBackGround(), 0, 0);
		contentSize = getSpriteBackGround()->getTexture()->getContentSize();
	} else {
		Scale9Sprite *background = getSprite9BackGround();
		background->setContentSize(getContentSize());
		background->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
		this->addChild(background, 0, 0);
		contentSize = getContentSize();
	}


	// 添加按钮，并设置其位置
	this->addChild(getMenuButton());
	float btnWidth = contentSize.width / (getMenuButton()->getChildrenCount() + 1);

	Vector<Node *> array = getMenuButton()->getChildren();
	getMenuButton()->alignItemsHorizontallyWithPadding(50);
	getMenuButton()->setPosition(Vec2(winSize.width / 2, winSize.height / 2 - contentSize.height / 3));
// 	int i = 0;
// 	for(auto &bt: array)
// 	{
// 		Node* node = dynamic_cast<Node*>(bt);
// 		node->setPosition(Vec2( winSize.width / 2 - contentSize.width / 2 + btnWidth * (i + 1), winSize.height / 2 - contentSize.height / 3));
// 		i++;
// 	}


	// 显示对话框标题
	if (getLabelTitle()){
		getLabelTitle()->setPosition((pCenter + Vec2(0, contentSize.height / 2 - 35.0f)));
		this->addChild(getLabelTitle());
	}

	if(getImgTitle())
	{
		getImgTitle()->setPosition((pCenter + Vec2(0, contentSize.height / 2 - 40.0f)));
		this->addChild(getImgTitle());
	}

	// 显示文本内容
	if (getLabelContentText()){
		LabelTTF* ltf = getLabelContentText();
		//ltf->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
		ltf->setPosition(pCenter);
		ltf->setDimensions(Size(contentSize.width - m_contentPadding * 2, contentSize.height - m_contentPaddingTop));
		ltf->setHorizontalAlignment(TextHAlignment::LEFT);
		this->addChild(ltf);
	}

	// 弹出效果
	Action* popupLayer = Sequence::create(ScaleTo::create(0.0, 0.0),
		ScaleTo::create(0.06, 1.05),
		ScaleTo::create(0.08, 0.95),
		ScaleTo::create(0.08, 1.0), NULL);
	this->runAction(popupLayer);
}

void PopLayer::onExit()
{
	log("popup on exit.");
	Layer::onExit();
}

void PopLayer::buttonCallback(Ref* pSender)
{
	Node* node = dynamic_cast<Node*>(pSender);
	log("touch tag: %d", node->getTag());
	if (m_callback && m_callbackListener){
		(m_callbackListener->*m_callback)(node);
	}
	this->removeFromParent();
}
