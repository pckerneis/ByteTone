/*
  ==============================================================================

    InterpreterTest.h
    Created: 13 Dec 2022 4:19:55pm
    Author:  pcker

  ==============================================================================
*/

#pragma once

#include "Interpreter.h"


class SyntaxTest : public juce::UnitTest
{
public:
    SyntaxTest() : UnitTest("SyntaxTest")
    {
    }

    void runTest() override
    {
        beginTest("function calls");

        expectDoesNotThrow(eval("sin()"));
        expectDoesNotThrow(eval("sin(12.0)"));
        expectDoesNotThrow(eval("sin(12.0, toto + b)"));

        expectThrows(eval("sin(hello]"));
        expectThrows(eval("sin(hey"));
        expectThrows(eval("sin("));


        beginTest("array definition");

        expectDoesNotThrow(eval("[]"));
        expectDoesNotThrow(eval("[1]"));
        expectDoesNotThrow(eval("[1, two, 3]"));

        expectThrows(eval("[1, two)"));
        expectThrows(eval("["));
        expectThrows(eval("]"));
    }

    Var eval(juce::String source)
    {
        return interpretAt(0, source);
    }

    Var interpretAt(int position, juce::String source)
    {
        Interpreter interpreter;
        return interpreter.generateRange(source, position, 1, Environment::withTickRate(8000))[0];
    }
};

static SyntaxTest syntaxTest;

class ArithmeticOperationsTest : public juce::UnitTest
{
public:
    ArithmeticOperationsTest() : UnitTest("ArithmeticOperationsTest")
    {
    }

    void runTest() override
    {
        beginTest("minus unary");
        expect(eval("-5") == Var(-5));
        expect(eval("-5.0") == Var(-5.0));
        expect(eval("-0.0") == Var(-0.0));
        expect(eval("0.0") == Var(0.0));

        beginTest("bang");
        expect(eval("! 1") == Var(! 1));
        expect(eval("! 0.1") == Var(! 0.1));
        expect(eval("! 0") == Var(! 0));
        expect(eval("!!0") == Var(!!0));
        expect(eval("!true") == Var(!true));
        expect(eval("!false") == Var(!false));

        beginTest("bitwise complement");
        expect(eval("~1") == Var(~1));
        expect(eval("~0") == Var(~0));
        expect(eval("~~40") == Var(~~40));
        expect(eval("~true") == Var(~true));
        expect(eval("~false") == Var(~false));

        beginTest("plus");
        expect(eval("0 + 1") == Var(0 + 1));
        expect(eval("0.1 + 1") == Var(0.1 + 1));
        expect(eval("0 + 1.1") == Var(0 + 1.1));
        expect(eval("0 + 0") == Var(0 + 0));
        expect(eval("0.0 + 0") == Var(0.0 + 0));

        beginTest("minus");
        expect(eval("0 - 1") == Var(0 - 1));
        expect(eval("0.1 - 1") == Var(0.1 - 1));
        expect(eval("0 - 1.1") == Var(0 - 1.1));
        expect(eval("0 - 0") == Var(0 - 0));
        expect(eval("0.0 - 0") == Var(0.0 - 0));

        beginTest("mult");
        expect(eval("0 * 1") == Var(0 * 1));
        expect(eval("0.0 * 1") == Var(0.0 * 1));
        expect(eval("12 * 4") == Var(12 * 4));
        expect(eval("-15 * 2.0") == Var(-15 * 2.0));

        beginTest("div");
        expect(eval("1 / 2") == Var(1 / 2));
        expect(eval("1.0 / 2") == Var(1.0 / 2));
        expect(eval("0 / 1") == Var(0 / 1));
        expect(eval("0.1 / 1") == Var(0.1 / 1));
        expect(eval("0 / 1.1") == Var(0 / 1.1));

        beginTest("div by 0");
        expect(eval("0 / 0") == Var(0));
        expect(eval("0.0 / 0") == Var(0.0));
        expect(eval("0 / 0.0") == Var(0.0));

        beginTest("modulus");
        expect(eval("42 % 8") == Var(42 % 8));
        expect(eval("-15 % 5") == Var(-15 % 5));
        expect(eval("15 % -5") == Var(15 % -5));

        beginTest("modulus 0");
        expect(eval("0 % 0") == Var(0));

        beginTest("bitshift left");
        expect(eval("1 << 4") == Var(1 << 4));
        expect(eval("0 << 4") == Var(0 << 4));
        expect(eval("1 << 0") == Var(1 << 0));
        expect(eval("12 << true") == Var(12 << true));
        expect(eval("true << 17") == Var(true << 17));
        // Technically undefined...
        expect(eval("-12 << 4") == Var(-12 << 4));

        // Undefined (negative shift count) and doesn't pass
        // expect(eval("1 << -50") == Var(1 << -50));

        beginTest("bitshift right");
        expect(eval("1 >> 4") == Var(1 >> 4));
        expect(eval("0 >> 4") == Var(0 >> 4));
        expect(eval("1 >> 0") == Var(1 >> 0));
        expect(eval("12 >> true") == Var(12 >> true));
        expect(eval("true >> 17") == Var(true >> 17));
        expect(eval("-12 >> 4") == Var(-12 >> 4));
        
        beginTest("bitwise and");
        expect(eval("1 & 4") == Var(1 & 4));
        expect(eval("0 & 4") == Var(0 & 4));
        expect(eval("1 & 0") == Var(1 & 0));
        expect(eval("12 & true") == Var(12 & true));
        expect(eval("true & 17") == Var(true & 17));
        expect(eval("-12 & 4") == Var(-12 & 4));
        
        beginTest("bitwise or");
        expect(eval("1 | 4") == Var(1 | 4));
        expect(eval("0 | 4") == Var(0 | 4));
        expect(eval("1 | 0") == Var(1 | 0));
        expect(eval("12 | true") == Var(12 | true));
        expect(eval("true | 17") == Var(true | 17));
        expect(eval("-12 | 4") == Var(-12 | 4));
        
        beginTest("bitwise xor");
        expect(eval("1 ^ 4") == Var(1 ^ 4));
        expect(eval("0 ^ 4") == Var(0 ^ 4));
        expect(eval("1 ^ 0") == Var(1 ^ 0));
        expect(eval("12 ^ true") == Var(12 ^ true));
        expect(eval("true ^ 17") == Var(true ^ 17));
        expect(eval("-12 ^ 4") == Var(-12 ^ 4));

        beginTest("greater");
        expect(eval("1 > 4") == Var(1 > 4));
        expect(eval("0 > 4") == Var(0 > 4));
        expect(eval("1 > 0") == Var(1 > 0));
        expect(eval("12 > true") == Var(12 > true));
        expect(eval("true > 17") == Var(true > 17));
        expect(eval("-12 > 4") == Var(-12 > 4));
        expect(eval("1.0 > 4") == Var(1.0 > 4));
        expect(eval("0 > 4.0") == Var(0 > 4.0));

        beginTest("greater or equal");
        expect(eval("1 >= 4") == Var(1 >= 4));
        expect(eval("0 >= 4") == Var(0 >= 4));
        expect(eval("1 >= 0") == Var(1 >= 0));
        expect(eval("12 >= true") == Var(12 >= true));
        expect(eval("true >= 17") == Var(true >= 17));
        expect(eval("-12 >= 4") == Var(-12 >= 4));
        expect(eval("1.0 >= 4") == Var(1.0 >= 4));
        expect(eval("0 >= 4.0") == Var(0 >= 4.0));

        beginTest("less");
        expect(eval("1 < 4") == Var(1 < 4));
        expect(eval("0 < 4") == Var(0 < 4));
        expect(eval("1 < 0") == Var(1 < 0));
        expect(eval("12 < true") == Var(12 < true));
        expect(eval("true < 17") == Var(true < 17));
        expect(eval("-12 < 4") == Var(-12 < 4));
        expect(eval("1.0 < 4") == Var(1.0 < 4));
        expect(eval("0 < 4.0") == Var(0 < 4.0));

        beginTest("less or equal");
        expect(eval("1 <= 4") == Var(1 <= 4));
        expect(eval("0 <= 4") == Var(0 <= 4));
        expect(eval("1 <= 0") == Var(1 <= 0));
        expect(eval("12 <= true") == Var(12 <= true));
        expect(eval("true <= 17") == Var(true <= 17));
        expect(eval("-12 <= 4") == Var(-12 <= 4));
        expect(eval("1.0 <= 4") == Var(1.0 <= 4));
        expect(eval("0 <= 4.0") == Var(0 <= 4.0));

        beginTest("equal");
        expect(eval("1 == 4") == Var(1 == 4));
        expect(eval("0 == 4") == Var(0 == 4));
        expect(eval("1 == 0") == Var(1 == 0));
        expect(eval("12 == true") == Var(12 == true));
        expect(eval("true == 17") == Var(true == 17));
        expect(eval("-12 == 4") == Var(-12 == 4));
        expect(eval("1.0 == 4") == Var(1.0 == 4));
        expect(eval("0 == 4.0") == Var(0 == 4.0));

        beginTest("not equal");
        expect(eval("1 != 4") == Var(1 != 4));
        expect(eval("0 != 4") == Var(0 != 4));
        expect(eval("1 != 0") == Var(1 != 0));
        expect(eval("12 != true") == Var(12 != true));
        expect(eval("true != 17") == Var(true != 17));
        expect(eval("-12 != 4") == Var(-12 != 4));
        expect(eval("1.0 != 4") == Var(1.0 != 4));
        expect(eval("0 != 4.0") == Var(0 != 4.0));

        beginTest("complex one");
        expect(interpretAt(158955, "t>>5|t>>4") == Var(14319));
        expect(interpretAt(158955, "t%42") == Var(27));
        expect(interpretAt(158955, "42*(t>>4)") == Var(417228));
        expectEquals(interpretAt(158955, "27*(t>>4)").getIntValue(), 268218);
        expectEquals(interpretAt(158955, "t%42*(t>>4)").getIntValue(), 268218);
        expectEquals(interpretAt(158955, "t>>5|t>>4|(t%42*(t>>4))").getIntValue(), 276479);
        expectEquals(interpretAt(158955, "t>>5|t>>4|(t%42*(t>>4)|357052691-(t>>4))/(t>>16)^(t|t>>4)").getIntValue(), 178550783);
    }

    Var eval(juce::String source)
    {
        return interpretAt(0, source);
    }

    Var interpretAt(int position, juce::String source)
    {
        Interpreter interpreter;
        return interpreter.generateRange(source, position, 1, Environment::withTickRate(8000))[0];
    }
};

static ArithmeticOperationsTest arithmeticOperationsTest;