#include "XmlGen.h"
#include <stdlib.h>
#include <stdlib_noniso.h>

using namespace Xml;

IPrintableObject::IPrintableObject()
{
	objectLeaks++;
}

IPrintableObject::~IPrintableObject()
{
	objectLeaks--;
}

class IPrintableAccess : public IPrintableObject
{
  public:
	void print(char *text)
	{
		IPrintableObject::print(text);
	}
};

class IChildAccess : public IChildObject
{
  public:
	void setChild(IChildObject *child)
	{
		IChildObject::setChild(child);
	}

	IChildObject *getChild()
	{
		return IChildObject::getChild();
	}
};

void IPrintableObject::print(char *text)
{
	strcat(text, " ");
}

void IChildObject::setChild(IChildObject *child)
{
	this->child = child;
}

IChildObject *IChildObject::getChild()
{
	return this->child;
}

IChildObject::~IChildObject()
{
	if (child != nullptr)
		delete child;
}

void XmlValue::print(char *text)
{
	strcat(text, value);
};


XmlValue::~XmlValue(){
	delete[] this->value;
}

XmlValue::XmlValue(const char *value)
{
	this->value = new char[strlen(value) + 1];
	this->value[0] = '\0';
	strcat(this->value, value);
}

XmlValue::XmlValue(int value)
{
	this->value = new char[10];
	sprintf(this->value, "%d", value);
}

XmlValue::XmlValue(float value)
{
	this->value = new char[15];
	dtostrf(value, 4, 2, this->value);
}

XmlValue::XmlValue(double value)
{
	this->value = new char[15];
	dtostrf(value, 4, 2, this->value);
}


char *XmlValue::getStringValue()
{
	return value;
}

int XmlValue::getIntValue()
{
	return atoi(value);
}

float XmlValue::getFloatValue()
{
	return atof(value);
}

IPrintableObject *XmlArray::getFirst()
{
	return (IPrintableObject *)head;
}

IPrintableObject *XmlArray::getNext(IPrintableObject *current)
{
	if (current != nullptr)
	{
		return (IPrintableObject *)((IChildAccess *)current)->getChild();
	}
	else
	{
		return nullptr;
	}
}

void XmlArray::print(char *text)
{
	IPrintableObject *current = getFirst();
	do
	{
		if (current != nullptr)
		{
			((IPrintableAccess *)current)->print(text);
		}
	} while ((current = this->getNext(current)) != nullptr);
}

int XmlArray::insert(IPrintableObject *object, InsertPosition position)
{
	if (head == nullptr)
	{
		head = (IPrintableAccess *)object;
		last = head;
		return count++;
	}

	if (position == ipLast)
	{
		((IChildAccess *)last)->setChild((IChildObject *)object);
		last = (IPrintableAccess *)object;
	}
	else
	{
		((IChildAccess *)object)->setChild(((IChildAccess *)head));
		head = (IChildAccess *)object;
	}
	return count++;
}

IPrintableObject *XmlArray::get(int index)
{
	int i = 0;
	IPrintableObject *current = getFirst();
	do
	{
		if (current != nullptr)
		{
			if (i == index)
				return current;
			i++;
		}
	} while ((current = this->getNext(current)) != nullptr);
	return nullptr;
}

int XmlArray::append(IPrintableObject *val)
{
	return insert(val, ipLast);
}

void XmlArray::clear()
{
	if (head != nullptr)
		delete head;
	head = nullptr;
	last = nullptr;
}

bool XmlArray::hasObject(IPrintableObject *val)
{
	IPrintableAccess *current = (IPrintableAccess *)getFirst();
	do
	{
		if (current != nullptr)
		{
			if (current == val)
			{
				return true;
			}
		}
	} while ((current = (IPrintableAccess *)this->getNext(current)) != nullptr);
	return false;
}

bool XmlArray::isEmpty()
{
	return head == nullptr;
}

XmlArray::~XmlArray()
{
	clear();
}

void XmlAttribute::print(char *text)
{
	IPrintableObject::print(text);
	strcat(text, key);
	if (val != nullptr)
	{
		strcat(text, "=");
		strcat(text, "\"");
		((IPrintableAccess *)val)->print(text);
		strcat(text, "\"");
	}
}

void XmlAttribute::setValue(IPrintableObject *val)
{
	if (this->val != nullptr)
	{
		delete this->val;
		this->val = nullptr;
	}
	this->val = val;
}

XmlAttribute::XmlAttribute(const char *key, IPrintableObject *val)
{
	this->key = new char[strlen(key) + 1];
	this->key[0] = '\0';
	strcat(this->key, key);
	setValue(val);
}

XmlAttribute::~XmlAttribute()
{
	delete[] this->key;
	if (val != nullptr)
		delete val;
	val = nullptr;
}

char *XmlAttribute::getKey()
{
	return this->key;
}

IPrintableObject *XmlAttribute::getObject()
{
	return this->val;
}

int XmlAttribute::getIntValue()
{
	return ((XmlValue *)this->val)->getIntValue();
}

float XmlAttribute::getFloatValue()
{
	return ((XmlValue *)this->val)->getFloatValue();
}

char *XmlAttribute::getStringValue()
{
	return ((XmlValue *)this->val)->getStringValue();
}

XmlAttribute *XmlAttributes::getAttributeByKey(const char *key)
{
	IPrintableAccess *current = (IPrintableAccess *)getFirst();
	do
	{
		if (current != nullptr)
		{
			if (strcmp(key, ((XmlAttribute *)current)->getKey()) == 0)
			{
				return (XmlAttribute *)current;
			}
		}
	} while ((current = (IPrintableAccess *)this->getNext(current)) != nullptr);
	return nullptr;
}

char *XmlAttributes::getStringValueByKey(const char *key)
{
	XmlAttribute *attr = getAttributeByKey(key);
	if (attr != nullptr)
		return attr->getStringValue();
	return "";
}

int XmlAttributes::getIntValueByKey(const char *key)
{
	XmlAttribute *attr = getAttributeByKey(key);
	if (attr != nullptr)
		return attr->getIntValue();
	return 0;
}

float XmlAttributes::getFloatValueByKey(const char *key)
{
	XmlAttribute *attr = getAttributeByKey(key);
	if (attr != nullptr)
		return attr->getFloatValue();
	return 0;
}

XmlAttribute *XmlAttributes::append(const char *key, IPrintableObject *val)
{
	XmlAttribute *item = this->getAttributeByKey(key);

	if (item == nullptr)
	{
		item = new XmlAttribute(key, val);
		XmlArray::append(item);
		return item;
	}

	item->setValue(val);

	return item;
}

XmlAttribute *XmlAttributes::append(const char *key, int val)
{
	return append(key, new XmlValue(val));
}

XmlAttribute *XmlAttributes::append(const char *key, float val)
{
	return append(key, new XmlValue(val));
}

XmlAttribute *XmlAttributes::append(const char *key, char *val)
{
	return append(key, new XmlValue(val));
}

XmlAttribute *XmlAttributes::append(const char *key, const char *val)
{
	return append(key, new XmlValue(val));
}

XmlAttribute *XmlAttributes::append(const char *key)
{
	return append(key, (XmlAttribute *)nullptr);
}

void XmlTag::print(char *text)
{
	strcat(text, "<");
	strcat(text, name);
	if ((attributes != nullptr) && (!attributes->isEmpty()))
	{
		((IPrintableAccess *)attributes)->print(text);
	}
	if (this->isEmpty())
	{
		strcat(text, "/>");
		return;
	}

	strcat(text, ">");
	XmlArray::print(text);
	strcat(text, "</");
	strcat(text, name);
	strcat(text, ">");
}

XmlTag::XmlTag(const char *name)
{
	this->name = new char[strlen(name) + 1];
	this->name[0] = '\0';
	strcat(this->name, name);
}

XmlTag::~XmlTag()
{
	delete[] name;
	if (attributes != nullptr)
		delete attributes;
}

void XmlTag::setText(IPrintableObject *value)
{
	if (value != nullptr){
		if (!this->isEmpty())
		{
			this->clear();
		}
		this->append(value);
	} else {
		this->append(new XmlValue(0));
	}
}

void XmlTag::setText(double value)
{
	setText(new XmlValue(value));
}

void XmlTag::setText(int value)
{
	setText(new XmlValue(value));
}

void XmlTag::setText(float value)
{
	setText(new XmlValue(value));
}

void XmlTag::setText(const char *value)
{
	setText(new XmlValue(value));
}

XmlAttributes *XmlTag::getAttributes()
{
	if (attributes == nullptr)
	{
		attributes = new XmlAttributes();
	}
	return attributes;
}

