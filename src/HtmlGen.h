/*
html.h - Library for easy generate Html page.
Created by Yuri Tarzimanov, November 06, 2017.
Released into the public domain.
*/

#ifndef HTMLGEN_H
#define HTMLGEN_H

#include "XmlGen.h"

using namespace Xml;

enum HtmlFormMethod
{
	Get,
	Post
};

class HtmlStyleAttribute : public IChildObject
{
private:
	char *key;
	char *val;

protected:
	void print(char *text);

public:
	HtmlStyleAttribute(const char *key, char *val);
	HtmlStyleAttribute(const char *key, int val);
	HtmlStyleAttribute(const char *key, float val);
	HtmlStyleAttribute(const char *key, const char *val);

	~HtmlStyleAttribute();
};

class HtmlStyleAttributeArray : public XmlArray
{
protected:
	void print(char *text);
public:
	void append(const char *key,  int value); 
	void append(const char *key,  float value); 
	void append(const char *key,  char *value);
	void append(const char *key,  const char *value);  
};

class HtmlStyleGroup : public HtmlStyleAttributeArray{
private:
	char *tag;
protected:
	 void print(char *text);
public:
	 HtmlStyleGroup(char *tag);
	 ~HtmlStyleGroup();
};

class HtmlCustomTag : public XmlTag
{
private:
	HtmlStyleAttributeArray *style = nullptr;
protected:
	void print(char *text);
public:
	HtmlCustomTag(char *tag);
	HtmlCustomTag(const char *tag);
	
	HtmlStyleAttributeArray *getStyle();
};

class HtmlTag : public HtmlCustomTag
{
public:
	HtmlTag(char *tag);
    HtmlTag(const char *tag);


	void append(IPrintableObject *val);
};

class HtmlStyle : public XmlTag{
public:
	HtmlStyle();
	HtmlStyleGroup* createGroup(char *tag);
};

class HtmlInputableTag : public HtmlCustomTag{
private:
	bool disabled = false;
	int size = 20;	
protected:
	void print(char *text);

public:
	HtmlInputableTag(const char *tag);
	
	int getSize();
	void setSize(int size);
	void setDisabled(bool state);
	bool isDisabled();

	void setName(char *name);
	void setName(const char *name);
	char *getName();
	char *getId();
	void setId(char *id);
};

class HtmlInput : public HtmlInputableTag
{
private:
	bool state = false;

protected:
	void print(char *text);

public:
	HtmlInput();

	void setValue(char *value);
	void setValue(float value);
	void setValue(bool value);
	void setValue(int value);
};

class HtmlPassword : public HtmlInput
{
public:
	HtmlPassword();
};

class HtmlUpload : public HtmlInput
{
public:
	HtmlUpload();
};

class HtmlSubmit : public HtmlInput
{
public:
	HtmlSubmit();
};

class HtmlText : public HtmlInput
{
public:
	HtmlText();
};

class HtmlNumber : public HtmlInput
{
private:
	int max = 0;
	int min = 9999;
	float step = 1;

protected:
	void print(char *text);

public:
	HtmlNumber();
	void setMax(int max);
	int getMax();
	void setMin(int min);
	int getMin();
	void setStep(float step);
};

class HtmlHidden : public HtmlInput
{
public:
	HtmlHidden();
};

class HtmlCheckBox : public HtmlInput
{
private:
	bool state = false;
	
protected:
	void print(char *text);

public:
	HtmlCheckBox();

	void setValue(bool value);
};

class HtmlOption : public HtmlCustomTag
{
private:
	bool selected = false;

protected:
	void print(char *text);

public:
	HtmlOption();
	void setKey(char *key);
	void setKey(int key);
	int getKeyInt();
	char *getKey();
	bool getSelected();
	void setSelected(bool selected);
};

class HtmlComboBox : public HtmlInputableTag
{
public:
	HtmlComboBox();
	void append(char *key, char *value);
	void append(const char *key, const char *value);
	void append(int key, char *value);
	void append(int key, const char *value);
	void setSelected(char *key);
	void setSelected(const char *key);
	void setSelected(int key);
};

class HtmlHeader : public XmlTag
{
private:
	HtmlCustomTag *title = nullptr;
	HtmlStyle *style = nullptr;
protected:
	void print(char *text);

public:
	HtmlHeader(const char *name);
	void setTitle(char *title);
	HtmlStyle *getStyle();

	void append(IPrintableObject *val);
};

class HtmlPage : public XmlTag
{
private:
	HtmlHeader *header = nullptr;
	HtmlTag *footer = nullptr;

public:
	HtmlPage();
	~HtmlPage();
	HtmlHeader *getHeader();
	HtmlTag *getFooter();
	void append(IPrintableObject *val);
	void print(char *text);
};

class HtmlCols : public XmlArray
{
protected:
	void print(char *text);
public:
	void append(IPrintableObject *val);
};

class HtmlRow : public HtmlCustomTag
{
private:
	HtmlCols *cols;

protected:
	void print(char *text);

public:
	HtmlRow(int col);
	HtmlCustomTag *getCol(int i);
};

class HtmlRows : public XmlArray{
public:
	int append(IPrintableObject *val);	
};

class HtmlTable : public HtmlCustomTag
{
private:
	HtmlRows *rows = nullptr;
	int cols = 0;

protected:
	void print(char *text);

public:
	HtmlTable(int cols);
	int append();
	void append(int rows);
	void setCell(int row, int col, IPrintableObject *value);
	void setCell(int row, int col, int value);
	void setCell(int row, int col, float value);
	void setCell(int row, int col, double value);
	void setCell(int row, int col, const char *value);
	HtmlStyleAttributeArray *getCellStyle(int row, int col);
	XmlAttributes *getCellAttribute(int row, int col);
	HtmlStyleAttributeArray *getRowStyle(int row);
};

class HtmlLink : public HtmlCustomTag
{
public:
	HtmlLink(char *link, char *caption);
	HtmlLink(char *link);
};

class HtmlForm : public HtmlTag
{
public:
	HtmlForm(char *id, char *url, HtmlFormMethod method);
};


class HtmlButton : public HtmlCustomTag
{
public:
	HtmlButton(char *caption, char *formId);
	HtmlButton(char *caption);
};

#endif
