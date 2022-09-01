#pragma once

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"

namespace ScoreUtils::MaxScoreRetriever{
using RetrievedMaxScoreEvent = UnorderedEventCallback<const int>;
	/// @brief returns event that you can subscribe to in order to receive max score on level select (and multiplayer level load)
	RetrievedMaxScoreEvent& GetRetrievedMaxScoreCallback();
	/// @brief returns max score data if exists in cache. returns -1 otherwise.
	int RetrieveMaxScoreDataFromCache(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap);
	/// @brief retrieves max score and triggers a custom callback
	void RetrieveMaxScoreDataCustomCallback(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, function_ptr_t<void, int> callback);
}