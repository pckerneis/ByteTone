/*
  ==============================================================================

    CodeEditor.h
    Created: 11 Dec 2022 1:50:58pm
    Author:  pcker

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class CodeEditor  : public juce::TextEditor
{
public:
    CodeEditor() {}
    ~CodeEditor() override {}

    bool keyPressed(const juce::KeyPress& key) override
    {
        if (key == juce::KeyPress(key.spaceKey, juce::ModifierKeys::commandModifier, 0)) {
            return false;
        }

        return TextEditor::keyPressed(key);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CodeEditor)
};
