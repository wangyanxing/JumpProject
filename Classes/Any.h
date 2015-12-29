//
//  Any.h
//  jumpproj
//
//  Created by Yanxing Wang on 12/29/15.
//
//

#ifndef Any_h
#define Any_h

#include <algorithm>
#include <typeinfo>

class Any {
public:
  Any() : mContent(0){
  }

  template<typename ValueType>
  explicit Any(const ValueType & value)
    : mContent(new holder<ValueType>(value)) {
  }

  Any(const Any & other)
    : mContent(other.mContent ? other.mContent->clone() : 0) {
  }

  virtual ~Any() {
    destroy();
  }

public:
  Any& swap(Any & rhs) {
    std::swap(mContent, rhs.mContent);
    return *this;
  }

  template<typename ValueType>
  Any& operator=(const ValueType & rhs) {
    Any(rhs).swap(*this);
    return *this;
  }

  Any & operator=(const Any & rhs) {
    Any(rhs).swap(*this);
    return *this;
  }

public:
  bool isEmpty() const {
    return !mContent;
  }

  const std::type_info& getType() const {
    return mContent ? mContent->getType() : typeid(void);
  }

  void destroy() {
    delete mContent;
    mContent = NULL;
  }

protected:
  class placeholder {
  public:
    virtual ~placeholder() {
    }

  public:
    virtual const std::type_info& getType() const = 0;

    virtual placeholder * clone() const = 0;
  };

  template<typename ValueType>
  class holder : public placeholder {
  public:
    holder(const ValueType & value)
      : held(value) {
    }

  public:
    virtual const std::type_info & getType() const {
      return typeid(ValueType);
    }

    virtual placeholder * clone() const {
      return new holder(held);
    }

  public:
    ValueType held;
  };

protected:
  placeholder * mContent;

  template<typename ValueType>
  friend ValueType * any_cast(Any *);

public:
  template<typename ValueType>
  ValueType operator()() const {
    if (!mContent) {
      CCLOGERROR("Any::operator() content is NULL");
    } else if(getType() == typeid(ValueType)) {
      return static_cast<Any::holder<ValueType> *>(mContent)->held;
    } else {
      CCLOGERROR("Any::operator() error type : %s, %s", getType().name(), typeid(ValueType).name());
    }
  }
};

template<typename ValueType>
ValueType * any_cast(Any * operand) {
  return operand && operand->getType() == typeid(ValueType)
    ? &static_cast<Any::holder<ValueType> *>(operand->mContent)->held
    : 0;
}

template<typename ValueType>
const ValueType * any_cast(const Any * operand) {
  return any_cast<ValueType>(const_cast<Any *>(operand));
}

template<typename ValueType>
ValueType any_cast(const Any & operand) {
  const ValueType * result = any_cast<ValueType>(&operand);
  if(!result) {
    CCLOG("any_cast failed");
  }
  return *result;
}

#endif /* Any_h */
