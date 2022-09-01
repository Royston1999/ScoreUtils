#pragma once
#include "main.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "custom-types/shared/coroutine.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/PlayerLevelStatsData.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include <map>

using DiffMap = std::map<int, int>;
using CharacMap = std::map<std::string, DiffMap>;
using ScoreValuesMap = std::map<std::string, CharacMap>;

namespace ScoreUtils::MaxScoreRetriever{
    /// @brief retrieves the maximum score available for the selected map. returns -1 if unable to get data.
    void acquireMaxScore(GlobalNamespace::PlayerData* playerData, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap);
    void RetrieveMaxScoreFromMapData(GlobalNamespace::PlayerData* playerData, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, function_ptr_t<void, int> callback = nullptr);
}