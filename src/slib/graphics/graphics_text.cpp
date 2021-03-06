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

#include "slib/graphics/text.h"

#include "slib/graphics/font_atlas.h"
#include "slib/graphics/util.h"
#include "slib/core/xml.h"
#include "slib/core/string_buffer.h"
#include "slib/math/calculator.h"
#include "slib/device/device.h"

namespace slib
{
	
	SLIB_DEFINE_CLASS_DEFAULT_MEMBERS(TextStyle)

	TextStyle::TextStyle() noexcept : flagUnderline(sl_false), flagOverline(sl_false), flagLineThrough(sl_false), textColor(Color::Zero), backgroundColor(Color::Zero), lineHeight(-1), yOffset(0)
	{
	}
	
	Ref<TextStyle> TextStyle::duplicate() const noexcept
	{
		return new TextStyle(*this);
	}
	
	SLIB_DEFINE_OBJECT(TextItem, Object)

	TextItem::TextItem(TextItemType type) noexcept
	 : m_type(type), m_layoutPosition(0, 0), m_layoutSize(0, 0)
	{
	}

	TextItem::~TextItem() noexcept
	{
	}

	TextItemType TextItem::getType() noexcept
	{
		return m_type;
	}
	
	Ref<TextStyle> TextItem::getStyle() noexcept
	{
		return m_style;
	}
	
	void TextItem::setStyle(const Ref<TextStyle>& style) noexcept
	{
		m_style = style;
	}
	
	Ref<Font> TextItem::getFont() noexcept
	{
		Ref<TextStyle> style = m_style;
		if (style.isNotNull()) {
			return style->font;
		}
		return sl_null;
	}

	Point TextItem::getLayoutPosition() noexcept
	{
		return m_layoutPosition;
	}

	void TextItem::setLayoutPosition(const Point& pt) noexcept
	{
		m_layoutPosition = pt;
	}

	Size TextItem::getLayoutSize() noexcept
	{
		return m_layoutSize;
	}

	void TextItem::setLayoutSize(const Size& size) noexcept
	{
		m_layoutSize = size;
	}

	Rectangle TextItem::getLayoutFrame() noexcept
	{
		return Rectangle(m_layoutPosition.x, m_layoutPosition.y, m_layoutPosition.x + m_layoutSize.x, m_layoutPosition.y + m_layoutSize.y);
	}


	SLIB_DEFINE_OBJECT(TextWordItem, TextItem)

	TextWordItem::TextWordItem() noexcept
	 : TextItem(TextItemType::Word)
	{
		m_widthCached = 0;
		m_heightCached = 0;
	}

	TextWordItem::~TextWordItem() noexcept
	{
	}

	Ref<TextWordItem> TextWordItem::create(const String16& text, const Ref<TextStyle>& style) noexcept
	{
		if (style.isNotNull()) {
			Ref<TextWordItem> ret = new TextWordItem;
			if (ret.isNotNull()) {
				ret->m_text = text;
				ret->m_style = style;
				return ret;
			}
		}
		return sl_null;
	}

	String16 TextWordItem::getText() noexcept
	{
		return m_text;
	}

	void TextWordItem::setText(const String16& text) noexcept
	{
		m_text = text;
	}

	Size TextWordItem::getSize() noexcept
	{
		ObjectLocker lock(this);
		
		String16 text = m_text;
		Ref<Font> font = getFont();
		if (m_textCached == text && m_fontCached == font) {
			return Size(m_widthCached, m_heightCached);
		}
		if (font.isNotNull()) {
			Ref<FontAtlas> atlas = font->getSharedAtlas();
			if (atlas.isNotNull()) {
				Size size = atlas->measureText(text, sl_false);
				m_textCached = text;
				m_fontCached = font;
				m_widthCached = size.x;
				m_heightCached = size.y;
				return size;
			}
		}
		m_widthCached = 0;
		m_heightCached = 0;
		return Size::zero();
	}
	
	void TextWordItem::draw(Canvas* canvas, sl_real x, sl_real y, const Color& color)
	{
#if defined(SLIB_PLATFORM_IS_MACOS)
		CanvasType canvasType = canvas->getType();
		if (canvasType != CanvasType::View && canvasType != CanvasType::Bitmap) {
			canvas->drawText16(m_text, x, y, getFont(), color);
			return;
		}
		Ref<Font> font = getFont();
		if (font.isNull()) {
			return;
		}
		Ref<FontAtlas> atlas = font->getSharedAtlas();
		if (atlas.isNull()) {
			return;
		}
		String16 str = m_text;
		sl_char16* sz = str.getData();
		sl_size len = str.getLength();
		if (!len) {
			return;
		}
		String16 s = String16::allocate(1);
		sl_char16* t = s.getData();
		for (sl_size i = 0; i < len; i++) {
			sl_char16 ch = sz[i];
			Size size = atlas->getFontSize(ch);
			t[0] = ch;
			canvas->drawText16(s, x, y, font, color);
			x += size.x;
		}
#else
		canvas->drawText16(m_text, x, y, getFont(), color);
#endif
	}
	
	
	SLIB_DEFINE_OBJECT(TextSpaceItem, TextItem)

	TextSpaceItem::TextSpaceItem() noexcept
	 : TextItem(TextItemType::Space)
	{
	}

	TextSpaceItem::~TextSpaceItem() noexcept
	{
	}

	Ref<TextSpaceItem> TextSpaceItem::create(const Ref<TextStyle>& style) noexcept
	{
		if (style.isNotNull()) {
			Ref<TextSpaceItem> ret = new TextSpaceItem;
			if (ret.isNotNull()) {
				ret->m_style = style;
				return ret;
			}
		}
		return sl_null;
	}

	Size TextSpaceItem::getSize() noexcept
	{
		Ref<Font> font = getFont();
		if (font.isNotNull()) {
			sl_real h = font->getFontHeight();
			return Size(h * 0.3f, h);
		} else {
			return Size::zero();
		}
	}


	SLIB_DEFINE_OBJECT(TextTabItem, TextItem)

	TextTabItem::TextTabItem() noexcept
	 : TextItem(TextItemType::Tab)
	{
	}

	TextTabItem::~TextTabItem() noexcept
	{
	}

	Ref<TextTabItem> TextTabItem::create(const Ref<TextStyle>& style) noexcept
	{
		if (style.isNotNull()) {
			Ref<TextTabItem> ret = new TextTabItem;
			if (ret.isNotNull()) {
				ret->m_style = style;
				return ret;
			}
		}
		return sl_null;
	}

	sl_real TextTabItem::getHeight() noexcept
	{
		Ref<Font> font = getFont();
		if (font.isNotNull()) {
			return font->getFontHeight();
		} else {
			return 0;
		}
	}


	SLIB_DEFINE_OBJECT(TextLineBreakItem, TextItem)

	TextLineBreakItem::TextLineBreakItem() noexcept
	 : TextItem(TextItemType::LineBreak)
	{
	}

	TextLineBreakItem::~TextLineBreakItem() noexcept
	{
	}

	Ref<TextLineBreakItem> TextLineBreakItem::create(const Ref<TextStyle>& style) noexcept
	{
		if (style.isNotNull()) {
			Ref<TextLineBreakItem> ret = new TextLineBreakItem;
			if (ret.isNotNull()) {
				ret->m_style = style;
				return ret;
			}
		}
		return sl_null;
	}

	sl_real TextLineBreakItem::getHeight() noexcept
	{
		Ref<Font> font = getFont();
		if (font.isNotNull()) {
			return font->getFontHeight();
		} else {
			return 0;
		}
	}


	SLIB_DEFINE_OBJECT(TextAttachItem, TextItem)

	TextAttachItem::TextAttachItem() noexcept
	 : TextItem(TextItemType::Attach)
	{
	}

	TextAttachItem::~TextAttachItem() noexcept
	{
	}


	TextParagraphLayoutParam::TextParagraphLayoutParam() noexcept
	{
		width = 1;
		tabWidth = 1;
		tabMargin = 1;
		align = Alignment::Left;
		multiLineMode = MultiLineMode::Single;
	}

	TextParagraphLayoutParam::~TextParagraphLayoutParam() noexcept
	{
	}

	SLIB_DEFINE_OBJECT(TextParagraph, Object)

	TextParagraph::TextParagraph() noexcept
	{
		m_maxWidth = 0;
		m_totalHeight = 0;
		
		m_positionLength = 0;
	}

	TextParagraph::~TextParagraph() noexcept
	{
	}

	void TextParagraph::addText(const String16& text, const Ref<TextStyle>& style) noexcept
	{
		if (text.isEmpty()) {
			return;
		}
		if (style.isNull()) {
			return;
		}

		ObjectLocker lock(this);
		
		sl_char16* sz = text.getData();
		sl_size len = text.getLength();
		sl_size startWord = 0;
		sl_size pos = 0;
		while (pos < len) {
			sl_char16 ch = sz[pos];
			if (SLIB_CHAR_IS_WHITE_SPACE(ch)) {
				if (startWord < pos) {
					Ref<TextWordItem> item = TextWordItem::create(String16(sz + startWord, pos - startWord), style);
					if (item.isNotNull()) {
						m_items.add_NoLock(item);
						m_positionLength += pos - startWord;
					}
				}
				if (ch == ' ') {
					Ref<TextSpaceItem> item = TextSpaceItem::create(style);
					if (item.isNotNull()) {
						m_items.add_NoLock(item);
						m_positionLength++;
					}
				} else if (ch == '\t') {
					Ref<TextTabItem> item = TextTabItem::create(style);
					if (item.isNotNull()) {
						m_items.add_NoLock(item);
						m_positionLength++;
					}
				} else if (ch == '\r' || ch == '\n') {
					Ref<TextLineBreakItem> item = TextLineBreakItem::create(style);
					if (item.isNotNull()) {
						m_items.add_NoLock(item);
						m_positionLength++;
					}
					if (ch == '\r' && pos + 1 < len) {
						if (sz[pos + 1] == '\n') {
							pos++;
						}
					}
				}
				startWord = pos + 1;
			}
			pos++;
		}
		if (startWord == 0) {
			Ref<TextWordItem> item = TextWordItem::create(text, style);
			if (item.isNotNull()) {
				m_items.add_NoLock(item);
				m_positionLength += len;
			}
		} else if (startWord < len) {
			Ref<TextWordItem> item = TextWordItem::create(String16(sz + startWord, len - startWord), style);
			if (item.isNotNull()) {
				m_items.add_NoLock(item);
				m_positionLength += len - startWord;
			}
		}
	}
	
	void TextParagraph::addHyperTextNodeGroup(const Ref<XmlNodeGroup>& group, const Ref<TextStyle>& style) noexcept
	{
		if (group.isNull()) {
			return;
		}
		sl_size n = group->getChildrenCount();
		for (sl_size i = 0; i < n; i++) {
			Ref<XmlNode> child = group->getChild(i);
			if (child.isNotNull()) {
				XmlNodeType type = child->getType();
				if (type == XmlNodeType::Element) {
					addHyperTextElement(Ref<XmlElement>::from(child), style);
				} else if (type == XmlNodeType::WhiteSpace) {
					XmlWhiteSpace* space = (XmlWhiteSpace*)(child.get());
					addText(space->getContent(), style);
				} else if (type == XmlNodeType::Text) {
					XmlText* text = (XmlText*)(child.get());
					addText(text->getText(), style);
				}
			}
		}
	}
	
	static sl_bool _priv_TextParagraph_parseSize(const XmlString& _str, const Ref<Font>& _font, float* _out)
	{
		sl_real sizeBase;
		Ref<Font> font = _font;
		if (font.isNotNull()) {
			sizeBase = font->getSize();
		} else {
			font = Font::getDefault();
			if (font.isNotNull()) {
				sizeBase = font->getSize();
			} else {
				sizeBase = Font::getDefaultFontSize();
			}
		}
		XmlString str = _str.trim().toLower();
		sl_real f = -1;
		if (str == "medium") {
			f = Font::getDefaultFontSize();
		} else if (str == "xx-small") {
			f = Font::getDefaultFontSize() / 4;
		} else if (str == "x-small") {
			f = Font::getDefaultFontSize() / 2;
		} else if (str == "small") {
			f = Font::getDefaultFontSize() / 4 * 3;
		} else if (str == "large") {
			f = Font::getDefaultFontSize() / 2 * 3;
		} else if (str == "x-large") {
			f = Font::getDefaultFontSize() * 2;
		} else if (str == "xx-large") {
			f = Font::getDefaultFontSize() * 4;
		} else if (str == "smaller") {
			f = sizeBase / 1.5f;
		} else if (str == "initial") {
			f = Font::getDefaultFontSize();
		} else if (str == "inherit") {
			f = sizeBase;
		}
		if (f >= 0) {
			if (_out) {
				*_out = f;
			}
			return sl_true;
		}
		sl_char16* sz = str.getData();
		sl_size len = str.getLength();
		f = 0;
		sl_reg pos = Calculator::calculate(&f, sl_null, sz, 0, len);
		if (pos > 0) {
			XmlString unit = XmlString(sz + pos, len - pos).trim();
			if (Math::isAlmostZero(f) && unit.isEmpty()) {
			} else if (unit == SLIB_UNICODE("%")) {
				f = sizeBase * f / 100;
			} else if (unit == SLIB_UNICODE("cm")) {
				f = GraphicsUtil::centimeterToPixel(f);
			} else if (unit == SLIB_UNICODE("mm")) {
				f = GraphicsUtil::millimeterToPixel(f);
			} else if (unit == SLIB_UNICODE("in")) {
				f = GraphicsUtil::inchToPixel(f);
			} else if (unit == SLIB_UNICODE("px")) {
			} else if (unit == SLIB_UNICODE("pt")) {
				f = GraphicsUtil::pointToPixel(f);
			} else if (unit == SLIB_UNICODE("pc")) {
				f = GraphicsUtil::picasToPixel(f);
			} else if (unit == SLIB_UNICODE("em")) {
				f = sizeBase * f;
			} else if (unit == SLIB_UNICODE("rem")) {
				f = Font::getDefaultFontSize() * f;
			} else if (unit == SLIB_UNICODE("ch")) {
				if (font.isNotNull()) {
					f = font->getFontHeight() * f;
				}
			} else if (unit == SLIB_UNICODE("ex")) {
				if (font.isNotNull()) {
					f = font->measureText("0").x * f;
				}
			} else if (unit == SLIB_UNICODE("vw")) {
				f = (sl_real)(Device::getScreenWidth() * f / 100);
			} else if (unit == SLIB_UNICODE("vh")) {
				f = (sl_real)(Device::getScreenHeight() * f / 100);
			} else if (unit == SLIB_UNICODE("vmin")) {
				f = (sl_real)(Math::min(Device::getScreenWidth(), Device::getScreenHeight()) * f / 100);
			} else if (unit == SLIB_UNICODE("vmax")) {
				f = (sl_real)(Math::max(Device::getScreenWidth(), Device::getScreenHeight()) * f / 100);
			} else {
				return sl_false;
			}
			if (_out) {
				*_out = f;
			}
			return sl_true;
		}
		return sl_false;
	}
	
	void TextParagraph::addHyperTextElement(const Ref<XmlElement>& element, const Ref<TextStyle>& style) noexcept
	{
		if (element.isNull()) {
			return;
		}
		
		Ref<Font> font = style->font;
		
		sl_bool flagDefineTextColor = sl_false;
		Color attrTextColor;
		sl_bool flagDefineBackColor = sl_false;
		Color attrBackColor;
		sl_bool flagDefineFamilyName = sl_false;
		XmlString attrFamilyName;
		sl_bool flagDefineFontSize = sl_false;
		XmlString attrFontSize;
		sl_real attrFontSizeParsed = 0;
		sl_bool flagDefineBold = sl_false;
		sl_bool attrBold = sl_false;
		sl_bool flagDefineUnderline = sl_false;
		sl_bool attrUnderline = sl_false;
		sl_bool flagDefineOverline = sl_false;
		sl_bool attrOverline = sl_false;
		sl_bool flagDefineLineThrough = sl_false;
		sl_bool attrLineThrough = sl_false;
		sl_bool flagDefineItalic = sl_false;
		sl_bool attrItalic = sl_false;
		sl_bool flagDefineHref = sl_false;
		XmlString attrHref;
		sl_bool flagDefineLineHeight = sl_false;
		XmlString attrLineHeight;
		sl_real attrLineHeightParsed = 0;
		sl_bool flagDefineYOffset = sl_false;
		sl_real attrYOffset = 0;

		XmlString name = element->getName().toLower();
		if (name == SLIB_UNICODE("a")) {
			flagDefineTextColor = sl_true;
			attrTextColor = Color::Blue;
			flagDefineUnderline = sl_true;
			attrUnderline = sl_true;
		} else if (name == SLIB_UNICODE("b")) {
			flagDefineBold = sl_true;
			attrBold = sl_true;
		} else if (name == SLIB_UNICODE("i")) {
			flagDefineItalic = sl_true;
			attrItalic = sl_true;
		} else if (name == SLIB_UNICODE("u")) {
			flagDefineUnderline = sl_true;
			attrUnderline = sl_true;
		} else if (name == SLIB_UNICODE("sup")) {
			if (font.isNotNull()) {
				flagDefineYOffset = sl_true;
				attrYOffset = style->yOffset - font->getFontHeight() / 4;
				flagDefineFontSize = sl_true;
				attrFontSizeParsed = font->getSize() * 2 / 3;
			}
		} else if (name == SLIB_UNICODE("sub")) {
			if (font.isNotNull()) {
				flagDefineYOffset = sl_true;
				attrYOffset = style->yOffset + font->getFontHeight() / 4;
				flagDefineFontSize = sl_true;
				attrFontSizeParsed = font->getSize() * 2 / 3;
			}
		}

		{
			XmlString value = element->getAttribute(SLIB_UNICODE("href"));
			if (value.isNotNull()) {
				flagDefineHref = sl_true;
				attrHref = value;
			}
		}
		{
			XmlString value = element->getAttribute(SLIB_UNICODE("face"));
			if (value.isNotNull()) {
				flagDefineFamilyName = sl_true;
				attrFamilyName = value;
			}
		}
		{
			XmlString value = element->getAttributeIgnoreCase(SLIB_UNICODE("size"));
			if (value.isNotNull()) {
				flagDefineFontSize = sl_true;
				attrFontSize = value.trim().toLower();
			}
		}
		{
			XmlString value = element->getAttributeIgnoreCase(SLIB_UNICODE("color"));
			if (value.isNotNull()) {
				if (attrTextColor.parse(value)) {
					flagDefineTextColor = sl_true;
				}
			}
		}
		{
			XmlString value = element->getAttributeIgnoreCase(SLIB_UNICODE("bgcolor"));
			if (value.isNotNull()) {
				if (attrBackColor.parse(value)) {
					flagDefineBackColor = sl_true;
				}
			}
		}
		
		XmlString attrStyle = element->getAttributeIgnoreCase(SLIB_UNICODE("style"));
		if (attrStyle.isNotEmpty()) {
			attrStyle = attrStyle.toLower();
			sl_char16* buf = attrStyle.getData();
			sl_size len = attrStyle.getLength();
			sl_size pos = 0;
			while (pos < len) {
				sl_reg end = attrStyle.indexOf(';', pos);
				if (end < 0) {
					end = len;
				}
				sl_size d = pos;
				for (; (sl_reg)d < end; d++) {
					if (buf[d] == ':') {
						break;
					}
				}
				if (pos < d && (sl_reg)d < end - 1) {
					XmlString name = attrStyle.substring(pos, d).trim().toLower();
					XmlString value = attrStyle.substring(d + 1, end).trim().toLower();
					if (name == SLIB_UNICODE("background-color")) {
						if (attrBackColor.parse(value)) {
							flagDefineBackColor = sl_true;
						}
					} else if (name == SLIB_UNICODE("color")) {
						if (attrTextColor.parse(value)) {
							flagDefineTextColor = sl_true;
						}
					} else if (name == SLIB_UNICODE("line-height")) {
						flagDefineLineHeight = sl_true;
						attrLineHeight = value;
					} else if (name == SLIB_UNICODE("font-family")) {
						flagDefineFamilyName = sl_true;
						attrFamilyName = value;
					} else if (name == SLIB_UNICODE("font-size")) {
						flagDefineFontSize = sl_true;
						attrFontSize = value;
					} else if (name == SLIB_UNICODE("font-weight")) {
						flagDefineBold = sl_true;
						attrBold = value == SLIB_UNICODE("bold");
					} else if (name == SLIB_UNICODE("font-style")) {
						flagDefineItalic = sl_true;
						attrItalic = value == SLIB_UNICODE("italic") || value == SLIB_UNICODE("oblique");
					} else if (name == SLIB_UNICODE("font")) {
						ListElements<XmlString> elements(value.split(SLIB_UNICODE(" ")));
						sl_size indexSize = 0;
						for (; indexSize < elements.count; indexSize++) {
							XmlString& s = elements[indexSize];
							if (s == SLIB_UNICODE("oblique") || s == SLIB_UNICODE("italic")) {
								flagDefineItalic = sl_true;
								attrBold = sl_true;
							} else if (s == SLIB_UNICODE("bold")) {
								flagDefineBold = sl_true;
								attrBold = sl_true;
							}
							sl_reg indexLineHeight = s.indexOf('/');
							if (indexLineHeight < 0) {
								if (_priv_TextParagraph_parseSize(s, font, &attrFontSizeParsed)) {
									attrFontSize.setNull();
									flagDefineFontSize = sl_true;
									break;
								}
							} else {
								if (_priv_TextParagraph_parseSize(s.substring(indexLineHeight + 1), font, &attrLineHeightParsed)) {
									attrLineHeight.setNull();
									flagDefineLineHeight = sl_true;
								}
								if (_priv_TextParagraph_parseSize(s.substring(0, indexLineHeight), font, &attrFontSizeParsed)) {
									attrFontSize.setNull();
									flagDefineFontSize = sl_true;
								}
								break;
							}
						}
						XmlString face = XmlStringBuffer::join(SLIB_UNICODE(" "), elements.list, indexSize + 1);
						if (face.isNotEmpty()) {
							flagDefineFamilyName = sl_true;
							attrFamilyName = face;
						}
					} else if (name == SLIB_UNICODE("text-decoration") || name == SLIB_UNICODE("text-decoration-line")) {
						flagDefineUnderline = sl_true;
						attrUnderline = value.contains(SLIB_UNICODE("underline"));
						flagDefineOverline = sl_true;
						attrOverline = value.contains(SLIB_UNICODE("overline"));
						flagDefineLineThrough = sl_true;
						attrLineThrough = value.contains(SLIB_UNICODE("line-through"));
					}
				}
				pos = end + 1;
			}
		}
		
		if (flagDefineFontSize) {
			if (attrFontSize.isNotNull()) {
				if (!(_priv_TextParagraph_parseSize(attrFontSize, font, &attrFontSizeParsed))) {
					flagDefineFontSize = sl_false;
				}
			}
		}
		if (flagDefineLineHeight) {
			if (attrLineHeight.isNotNull()) {
				if (!(_priv_TextParagraph_parseSize(attrLineHeight, font, &attrLineHeightParsed))) {
					flagDefineLineHeight = sl_false;
				}
			}
		}
		
		Ref<TextStyle> styleNew = style;
		FontDesc fontDesc;
		sl_bool flagNewFont = sl_false;
		do {
			if (font.isNull()) {
				flagNewFont = sl_true;
				break;
			}
			font->getDesc(fontDesc);
			if (flagDefineFamilyName) {
				if (fontDesc.familyName != attrFamilyName) {
					flagNewFont = sl_true;
					break;
				}
			}
			if (flagDefineFontSize) {
				if (!(Math::isAlmostZero(fontDesc.size - attrFontSizeParsed))) {
					flagNewFont = sl_true;
					break;
				}
			}
			if (flagDefineBold) {
				if (fontDesc.flagBold != attrBold) {
					flagNewFont = sl_true;
					break;
				}
			}
			if (flagDefineItalic) {
				if (fontDesc.flagItalic != attrItalic) {
					flagNewFont = sl_true;
					break;
				}
			}
		} while (0);
		
		sl_bool flagNewStyle = flagNewFont;
		if (!flagNewStyle) {
			do {
				if (flagDefineTextColor) {
					if (style->textColor != attrTextColor) {
						flagNewStyle = sl_true;
						break;
					}
				}
				if (flagDefineBackColor) {
					if (style->backgroundColor != attrBackColor) {
						flagNewStyle = sl_true;
						break;
					}
				}
				if (flagDefineHref) {
					if (style->href != attrHref) {
						flagNewStyle = sl_true;
						break;
					}
				}
				if (flagDefineYOffset) {
					if (style->yOffset != attrYOffset) {
						flagNewStyle = sl_true;
						break;
					}
				}
				if (flagDefineLineHeight) {
					if (!(Math::isAlmostZero(style->lineHeight - attrLineHeightParsed))) {
						flagNewStyle = sl_true;
						break;
					}
				}
				if (flagDefineUnderline) {
					if (style->flagUnderline != attrUnderline) {
						flagNewStyle = sl_true;
						break;
					}
				}
				if (flagDefineOverline) {
					if (style->flagOverline != attrOverline) {
						flagNewStyle = sl_true;
						break;
					}
				}
				if (flagDefineLineThrough) {
					if (style->flagLineThrough != attrLineThrough) {
						flagNewStyle = sl_true;
						break;
					}
				}
			} while (0);
		}
		
		if (flagNewStyle) {
			styleNew = style->duplicate();
			if (flagNewFont) {
				if (flagDefineFamilyName) {
					fontDesc.familyName = attrFamilyName;
				}
				if (flagDefineFontSize) {
					fontDesc.size = attrFontSizeParsed;
				}
				if (flagDefineBold) {
					fontDesc.flagBold = attrBold;
				}
				if (flagDefineItalic) {
					fontDesc.flagItalic = attrItalic;
				}
				font = Font::create(fontDesc);
				styleNew->font = font;
			}
			if (flagDefineUnderline) {
				styleNew->flagUnderline = attrUnderline;
			}
			if (flagDefineOverline) {
				styleNew->flagOverline = attrOverline;
			}
			if (flagDefineLineThrough) {
				styleNew->flagLineThrough = attrLineThrough;
			}
			if (flagDefineTextColor) {
				styleNew->textColor = attrTextColor;
			}
			if (flagDefineBackColor) {
				styleNew->backgroundColor = attrBackColor;
			}
			if (flagDefineHref) {
				styleNew->href = attrHref;
			}
			if (flagDefineYOffset) {
				styleNew->yOffset = attrYOffset;
			}
			if (flagDefineLineHeight) {
				styleNew->lineHeight = attrLineHeightParsed;
			}
		}
		
		if (name == SLIB_UNICODE("br")) {
			SLIB_STATIC_STRING16(line, "\n");
			addText(line, styleNew);
		}
		addHyperTextNodeGroup(Ref<XmlNodeGroup>::from(element), styleNew);
	}
	
	void TextParagraph::addHyperText(const String16& text, const Ref<TextStyle>& style) noexcept
	{
		XmlParseParam param;
		param.flagLogError = sl_false;
		param.setCreatingOnlyElementsAndTexts();
		param.flagCreateWhiteSpaces = sl_true;
		param.flagCheckWellFormed = sl_false;
		Ref<XmlDocument> xml = Xml::parseXml16(text, param);
		if (xml.isNotNull()) {
			addHyperTextNodeGroup(Ref<XmlNodeGroup>::from(xml), style);
		}
	}


	class _priv_TextParagraph_Layouter
	{
	public:
		CList< Ref<TextItem> >* m_layoutItems;
		sl_real m_layoutWidth;
		Alignment m_align;
		MultiLineMode m_multiLineMode;
		EllipsizeMode m_ellipsizeMode;
		sl_real m_tabMargin;
		sl_real m_tabWidth;
		
		sl_bool m_flagEnd;
		sl_real m_x;
		sl_real m_y;
		
		CList< Ref<TextItem> > m_lineItems;
		sl_real m_lineWidth;
		sl_real m_lineHeight;
		
		sl_real m_maxWidth;
		
	public:
		_priv_TextParagraph_Layouter(CList< Ref<TextItem> >* layoutItems, const TextParagraphLayoutParam& param) noexcept
		{
			m_layoutItems = layoutItems;
			m_layoutWidth = param.width;
			m_align = param.align & Alignment::HorizontalMask;
			m_multiLineMode = param.multiLineMode;
			m_ellipsizeMode = param.ellipsisMode;
			m_tabWidth = param.tabWidth;
			m_tabMargin = param.tabMargin;
			
			m_flagEnd = sl_false;
			m_x = 0;
			m_y = 0;

			m_lineWidth = 0;
			m_lineHeight = 0;
			m_maxWidth = 0;
		}
		
		void endLine() noexcept
		{
			sl_size n = m_lineItems.getCount();
			if (n == 0) {
				return;
			}
			sl_real x;
			if (m_align == Alignment::Center) {
				x = (m_layoutWidth - m_lineWidth) / 2;
			} else if (m_align == Alignment::Right) {
				x = m_layoutWidth - m_lineWidth;
			} else {
				x = 0;
			}
			if (m_ellipsizeMode != EllipsizeMode::None) {
				if (m_lineWidth > m_layoutWidth) {
					x = 0;
				}
			}
			sl_real bottom = m_y + m_lineHeight;
			Ref<TextItem>* p = m_lineItems.getData();
			for (sl_size i = 0; i < n; i++) {
				TextItem* item = p[i].get();
				Size size = item->getLayoutSize();
				Point pt(x, m_y + (m_lineHeight - size.y) / 2);
				item->setLayoutPosition(pt);
				TextItemType type = item->getType();
				if (type == TextItemType::Word || type == TextItemType::Space || type == TextItemType::Tab) {
					m_layoutItems->add_NoLock(item);
				} else if (type == TextItemType::Attach) {
					(static_cast<TextAttachItem*>(item))->setPosition(pt);
				}
				x += size.x;
			}
			
			if (m_ellipsizeMode != EllipsizeMode::None) {
				if (m_lineWidth > m_layoutWidth) {
					endEllipsize();
				}
			}
			
			m_lineItems.removeAll_NoLock();
			if (m_lineWidth > m_maxWidth) {
				m_maxWidth = m_lineWidth;
			}
			m_x = 0;
			m_lineWidth = 0;
			m_y = bottom;
			m_lineHeight = 0;
			
		}
		
		void endEllipsize() noexcept
		{
			SLIB_STATIC_STRING16_BY_ARRAY(strEllipsis, '.', '.', '.')
			sl_size nItems = m_layoutItems->getCount();
			Ref<TextItem>* items = m_layoutItems->getData();
			if (nItems < 1) {
				return;
			}
			Ref<TextStyle> style = items[nItems - 1]->getStyle();
			Ref<TextWordItem> itemEllipsis = TextWordItem::create(strEllipsis, style);
			if (itemEllipsis.isNull()) {
				return;
			}
			Size sizeEllipsis = itemEllipsis->getSize();
			if (m_layoutWidth < sizeEllipsis.x) {
				return;
			}
			sl_real xLimit = m_layoutWidth - sizeEllipsis.x;
			if (m_ellipsizeMode == EllipsizeMode::End) {
				for (sl_size i = 0; i < nItems; i++) {
					TextItem* item = items[i].get();
					Point pos = item->getLayoutPosition();
					if (pos.x + item->getLayoutSize().x > xLimit) {
						itemEllipsis->setLayoutPosition(pos);
						itemEllipsis->setLayoutSize(sizeEllipsis);
						if (item->getType() == TextItemType::Word) {
							String16 text = ((TextWordItem*)item)->getText();
							Ref<TextWordItem> word = TextWordItem::create(text, item->getStyle());
							if (word.isNotNull()) {
								sl_real widthLimit = xLimit - pos.x;
								sl_size n = text.getLength();
								sl_size k = n;
								for (; k > 0; k--) {
									word->setText(text.substring(0, k));
									if (word->getSize().x <= widthLimit) {
										break;
									}
								}
								m_layoutItems->setCount_NoLock(i);
								if (k > 0) {
									word->setLayoutPosition(item->getLayoutPosition());
									word->setLayoutSize(word->getSize());
									m_layoutItems->add_NoLock(word);
									pos.x += word->getSize().x;
									itemEllipsis->setLayoutPosition(pos);
								}
							} else {
								m_layoutItems->setCount_NoLock(i);
							}
						} else {
							m_layoutItems->setCount_NoLock(i);
						}
						m_layoutItems->add_NoLock(itemEllipsis);
						m_flagEnd = sl_true;
						return;
					}
				}
			} else if (m_ellipsizeMode == EllipsizeMode::Start) {
				for (sl_size i = 0; i < nItems; i++) {
					TextItem* item = items[nItems - 1 - i].get();
					Point pos = item->getLayoutPosition();
					pos.x = m_layoutWidth - m_lineWidth + pos.x;
					item->setLayoutPosition(pos);
					if (pos.x < sizeEllipsis.x) {
						if (i > 0) {
							itemEllipsis->setLayoutPosition(Point(items[nItems - i]->getLayoutPosition().x - sizeEllipsis.x, item->getLayoutPosition().y));
						} else {
							itemEllipsis->setLayoutPosition(Point(pos.x + item->getLayoutSize().x - sizeEllipsis.x, item->getLayoutPosition().y));
						}
						itemEllipsis->setLayoutSize(sizeEllipsis);
						if (item->getType() == TextItemType::Word) {
							sl_real widthWord = item->getLayoutSize().x;
							String16 text = ((TextWordItem*)item)->getText();
							Ref<TextWordItem> word = TextWordItem::create(text, item->getStyle());
							if (word.isNotNull()) {
								sl_real widthLimit = widthWord - (sizeEllipsis.x - pos.x);
								sl_size n = text.getLength();
								sl_size k = n;
								for (; k > 0; k--) {
									word->setText(text.substring(n - k, n));
									if (word->getSize().x <= widthLimit) {
										break;
									}
								}
								m_layoutItems->removeRange_NoLock(0, nItems - i);
								if (k > 0) {
									pos.x = pos.x + widthWord - word->getSize().x;
									word->setLayoutPosition(pos);
									word->setLayoutSize(word->getSize());
									m_layoutItems->add_NoLock(word);
									pos.x -= sizeEllipsis.x;
									itemEllipsis->setLayoutPosition(pos);
								}
							} else {
								m_layoutItems->removeRange_NoLock(0, nItems - i);
							}
						} else {
							m_layoutItems->removeRange_NoLock(0, nItems - i);
						}
						m_layoutItems->insert_NoLock(0, itemEllipsis);
						m_flagEnd = sl_true;
						return;
					}
				}
			} else if (m_ellipsizeMode == EllipsizeMode::Middle) {
				sl_real xEllipsis = 0;
				itemEllipsis->setLayoutSize(sizeEllipsis);
				sl_size i;
				for (i = 0; i < nItems; i++) {
					TextItem* item = items[i].get();
					Point pos = item->getLayoutPosition();
					if (pos.x + item->getLayoutSize().x > xLimit / 2) {
						xEllipsis = pos.x;
						break;
					}
				}
				itemEllipsis->setLayoutPosition(Point(xEllipsis, items[0]->getLayoutPosition().y));
				sl_size indexMid = i;
				for (i = 0; i < nItems - indexMid; i++) {
					TextItem* item = items[nItems - 1 - i].get();
					Point pos = item->getLayoutPosition();
					pos.x = m_layoutWidth - m_lineWidth + pos.x;
					item->setLayoutPosition(pos);
					if (pos.x < itemEllipsis->getLayoutFrame().right + sizeEllipsis.x * 0.2f) {
						if (item->getType() == TextItemType::Word) {
							sl_real widthWord = item->getLayoutSize().x;
							String16 text = ((TextWordItem*)item)->getText();
							Ref<TextWordItem> word = TextWordItem::create(text, item->getStyle());
							if (word.isNotNull()) {
								sl_real widthLimit = widthWord - (itemEllipsis->getLayoutFrame().right + sizeEllipsis.x * 0.2f - pos.x);
								sl_size n = text.getLength();
								sl_size k = n;
								for (; k > 0; k--) {
									word->setText(text.substring(n - k, n));
									if (word->getSize().x <= widthLimit) {
										break;
									}
								}
								m_layoutItems->removeRange(indexMid, nItems - indexMid - i);
								if (k > 0) {
									pos.x = pos.x + widthWord - word->getSize().x;
									word->setLayoutPosition(pos);
									word->setLayoutSize(word->getSize());
									m_layoutItems->insert_NoLock(indexMid, word);
									pos.x -= sizeEllipsis.x + sizeEllipsis.x * 0.2f;
									itemEllipsis->setLayoutPosition(pos);
								}
							} else {
								m_layoutItems->removeRange_NoLock(indexMid, nItems - indexMid - i);
							}
						} else {
							m_layoutItems->removeRange_NoLock(indexMid, nItems - indexMid - i);
						}
						m_layoutItems->insert_NoLock(indexMid, itemEllipsis);
						m_flagEnd = sl_true;
						return;
					}
				}
			}
		}
		
		void breakWord(TextWordItem* breakItem) noexcept
		{
			Ref<TextStyle> style = breakItem->getStyle();
			if (style.isNull()) {
				return;
			}
			
			Ref<Font> font = style->font;
			if (font.isNull()) {
				return;
			}
			
			String16 text = breakItem->getText();
			if (text.isEmpty()) {
				return;
			}
			
			Ref<FontAtlas> atlas = font->getSharedAtlas();
			if (atlas.isNull()) {
				return;
			}
			
			ObjectLocker lockAtlas(atlas.get());
			
			sl_char16* sz = text.getData();
			sl_size len = text.getLength();
			
			sl_real widthRemaining = m_layoutWidth - m_x;
			
			Size size = atlas->getFontSize_NoLock(sz[0]);
			sl_real x = size.x;
			sl_real height = size.y;
			sl_size startLine = 0;
			sl_size pos = 1;
			
			if (size.x > widthRemaining && m_x > 0) {
				endLine();
				widthRemaining = m_layoutWidth;
			}

			while (pos < len) {
				size = atlas->getFontSize_NoLock(sz[pos]);
				if (pos > startLine && x + size.x > widthRemaining) {
					Ref<TextWordItem> newItem = TextWordItem::create(String16(sz + startLine, pos - startLine), style);
					if (newItem.isNotNull()) {
						newItem->setLayoutSize(Size(x, height));
						m_lineItems.add(newItem);
					}
					startLine = pos;
					m_x += x;
					m_lineWidth = m_x;
					applyLineHeight(breakItem, height);
					endLine();
					x = 0;
					height = 0;
					widthRemaining = m_layoutWidth;
				}
				x += size.x;
				if (size.y > height) {
					height = size.y;
				}
				pos++;
			}
			if (len > startLine) {
				Ref<TextWordItem> newItem = TextWordItem::create(String16(sz + startLine, len - startLine), style);
				if (newItem.isNotNull()) {
					newItem->setLayoutSize(Size(x, height));
					m_lineItems.add(newItem);
				}
				m_x += x;
				m_lineWidth = m_x;
				applyLineHeight(breakItem, height);
			}
		}
		
		sl_size processWords(Ref<TextItem>* items, sl_size nItems) noexcept
		{
			sl_real x = 0;
			TextWordItem* lastWord = sl_null;
			
			sl_size nWords = 0;
			while (nWords < nItems) {
				TextItem* item = items[nWords].get();
				if (item->getType() == TextItemType::Word) {
					lastWord = static_cast<TextWordItem*>(item);
					Size size = lastWord->getSize();
					lastWord->setLayoutSize(size);
					x += size.x;
					applyLineHeight(item, size.y);
				} else {
					break;
				}
				nWords++;
			}
			if (nWords == 0) {
				return 0;
			}
			
			sl_bool flagBreakWord = sl_false;
			if (m_x + x > m_layoutWidth) {
				if (m_multiLineMode == MultiLineMode::WordWrap) {
					if (m_lineItems.getCount() == 0) {
						flagBreakWord = sl_true;
					} else {
						endLine();
						if (lastWord) {
							applyLineHeight(lastWord, lastWord->getSize().y);
						}
						if (x > m_layoutWidth) {
							flagBreakWord = sl_true;
						}
					}
				} else if (m_multiLineMode == MultiLineMode::BreakWord) {
					flagBreakWord = sl_true;
				}
			}
			
			if (flagBreakWord) {
				for (sl_size i = 0; i < nWords; i++) {
					TextWordItem* item = static_cast<TextWordItem*>(items[i].get());
					Size size = item->getLayoutSize();
					if (m_x + size.x > m_layoutWidth) {
						breakWord(item);
						return i;
					} else {
						m_lineItems.add_NoLock(item);
						m_x += size.x;
						m_lineWidth = m_x;
						applyLineHeight(item, size.y);
					}
				}
			} else {
				m_lineItems.addElements_NoLock(items, nWords);
				m_x += x;
				m_lineWidth = m_x;
			}
			return nWords - 1;
		}
		
		void processSpace(TextSpaceItem* item) noexcept
		{
			Size size = item->getSize();
			applyLineHeight(item, size.y);
			item->setLayoutPosition(Point(m_x, m_y));
			item->setLayoutSize(size);
			m_lineItems.add_NoLock(item);
			m_x += size.x;
			m_lineWidth = m_x;
		}
		
		void processTab(TextTabItem* item) noexcept
		{
			sl_real h = item->getHeight();
			applyLineHeight(item, h);
			sl_real tabX = m_x + m_tabMargin;
			tabX = (Math::floor(tabX / m_tabWidth) + 1 ) * m_tabWidth;
			item->setLayoutPosition(Point(m_x, m_y));
			item->setLayoutSize(Size(tabX - m_x, h));
			m_lineItems.add_NoLock(item);
			m_x = tabX;
			m_lineWidth = m_x;
		}
		
		void processLineBreak(TextLineBreakItem* item) noexcept
		{
			sl_real h = item->getHeight();
			applyLineHeight(item, h);
			item->setLayoutSize(Size(h / 2, h));
			m_lineItems.add_NoLock(item);
			endLine();
			item->setLayoutPosition(Point(m_x, m_y));
			if (m_multiLineMode == MultiLineMode::Single) {
				m_flagEnd = sl_true;
			}
		}
		
		void processAttach(TextAttachItem* item) noexcept
		{
			Size size = item->getSize();
			applyLineHeight(item, size.y);
			item->setLayoutSize(size);
			m_lineItems.add_NoLock(item);
			m_x += size.x;
			m_lineWidth = m_x;
		}
		
		void applyLineHeight(TextItem* item, sl_real height)
		{
			sl_real lineHeight = height;
			Ref<TextStyle> style = item->getStyle();
			if (style.isNotNull()) {
				if (style->lineHeight >= 0) {
					lineHeight = style->lineHeight;
				}
			}
			if (lineHeight > m_lineHeight) {
				m_lineHeight = lineHeight;
			}
		}

		void layout(CList< Ref<TextItem> >* list) noexcept
		{
			sl_size n = list->getCount();
			Ref<TextItem>* items = list->getData();
			
			for (sl_size i = 0; i < n; i++) {
				
				TextItem* item = items[i].get();
				
				TextItemType type = item->getType();
				
				switch (type) {
						
					case TextItemType::Word:
						i += processWords(items + i, n - i);
						break;
						
					case TextItemType::Space:
						processSpace(static_cast<TextSpaceItem*>(item));
						break;
						
					case TextItemType::Tab:
						processTab(static_cast<TextTabItem*>(item));
						break;
						
					case TextItemType::LineBreak:
						processLineBreak(static_cast<TextLineBreakItem*>(item));
						break;
						
					case TextItemType::Attach:
						processAttach(static_cast<TextAttachItem*>(item));
						break;
				}
				
				if (m_flagEnd) {
					break;
				}
			}
			
			endLine();

		}
		
	};

	void TextParagraph::layout(const TextParagraphLayoutParam& param) noexcept
	{
		ObjectLocker lock(this);

		m_layoutItems.removeAll_NoLock();
		
		if (param.multiLineMode == MultiLineMode::WordWrap || param.multiLineMode == MultiLineMode::BreakWord) {
			if (param.width < SLIB_EPSILON) {
				return;
			}
		}
		
		_priv_TextParagraph_Layouter layouter(&m_layoutItems, param);
		
		layouter.layout(&m_items);

		m_maxWidth = layouter.m_maxWidth;
		m_totalHeight = layouter.m_y;
		
	}

	void TextParagraph::draw(Canvas* canvas, sl_real x, sl_real y, const Color& colorDefault) noexcept
	{
		Rectangle rc = canvas->getInvalidatedRect();
		
		ObjectLocker lock(this);
		
		ListElements< Ref<TextItem> > items(m_layoutItems);
		for (sl_size i = 0; i < items.count; i++) {
			TextItem* item = items[i].get();
			TextItemType type = item->getType();
			Ref<TextStyle> style = item->getStyle();
			if (style.isNotNull()) {
				Color color = style->textColor;
				if (color.isZero()) {
					color = colorDefault;
				}
				if (type == TextItemType::Word) {
					TextWordItem* wordItem = static_cast<TextWordItem*>(item);
					Rectangle frame = wordItem->getLayoutFrame();
					frame.top += style->yOffset;
					frame.bottom += style->yOffset;
					if (rc.intersectRectangle(frame)) {
						Ref<Font> font = style->font;
						if (font.isNotNull()) {
							Color backColor = style->backgroundColor;
							if (backColor.a > 0) {
								canvas->fillRectangle(Rectangle(x + frame.left, y + frame.top, x + frame.right, y + frame.bottom), backColor);
							}
							wordItem->draw(canvas, x + frame.left, y + frame.top, color);
						}
					}
				}
				if (style->flagUnderline || style->flagOverline || style->flagLineThrough) {
					if (type == TextItemType::Word || type == TextItemType::Space || type == TextItemType::Tab) {
						Rectangle frame = item->getLayoutFrame();
						frame.top += style->yOffset;
						frame.bottom += style->yOffset;
						if (rc.intersectRectangle(frame)) {
							Ref<Font> font = style->font;
							if (font.isNotNull()) {
								Ref<Pen> pen = Pen::createSolidPen(1, color);
								if (pen.isNotNull()) {
									FontMetrics fm;
									if (font->getFontMetrics(fm)) {
										if (style->flagUnderline) {
											sl_real yLine = frame.bottom - fm.descent / 2;
											canvas->drawLine(Point(frame.left, yLine), Point(frame.right, yLine), pen);
										}
										if (style->flagOverline) {
											sl_real yLine = frame.bottom - fm.descent - fm.ascent;
											canvas->drawLine(Point(frame.left, yLine), Point(frame.right, yLine), pen);
										}
										if (style->flagLineThrough) {
											sl_real yLine = frame.bottom - (fm.descent + fm.ascent) / 2;
											canvas->drawLine(Point(frame.left, yLine), Point(frame.right, yLine), pen);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	Ref<TextItem> TextParagraph::getTextItemAtPosition(sl_real x, sl_real y) noexcept
	{
		ListElements< Ref<TextItem> > items(m_layoutItems);
		for (sl_size i = 0; i < items.count; i++) {
			TextItem* item = items[i].get();
			Ref<TextStyle> style = item->getStyle();
			if (style.isNotNull()) {
				Rectangle frame = item->getLayoutFrame();
				frame.top += style->yOffset;
				frame.bottom += style->yOffset;
				if (frame.containsPoint(x, y)) {
					return item;
				}
			}
		}
		return sl_null;
	}

	sl_real TextParagraph::getMaximumWidth() noexcept
	{
		return m_maxWidth;
	}

	sl_real TextParagraph::getTotalHeight() noexcept
	{
		return m_totalHeight;
	}
	
	sl_real TextParagraph::getPositionLength() noexcept
	{
		return m_positionLength;
	}


	SLIB_DEFINE_OBJECT(SimpleTextBox, Object)

	SimpleTextBox::SimpleTextBox() noexcept
	{
		m_flagHyperText = sl_false;
		m_width = 0;
		m_multiLineMode = MultiLineMode::Single;
		m_align = Alignment::Left;
		
		m_contentWidth = 0;
		m_contentHeight = 0;
		
		m_style = new TextStyle;
	}

	SimpleTextBox::~SimpleTextBox() noexcept
	{
	}

	void SimpleTextBox::update(const String& text, sl_bool flagHyperText, const Ref<Font>& font, sl_real width, sl_bool flagWrappingWidth, MultiLineMode multiLineMode, EllipsizeMode ellipsizeMode, const Alignment& _align) noexcept
	{
		ObjectLocker lock(this);
		
		if (font.isNull()) {
			return;
		}

		m_style->font = font;
		m_style->flagUnderline = font->isUnderline();
		m_style->flagLineThrough = font->isStrikeout();
		
		if (width < 0) {
			width = 0;
		}
		
		if (flagWrappingWidth) {
			if (multiLineMode != MultiLineMode::Single) {
				multiLineMode = MultiLineMode::Multiple;
			}
			ellipsizeMode = EllipsizeMode::None;
		}
		if (multiLineMode != MultiLineMode::Single) {
			ellipsizeMode = EllipsizeMode::None;
		}
		
		Alignment align = _align & Alignment::HorizontalMask;
		
		if (align == Alignment::Left && (multiLineMode == MultiLineMode::Single || multiLineMode == MultiLineMode::Multiple) && ellipsizeMode == EllipsizeMode::None) {
			width = 0;
		}
		
		sl_bool flagReLayout = sl_false;
		if (m_text != text || m_flagHyperText != flagHyperText || (flagHyperText && m_font != font)) {
			m_paragraph.setNull();
			m_contentWidth = 0;
			m_contentHeight = 0;
			if (text.isNotEmpty()) {
				m_paragraph = new TextParagraph;
				if (m_paragraph.isNotNull()) {
					if (flagHyperText) {
						m_font = font;
						m_paragraph->addHyperText(text, m_style);
					} else {
						m_paragraph->addText(text, m_style);
					}
				}
			}
			m_text = text;
			m_flagHyperText = flagHyperText;
			flagReLayout = sl_true;
		}
		if (text.isEmpty()) {
			return;
		}
		if (m_paragraph.isNotNull()) {
			if (m_font != font) {
				flagReLayout = sl_true;
			}
			if (!(Math::isAlmostZero(m_width - width)) || m_multiLineMode != multiLineMode || m_ellipsisMode != ellipsizeMode || m_align != align) {
				flagReLayout = sl_true;
			}
			if (flagReLayout) {
				TextParagraphLayoutParam param;
				param.width = width;
				param.tabWidth = font->getFontHeight() * 2;
				param.tabMargin = param.tabWidth / 4;
				param.multiLineMode = multiLineMode;
				param.ellipsisMode = ellipsizeMode;
				param.align = align;
				m_paragraph->layout(param);
				
				m_font = font;
				m_multiLineMode = multiLineMode;
				m_ellipsisMode = ellipsizeMode;
				m_align = align;
				m_width = width;
				
				m_contentWidth = m_paragraph->getMaximumWidth();
				m_contentHeight = m_paragraph->getTotalHeight();
			}
		}
	}

	void SimpleTextBox::draw(Canvas* canvas, const String& text, sl_bool flagHyperText, const Ref<Font>& font, const Rectangle& frame, sl_bool flagWrappingWidth, MultiLineMode multiLineMode, EllipsizeMode ellipsizeMode, const Alignment& align, const Color& color) noexcept
	{
		if (color.isZero()) {
			return;
		}
		sl_real width = frame.getWidth();
		if (width < SLIB_EPSILON) {
			return;
		}
		ObjectLocker lock(this);
		Ref<TextParagraph> paragraphOld = m_paragraph;
		update(text, flagHyperText, font, width, flagWrappingWidth, multiLineMode, ellipsizeMode, align);
		if (m_paragraph.isNotNull()) {
			sl_real height = m_paragraph->getTotalHeight();
			Alignment valign = align & Alignment::VerticalMask;
			sl_real y;
			if (valign == Alignment::Middle) {
				y = (frame.left + frame.bottom - height) / 2;
			} else if (valign == Alignment::Bottom) {
				y = frame.bottom - height;
			} else {
				y = frame.top;
			}
			m_paragraph->draw(canvas, frame.left, y, color);
		}
	}

	Ref<TextItem> SimpleTextBox::getTextItemAtPosition(sl_real x, sl_real y) noexcept
	{
		ObjectLocker lock(this);
		if (m_paragraph.isNotNull()) {
			return m_paragraph->getTextItemAtPosition(x, y);
		}
		return sl_null;
	}

	sl_real SimpleTextBox::getContentWidth() noexcept
	{
		return m_contentWidth;
	}

	sl_real SimpleTextBox::getContentHeight() noexcept
	{
		return m_contentHeight;
	}

}
