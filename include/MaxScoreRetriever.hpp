#pragma once
#include "main.hpp"

#include "beatsaber-hook/shared/utils/typedefs.h"

#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/PlayerData.hpp"

#include <map>

using DiffMap = std::map<int, int>;
using CharacMap = std::map<std::string, DiffMap>;
using ScoreValuesMap = std::map<std::string, CharacMap>;

namespace ScoreUtils::MaxScoreRetriever{
    /// @brief retrieves the maximum score available for the selected map. returns -1 if unable to get data.
    void acquireMaxScore(GlobalNamespace::PlayerData* playerData, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap);
    void RetrieveMaxScoreFromMapData(GlobalNamespace::PlayerData* playerData, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, function_ptr_t<void, int> callback = nullptr);
    extern GlobalNamespace::IDifficultyBeatmap* currentlySelectedMap;
}