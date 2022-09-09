#pragma once
#include "main.hpp"

#include "beatsaber-hook/shared/utils/typedefs.h"

#include <map>

using DiffMap = std::map<int, int>;
using CharacMap = std::map<std::string, DiffMap>;
using ScoreValuesMap = std::map<std::string, CharacMap>;

namespace ScoreUtils::MaxScoreRetriever{
    /// @brief retrieves the maximum score available for the selected map. returns -1 if unable to get data.
    void acquireMaxScore(Il2CppObject* playerData, Il2CppObject* difficultyBeatmap);
    void RetrieveMaxScoreFromMapData(Il2CppObject* playerData, Il2CppObject* difficultyBeatmap, function_ptr_t<void, int> callback = nullptr);
    extern Il2CppObject* currentDifficultyBeatmap;
    extern std::map<Il2CppObject*, std::function<void(Il2CppObject*)>> taskMap;
}