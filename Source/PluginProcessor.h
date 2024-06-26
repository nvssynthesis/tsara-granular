/** TODO:
	-output gain

 */
#pragma once

#include <JuceHeader.h>
#include "Synthesis/TsaraGranularSynth.h"
#include "Analysis/ThreadedAnalyzer.h"
#include "./Synthesis/JuceTsaraGranularSynthesizer.h"
#include "../slicer_granular/Source/AudioBuffersChannels.h"
#include "../slicer_granular/Source/dsp_util.h"
#include "../slicer_granular/Source/misc_util.h"

//==============================================================================

class TsaraGranularAudioProcessor  : public juce::AudioProcessor
							#if JucePlugin_Enable_ARA
							 , public juce::AudioProcessorARAExtension
							#endif
,									private juce::ChangeListener
{
public:
	//==============================================================================
	TsaraGranularAudioProcessor();
	~TsaraGranularAudioProcessor() override;
	
	//==============================================================================
	void prepareToPlay (double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;
	
#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
	
	void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
	
	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;
	
	//==============================================================================
	const juce::String getName() const override;
	
	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;
	
	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram (int index) override;
	const juce::String getProgramName (int index) override;
	void changeProgramName (int index, const juce::String& newName) override;
	
	//==============================================================================
	void getStateInformation (juce::MemoryBlock& destData) override;
	void setStateInformation (const void* data, int sizeInBytes) override;
	
	juce::AudioProcessorValueTreeState apvts;
	
	void writeToLog(std::string const &s);
	void loadAudioFile(juce::File const f, juce::AudioThumbnail *const thumbnail);
	
	void askForAnalysis();
	std::vector<float> getOnsets() const;
	std::vector<std::vector<float>> getOnsetwiseBFCCs() const;
	std::vector<std::vector<float>> getPCA() const;
	void writeEvents();
	
	nvs::util::EditorInformant<float> rmsInformant;
	nvs::util::EditorInformant<float> rmsWAinformant;
	
	juce::AudioFormatManager &getAudioFormatManager(){
		return formatManager;
	}
	
	size_t getCurrentWaveSize()  {
		return audioBuffersChannels.getActiveSpanRef().size();
	}
	std::span<float> const &getActiveSpanRef()  {
		return audioBuffersChannels.getActiveSpanRef();
	}
	
	nvs::analysis::onsetSettings getOnsetSettings(){
		return _analyzer.getOnsetSettings();
	}
	void setOnsetSettings(nvs::analysis::onsetSettings settings){
		_analyzer.setOnsetSettings(settings);
	}
	nvs::analysis::analysisSettings getAnalysisSettings() const {
		return _analyzer.getAnalysisSettings();
	}
	void setAnalysisSettings(nvs::analysis::analysisSettings settings){
		_analyzer.setAnalysisSettings(settings);
	}
private:
	
	AudioBuffersChannels audioBuffersChannels;
	
	JuceTsaraGranularSynthesizer tsara_granular_synth_juce;
	constexpr static int num_voices =
#if defined(DEBUG_BUILD) | defined(DEBUG) | defined(_DEBUG)
										6;
#else
										16;
#endif
	nvs::analysis::ThreadedAnalyzer _analyzer;
	
	struct Features {
		std::optional<std::vector<float>> onsetsInSeconds;
		std::optional<std::vector<std::vector<float>>> onsetwiseBFCCs;
		std::optional<std::vector<std::vector<float>>> PCA;
	};
	Features _feat;
	void changeListenerCallback(juce::ChangeBroadcaster*  source) override;
	
	double lastSampleRate 	{ 0.0 };
	int lastSamplesPerBlock { 0 };
	
	float normalizationValue {1.f};	// a MULTIPLIER for the overall output, based on the inverse of the absolute max value for the current sample

	nvs::util::RMS<float> rms;
	nvs::util::WeightedAveragingBuffer<float, 3> weightAvg;
	
	juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	juce::AudioFormatManager formatManager;
	std::string currentFile;

	//======logging=======================
	juce::File logFile;
	juce::FileLogger fileLogger;
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TsaraGranularAudioProcessor)
};
