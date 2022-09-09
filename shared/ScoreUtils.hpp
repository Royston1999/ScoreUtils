#pragma once

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

namespace ScoreUtils{
	/// @brief initialise lib
	void Init();
	namespace MaxScoreRetriever{
		using RetrievedMaxScoreEvent = UnorderedEventCallback<const int>;
		/// @brief returns event that you can subscribe to in order to receive max score on level select (and multiplayer level load)
		RetrievedMaxScoreEvent& GetRetrievedMaxScoreCallback();
		/// @brief returns max score data for currently selected map if exists in cache. returns -1 otherwise.
		int RetrieveMaxScoreDataFromCache();
		/// @brief retrieves max score and triggers a custom callback
		void RetrieveMaxScoreDataCustomCallback(function_ptr_t<void, int> callback);
	}
}