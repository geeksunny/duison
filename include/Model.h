#ifndef SPHUE_INCLUDE_MODELS_H_
#define SPHUE_INCLUDE_MODELS_H_

#include <JSON.h>
#include <vector>

namespace duison {

class NamedValue : public json::JsonModel {
 public:
  NamedValue() = default;
  explicit NamedValue(std::shared_ptr<String> &prefix);
  enum Type {
    UNKNOWN,
    BOOL,
    INT,
    FLOAT,
    STRING
  };
  const String &getPrefix() const;
  Type getType() const;
  const String &getName() const;
  String getFullName() const;
  const String &getString() const;
  int getInt() const;
  float getFloat() const;
  bool getBool() const;
 private:
  std::shared_ptr<String> prefix_;
  Type type_ = Type::UNKNOWN;
  String name_;
  String value_;
  bool onKey(String &key, json::JsonParser &parser) override;
};

template<typename K, typename T>
class ParsedMap : public json::JsonModel {
 public:
  std::map<K, T> &operator*() {
    return values_;
  }
  const std::map<K, T> &operator*() const {
    return values_;
  }
 protected:
  std::map<K, T> values_;
  virtual inline K getKey(String &from) = 0;
 private:
  bool onKey(String &key, json::JsonParser &parser) override {
    T value;
    bool success = parser.get(value);
    values_[getKey(key)] = value;
    return success;
  }
};

template<typename T>
class ParsedIntMap : public ParsedMap<uint8_t, T> {
 protected:
  uint8_t getKey(String &from) override {
    return from.toInt();
  }
};

template<typename T>
class ParsedStringMap : public ParsedMap<String, T> {
 protected:
  String getKey(String &from) override {
    return from;
  }
};

template<typename T = json::JsonObject>
class BuildableObject : public T {
 public:
  String toJson() override {
    build();
    return T::toJson();
  }
 private:
  virtual void build() = 0;
};

}

#endif //SPHUE_INCLUDE_MODELS_H_
