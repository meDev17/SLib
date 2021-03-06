/*
 *   Copyright (c) 2008-2018 SLIBIO <https://github.com/SLIBIO>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */

#include "slib/core/definition.h"

#if defined(SLIB_UI_IS_IOS)

#include "slib/ui/edit_view.h"

#include "view_ios.h"

@interface _priv_Slib_iOS_TextField : UITextField<UITextFieldDelegate> {
	
	@public slib::WeakRef<slib::iOS_ViewInstance> m_viewInstance;
	
}

@end

@interface _priv_Slib_iOS_TextArea : UITextView<UITextViewDelegate> {
	
	@public slib::WeakRef<slib::iOS_ViewInstance> m_viewInstance;
	
}

@property (nonatomic, retain) NSString *placeholder;
@property (nonatomic, retain) UIColor *placeholderColor;
@property (nonatomic, retain) UILabel *placeHolderLabel;

-(void) refreshPlaceholder;

@end

namespace slib
{
	class EditView_Impl : public EditView
	{
	public:
		static ::UIReturnKeyType convertReturnKeyType(UIReturnKeyType type)
		{
			switch (type) {
				case UIReturnKeyType::Search:
					return UIReturnKeySearch;
				case UIReturnKeyType::Next:
					return UIReturnKeyNext;
				case UIReturnKeyType::Continue:
					return UIReturnKeyContinue;
				case UIReturnKeyType::Go:
					return UIReturnKeyGo;
				case UIReturnKeyType::Send:
					return UIReturnKeySend;
				case UIReturnKeyType::Join:
					return UIReturnKeyJoin;
				case UIReturnKeyType::Route:
					return UIReturnKeyRoute;
				case UIReturnKeyType::EmergencyCall:
					return UIReturnKeyEmergencyCall;
				case UIReturnKeyType::Google:
					return UIReturnKeyGoogle;
				case UIReturnKeyType::Yahoo:
					return UIReturnKeyYahoo;
				case UIReturnKeyType::Return:
					return UIReturnKeyDefault;
				case UIReturnKeyType::Default:
				case UIReturnKeyType::Done:
				default:
					return UIReturnKeyDone;
			}
		}
		
		static ::UIKeyboardType convertKeyboardType(UIKeyboardType type)
		{
			switch (type) {
				case UIKeyboardType::Numpad:
					return UIKeyboardTypeNumberPad;
				case UIKeyboardType::Phone:
					return UIKeyboardTypePhonePad;
				case UIKeyboardType::Email:
					return UIKeyboardTypeEmailAddress;
				case UIKeyboardType::Alphabet:
					return UIKeyboardTypeAlphabet;
				case UIKeyboardType::Url:
					return UIKeyboardTypeURL;
				case UIKeyboardType::WebSearch:
					return UIKeyboardTypeWebSearch;
				case UIKeyboardType::Twitter:
					return UIKeyboardTypeTwitter;
				case UIKeyboardType::NumbersAndPunctuation:
					return UIKeyboardTypeNumbersAndPunctuation;
				case UIKeyboardType::NamePhone:
					return UIKeyboardTypeNamePhonePad;
				case UIKeyboardType::Ascii:
					return UIKeyboardTypeASCIICapable;
				case UIKeyboardType::AsciiNumpad:
					return UIKeyboardTypeASCIICapableNumberPad;
				case UIKeyboardType::Default:
				default:
					return UIKeyboardTypeDefault;
			}
		}
		
		static ::UITextAutocapitalizationType convertAutoCapitalizationType(UIAutoCapitalizationType type)
		{
			switch (type) {
				case UIAutoCapitalizationType::None :
					return UITextAutocapitalizationTypeNone;
				case UIAutoCapitalizationType::Words:
					return UITextAutocapitalizationTypeWords;
				case UIAutoCapitalizationType::Sentences:
					return UITextAutocapitalizationTypeSentences;
				case UIAutoCapitalizationType::AllCharacters:
					return UITextAutocapitalizationTypeAllCharacters;
				default:
					return UITextAutocapitalizationTypeSentences;
			}
		}
	
		void applyPlaceholder(UITextField* handle)
		{
			NSAttributedString* attr;
			String _text = m_hintText;
			if (_text.isEmpty()) {
				attr = nil;
			} else {
				NSString* text = Apple::getNSStringFromString(_text);
				UIColor* color = GraphicsPlatform::getUIColorFromColor(m_hintTextColor);
				NSMutableParagraphStyle* paragraphStyle = [[NSMutableParagraphStyle alloc] init];
				[paragraphStyle setAlignment:translateAlignment(m_textAlignment)];
				UIFont* hFont = handle.font;
				if (hFont != nil) {
					attr = [[NSAttributedString alloc] initWithString:text attributes:@{NSForegroundColorAttributeName: color, NSParagraphStyleAttributeName: paragraphStyle, NSFontAttributeName: hFont}];
				} else {
					attr = [[NSAttributedString alloc] initWithString:text attributes:@{NSForegroundColorAttributeName: color, NSParagraphStyleAttributeName: paragraphStyle}];
				}
			}
			[handle setAttributedPlaceholder: attr];
		}
		
		void applyProperties(UITextField* handle)
		{
			[handle setText:(Apple::getNSStringFromString(m_text))];
			[handle setTextAlignment:translateAlignment(m_textAlignment)];
			[handle setBorderStyle:(isBorder()?UITextBorderStyleRoundedRect:UITextBorderStyleNone)];
			applyPlaceholder(handle);
			[handle setTextColor:(GraphicsPlatform::getUIColorFromColor(m_textColor))];
			[handle setBackgroundColor:(GraphicsPlatform::getUIColorFromColor(getBackgroundColor()))];
			[handle setEnabled:(m_flagReadOnly ? NO : YES)];
			[handle setSecureTextEntry:(m_flagPassword ? YES : NO)];
			Ref<Font> font = getFont();
			UIFont* hFont = GraphicsPlatform::getUIFont(font.get(), UIPlatform::getGlobalScaleFactor());
			if (hFont != nil) {
				[handle setFont:hFont];
			}
			[handle setReturnKeyType:convertReturnKeyType(m_returnKeyType)];
			[handle setKeyboardType:convertKeyboardType(m_keyboardType)];
			[handle setAutocapitalizationType:convertAutoCapitalizationType(m_autoCapitalizationType)];
		}
		
		void applyProperties(_priv_Slib_iOS_TextArea* handle)
		{
			[handle setText:(Apple::getNSStringFromString(m_text))];
			[handle setTextAlignment:translateAlignment(m_textAlignment)];
			if (isBorder()) {
				[handle.layer setBorderColor:([[UIColor grayColor] CGColor])];
				[handle.layer setBorderWidth:1];
			} else {
				[handle.layer setBorderWidth:0];
			}
			[handle setTextColor:(GraphicsPlatform::getUIColorFromColor(m_textColor))];
			[handle setBackgroundColor:(GraphicsPlatform::getUIColorFromColor(getBackgroundColor()))];
			[handle setEditable:(m_flagReadOnly?FALSE:TRUE)];
			[handle setPlaceholder:(Apple::getNSStringFromString(m_hintText))];
			[handle setPlaceholderColor:(GraphicsPlatform::getUIColorFromColor(m_hintTextColor))];
			[handle refreshPlaceholder];
			[handle setSelectable:TRUE];
			Ref<Font> font = getFont();
			UIFont* hFont = GraphicsPlatform::getUIFont(font.get(), UIPlatform::getGlobalScaleFactor());
			if (hFont != nil) {
				[handle setFont:hFont];
			}
			[handle setReturnKeyType:convertReturnKeyType(m_returnKeyType)];
			[handle setKeyboardType:convertKeyboardType(m_keyboardType)];
			[handle setAutocapitalizationType:convertAutoCapitalizationType(m_autoCapitalizationType)];
		}
		
		static NSTextAlignment translateAlignment(Alignment _align)
		{
			Alignment align = _align & Alignment::HorizontalMask;
			if (align == Alignment::Center) {
				return NSTextAlignmentCenter;
			} else if (align == Alignment::Right) {
				return NSTextAlignmentRight;
			}
			return NSTextAlignmentLeft;
		}
		
		static void onChangeText(iOS_ViewInstance* instance, UITextField* field, UITextView* area)
		{
			Ref<View> _view = instance->getView();
			if (EditView_Impl* view = CastInstance<EditView_Impl>(_view.get())) {
				String text;
				if (field != nil) {
					text = Apple::getStringFromNSString([field text]);
				} else if (area != nil) {
					text = Apple::getStringFromNSString([area text]);
				}
				String textNew = text;
				view->dispatchChange(&textNew);
				if (text != textNew) {
					NSString* str = Apple::getNSStringFromString(textNew);
					if (field != nil) {
						[field setText:str];
					}
					if (area != nil) {
						[area setText:str];
					}
				}
			}
		}
		
		static void onEnterAction(iOS_ViewInstance* instance, UITextField* field, UITextView* area)
		{
			Ref<View> _view = instance->getView();
			if (EditView_Impl* view = CastInstance<EditView_Impl>(_view.get())) {
				view->dispatchReturnKey();
				if (view->isAutoDismissKeyboard()) {
					if (field != nil) {
						[field resignFirstResponder];
					}
					if (area != nil) {
						[area resignFirstResponder];
					}
				}
			}
		}

	};
	
	Ref<ViewInstance> EditView::createNativeWidget(ViewInstance* _parent)
	{
		IOS_VIEW_CREATE_INSTANCE_BEGIN
		_priv_Slib_iOS_TextField* handle = [[_priv_Slib_iOS_TextField alloc] initWithFrame:frame];
		
		if (handle != nil) {
			((EditView_Impl*)this)->applyProperties(handle);
		}
		IOS_VIEW_CREATE_INSTANCE_END
		return ret;
	}
	
	Ref<ViewInstance> TextArea::createNativeWidget(ViewInstance* _parent)
	{
		IOS_VIEW_CREATE_INSTANCE_BEGIN
		_priv_Slib_iOS_TextArea* handle = [[_priv_Slib_iOS_TextArea alloc] initWithFrame:frame];
		if (handle != nil) {
			((EditView_Impl*)this)->applyProperties(handle);
		}
		IOS_VIEW_CREATE_INSTANCE_END
		return ret;
	}
	
	
	void EditView::_getText_NW()
	{
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				NSString* s = [tv text];
				m_text = Apple::getStringFromNSString(s);
			} else if ([handle isKindOfClass:[UITextView class]]) {
				UITextView* tv = (UITextView*)handle;
				NSString* s = [tv text];
				m_text = Apple::getStringFromNSString(s);
			}
		}
	}
	
	void EditView::_setText_NW(const String& value)
	{
		if (!(isUiThread())) {
			dispatchToUiThread(SLIB_BIND_WEAKREF(void(), EditView, _setText_NW, this, value));
			return;
		}
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				[tv setText:(Apple::getNSStringFromString(value))];
			} else if ([handle isKindOfClass:[UITextView class]]) {
				UITextView* tv = (UITextView*)handle;
				[tv setText:(Apple::getNSStringFromString(value))];
			}
		}
	}
	
	void EditView::_setBorder_NW(sl_bool flag)
	{
		if (!(isUiThread())) {
			dispatchToUiThread(SLIB_BIND_WEAKREF(void(), EditView, _setBorder_NW, this, flag));
			return;
		}
		UIView* handle = UIPlatform::getViewHandle(this);
		if (flag) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				[tv setBorderStyle:(flag?UITextBorderStyleRoundedRect:UITextBorderStyleNone)];
			} else if ([handle isKindOfClass:[UITextView class]]) {
				UITextView* tv = (UITextView*)handle;
				if (flag) {
					[tv.layer setBorderColor:([[UIColor grayColor] CGColor])];
					[tv.layer setBorderWidth:1];
				} else {
					[tv.layer setBorderWidth:0];
				}
			}
		}
	}
	
	void EditView::_setTextAlignment_NW(Alignment align)
	{
		if (!(isUiThread())) {
			dispatchToUiThread(SLIB_BIND_WEAKREF(void(), EditView, _setTextAlignment_NW, this, align));
			return;
		}
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				[tv setTextAlignment:EditView_Impl::translateAlignment(align)];
				((EditView_Impl*)this)->applyPlaceholder(tv);
			} else if ([handle isKindOfClass:[UITextView class]]) {
				UITextView* tv = (UITextView*)handle;
				[tv setTextAlignment:EditView_Impl::translateAlignment(align)];
			}
		}
	}
	
	void EditView::_setHintText_NW(const String& value)
	{
		if (!(isUiThread())) {
			dispatchToUiThread(SLIB_BIND_WEAKREF(void(), EditView, _setHintText_NW, this, value));
			return;
		}
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				((EditView_Impl*)this)->applyPlaceholder(tv);
			} else if ([handle isKindOfClass:[_priv_Slib_iOS_TextArea class]]) {
				_priv_Slib_iOS_TextArea* tv = (_priv_Slib_iOS_TextArea*)handle;
				NSString* s = Apple::getNSStringFromString(value);
				[tv setPlaceholder:s];
				[tv refreshPlaceholder];
			}
		}
	}
	
	void EditView::_setHintTextColor_NW(const Color& value)
	{
		if (!(isUiThread())) {
			dispatchToUiThread(SLIB_BIND_WEAKREF(void(), EditView, _setHintTextColor_NW, this, value));
			return;
		}
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				((EditView_Impl*)this)->applyPlaceholder(tv);
			} else if ([handle isKindOfClass:[_priv_Slib_iOS_TextArea class]]) {
				_priv_Slib_iOS_TextArea* tv = (_priv_Slib_iOS_TextArea*)handle;
				[tv setPlaceholderColor:(GraphicsPlatform::getUIColorFromColor(value))];
				[tv refreshPlaceholder];
			}
		}
	}

	void EditView::_setReadOnly_NW(sl_bool flag)
	{
		if (!(isUiThread())) {
			dispatchToUiThread(SLIB_BIND_WEAKREF(void(), EditView, _setReadOnly_NW, this, flag));
			return;
		}
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				[tv setEnabled:(flag?NO:YES)];
			} else if ([handle isKindOfClass:[UITextView class]]) {
				UITextView* tv = (UITextView*)handle;
				[tv setEditable:(flag?NO:YES)];
			}
		}
	}
	
	void EditView::_setPassword_NW(sl_bool flag)
	{
		if (!(isUiThread())) {
			dispatchToUiThread(SLIB_BIND_WEAKREF(void(), EditView, _setPassword_NW, this, flag));
			return;
		}
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				[tv setSecureTextEntry:(flag?YES:NO)];
			}
		}
	}
	
	void EditView::_setMultiLine_NW(sl_bool flag)
	{
	}
	
	void EditView::_setTextColor_NW(const Color& color)
	{
		if (!(isUiThread())) {
			dispatchToUiThread(SLIB_BIND_WEAKREF(void(), EditView, _setTextColor_NW, this, color));
			return;
		}
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				[tv setTextColor:(GraphicsPlatform::getUIColorFromColor(color))];
			} else if ([handle isKindOfClass:[UITextView class]]) {
				UITextView* tv = (UITextView*)handle;
				[tv setTextColor:(GraphicsPlatform::getUIColorFromColor(color))];
			}
		}
	}
	
	void EditView::_setBackgroundColor_NW(const Color& color)
	{
		if (!(isUiThread())) {
			dispatchToUiThread(SLIB_BIND_WEAKREF(void(), EditView, _setBackgroundColor_NW, this, color));
			return;
		}
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				[tv setBackgroundColor:(GraphicsPlatform::getUIColorFromColor(color))];
			} else if ([handle isKindOfClass:[UITextView class]]) {
				UITextView* tv = (UITextView*)handle;
				[tv setBackgroundColor:(GraphicsPlatform::getUIColorFromColor(color))];
			}
		}
	}
	
	void EditView::_setFont_NW(const Ref<Font>& font)
	{
		if (!(isUiThread())) {
			dispatchToUiThread(SLIB_BIND_WEAKREF(void(), EditView, _setFont_NW, this, font));
			return;
		}
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				UIFont* hFont = GraphicsPlatform::getUIFont(font.get(), UIPlatform::getGlobalScaleFactor());
				if (hFont != nil) {
					[tv setFont:hFont];
					((EditView_Impl*)this)->applyPlaceholder(tv);
				}
			} else if ([handle isKindOfClass:[UITextView class]]) {
				UITextView* tv = (UITextView*)handle;
				UIFont* hFont = GraphicsPlatform::getUIFont(font.get(), UIPlatform::getGlobalScaleFactor());
				if (hFont != nil) {
					[tv setFont:hFont];
				}
			}
		}
	}
	
	void EditView::_setReturnKeyType_NW(UIReturnKeyType type)
	{
		if (!(isUiThread())) {
			dispatchToUiThread(SLIB_BIND_WEAKREF(void(), EditView, _setReturnKeyType_NW, this, type));
			return;
		}
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				[tv setReturnKeyType:EditView_Impl::convertReturnKeyType(type)];
			} else if ([handle isKindOfClass:[UITextView class]]) {
				UITextView* tv = (UITextView*)handle;
				[tv setReturnKeyType:EditView_Impl::convertReturnKeyType(type)];
			}
		}
	}
	
	void EditView::_setKeyboardType_NW(UIKeyboardType type)
	{
		if (!(isUiThread())) {
			dispatchToUiThread(SLIB_BIND_WEAKREF(void(), EditView, _setKeyboardType_NW, this, type));
			return;
		}
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				[tv setKeyboardType:EditView_Impl::convertKeyboardType(type)];
			} else if ([handle isKindOfClass:[UITextView class]]) {
				UITextView* tv = (UITextView*)handle;
				[tv setKeyboardType:EditView_Impl::convertKeyboardType(type)];
			}
		}
	}
	
	void EditView::_setAutoCapitalizationType_NW(UIAutoCapitalizationType type)
	{
		if (!(isUiThread())) {
			dispatchToUiThread(SLIB_BIND_WEAKREF(void(), EditView, _setAutoCapitalizationType_NW, this, type));
			return;
		}
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil) {
			if ([handle isKindOfClass:[UITextField class]]) {
				UITextField* tv = (UITextField*)handle;
				[tv setAutocapitalizationType:EditView_Impl::convertAutoCapitalizationType(type)];
			} else if ([handle isKindOfClass:[UITextView class]]) {
				UITextView* tv = (UITextView*)handle;
				[tv setAutocapitalizationType:EditView_Impl::convertAutoCapitalizationType(type)];
			}
		}
	}
}

@implementation _priv_Slib_iOS_TextField

IOS_VIEW_DEFINE_ON_FOCUS

-(id)initWithFrame:(CGRect)frame
{
	self = [super initWithFrame:frame];
	if (self != nil) {
		[self addTarget:self action:@selector(textFieldDidChange) forControlEvents:UIControlEventEditingChanged];
		[self setDelegate:self];
	}
	return self;
}

-(void)textFieldDidChange
{
	slib::Ref<slib::iOS_ViewInstance> instance = m_viewInstance;
	if (instance.isNotNull()) {
		slib::EditView_Impl::onChangeText(instance.get(), self, nil);
	}
}

- (void)textFieldDidEndEditing:(UITextField *)textField
{
}

-(BOOL) textFieldShouldReturn:(UITextField *)textField{
	slib::Ref<slib::iOS_ViewInstance> instance = m_viewInstance;
	if (instance.isNotNull()) {
		slib::EditView_Impl::onEnterAction(instance.get(), self, nil);
	}
	return NO;
}

@end

@implementation _priv_Slib_iOS_TextArea

IOS_VIEW_DEFINE_ON_FOCUS

-(id)initWithFrame:(CGRect)frame
{
	self = [super initWithFrame:frame];
	if (self != nil) {
		[self setScrollEnabled:TRUE];
		[self setDelegate:self];
		[self setPlaceholder:@""];
		[self setPlaceholderColor:[UIColor lightGrayColor]];
	}
	return self;
}

-(void) refreshPlaceholder {
	if( [[self placeholder] length] > 0 ) {
		if (_placeHolderLabel == nil ) {
			_placeHolderLabel = [[UILabel alloc] initWithFrame:CGRectMake(8,8,self.bounds.size.width - 16,0)];
			[_placeHolderLabel setTextAlignment:self.textAlignment];
			_placeHolderLabel.lineBreakMode = NSLineBreakByWordWrapping;
			_placeHolderLabel.numberOfLines = 0;
			_placeHolderLabel.font = self.font;
			_placeHolderLabel.backgroundColor = [UIColor clearColor];
			_placeHolderLabel.textColor = self.placeholderColor;
			_placeHolderLabel.alpha = 0;
			[self addSubview:_placeHolderLabel];
		} else {
			[_placeHolderLabel setFrame:CGRectMake(8,8,self.bounds.size.width - 16,0)];
		}
		
		_placeHolderLabel.text = self.placeholder;
		[_placeHolderLabel sizeToFit];
		[_placeHolderLabel setFrame:CGRectMake(8,8,self.bounds.size.width - 16,_placeHolderLabel.frame.size.height)];
		[self sendSubviewToBack:_placeHolderLabel];
	}
	
	if( [[self text] length] == 0 && [[self placeholder] length] > 0 ) {
		[[self placeHolderLabel] setAlpha:1.f];
	} else {
		[[self placeHolderLabel] setAlpha:0.f];
	}
}

-(void)textViewDidChange:(UITextView *)textView
{
	slib::Ref<slib::iOS_ViewInstance> instance = m_viewInstance;
	if (instance.isNotNull()) {
		slib::EditView_Impl::onChangeText(instance.get(), nil, self);
		
		if([[self placeholder] length] > 0) {
			[UIView animateWithDuration:0.25f animations:^{
				if([[self text] length] == 0) {
					[[self placeHolderLabel] setAlpha:1.f];
				} else {
					[[self placeHolderLabel] setAlpha:0.f];
				}
			}];
		}
	}
}

- (void)insertText:(NSString *)text
{
	[super insertText:text];
	slib::Ref<slib::iOS_ViewInstance> instance = m_viewInstance;
	if (instance.isNotNull()) {
		if ([text isEqualToString:@"\n"]) {
			slib::EditView_Impl::onEnterAction(instance.get(), nil, self);
		}
	}
}

- (void)drawRect:(CGRect)rect
{
	[self refreshPlaceholder];
	[super drawRect:rect];
}

@end

#endif
