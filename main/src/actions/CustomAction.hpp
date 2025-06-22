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
  
  CustomActionField();
  CustomActionField(const CustomActionField& other);
  CustomActionField(const CustomActionField&& other);
  CustomActionField& operator=(const CustomActionField& other);
  ~CustomActionField();
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
