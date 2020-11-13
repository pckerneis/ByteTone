/*
  ==============================================================================

    Value.h
    Created: 13 Nov 2020 6:06:22pm
    Author:  Pierre

  ==============================================================================
*/

#pragma once

class BaseValue
{
public:
    virtual ~BaseValue();
    virtual bool isTruthy() = 0;

    virtual bool toBool() = 0;
    virtual int toInt() = 0;
    virtual float toFloat() = 0;

    virtual BaseValue* substract(BaseValue* other);
    virtual BaseValue* multiply(BaseValue* other);
    virtual BaseValue* divide(BaseValue* other);
    virtual BaseValue* add(BaseValue* other);
    virtual BaseValue* modulus(BaseValue* other);

    virtual BaseValue* bitwiseAnd(BaseValue* other);
    virtual BaseValue* bitwiseOr(BaseValue* other);
    virtual BaseValue* bitwiseXor(BaseValue* other);

    virtual BaseValue* bitShiftLeft(BaseValue* other);
    virtual BaseValue* bitShiftRight(BaseValue* other);

    virtual BaseValue* greater(BaseValue* other);
    virtual BaseValue* greaterEqual(BaseValue* other);
    virtual BaseValue* less(BaseValue* other);
    virtual BaseValue* lessEqual(BaseValue* other);
    virtual BaseValue* equalEqual(BaseValue* other);
    virtual BaseValue* notEqual(BaseValue* other);

    virtual BaseValue* negate();
    virtual BaseValue* bitwiseComplement();
};

template <typename Type>
class Primitive : public BaseValue
{
public:
    virtual ~Primitive();

    bool isTruthy() override;

    bool toBool() override { return value; }
    int toInt() override { return value; }
    float toFloat() override { return value; }

protected:
    Primitive(Type initialValue) : value(initialValue) {}

    Type value;
};

class Bool : public Primitive<bool>
{
public:
    Bool(bool initialValue) : Primitive(initialValue) {}
    virtual ~Bool() {}
};

class Int : public Primitive<int>
{
public:
    Int(int initialValue) : Primitive(initialValue) {}
    virtual ~Int() {}

    BaseValue* substract(BaseValue* other)  override { return new Int(value - other->toInt()); }
    BaseValue* add(BaseValue* other)        override { return new Int(value + other->toInt()); }
    BaseValue* divide(BaseValue* other)     override { return new Int(value / other->toInt()); }
    BaseValue* multiply(BaseValue* other)   override { return new Int(value * other->toInt()); }
    BaseValue* modulus(BaseValue* other)    override { return new Int(value % other->toInt()); }

    BaseValue* bitwiseAnd(BaseValue* other) override { return new Int(value & other->toInt()); }
    BaseValue* bitwiseOr(BaseValue* other)  override { return new Int(value | other->toInt()); }
    BaseValue* bitwiseXor(BaseValue* other) override { return new Int(value ^ other->toInt()); }

    BaseValue* bitShiftLeft(BaseValue* other)  override { return new Int(value << other->toInt()); }
    BaseValue* bitShiftRight(BaseValue* other) override { return new Int(value >> other->toInt()); }

    BaseValue* greater(BaseValue* other)       override { return new Bool(value >  other->toInt()); }
    BaseValue* greaterEqual(BaseValue* other)  override { return new Bool(value >= other->toInt()); }
    BaseValue* less(BaseValue* other)          override { return new Bool(value <  other->toInt()); }
    BaseValue* lessEqual(BaseValue* other)     override { return new Bool(value <= other->toInt()); }
    BaseValue* equalEqual(BaseValue* other)    override { return new Bool(value == other->toInt()); }
    BaseValue* notEqual(BaseValue* other)      override { return new Bool(value != other->toInt()); }

    BaseValue* negate() { return new Int(-value); }
    BaseValue* bitwiseComplement() { return new Int(~value); }
};

class Float : public Primitive<float>
{
public:
    Float(float initialValue) : Primitive(initialValue) {}
    virtual ~Float() {}
};

template<typename Type>
Primitive<Type>::~Primitive() {}

template<typename Type>
bool Primitive<Type>::isTruthy()
{
    return value;
}
