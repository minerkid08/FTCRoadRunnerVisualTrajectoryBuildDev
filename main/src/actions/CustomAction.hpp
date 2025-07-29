#pragma once

#include <vector>

#define FIELDTYPE_INT 0
#define FIELDTYPE_DOUBLE 1
#define FIELDTYPE_BOOL 2
#define FIELDTYPE_STRING 3

struct CustomActionField
{
	int type;
	char* name;
	void* value;

	bool rangeChecks = false;
	void* min = 0;
	void* max = (void*)10;
	void* step = (void*)1;

	CustomActionField();
	CustomActionField(const CustomActionField& other);
	CustomActionField(const CustomActionField&& other);
	CustomActionField& operator=(const CustomActionField& other);
	~CustomActionField();

  void reload(const CustomActionField& def);
};

struct CustomActionDef
{
	char* name;
	std::vector<CustomActionField> fields;

	CustomActionDef();
	CustomActionDef(const CustomActionDef& other);
	CustomActionDef(const CustomActionDef&& other);
	CustomActionDef& operator=(const CustomActionDef& other);
	~CustomActionDef();
};

void customActionReload(std::vector<CustomActionField>* fields, const CustomActionDef& def);
