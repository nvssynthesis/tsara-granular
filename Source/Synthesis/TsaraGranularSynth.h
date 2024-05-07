/*
  ==============================================================================

    TsaraGranularSynth.h
    Created: 4 Sep 2023 1:54:00am
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include "Analysis/EssentiaSetup.h"
#include "../slicer_granular/Source/Synthesis/GranularSynthesis.h"
#include "Analysis/OnsetAnalysis/OnsetAnalysis.h"
#include "Analysis/Analyzer.h"
#include <JuceHeader.h>


/** TODO:
	-bake in some way to be sure that the currently held onsets match the current span
	
 */
namespace nvs	{
namespace gran	{
class TsaraGranular		:	public genGranPoly1
{
public: 
	TsaraGranular(double const &sampleRate, std::span<float> const &wavespan,
											double const&fileSampleRate, size_t nGrains);
	virtual ~TsaraGranular() = default;

	void loadOnsets(std::span<float> const onsetsInSeconds);
#if 0
	void writeEventsToWav(std::string_view ogPath, nvs::analysis::Analyzer &analyzer) {
		if (!_wavespan.data() | !_onsetsInSeconds.data()){
			std::cerr << "TsaraGranular::writeEventsToWav attempt using null data\n";
			return;
		}
		std::vector<float> wave(_wavespan.size());
		wave.assign(_wavespan.begin(), _wavespan.end());
		
		std::vector<float> onsets(_onsetsInSeconds.size());
		onsets.assign(_onsetsInSeconds.begin(), _onsetsInSeconds.end());
		nvs::analysis::writeEventsToWav(wave, onsets, ogPath, analyzer.ess_hold.factory, analyzer._splitSettings);
	}
#endif
private:
	std::vector<float> _onsetsNormalized;
	
	void doSetPosition(double position) override;
	void doSetPositionRandomness(double rand) override;
	void doSetDuration(double dur) override;
	void doSetDurationRandomness(double rand) override;
};

}	// namespace gran
}	// namespace nvs
