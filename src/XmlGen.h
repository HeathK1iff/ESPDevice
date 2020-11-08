/*
xml.h - Library for easy generate xml tags.
Created by Yuri Tarzimanov, November 06, 2017.
Released into the public domain.
*/

#ifndef XMLGEN_H
#define XMLGEN_H

#include <stdio.h>
#include <string.h>

static int objectLeaks = 0;

namespace Xml
{
	enum InsertPosition
	{
		ipFirst,
		ipLast
	};

	class IPrintableObject
	{
	protected:
		virtual void print(char *text);

	public:
		IPrintableObject();
		virtual ~IPrintableObject();
	};

	class IChildObject : public IPrintableObject
	{
	private:
		IChildObject *child = nullptr;

	protected:
		void setChild(IChildObject *child);
		IChildObject *getChild();

	public:
		~IChildObject();
	};

	class XmlValue : public IChildObject
	{
	private:
		char *value;

	protected:
		void print(char *text);
		void setValue(const char *value);
	public:
		XmlValue(const char *value);
		XmlValue(int value);
		XmlValue(float value);
		XmlValue(double value);
		~XmlValue();
		char *getStringValue();
		int getIntValue();
		float getFloatValue();
	};

	class XmlArray : public IChildObject
	{
	private:
		int count = 0;
		IPrintableObject *head = nullptr;
		IPrintableObject *last = nullptr;

	protected:
		IPrintableObject *getFirst();
		IPrintableObject *getNext(IPrintableObject *current);
		int insert(IPrintableObject *object, InsertPosition position);
		void print(char *text);
		int append(IPrintableObject *val);
	public:
		void clear();
		IPrintableObject *get(int index);
		bool hasObject(IPrintableObject *val);
		bool isEmpty();
		~XmlArray();
	};

	class XmlAttribute : public IChildObject
	{
	private:
		char *key;
		IPrintableObject *val = nullptr;

	protected:
		void print(char *text);

	public:
		XmlAttribute(const char *key, IPrintableObject *val);
		~XmlAttribute();
		char *getKey();
		void setValue(IPrintableObject *val);

		IPrintableObject *getObject();
		int getIntValue();
		float getFloatValue();
		char *getStringValue();
	};

	class XmlAttributes : public XmlArray
	{
	public:
		XmlAttribute *getAttributeByKey(const char *key);

		char *getStringValueByKey(const char *key);
		int getIntValueByKey(const char *key);
		float getFloatValueByKey(const char *key);

		XmlAttribute *append(const char *key, IPrintableObject *val);
		XmlAttribute *append(const char *key, int val);
		XmlAttribute *append(const char *key, float val);
		XmlAttribute *append(const char *key, char *val);
		XmlAttribute *append(const char *key, const char *val);
		XmlAttribute *append(const char *key);
	};

	class XmlTag : public XmlArray
	{
	private:
		char *name;
		XmlAttributes *attributes = nullptr;

	protected:
		void print(char *text);

	public:
		XmlTag(const char *name);
		
		~XmlTag();

		void setText(IPrintableObject *value);
		void setText(int value);
		void setText(float value);
		void setText(double value);
		void setText(const char *value);

		XmlAttributes *getAttributes();
	};

}
#endif