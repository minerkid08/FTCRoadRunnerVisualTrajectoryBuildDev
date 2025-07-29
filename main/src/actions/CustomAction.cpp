#include "CustomAction.hpp"

#include <cstdlib>
#include <cstring>
#include <utility>

CustomActionField::CustomActionField()
{
	name = (char*)malloc(64);
	strcpy(name, "new field");
	type = FIELDTYPE_INT;
	value = 0;
	rangeChecks = false;
	min = (int*)0;
	max = (int*)10;
}

CustomActionField::CustomActionField(const CustomActionField& other)
{
	name = (char*)malloc(64);
	strcpy(name, other.name);
	type = other.type;
	rangeChecks = other.rangeChecks;
	min = other.min;
	max = other.max;
	if (type == FIELDTYPE_STRING)
	{
		value = (char*)malloc(64);
		strcpy((char*)value, (char*)other.value);
	}
	else
		value = other.value;
}

CustomActionField::CustomActionField(const CustomActionField&& other)
{
	name = other.name;
	type = other.type;
	value = other.value;
	rangeChecks = other.rangeChecks;
	min = other.min;
	max = other.max;
}

CustomActionField& CustomActionField::operator=(const CustomActionField& other)
{
	strcpy(name, other.name);
	if (type == other.type)
		value = other.value;
	else if (other.type == FIELDTYPE_STRING)
	{
		value = (char*)malloc(64);
		strcpy((char*)value, (char*)other.value);
	}
	else if (type == FIELDTYPE_STRING)
	{
		free(value);
		value = other.value;
	}
	else
		value = other.value;
	type = other.type;
	rangeChecks = other.rangeChecks;
	min = other.min;
	max = other.max;

	return *this;
}

CustomActionField::~CustomActionField()
{
	free(name);
	if (type == FIELDTYPE_STRING)
		free(value);
}

CustomActionDef::CustomActionDef()
{
	name = (char*)malloc(64);
	strcpy(name, "new action");
}

CustomActionDef::CustomActionDef(const CustomActionDef& other)
{
	name = (char*)malloc(64);
	strcpy(name, other.name);
	fields = other.fields;
}

CustomActionDef::CustomActionDef(const CustomActionDef&& other)
{
	name = (char*)malloc(64);
	strcpy(name, other.name);
	fields = std::move(other.fields);
}

CustomActionDef& CustomActionDef::operator=(const CustomActionDef& other)
{
	strcpy(name, other.name);
	fields = other.fields;
	return *this;
}

CustomActionDef::~CustomActionDef()
{
	free(name);
}
