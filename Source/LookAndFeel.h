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
    CustomCaretComponent(Component* keyFocusOwner) : CaretComponent(keyFocusOwner),
        owner(keyFocusOwner)
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
    CustomLookAndFeel() : 
        backgroundColour(juce::Colours::black),
        textColour(juce::Colours::limegreen),
        selectionColour(textColour.withAlpha(0.3f))
    {
        setDefaultSansSerifTypeface(getDefaultTypeface());

        setColour(TextEditor::backgroundColourId, backgroundColour);
        setColour(TextEditor::outlineColourId, Colours::transparentBlack);
        setColour(TextEditor::focusedOutlineColourId, Colours::transparentBlack);
        setColour(TextEditor::highlightColourId, textColour);
        setColour(TextEditor::highlightedTextColourId, backgroundColour);
        setColour(TextEditor::textColourId, textColour);

        setColour(CaretComponent::caretColourId, textColour);

        setColour(ResizableWindow::backgroundColourId, backgroundColour);

        setColour(PopupMenu::backgroundColourId, backgroundColour);
        setColour(PopupMenu::textColourId, textColour);
        setColour(PopupMenu::highlightedBackgroundColourId, textColour);
        setColour(PopupMenu::highlightedTextColourId, backgroundColour);

        setColour(TextButton::buttonColourId, backgroundColour);
        setColour(TextButton::textColourOnId, textColour);
        setColour(TextButton::textColourOffId, textColour);

        setColour(ComboBox::textColourId, textColour);
        setColour(ComboBox::outlineColourId, Colours::transparentBlack);
        setColour(ComboBox::arrowColourId, textColour);
        setColour(ComboBox::focusedOutlineColourId, Colours::transparentBlack);
        setColour(ComboBox::buttonColourId, Colours::transparentBlack);
        setColour(ComboBox::backgroundColourId, Colours::transparentBlack);

        setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
        setColour(Slider::backgroundColourId, Colours::transparentBlack);
        setColour(Slider::textBoxTextColourId, textColour);
        setColour(Slider::textBoxHighlightColourId, textColour);
        setColour(Slider::trackColourId, selectionColour);

        // setColour(Label::backgroundWhenEditingColourId, Colours::transparentBlack);
        setColour(Label::textColourId, textColour);
        setColour(Label::backgroundWhenEditingColourId, backgroundColour);
        setColour(Label::textWhenEditingColourId, textColour);

        setColour(ListBox::backgroundColourId, backgroundColour);
        setColour(ListBox::textColourId, textColour);
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
        auto bounds = Rectangle<float> (0, 0, width, height).reduced(0.5f, 0.5f);
        Colour colour = box.findColour(TextButton::textColourOnId);
        drawFieldBackground(g, colour, bounds, false, box.hasKeyboardFocus(true));

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

    void drawLinearSlider(Graphics& g, int x, int y, int width, int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        const Slider::SliderStyle style, Slider& slider)
    {
        if (slider.isBar())
        {
            auto bounds = slider.getLocalBounds().toFloat().reduced(0.5f, 0.5f).withX(0).withY(0);
            Colour colour = slider.findColour(TextButton::textColourOnId);
            drawFieldBackground(g, colour, bounds, false, slider.hasKeyboardFocus(true));
        }
        else
        {
            LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        }
    }

    Label* createSliderTextBox(Slider& slider) override
    {
        auto* l = LookAndFeel_V2::createSliderTextBox(slider);
        l->setJustificationType(juce::Justification::centredLeft);
        return l;
    }

    void drawButtonBackground(Graphics& g,
        Button& button,
        const Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown)
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
        Colour colour = button.findColour(TextButton::textColourOnId);

        drawFieldBackground(g, colour, bounds, button.getToggleState(), button.hasKeyboardFocus(true));
    }

    void drawFieldBackground(Graphics& g, Colour colour, Rectangle<float> bounds, bool active, bool focused)
    {

        g.setColour(active ? colour.withAlpha(0.2f) : Colours::transparentBlack);
        g.fillRect(bounds);
            
        if (active)
        {
            colour = colour.withMultipliedBrightness(focused ? 1.0f : 0.5f);
        }
        else
        {
            colour = colour.withMultipliedBrightness(focused ? 1.0f : 0.0f);
        }

        g.setColour(colour);
        g.drawRect(bounds, 1.0f);
    }

    juce::Colour backgroundColour;
    juce::Colour textColour;
    juce::Colour selectionColour;
};
