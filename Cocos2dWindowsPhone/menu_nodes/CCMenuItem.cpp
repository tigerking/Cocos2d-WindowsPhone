/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org
Copyright (c) 2008-2011 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "pch.h"
#include <cstring>
#include "CCMenuItem.h"
#include "CCPointExtension.h"
#include "CCActionInterval.h"
#include "CCSprite.h"
#include "CCLabelAtlas.h"
#include "CCLabelTTF.h"
#include "CCScriptSupport.h"
#include <stdarg.h>

NS_CC_BEGIN

	static unsigned int _fontSize = kCCItemSize;
static std::string _fontName = "Marker Felt.ttf";
static bool _fontNameRelease = false;

const unsigned int	kCurrentItem = 0xc0c05001;
const unsigned int	kZoomActionTag = 0xc0c05002;

const unsigned int	kNormalTag = 0x1;
const unsigned int	kSelectedTag = 0x2;
const unsigned int	kDisableTag = 0x3;
//
// CCMenuItem
//
CCMenuItem * CCMenuItem::create(CCObject *rec, SEL_MenuHandler selector)
{
	CCMenuItem *pRet = new CCMenuItem();
	pRet->initWithTarget(rec, selector);
	pRet->autorelease();
	return pRet;
}
bool CCMenuItem::initWithTarget(CCObject *rec, SEL_MenuHandler selector)
{
	setAnchorPoint(ccp(0.5f, 0.5f));
	m_pListener = rec;
	m_pfnSelector = selector;
	m_bIsEnabled = true;
	m_bIsSelected = false;
	return true;
}

CCMenuItem::~CCMenuItem()
{
	unregisterScriptHandler();
}

void CCMenuItem::selected()
{
	m_bIsSelected = true;
}

void CCMenuItem::unselected()
{
	m_bIsSelected = false;
}

void CCMenuItem::registerScriptTapHandler(int nHandler)
{
    unregisterScriptTapHandler();
    m_nScriptTapHandler = nHandler;
    LUALOG("[LUA] Add CCMenuItem script handler: %d", m_nScriptTapHandler);
}

void CCMenuItem::unregisterScriptTapHandler(void)
{
    if (m_nScriptTapHandler)
    {
        //CCScriptEngineManager::sharedManager()->getScriptEngine()->removeScriptHandler(m_nScriptTapHandler);
        //LUALOG("[LUA] Remove CCMenuItem script handler: %d", m_nScriptTapHandler);
        m_nScriptTapHandler = 0;
    }
}

void CCMenuItem::activate()
{
	if (m_bIsEnabled)
	{
		if (m_pListener && m_pfnSelector)
		{
			(m_pListener->*m_pfnSelector)(this);
		}

		if (m_nScriptHandler)
		{
            CCScriptEngineManager::sharedManager()->getScriptEngine()->executeMenuItemEvent(this);
		}
	}
}

void CCMenuItem::setEnabled(bool enabled)
{
    m_bIsEnabled = enabled;
}

bool CCMenuItem::isEnabled()
{
    return m_bIsEnabled;
}

CCRect CCMenuItem::rect()
{
	return CCRectMake( m_obPosition.x - m_tContentSize.width * m_tAnchorPoint.x, 
		m_obPosition.y - m_tContentSize.height * m_tAnchorPoint.y,
		m_tContentSize.width, m_tContentSize.height);
}

bool CCMenuItem::isSelected()
{
    return m_bIsSelected;
}

void CCMenuItem::setTarget(CCObject *rec, SEL_MenuHandler selector)
{
	m_pListener = rec;
	m_pfnSelector = selector;
}

//
//CCMenuItemLabel
//
const ccColor3B& CCMenuItemLabel::getDisabledColor()
{
	return m_tDisabledColor;
}
void CCMenuItemLabel::setDisabledColor(const ccColor3B& var)
{
	m_tDisabledColor = var;
}
CCNode *CCMenuItemLabel::getLabel()
{
	return m_pLabel;
}
void CCMenuItemLabel::setLabel(CCNode* var)
{
	if (var)
	{
		addChild(var);
		var->setAnchorPoint(ccp(0, 0));
		setContentSize(var->getContentSize());
	}

	if (m_pLabel)
	{
		removeChild(m_pLabel, true);
	}

	m_pLabel = var;
}
CCMenuItemLabel * CCMenuItemLabel::create(CCNode*label, CCObject* target, SEL_MenuHandler selector)
{
	CCMenuItemLabel *pRet = new CCMenuItemLabel();
	pRet->initWithLabel(label, target, selector);
	pRet->autorelease();
	return pRet;
}
CCMenuItemLabel* CCMenuItemLabel::create(CCNode *label)
{
	CCMenuItemLabel *pRet = new CCMenuItemLabel();
	pRet->initWithLabel(label, NULL, NULL);
	pRet->autorelease();
	return pRet;
}
bool CCMenuItemLabel::initWithLabel(CCNode* label, CCObject* target, SEL_MenuHandler selector)
{
	CCMenuItem::initWithTarget(target, selector);
	m_fOriginalScale = 1.0f;
	m_tColorBackup = ccWHITE;
	m_tDisabledColor = ccc3(126,126,126);
	this->setLabel(label);
	return true;
}
CCMenuItemLabel::~CCMenuItemLabel()
{
}
void CCMenuItemLabel::setString(const char * label)
{
	dynamic_cast<CCLabelProtocol*>(m_pLabel)->setString(label);
	this->setContentSize(m_pLabel->getContentSize());
}
void CCMenuItemLabel::activate()
{
	if(m_bIsEnabled)
	{
		this->stopAllActions();
		this->setScale( m_fOriginalScale );
		CCMenuItem::activate();
	}
}
void CCMenuItemLabel::selected()
{
	// subclass to change the default action
	if(m_bIsEnabled)
	{
		CCMenuItem::selected();

		CCAction *action = getActionByTag(kZoomActionTag);
		if (action)
		{
			this->stopAction(action);
		}
		else
		{
			m_fOriginalScale = this->getScale();
		}

		CCAction *zoomAction = CCScaleTo::create(0.1f, m_fOriginalScale * 1.2f);
		zoomAction->setTag(kZoomActionTag);
		this->runAction(zoomAction);
	}
}
void CCMenuItemLabel::unselected()
{
	// subclass to change the default action
	if(m_bIsEnabled)
	{
		CCMenuItem::unselected();
		this->stopActionByTag(kZoomActionTag);
		CCAction *zoomAction = CCScaleTo::create(0.1f, m_fOriginalScale);
		zoomAction->setTag(kZoomActionTag);
		this->runAction(zoomAction);
	}
}
void CCMenuItemLabel::setIsEnabled(bool enabled)
{
	if( m_bIsEnabled != enabled ) 
	{
		if(enabled == false)
		{
			m_tColorBackup = dynamic_cast<CCRGBAProtocol*>(m_pLabel)->getColor();
			dynamic_cast<CCRGBAProtocol*>(m_pLabel)->setColor(m_tDisabledColor);
		}
		else
		{
			dynamic_cast<CCRGBAProtocol*>(m_pLabel)->setColor(m_tColorBackup);
		}
	}
	CCMenuItem::setEnabled(enabled);
}
void CCMenuItemLabel::setOpacity(CCubyte opacity)
{
	dynamic_cast<CCRGBAProtocol*>(m_pLabel)->setOpacity(opacity);
}
CCubyte CCMenuItemLabel::getOpacity()
{
	return dynamic_cast<CCRGBAProtocol*>(m_pLabel)->getOpacity();
}
void CCMenuItemLabel::setColor(const ccColor3B& color)
{
	dynamic_cast<CCRGBAProtocol*>(m_pLabel)->setColor(color);
}
const ccColor3B& CCMenuItemLabel::getColor()
{
	return dynamic_cast<CCRGBAProtocol*>(m_pLabel)->getColor();
}

//
//CCMenuItemAtlasFont
//
CCMenuItemAtlasFont * CCMenuItemAtlasFont::create(const char *value, const char *charMapFile, int itemWidth, int itemHeight, char startCharMap)
{
	return CCMenuItemAtlasFont::create(value, charMapFile, itemWidth, itemHeight, startCharMap, NULL, NULL);
}

CCMenuItemAtlasFont * CCMenuItemAtlasFont::create(const char *value, const char *charMapFile, int itemWidth, int itemHeight, char startCharMap, CCObject* target, SEL_MenuHandler selector)
{
	CCMenuItemAtlasFont *pRet = new CCMenuItemAtlasFont();
	pRet->initFromString(value, charMapFile, itemWidth, itemHeight, startCharMap, target, selector);
	pRet->autorelease();
	return pRet;
}
bool CCMenuItemAtlasFont::initFromString(const char *value, const char *charMapFile, int itemWidth, int itemHeight, char startCharMap, CCObject* target, SEL_MenuHandler selector)
{
	CCAssert( value != NULL && strlen(value) != 0, "value length must be greater than 0");
	CCLabelAtlas *label = new CCLabelAtlas();
	label->initWithString(value, charMapFile, itemWidth, itemHeight, startCharMap);
	label->autorelease();
	if (CCMenuItemLabel::initWithLabel(label, target, selector))
	{
		// do something ?
	}
	return true;
}
//
//CCMenuItemFont
//
void CCMenuItemFont::setFontSize(unsigned int s)
{
	_fontSize = s;
}
unsigned int CCMenuItemFont::fontSize()
{
	return _fontSize;
}
void CCMenuItemFont::setFontName(const char *name)
{
	if( _fontNameRelease )
	{
		_fontName.clear();
	}
	_fontName = name;
	_fontNameRelease = true;
}
const char * CCMenuItemFont::fontName()
{
	return _fontName.c_str();
}
CCMenuItemFont * CCMenuItemFont::create(const char *value, CCObject* target, SEL_MenuHandler selector)
{
	CCMenuItemFont *pRet = new CCMenuItemFont();
	pRet->initFromString(value, target, selector);
	pRet->autorelease();
	return pRet;
}
CCMenuItemFont * CCMenuItemFont::create(const char *value)
{
	CCMenuItemFont *pRet = new CCMenuItemFont();
	pRet->initFromString(value, NULL, NULL);
	pRet->autorelease();
	return pRet;
}
bool CCMenuItemFont::initFromString(const char *value, CCObject* target, SEL_MenuHandler selector)
{
	CCAssert( value != NULL && strlen(value) != 0, "Value length must be greater than 0");

	m_strFontName = _fontName;
	m_uFontSize = _fontSize;

	CCLabelTTF *label = CCLabelTTF::create(value, m_strFontName.c_str(), (float)m_uFontSize);
	if (CCMenuItemLabel::initWithLabel(label, target, selector))
	{
		// do something ?
	}
	return true;
}

void CCMenuItemFont::recreateLabel()
{
	CCLabelTTF *label = CCLabelTTF::create(dynamic_cast<CCLabelProtocol*>(m_pLabel)->getString(), 
		m_strFontName.c_str(), (float)m_uFontSize);
	this->setLabel(label);
}

void CCMenuItemFont::setFontSizeObj(unsigned int s)
{
	m_uFontSize = s;
	recreateLabel();
}

unsigned int CCMenuItemFont::fontSizeObj()
{
	return m_uFontSize;
}

void CCMenuItemFont::setFontNameObj(const char* name)
{
	m_strFontName = name;
	recreateLabel();
}

const char* CCMenuItemFont::fontNameObj()
{
	return m_strFontName.c_str();
}

//
//CCMenuItemSprite
//
CCNode * CCMenuItemSprite::getNormalImage()
{
	return m_pNormalImage;
}
void CCMenuItemSprite::setNormalImage(CCNode* var)
{
	if (var)
	{
		addChild(var, 0, kNormalTag);
		var->setAnchorPoint(ccp(0, 0));
		var->setVisible(true);
	}

	if (m_pNormalImage)
	{
		removeChild(m_pNormalImage, true);
	}

	m_pNormalImage = var;
}
CCNode * CCMenuItemSprite::getSelectedImage()
{
	return m_pSelectedImage;
}
void CCMenuItemSprite::setSelectedImage(CCNode* var)
{
	if (var)
	{
		addChild(var, 0, kSelectedTag);
		var->setAnchorPoint(ccp(0, 0));
		var->setVisible(false);
	}

	if (m_pSelectedImage)
	{
		removeChild(m_pSelectedImage, true);
	}

	m_pSelectedImage = var;
}
CCNode * CCMenuItemSprite::getDisabledImage()
{
	return m_pDisabledImage;
}
void CCMenuItemSprite::setDisabledImage(CCNode* var)
{
	if (var)
	{
		addChild(var, 0, kDisableTag);
		var->setAnchorPoint(ccp(0, 0));
		var->setVisible(false);
	}

	if (m_pDisabledImage)
	{
		removeChild(m_pDisabledImage, true);
	}

	m_pDisabledImage = var;
}
//
//CCMenuItemSprite - CCRGBAProtocol protocol
//
void CCMenuItemSprite::setOpacity(CCubyte opacity)
{
	dynamic_cast<CCRGBAProtocol*>(m_pNormalImage)->setOpacity(opacity);

	if (m_pSelectedImage)
	{
		dynamic_cast<CCRGBAProtocol*>(m_pSelectedImage)->setOpacity(opacity);
	}

	if (m_pDisabledImage)
	{
		dynamic_cast<CCRGBAProtocol*>(m_pDisabledImage)->setOpacity(opacity);
	}
}
void CCMenuItemSprite::setColor(const ccColor3B& color)
{
	dynamic_cast<CCRGBAProtocol*>(m_pNormalImage)->setColor(color);

	if (m_pSelectedImage)
	{
		dynamic_cast<CCRGBAProtocol*>(m_pSelectedImage)->setColor(color);
	}    

	if (m_pDisabledImage)
	{
		dynamic_cast<CCRGBAProtocol*>(m_pDisabledImage)->setColor(color);
	}
}
CCubyte CCMenuItemSprite::getOpacity()
{
	return dynamic_cast<CCRGBAProtocol*>(m_pNormalImage)->getOpacity();
}
const ccColor3B& CCMenuItemSprite::getColor()
{
	return dynamic_cast<CCRGBAProtocol*>(m_pNormalImage)->getColor();
}
CCMenuItemSprite * CCMenuItemSprite::create(CCNode* normalSprite, CCNode* selectedSprite, CCNode* disabledSprite)
{
	return CCMenuItemSprite::create(normalSprite, selectedSprite, disabledSprite, NULL, NULL);
}
CCMenuItemSprite * CCMenuItemSprite::create(CCNode* normalSprite, CCNode* selectedSprite, CCObject* target, SEL_MenuHandler selector)
{
	return CCMenuItemSprite::create(normalSprite, selectedSprite, NULL, target, selector);
}
CCMenuItemSprite * CCMenuItemSprite::create(CCNode *normalSprite, CCNode *selectedSprite, CCNode *disabledSprite, CCObject *target, SEL_MenuHandler selector)
{
	CCMenuItemSprite *pRet = new CCMenuItemSprite();
	pRet->initWithNormalSprite(normalSprite, selectedSprite, disabledSprite, target, selector); 
	pRet->autorelease();
	return pRet;
}
bool CCMenuItemSprite::initWithNormalSprite(CCNode* normalSprite, CCNode* selectedSprite, CCNode* disabledSprite, CCObject* target, SEL_MenuHandler selector)
{
	CCAssert(normalSprite != NULL, "");
	CCMenuItem::initWithTarget(target, selector); 
	setNormalImage(normalSprite);
	setSelectedImage(selectedSprite);
	setDisabledImage(disabledSprite);

	this->setContentSize(m_pNormalImage->getContentSize());
	return true;
}

/**
@since v0.99.5
*/
void CCMenuItemSprite::selected()
{
	CCMenuItem::selected();

	if (m_pDisabledImage)
	{
		m_pDisabledImage->setVisible(false);
	}

	if (m_pSelectedImage)
	{
		m_pNormalImage->setVisible(false);
		m_pSelectedImage->setVisible(true);
	}
	else
	{
		m_pNormalImage->setVisible(true);
	}
}

void CCMenuItemSprite::unselected()
{
	CCMenuItem::unselected();

	m_pNormalImage->setVisible(true);

	if (m_pSelectedImage)
	{
		m_pSelectedImage->setVisible(false);
	}

	if (m_pDisabledImage)
	{
		m_pDisabledImage->setVisible(false);
	}
}

void CCMenuItemSprite::setIsEnabled(bool bEnabled)
{
	CCMenuItem::setEnabled(bEnabled);

	if (m_pSelectedImage)
	{
		m_pSelectedImage->setVisible(false);
	}

	if (bEnabled)
	{
		m_pNormalImage->setVisible(true);

		if (m_pDisabledImage)
		{
			m_pDisabledImage->setVisible(false);
		}
	}
	else
	{
		if (m_pDisabledImage)
		{
			m_pDisabledImage->setVisible(true);
			m_pNormalImage->setVisible(false);
		}
		else
		{
			m_pNormalImage->setVisible(true);
		}
	}
}

///
/// CCMenuItemImage
///

CCMenuItemImage* CCMenuItemImage::create()
{
    CCMenuItemImage *pRet = new CCMenuItemImage();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool CCMenuItemImage::init(void)
{
    return initWithNormalImage(NULL, NULL, NULL, NULL, NULL);
}


CCMenuItemImage * CCMenuItemImage::create(const char *normalImage, const char *selectedImage)
{
	return CCMenuItemImage::create(normalImage, selectedImage, NULL, NULL, NULL);
}
CCMenuItemImage * CCMenuItemImage::create(const char *normalImage, const char *selectedImage, CCObject* target, SEL_MenuHandler selector)
{
	return CCMenuItemImage::create(normalImage, selectedImage, NULL, target, selector);
}
CCMenuItemImage * CCMenuItemImage::create(const char *normalImage, const char *selectedImage, const char *disabledImage, CCObject* target, SEL_MenuHandler selector)
{
	CCMenuItemImage *pRet = new CCMenuItemImage();
	if (pRet && pRet->initWithNormalImage(normalImage, selectedImage, disabledImage, target, selector))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}
CCMenuItemImage * CCMenuItemImage::create(const char *normalImage, const char *selectedImage, const char *disabledImage)
{
	CCMenuItemImage *pRet = new CCMenuItemImage();
	if (pRet && pRet->initWithNormalImage(normalImage, selectedImage, disabledImage, NULL, NULL))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}
bool CCMenuItemImage::initWithNormalImage(const char *normalImage, const char *selectedImage, const char *disabledImage, CCObject* target, SEL_MenuHandler selector)
{
	CCNode *normalSprite = NULL;
	CCNode *selectedSprite = NULL;
	CCNode *disabledSprite = NULL;

	if(normalImage)
	{
		normalSprite = CCSprite::create(normalImage);
	}

	if (selectedImage)
	{
		selectedSprite = CCSprite::create(selectedImage);
	}

	if(disabledImage)
	{
		disabledSprite = CCSprite::create(disabledImage);
	}
	return initWithNormalSprite(normalSprite, selectedSprite, disabledSprite, target, selector);
}
//
// MenuItemToggle
//
void CCMenuItemToggle::setSubItems(CCArray* var)
{
	CC_SAFE_RETAIN(var);
	CC_SAFE_RELEASE(m_pSubItems);
	m_pSubItems = var;
}
CCArray *CCMenuItemToggle::getSubItems()
{
	return m_pSubItems;
}
CCMenuItemToggle * CCMenuItemToggle::createWithTarget(CCObject* target, SEL_MenuHandler selector, CCMenuItem* item, ...)
{
	va_list args;
	va_start(args, item);
	CCMenuItemToggle *pRet = new CCMenuItemToggle();
	pRet->initWithTarget(target, selector, item, args);
	pRet->autorelease();
	va_end(args);
	return pRet;
}
bool CCMenuItemToggle::initWithTarget(CCObject* target, SEL_MenuHandler selector, CCMenuItem* item, va_list args)
{
	CCMenuItem::initWithTarget(target, selector);
	this->m_pSubItems = new CCArray();
	int z = 0;
	CCMenuItem *i = item;
	while(i) 
	{
		z++;
		m_pSubItems->addObject(i);
		i = va_arg(args, CCMenuItem*);
	}
	m_uSelectedIndex = UINT_MAX;
	this->setSelectedIndex(0);
	return true;
}

CCMenuItemToggle* CCMenuItemToggle::itemWithItem(CCMenuItem *item)
{
	CCMenuItemToggle *pRet = new CCMenuItemToggle();
	pRet->initWithItem(item);
	pRet->autorelease();
	return pRet;
}

bool CCMenuItemToggle::initWithItem(CCMenuItem *item)
{
	CCMenuItem::initWithTarget(NULL, NULL);
	this->m_pSubItems = new CCArray();
	m_pSubItems->addObject(item);
	m_uSelectedIndex = UINT_MAX;
	this->setSelectedIndex(0);
	return true;
}

void CCMenuItemToggle::addSubItem(CCMenuItem *item)
{
	m_pSubItems->addObject(item);
}

CCMenuItemToggle::~CCMenuItemToggle()
{
	CC_SAFE_RELEASE(m_pSubItems);
}
void CCMenuItemToggle::setSelectedIndex(unsigned int index)
{
	if( index != m_uSelectedIndex )
	{
		m_uSelectedIndex = index;
		this->removeChildByTag(kCurrentItem, false);
		CCMenuItem *item = (CCMenuItem * )m_pSubItems->objectAtIndex(m_uSelectedIndex);
		this->addChild(item, 0, kCurrentItem);
		const CCSize& s = item->getContentSize();
		this->setContentSize(s);
		item->setPosition( ccp( s.width/2, s.height/2 ) );
	}
}
unsigned int CCMenuItemToggle::getSelectedIndex()
{
	return m_uSelectedIndex;
}
void CCMenuItemToggle::selected()
{
	CCMenuItem::selected();
	((CCMenuItem * )m_pSubItems->objectAtIndex(m_uSelectedIndex))->selected();
}
void CCMenuItemToggle::unselected()
{
	CCMenuItem::unselected();
	((CCMenuItem * )m_pSubItems->objectAtIndex(m_uSelectedIndex))->unselected();
}
void CCMenuItemToggle::activate()
{
	// update index
	if( m_bIsEnabled ) 
	{
		unsigned int newIndex = (m_uSelectedIndex + 1) % m_pSubItems->count();
		this->setSelectedIndex(newIndex);
	}
	CCMenuItem::activate();
}
void CCMenuItemToggle::setIsEnabled(bool enabled)
{
	CCMenuItem::setEnabled(enabled);

	if(m_pSubItems && m_pSubItems->count() > 0)
	{
		//CCArray::CCMutableArrayIterator it;
		//for( it = m_pSubItems->begin(); it != m_pSubItems->end(); ++it)
		//{
		//	(*it)->setEnabled(enabled);
		//}
		CCObject* pObj = NULL;
        CCARRAY_FOREACH(m_pSubItems, pObj)
		{
            CCMenuItem* pItem = (CCMenuItem*)pObj;
            pItem->setEnabled(enabled);
		}
	}
}
CCMenuItem * CCMenuItemToggle::selectedItem()
{
	return (CCMenuItem *)m_pSubItems->objectAtIndex(m_uSelectedIndex);
}

//
// Setter of sprite frames
//
void CCMenuItemImage::setNormalSpriteFrame(CCSpriteFrame * frame)
{
    setNormalImage(CCSprite::createWithSpriteFrame(frame));
}

void CCMenuItemImage::setSelectedSpriteFrame(CCSpriteFrame * frame)
{
    setSelectedImage(CCSprite::createWithSpriteFrame(frame));
}

void CCMenuItemImage::setDisabledSpriteFrame(CCSpriteFrame * frame)
{
    setDisabledImage(CCSprite::createWithSpriteFrame(frame));
}

//
//CCMenuItemToggle - CCRGBAProtocol protocol
//
CCubyte CCMenuItemToggle::getOpacity()
{
	return m_cOpacity;
}
void CCMenuItemToggle::setOpacity(CCubyte opacity)
{
	//m_cOpacity = opacity;
	//if(m_pSubItems && m_pSubItems->count() > 0)
	//{
	//	CCArray::CCMutableArrayIterator it;
	//	for( it = m_pSubItems->begin(); it != m_pSubItems->end(); ++it)
	//	{
	//		dynamic_cast<CCRGBAProtocol*>(*it)->setOpacity(opacity);
	//	}
	//}
	m_cOpacity = opacity;
    if(m_pSubItems && m_pSubItems->count() > 0)
    {
        CCObject* pObj = NULL;
        CCARRAY_FOREACH(m_pSubItems, pObj)
        {
            CCMenuItem* pItem = (CCMenuItem*)pObj;
            dynamic_cast<CCRGBAProtocol*>(pItem)->setOpacity(opacity);
        }
    }
}
const ccColor3B& CCMenuItemToggle::getColor()
{
	return m_tColor;
}
void CCMenuItemToggle::setColor(const ccColor3B& color)
{
	m_tColor = color;
	if(m_pSubItems && m_pSubItems->count() > 0)
	{
		//CCArray::CCMutableArrayIterator it;
		//for( it = m_pSubItems->begin(); it != m_pSubItems->end(); ++it)
		//{
		//	dynamic_cast<CCRGBAProtocol*>(*it)->setColor(color);
		//}
		CCObject* pObj = NULL;
        CCARRAY_FOREACH(m_pSubItems, pObj)
        {
            CCMenuItem* pItem = (CCMenuItem*)pObj;
            dynamic_cast<CCRGBAProtocol*>(pItem)->setColor(color);
        }
	}
}

NS_CC_END
