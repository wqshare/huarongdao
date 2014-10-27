#ifndef POP_LAYER_H_H
#define POP_LAYER_H_H

#include "cocos2d.h"
#include "cocos-ext.h"

#define USE_IMAGE
USING_NS_CC;
USING_NS_CC_EXT;

//模态对话框
class PopLayer : public cocos2d::Layer
{
public:
	PopLayer();
	~PopLayer();

	virtual bool init();
	CREATE_FUNC(PopLayer);
		
	static PopLayer* create(const char* backgroundImage);


#ifdef USE_TEXT
	void setTitle(const char* title, int fontsize = 20);
#endif // USE_TEXT

#ifdef USE_IMAGE
	void setTitle(const char *fileName);
#endif
	
	void setContentText(const char* text, int fontsize = 20, int padding = 50, int paddingTop = 100);

	void setCallbackFunc(Node* target, SEL_CallFuncN callfun);

	bool addButton(const char* normalImage, const char* selectedImage, const char* title, int tag = 0);
	virtual void onEnter();
	virtual void onExit();

private:

	void buttonCallback(Ref* pSender);

	// 文字内容两边的空白区
	int m_contentPadding;
	int m_contentPaddingTop;

	Node* m_callbackListener;
	SEL_CallFuncN m_callback;

	CC_SYNTHESIZE_RETAIN(Menu*, m__pMenu, MenuButton);
	CC_SYNTHESIZE_RETAIN(Sprite*, m__sfBackGround, SpriteBackGround);
	CC_SYNTHESIZE_RETAIN(Scale9Sprite*, m__s9BackGround, Sprite9BackGround);
	CC_SYNTHESIZE_RETAIN(LabelTTF*, m__ltTitle, LabelTitle);
	CC_SYNTHESIZE_RETAIN(LabelTTF*, m__ltContentText, LabelContentText);

	CC_SYNTHESIZE_RETAIN(Sprite *, m__pTitle, ImgTitle);

};

#endif // !POP_LAYER_H_H
