/*
  ==============================================================================

    LookAndFeel.h
    Created: 11 Nov 2020 8:49:49pm
    Author:  Pierre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;

class CustomCaretComponent : public CaretComponent, private Timer
{
public:
    CustomCaretComponent(Component* keyFocusOwner) : CaretComponent(keyFocusOwner), owner(keyFocusOwner)
    {}

    void setCaretPosition(const Rectangle<int>& characterArea) override
    {
        Timer::startTimer(500);
        setVisible(shouldBeShown());

        // YES, that is a lot of magic numbers...
        const int height = 2;
        const int width = 7;

        setBounds(characterArea
            .withSize(width, height)
            .withY(characterArea.getBottom() - height)
            .withX(characterArea.getX() + 1));
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

    Component* owner;
};

class CustomLookAndFeel : public LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        setDefaultSansSerifTypeface(getDefaultTypeface());

        setColour(TextEditor::backgroundColourId, Colours::transparentBlack);
        setColour(TextEditor::outlineColourId, Colours::transparentBlack);
        setColour(TextEditor::focusedOutlineColourId, Colours::transparentBlack);
        setColour(TextEditor::highlightColourId, Colours::white);
        setColour(TextEditor::highlightedTextColourId, Colours::black);

        setColour(CaretComponent::caretColourId, Colours::white);

        setColour(ResizableWindow::backgroundColourId, Colours::black);

        setColour(PopupMenu::backgroundColourId, Colours::black);

        setColour(TextButton::buttonColourId, Colours::black);
        setColour(TextButton::textColourOnId, Colours::white);

        setColour(ComboBox::textColourId, Colours::white);
        setColour(ComboBox::outlineColourId, Colours::transparentBlack);
        setColour(ComboBox::arrowColourId, Colours::white);
        setColour(ComboBox::focusedOutlineColourId, Colours::transparentBlack);
        setColour(ComboBox::buttonColourId, Colours::transparentBlack);
        setColour(ComboBox::backgroundColourId, Colours::transparentBlack);

        setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
        setColour(Slider::backgroundColourId, Colours::transparentBlack);

        // setColour(Label::backgroundWhenEditingColourId, Colours::transparentBlack);

        
    }

    int getDefaultFontHeight() const
    {
        return 16;
    }

    static Typeface::Ptr getDefaultTypeface()
    {
        auto typeface = Typeface::createSystemTypefaceFor(BinaryData::Consolas_ttf,
            BinaryData::Consolas_ttfSize);
        return typeface;
    }

    CaretComponent* createCaretComponent(Component* keyFocusOwner) override
    {
        return new CustomCaretComponent(keyFocusOwner);
    }

    void drawComboBox(Graphics& g, int width, int height, bool,
        int, int, int, int, ComboBox& box) override
    {
        auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
        Rectangle<int> boxBounds(0, 0, width, height);

        g.setColour(box.findColour(ComboBox::backgroundColourId));
        g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

        g.setColour(box.findColour(ComboBox::outlineColourId));
        g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

        Rectangle<int> arrowZone(width - 20, 0, 12, height);
        Path path;
        path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
        path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 2.0f);
        path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

        g.setColour(box.findColour(ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, PathStrokeType(1.0f));
    }
    
    Font getLabelFont(Label& label)
    {
        return Font(getDefaultTypeface()).withHeight(getDefaultFontHeight());
    }

    void drawCornerResizer(Graphics& g, int w, int h, bool /*isMouseOver*/, bool /*isMouseDragging*/)
    {
    }

    void drawButtonText(Graphics& g, TextButton& button,
        bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) override
    {
        Font font(getDefaultTypeface());
        g.setFont(font.withHeight(getDefaultFontHeight()));
        g.setColour(button.findColour(button.getToggleState() ? TextButton::textColourOnId
            : TextButton::textColourOffId)
            .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

        const int textWidth = button.getWidth();

        if (textWidth > 0)
            g.drawText(button.getButtonText(),
                0, 0, button.getWidth(), button.getHeight(),
                Justification::centred, false);
    }

    Label* createSliderTextBox(Slider& slider) override
    {
        auto* l = LookAndFeel_V2::createSliderTextBox(slider);

        if (getCurrentColourScheme() == LookAndFeel_V4::getGreyColourScheme() && (slider.getSliderStyle() == Slider::LinearBar
            || slider.getSliderStyle() == Slider::LinearBarVertical))
        {
            // l->setColour(Label::textColourId, Colours::black.withAlpha(0.7f));
        }
        l->setJustificationType(juce::Justification::centredLeft);

        return l;
    }
};
