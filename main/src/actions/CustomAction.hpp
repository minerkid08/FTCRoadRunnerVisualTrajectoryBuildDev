#pragma once

#include <vector>

#define FIELDTYPE_INT 0
#define FIELDTYPE_DOUBLE 1
#define FIELDTYPE_BOOL 2
#define FIELDTYPE_STRING 3

union Value 
{
  int i;
  float f;
  bool b;
  char* s;
};

struct CustomActionField
{
	int type;
	char* name;
	Value value;

	bool rangeChecks = false;
	Value min;
	Value max;
	Value step;

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
