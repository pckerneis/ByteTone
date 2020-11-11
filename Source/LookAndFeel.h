/*
  ==============================================================================

    LookAndFeel.h
    Created: 11 Nov 2020 8:49:49pm
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CustomCaretComponent : public juce::CaretComponent, private juce::Timer
{
public:
    CustomCaretComponent(juce::Component* keyFocusOwner) : juce::CaretComponent(keyFocusOwner), owner(keyFocusOwner)
    {}

    void setCaretPosition(const juce::Rectangle<int>& characterArea) override
    {
        Timer::startTimer(500);
        setVisible(shouldBeShown());

        const int height = 2;
        const int width = 8;

        setBounds(characterArea
            .withHeight(height)
            .withY(characterArea.getBottom() - height)
            .withWidth(width));
    }

private:
    bool shouldBeShown() const
    {
        return owner == nullptr || (owner->hasKeyboardFocus(false)
            && !owner->isCurrentlyBlockedByAnotherModalComponent());
    }

    void timerCallback() override
    {
        setVisible(shouldBeShown() && !isVisible());
    }

    juce::Component* owner;
};

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::SourceCodeProRegular_ttf,
            BinaryData::SourceCodeProRegular_ttfSize);

        setDefaultSansSerifTypeface(typeface);

        setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
        setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
        setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::TextEditor::highlightColourId, juce::Colours::white);
        setColour(juce::TextEditor::highlightedTextColourId, juce::Colours::black);
        setColour(juce::CaretComponent::caretColourId, juce::Colours::white);
    }

    juce::CaretComponent* createCaretComponent(juce::Component* keyFocusOwner) override
    {
        return new CustomCaretComponent(keyFocusOwner);
    }
};
