#include "Model.h"
#include <ctime>
#include "PgmStringTools.hpp"
#include "EnumTools.hpp"

namespace duison {

bool stringHasChar(String &string, char find) {
  for (char c : string) {
    if (c == find) {
      return true;
    }
  }
  return false;
}

bool timestampFromString(String &time, long &dest) {
  // `time` should be a datetime string formatted as YYYY-MM-DDThh:mm:ss
  if (time.length() < 19) {
    return false;
  }
  const char *str = time.c_str();
  char *end;
  std::tm t = {};
  t.tm_year = strtol(&str[0], &end, 10) - 1900;
  t.tm_mon = strtol(&str[5], &end, 10) - 1;
  t.tm_mday = strtol(&str[8], &end, 10);
  t.tm_hour = strtol(&str[11], &end, 10);
  t.tm_min = strtol(&str[14], &end, 10);
  t.tm_sec = strtol(&str[17], &end, 10);
  dest = mktime(&t);
  return true;
}

bool parseArrayOfIntStrings(json::JsonParser &parser, std::vector<uint8_t> &dest) {
  if (parser.checkValueType() != json::ARRAY) {
    parser.skipValue();
    return false;
  }
  String value;
  json::JsonArrayIterator<String> array = parser.iterateArray<String>();
  while (array.hasNext()) {
    if (array.getNext(value)) {
      dest.push_back(value.toInt());
    }
  }
  array.finish();
  return true;
}

////////////////////////////////////////////////////////////////
// Class : NamedValue //////////////////////////////////////////
////////////////////////////////////////////////////////////////

NamedValue::NamedValue(std::shared_ptr<String> &prefix) : prefix_(prefix), type_(NamedValue::Type::UNKNOWN) {
  //
}


bool NamedValue::onKey(String &key, json::JsonParser &parser) {
  if (prefix_) {
    int index = key.indexOf(*prefix_.get());
    if (index == -1) {
      name_ = key;
    } else {
      name_ = key.substring(index + prefix_.get()->length());
    }
  } else {
    name_ = key;
  }
  bool success;
  switch (parser.checkValueType()) {
    case json::BOOL:
      type_ = Type::BOOL;
      bool value;
      success = parser.get(value);
      value_ = value ? "1" : "0";
      break;
    case json::NUMBER:
      success = parser.getAsString(value_);
      type_ = stringHasChar(value_, '.') ? Type::FLOAT : Type::INT;
      break;
    case json::STRING:
      type_ = Type::STRING;
      success = parser.get(value_);
      break;
    case json::INVALID:
    case json::OBJECT:
    case json::ARRAY:
    case json::NUL:
    default:
      success = false;
      break;
  }
  return success;
}


const String &NamedValue::getPrefix() const {
  return *prefix_.get();
}


NamedValue::Type NamedValue::getType() const {
  return type_;
}


const String &NamedValue::getName() const {
  return name_;
}


String NamedValue::getFullName() const {
  if (prefix_) {
    return *prefix_.get() + name_;
  } else {
    return name_;
  }
}


const String &NamedValue::getString() const {
  return value_;
}


int NamedValue::getInt() const {
  return value_.toInt();
}


float NamedValue::getFloat() const {
  return value_.toFloat();
}


bool NamedValue::getBool() const {
  return value_ == "1";
}

}