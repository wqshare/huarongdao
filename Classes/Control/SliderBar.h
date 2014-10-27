#ifndef __shaderTest__SliderBar__
#define __shaderTest__SliderBar__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;
class SliderBar:public Node{
public:
	/* 创建对象 参数：滑动条资源 背景资源 背景大小 滑动条大小比率 */
	static SliderBar * createSliderBar(const char * sliderFileName,const char * backgroundFileName,CCSize bgSize,float multiple);
	/* 初始化方法 */
	bool init(const char * sliderFileName,const char * backgroundFileName,CCSize bgSize,float multiple);
	SliderBar(); //构造函数
	~SliderBar(); //析构函数
	/* 设置滚动条精灵在其背景精灵上位置的百分比 区间[0,1] 当超过这个范围时进行缩放 */
	void setValue(float value);
public:
	/* 创建成员变量及get、set方法 */
	CC_SYNTHESIZE(Scale9Sprite *, m_sliderSprite, SliderSprite);
	CC_SYNTHESIZE(Scale9Sprite *, m_backgroundSprite, BackgroundSprite);
	/* 滚动条精灵初始位置 */
	CCPoint _beginPos;
	/* 滚动条精灵最终位置 */
	CCPoint _endPos;
	/* 滚动条背景精灵大小 */
	CCSize _bgSize;
	/* 滚动条精灵大小 */
	CCSize _sdSize;
};
#endif /* defined(__shaderTest__SliderBar__) */