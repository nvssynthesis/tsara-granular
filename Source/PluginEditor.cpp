/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================

TsaraGranularAudioProcessorEditor::TsaraGranularAudioProcessorEditor (TsaraGranularAudioProcessor& p)
	: AudioProcessorEditor (&p)
,	fileComp(juce::File(), "*.wav;*.aif;*.aiff", "", "Select file to open")
,	triggeringButton("hi")
,	audioProcessor (p)
{

	addAndMakeVisible (fileComp);
	fileComp.addListener (this);
	fileComp.getRecentFilesFromUserApplicationDataDirectory();
	
	addAndMakeVisible(triggeringButton);
	triggeringButton.onClick = [this, &p]{ updateToggleState(&triggeringButton, "Trigger", p.triggerValFromEditor);	};
	triggeringButton.setClickingTogglesState(true);
	
	for (size_t i = 0; i < static_cast<size_t>(params_e::count); ++i){
		params_e param = static_cast<params_e>(i);
		paramSliderAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, getParamName(param), paramSliders[i]);
		
		// if main param!
		if (isMainParam(param)){
			paramSliders[i].setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		} else if (isParamOfCategory(param, param_category_e::random)){
			// else rotary?
			paramSliders[i].setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		}
		paramSliders[i].setNormalisableRange(getNormalizableRange<double>(param));
		paramSliders[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
		
		paramSliders[i].setValue(getParamDefault(param));
		addAndMakeVisible(paramSliders[i]);
		
		paramLabels[i].setText(getParamName(param), juce::dontSendNotification);
		if (isMainParam(param)){
			paramLabels[i].setFont(juce::Font("Copperplate", 20, juce::Font::FontStyleFlags::plain));
			addAndMakeVisible(paramLabels[i]);
		}
	}
	
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setSize (800, 500);
	setResizable(true, true);
}

TsaraGranularAudioProcessorEditor::~TsaraGranularAudioProcessorEditor()
{
	fileComp.pushRecentFilesToFile();
	for (auto &a : paramSliderAttachments)
		a = nullptr;
}
//==============================================================================
void TsaraGranularAudioProcessorEditor::updateToggleState (juce::Button* button, juce::String name, bool &valToAffect)
{
	bool state = button->getToggleState();
	valToAffect = state;
	juce::String stateString = state ? "ON" : "OFF";

	juce::Logger::outputDebugString (name + " Button changed to " + stateString);
}
//==============================================================================
void TsaraGranularAudioProcessorEditor::paint (juce::Graphics& g)
{
#define way1 1
	
#if way1
	juce::Image image(juce::Image::ARGB, getWidth(), getHeight(), true);
	juce::Graphics tg(image);

	juce::Colour upperLeftColour  = gradientColors[(colourOffsetIndex + 0) % gradientColors.size()];
	juce::Colour lowerRightColour = gradientColors[(colourOffsetIndex + 4) % gradientColors.size()];
//    juce::ColourGradient cg = juce::ColourGradient::horizontal(red.darker(1.0), 0.0, red.darker(20.0), getWidth());
	juce::ColourGradient cg(upperLeftColour, 0, 0, lowerRightColour, getWidth(), getHeight(), true);
	cg.addColour(0.3, gradientColors[(colourOffsetIndex + 1) % gradientColors.size()]);
	cg.addColour(0.5, gradientColors[(colourOffsetIndex + 2) % gradientColors.size()]);
	cg.addColour(0.7, gradientColors[(colourOffsetIndex + 3) % gradientColors.size()]);
	tg.setGradientFill(cg);
	tg.fillAll();

	g.drawImage(image, getLocalBounds().toFloat());
#else
	auto gradient = juce::ColourGradient(juce::Colours::red, 0, 0, juce::Colours::black, getWidth(), getHeight(), false);
	g.setGradientFill(gradient);
#endif
//    g.fillAll (juce::Colours::black);

	g.setColour (juce::Colours::white);
	g.setFont (15.0f);
	g.drawFittedText ("tsaaaarrraaaaa grrrraaaaaanuuuuuulaaaaaate", getLocalBounds(), juce::Justification::centred, 1);
}

void TsaraGranularAudioProcessorEditor::resized()
{
	std::cout << "resized\n";

	
	int fileCompLeftPad = 10;
	int fileCompTopPad = 10;
	int fileCompWidth = getWidth() - (fileCompLeftPad * 2);
	int fileCompHeight = 20;
	fileComp.setBounds    (fileCompLeftPad, fileCompTopPad, fileCompWidth, fileCompHeight);
	
	triggeringButton.setBounds(fileCompLeftPad, fileCompTopPad + fileCompHeight + fileCompTopPad, 25, 25);

	
	int const numParams = static_cast<size_t>(params_e::count);
	
	int numMainParams = 0;
	int numRandomParams = 0;
	for (int i = 0; i < numParams; ++i){
		params_e param = static_cast<params_e>(i);
		if (isParamOfCategory(param, param_category_e::main)){
			numMainParams++;
		} else if (isParamOfCategory(param, param_category_e::random)){
			numRandomParams++;
		}
	}
	
	assert((numRandomParams + numMainParams) == numParams);
	
	int top = 0 + (fileCompTopPad) + fileCompHeight;
	int heightAfterFileComp = getHeight() - top;
	
	int const padHorizontal = 25;
	int const padVertical = 25;
	int const widthAfterPadding = getWidth() - (2 * padHorizontal);
	int const heightAfterPadding = heightAfterFileComp - (2 * padVertical);
	
	float sliderHeightToKnobHeightRatio = 5.f;
	
	int const sliderToKnobLowerPadding = 12;
	int const knobHeight = (heightAfterPadding - sliderToKnobLowerPadding) / sliderHeightToKnobHeightRatio;
	
	int const sliderWidth = (widthAfterPadding / numMainParams) / 1.1;
	int const sliderHeight = heightAfterPadding - knobHeight;
	
	int const effectiveWidth = widthAfterPadding - sliderWidth;
	
	int const widthUnit = effectiveWidth / (numMainParams - 1);
	
	for (int i = 0; i < numParams; ++i){
		auto x = widthUnit * i + padHorizontal;
		auto y = padVertical + top;
		auto width = sliderWidth;
		auto height = sliderHeight;

		if (isParamOfCategory(static_cast<params_e>(i), param_category_e::random)){
			auto const xOffset = widthUnit * numMainParams;
			
			y += sliderHeight;
			y += sliderToKnobLowerPadding;
			
			height = knobHeight;
			x -= xOffset;
			width = height;
			
			auto const extraOffset = (sliderWidth - width);
			width += extraOffset;
		}
		
		paramSliders[i].setBounds(x,		// x
								  y,		// y
								  width,	// width
								  height);	// height
		
		
		auto const labelY = height + y ;
		paramLabels[i].setBounds(x, labelY, width, 24);
	}
}

void TsaraGranularAudioProcessorEditor::sliderValueChanged(juce::Slider* sliderThatWasMoved)
{
	// nothing needed, everything changed is a parameter
}
void TsaraGranularAudioProcessorEditor::readFile (const juce::File& fileToRead)
{
	if (! fileToRead.existsAsFile())
		return;

	juce::String fn = fileToRead.getFullPathName();
	std::string st_str = fn.toStdString();
	
	audioProcessor.writeToLog(st_str);
	audioProcessor.loadAudioFile(fileToRead);
	
	fileComp.setCurrentFile(fileToRead, true);
}

void TsaraGranularAudioProcessorEditor::filenameComponentChanged (juce::FilenameComponent* fileComponentThatHasChanged)
{
	if (fileComponentThatHasChanged == &fileComp){
		readFile (fileComp.getCurrentFile());
	}
}