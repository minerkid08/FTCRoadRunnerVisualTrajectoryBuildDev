#include "CustomAction.hpp"

#include <cstdlib>
#include <cstring>
#include <utility>

CustomActionField::CustomActionField()
{
	name = (char*)malloc(64);
	strcpy(name, "new field");
	type = FIELDTYPE_INT;
	value.i = 0;
	rangeChecks = false;
	min.i = 0;
	max.i = 10;
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
		value.s = (char*)malloc(64);
		strcpy(value.s, other.value.s);
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
		value.s = (char*)malloc(64);
		strcpy(value.s, other.value.s);
	}
	else if (type == FIELDTYPE_STRING)
	{
		free(value.s);
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
		free(value.s);
}

void CustomActionField::reload(const CustomActionField& def)
{
	strcpy(name, def.name);
	rangeChecks = def.rangeChecks;
	min = def.min;
	max = def.max;
	step = def.step;
	if (def.type == type)
		return;

	if (type == FIELDTYPE_STRING)
		free(value.s);

	if (def.type == FIELDTYPE_STRING)
	{
		value.s = (char*)malloc(64);
		strcpy(value.s, def.value.s);
	}
	else
		value = def.value;
	type = def.type;
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

void customActionReload(std::vector<CustomActionField>* fields, const CustomActionDef& def)
{
	std::vector<CustomActionField> orig = *fields;

	fields->resize(def.fields.size());

	for (int i = 0; i < def.fields.size(); i++)
	{
		bool found = false;
		for (const CustomActionField& field : orig)
		{
			if (strcmp(field.name, def.fields[i].name) == 0)
			{
				(*fields)[i] = field;
				(*fields)[i].reload(def.fields[i]);
				found = true;
				break;
			}
		}
		if (!found)
			(*fields)[i] = (def.fields[i]);
	}
}
