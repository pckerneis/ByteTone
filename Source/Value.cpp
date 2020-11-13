/*
  ==============================================================================

    Value.cpp
    Created: 13 Nov 2020 6:06:22pm
    Author:  Pierre

  ==============================================================================
*/

#include "Value.h"

BaseValue::~BaseValue() {}

BaseValue* BaseValue::substract(BaseValue* other) { return new Int(0); }

BaseValue* BaseValue::multiply(BaseValue* other) { return new Int(0); }

BaseValue* BaseValue::divide(BaseValue* other) { return new Int(0); }

BaseValue* BaseValue::add(BaseValue* other) { return new Int(0); }

BaseValue* BaseValue::modulus(BaseValue* other) { return new Int(0); }

BaseValue* BaseValue::bitwiseAnd(BaseValue* other) { return new Int(0); }

BaseValue* BaseValue::bitwiseOr(BaseValue* other) { return new Int(0); }

BaseValue* BaseValue::bitwiseXor(BaseValue* other) { return new Int(0); }

BaseValue* BaseValue::bitShiftLeft(BaseValue* other) { return new Int(0); }

BaseValue* BaseValue::bitShiftRight(BaseValue* other) { return new Int(0); }

BaseValue* BaseValue::greater(BaseValue* other) { return new Bool(false); }

BaseValue* BaseValue::greaterEqual(BaseValue* other) { return new Bool(false); }

BaseValue* BaseValue::less(BaseValue* other) { return new Bool(false); }

BaseValue* BaseValue::lessEqual(BaseValue* other) { return new Bool(false); }

BaseValue* BaseValue::equalEqual(BaseValue* other) { return new Bool(false); }

BaseValue* BaseValue::notEqual(BaseValue* other) { return new Bool(false); }

BaseValue* BaseValue::negate() { return new Int(0); }

BaseValue* BaseValue::bitwiseComplement() { return new Int(0); }
