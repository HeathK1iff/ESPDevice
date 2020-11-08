#include "HtmlGen.h"
#include "XmlGen.h"
#include <stdlib_noniso.h>

const char PAGE_FOOTER[] = "footer";
const char PAGE_HEADER[] = "header";
const char PAGE_BODY[] = "body";
const char PAGE_TITLE[] = "title";
const char ATT_DISABLED[] = "disabled";
const char ATT_TYPE[] = "type";
const char ATT_CHECKBOX[] = "checkbox";
const char ATT_HIDDEN[] = "hidden";
const char ATT_PASSWORD[] = "password";
const char ATT_FILE[] = "file";
const char ATT_SUBMIT[] = "submit";
const char ATT_TEXT[] = "text";
const char ATT_STEP[] = "step";
const char ATT_MAX[] = "max";
const char ATT_MIN[] = "min";
const char ATT_SELECT[] = "select";
const char ATT_STYLE[] = "style";
const char ATT_SELECTED[] = "selected";
const char ATT_OPTION[] = "option";
const char ATT_NUMBER[] = "number";
const char ATT_VALUE[] = "value";
const char ATT_CHECKED[] = "checked";
const char ATT_ACTION[] = "action";
const char ATT_METHOD[] = "method";
const char ATT_GET_METHOD[] = "GET";
const char ATT_POST_METHOD[] = "POST";
const char ATT_HREF[] = "href";
const char TAG_BUTTON[] = "button";
const char TAG_STYLE[] = "style";
const char TAG_INPUT[] = "input";
const char TAG_NAME[] = "name";
const char TAG_ID[] = "id";
const char TAG_TITLE[] = "title";
const char TAG_TABLE[] = "table";
const char TAG_FORM[] = "form";
const char TAG_A[] = "a";
const char TAG_TD[] = "td";
const char TAG_TR[] = "tr";

class HtmlTagAccess : public XmlTag
{
  public:
	void print(char *text)
	{
		XmlTag::print(text);
	}
};

void HtmlStyleAttribute::print(char *text)
{
	strcat(text, key);
	strcat(text, ":");
	strcat(text, val);
	strcat(text, ";");
}

HtmlStyleAttribute::HtmlStyleAttribute(const char *key, char *val)
{
	this->key = new char[strlen(key) + 1];
	this->key[0] = '\0';
	strcat(this->key, key);

	this->val = new char[strlen(val) + 1];
	this->val[0] = '\0';
	strcat(this->val, val);
}

HtmlStyleAttribute::HtmlStyleAttribute(const char *key, const char *val)
{
	this->key = new char[strlen(key) + 1];
	this->key[0] = '\0';
	strcat(this->key, key);

	this->val = new char[strlen(val) + 1];
	this->val[0] = '\0';
	strcat(this->val, val);
}

HtmlStyleAttribute::HtmlStyleAttribute(const char *key, int val)
{
	this->key = new char[strlen(key) + 1];
	this->key[0] = '\0';
	strcat(this->key, key);
	this->val = new char[10];
	sprintf(this->val, "%d", val);
}

HtmlStyleAttribute::HtmlStyleAttribute(const char *key, float val){
	this->key = new char[strlen(key) + 1];
	this->key[0] = '\0';

	this->val = new char[15];
	dtostrf(val, 4, 2, this->val);
}

HtmlStyleAttribute::~HtmlStyleAttribute(){
	delete[] key;
	delete[] val;
}

void HtmlStyleAttributeArray::print(char *text)
{
	HtmlStyleAttribute *current = (HtmlStyleAttribute *)this->getFirst();
	do
	{
		if (current != nullptr)
		{
			((HtmlTagAccess *)current)->print(text);
		}
	} while ((current = (HtmlStyleAttribute *)this->getNext(current)) != nullptr);
}

void HtmlStyleAttributeArray::append(const char *key, char *value){
	HtmlStyleAttribute *attribute = new HtmlStyleAttribute(key, value);
	XmlArray::append(attribute);
}

void HtmlStyleAttributeArray::append(const char *key,  int value){
	HtmlStyleAttribute *attribute = new HtmlStyleAttribute(key, value);
	XmlArray::append(attribute);
}

void HtmlStyleAttributeArray::append(const char *key,  float value){
	HtmlStyleAttribute *attribute = new HtmlStyleAttribute(key, value);
	XmlArray::append(attribute);
}

void HtmlStyleAttributeArray::append(const char *key, const char *value){
	HtmlStyleAttribute *attribute = new HtmlStyleAttribute(key, value);
	XmlArray::append(attribute);
}

void HtmlCustomTag::print(char *text)
{
	if (style != nullptr)
		getAttributes()->append(ATT_STYLE, style);
	XmlTag::print(text);
}


HtmlCustomTag::HtmlCustomTag(char *tag) : XmlTag(tag){

};

HtmlCustomTag::HtmlCustomTag(const char *tag) : XmlTag(tag){

};	

HtmlStyleAttributeArray *HtmlCustomTag::getStyle()
{
	if (style == nullptr)
		style = new HtmlStyleAttributeArray();
	return style;
}

void HtmlInput::print(char *text)
{
	HtmlInputableTag::print(text);
}

void HtmlInputableTag::print(char *text){
	if (this->disabled)
		this->getAttributes()->append(ATT_DISABLED);
	HtmlCustomTag::print(text);
}

HtmlInput::HtmlInput() : HtmlInputableTag(TAG_INPUT)
{

};

void HtmlInputableTag::setName(char *name)
{
	getAttributes()->append(TAG_NAME, name);
}

void HtmlInputableTag::setName(const char *name)
{
	getAttributes()->append(TAG_NAME, name);
}

char *HtmlInputableTag::getName()
{
	return getAttributes()->getStringValueByKey(TAG_NAME);
}

char *HtmlInputableTag::getId()
{
	return getAttributes()->getStringValueByKey(TAG_ID);
}

void HtmlInputableTag::setId(char *id)
{
	getAttributes()->append(TAG_ID, id);
}

HtmlInputableTag::HtmlInputableTag(const char *id) : HtmlCustomTag(id)
{
	;
};

int HtmlInputableTag::getSize()
{
	return size;
}

void HtmlInputableTag::setSize(int size)
{
	this->size = size;
}

void HtmlInputableTag::setDisabled(bool state)
{
	this->disabled = state;
}

bool HtmlInputableTag::isDisabled()
{
	return this->disabled;
}

void HtmlInput::setValue(char *value)
{
	getAttributes()->append(ATT_VALUE, value);
}

void HtmlInput::setValue(float value)
{
	getAttributes()->append(ATT_VALUE, value);
}

void HtmlInput::setValue(bool value)
{
	if (value)
	{
		getAttributes()->append(ATT_VALUE, "true");
	}
	else
	{
		getAttributes()->append(ATT_VALUE, "false");
	}
}

void HtmlInput::setValue(int value)
{
	getAttributes()->append(ATT_VALUE, value);
}

HtmlPassword::HtmlPassword() : HtmlInput()
{
	getAttributes()->append(ATT_TYPE, ATT_PASSWORD);
};

HtmlUpload::HtmlUpload() : HtmlInput()
{
	getAttributes()->append(ATT_TYPE, ATT_FILE);
}

HtmlSubmit::HtmlSubmit() : HtmlInput()
{
	getAttributes()->append(ATT_TYPE, ATT_SUBMIT);
}

HtmlText::HtmlText() : HtmlInput()
{
	getAttributes()->append(ATT_TYPE, ATT_TEXT);
};

void HtmlNumber::print(char *text)
{
	this->getAttributes()->append(ATT_MAX, max);
	this->getAttributes()->append(ATT_MIN, min);
	this->getAttributes()->append(ATT_STEP, (float)step);
	HtmlInput::print(text);
}

HtmlNumber::HtmlNumber() : HtmlInput()
{
	getAttributes()->append(ATT_TYPE, ATT_NUMBER);
};

void HtmlNumber::setMax(int max)
{
	this->max = max;
}

int HtmlNumber::getMax()
{
	return max;
}

void HtmlNumber::setMin(int min)
{
	this->min = min;
}

int HtmlNumber::getMin()
{
	return this->min;
}

void HtmlNumber::setStep(float step)
{
	this->step = step;
}

HtmlHidden::HtmlHidden() : HtmlInput()
{
	getAttributes()->append(ATT_TYPE, ATT_HIDDEN);
};

void HtmlCheckBox::print(char *text)
{
	HtmlHidden *hidden = new HtmlHidden();
	hidden->setId(this->getId());
	hidden->setName(this->getName());
	hidden->setValue(false);
	
	((HtmlTagAccess *)hidden)->print(text);
	char buf[180];
	sprintf(buf, "%s_1", this->getId());
	this->setId(buf);
	buf[0] = '\0';
	sprintf(buf, "document.getElementById('%s').disabled = document.getElementById('%s').checked;", hidden->getId(), this->getId());
	getAttributes()->append("onClick", buf);
	getAttributes()->append(ATT_VALUE, "true");
	if (state)
		getAttributes()->append(ATT_CHECKED);
	delete hidden;

	HtmlInput::print(text);
}

HtmlCheckBox::HtmlCheckBox() : HtmlInput()
{
	getAttributes()->append(ATT_TYPE, ATT_CHECKBOX);
};

void HtmlCheckBox::setValue(bool value)
{
	state = value;
}

void HtmlOption::print(char *text)
{
	if (selected)
		getAttributes()->append(ATT_SELECTED);
	HtmlCustomTag::print(text);
}

HtmlOption::HtmlOption() : HtmlCustomTag(ATT_OPTION) {}

void HtmlOption::setKey(char *key)
{
	this->getAttributes()->append(ATT_VALUE, key);
}

void HtmlOption::setKey(int key)
{
	this->getAttributes()->append(ATT_VALUE, key);
}

int HtmlOption::getKeyInt()
{
	return this->getAttributes()->getIntValueByKey(ATT_VALUE);
}

char *HtmlOption::getKey()
{
	return this->getAttributes()->getStringValueByKey(ATT_VALUE);
}

bool HtmlOption::getSelected()
{
	return selected;
}

void HtmlOption::setSelected(bool selected)
{
	this->selected = selected;
}

HtmlComboBox::HtmlComboBox() : HtmlInputableTag(ATT_SELECT)
{

}

void HtmlComboBox::append(char *key, char *value)
{
	HtmlOption *option = new HtmlOption();
	option->getAttributes()->append(ATT_VALUE, key);
	option->setText(value);
	XmlArray::append(option);
}


void HtmlComboBox::append(const char *key, const char *value)
{
	this->append((char*)key, (char*)value);
}

void HtmlComboBox::append(int key, char *value)
{  
	char buf[10];
	sprintf(buf, "%d", key);
	append(buf, value);
}

void HtmlComboBox::append(int key, const char *value)
{  
	this->append(key, (char*) value);
}

void HtmlComboBox::setSelected(int key){
	char buf[10];
	sprintf(buf, "%d", key);
	setSelected(buf);
}

void HtmlComboBox::setSelected(char *key)
{
	HtmlOption *current = (HtmlOption *)HtmlCustomTag::getFirst();
	do
	{
		if (current != nullptr)
		{
			if (strcmp(current->getKey(), key) == 0)
			{
				current->setSelected(true);
				break;
			}
		}
	} while ((current = (HtmlOption *)this->getNext(current)) != nullptr);
}

void HtmlComboBox::setSelected(const char *key){
	this->setSelected((char*) key);
}

void HtmlHeader::print(char *text)
{
	if ((style != nullptr) && (!this->hasObject(style)))
		this->insert(style, ipFirst);
	if ((title != nullptr) && (!this->hasObject(title)))
		this->insert(title, ipFirst);
	XmlTag::print(text);
}

HtmlStyle *HtmlHeader::getStyle(){
	if (style == nullptr)
		style = new HtmlStyle();
	return style; 
}

HtmlHeader::HtmlHeader(const char *name) : XmlTag(name){};

void HtmlHeader::setTitle(char *title)
{
	if (this->title == nullptr)
		this->title = new HtmlCustomTag(TAG_TITLE);
	this->title->setText(title);
}


void HtmlHeader::append(IPrintableObject *val){
	XmlArray::append(val);
}

HtmlPage::HtmlPage() : XmlTag(PAGE_BODY){};

HtmlPage::~HtmlPage()
{
	if (header != nullptr)
		delete header;
	
	if (footer != nullptr)
		delete footer;
}

HtmlHeader *HtmlPage::getHeader()
{
	if (header == nullptr)
		header = new HtmlHeader(PAGE_HEADER);
	return header;
}

HtmlTag *HtmlPage::getFooter()
{
	if (footer == nullptr)
		footer = new HtmlTag(PAGE_FOOTER);
	return footer;
}

void HtmlPage::print(char *text)
{
	strcat(text, "<html>");
	if (header != nullptr)
		((HtmlTagAccess *)header)->print(text);
	XmlTag::print(text);
	if (footer != nullptr)
		((HtmlTagAccess *)footer)->print(text);
	strcat(text, "</html>");
}

void HtmlPage::append(IPrintableObject *val){
	XmlArray::append(val);
}

void HtmlCols::print(char *text)
{
	IPrintableObject *current = getFirst();
	do
	{
		if ((current != nullptr) && (!((XmlArray *)current)->isEmpty()))
		{
			((HtmlTagAccess *)current)->print(text);
		}
	} while ((current = this->getNext(current)) != nullptr);
}

void HtmlCols::append(IPrintableObject *val){
	XmlArray::append(val);
}

void HtmlRow::print(char *text)
{
	setText((IPrintableObject *)cols);
	XmlTag::print(text);
}

HtmlRow::HtmlRow(int col) : HtmlCustomTag(TAG_TR)
{
	cols = new HtmlCols();
	for (int i = 0; i < col; i++)
		cols->append(new HtmlTag(TAG_TD));
}

HtmlCustomTag *HtmlRow::getCol(int i)
{
	return (HtmlCustomTag *)cols->get(i);
}

void HtmlTable::print(char *text)
{
	setText((IPrintableObject *)rows);
	HtmlCustomTag::print(text);
}

HtmlTable::HtmlTable(int cols) : HtmlCustomTag(TAG_TABLE)
{
	rows = new HtmlRows();
	this->cols = cols;
}

int HtmlTable::append()
{
	return rows->append(new HtmlRow(cols));
}

void HtmlTable::append(int rows)
{
	for (int i = 0; i < rows; i++)
		this->append();
}

void HtmlTable::setCell(int row, int col, IPrintableObject *value)
{
	((HtmlRow *)rows->get(row))->getCol(col)->setText(value);
}

void HtmlTable::setCell(int row, int col, double value)
{
	((HtmlRow *)rows->get(row))->getCol(col)->setText(value);
}

void HtmlTable::setCell(int row, int col, int value)
{
	((HtmlRow *)rows->get(row))->getCol(col)->setText(value);
}

void HtmlTable::setCell(int row, int col, float value)
{
	((HtmlRow *)rows->get(row))->getCol(col)->setText(value);
}

void HtmlTable::setCell(int row, int col, const char *value)
{
	((HtmlRow *)rows->get(row))->getCol(col)->setText(value);
}

HtmlStyleAttributeArray *HtmlTable::getCellStyle(int row, int col)
{
	return ((HtmlRow *)rows->get(row))->getCol(col)->getStyle();
}

XmlAttributes *HtmlTable::getCellAttribute(int row, int col)
{
	return ((HtmlRow *)rows->get(row))->getCol(col)->getAttributes();
}

HtmlStyleAttributeArray *HtmlTable::getRowStyle(int row)
{
	return ((HtmlRow *)rows->get(row))->getStyle();
}

HtmlForm::HtmlForm(char *id, char* url, HtmlFormMethod method) : HtmlTag(TAG_FORM)
{
	getAttributes()->append(TAG_ID, id);
	getAttributes()->append(ATT_ACTION, url);
	if (method == Get)
	{
		getAttributes()->append(ATT_METHOD, ATT_GET_METHOD);
	}
	else
	{
		getAttributes()->append(ATT_METHOD, ATT_POST_METHOD);
	}
}

HtmlLink::HtmlLink(char *link, char *caption) : HtmlCustomTag(TAG_A)
{
	this->getAttributes()->append(ATT_HREF, link);
	this->setText(caption);
}

HtmlLink::HtmlLink(char *link) : HtmlCustomTag(TAG_A)
{
	this->getAttributes()->append(ATT_HREF, link);
	this->setText(link);
}

HtmlStyleGroup::HtmlStyleGroup(char *tag){
	this->tag = new char[strlen(tag) + 1];
	this->tag[0] = '\0';
	strcat(this->tag, tag);
}

HtmlStyleGroup::~HtmlStyleGroup(){
	delete[] tag;
}

void HtmlStyleGroup::print(char *text){
	strcat(text, tag);
	strcat(text, " {");
	HtmlStyleAttributeArray *current = (HtmlStyleAttributeArray *)this->getFirst();
	do
	{
		if (current != nullptr)
		{
			((HtmlTagAccess *)current)->print(text);
		}
	} while ((current = (HtmlStyleAttributeArray *)this->getNext(current)) != nullptr);
	strcat(text, "} ");
}

HtmlStyleGroup* HtmlStyle::createGroup(char *tag){
	HtmlStyleGroup* group = new HtmlStyleGroup(tag);
	append(group);
	return group;
}

HtmlStyle::HtmlStyle() : XmlTag(TAG_STYLE){
	;
}

HtmlButton::HtmlButton(char *caption, char *formId) : HtmlCustomTag(TAG_BUTTON){
	setText(caption);
	this->getAttributes()->append(TAG_FORM, formId);
}

HtmlButton::HtmlButton(char *caption): HtmlCustomTag(TAG_BUTTON){
	setText(caption);
}


HtmlTag::HtmlTag(char *tag): HtmlCustomTag(tag){
	;
}

HtmlTag::HtmlTag(const char *tag): HtmlCustomTag(tag){
	;
}

void HtmlTag::append(IPrintableObject *val){
	XmlArray::append(val);
}

int HtmlRows::append(IPrintableObject *val){
	return XmlArray::append(val);
}	