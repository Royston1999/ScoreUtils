#include "ScoreUtils.hpp"
#include "MaxScoreRetriever.hpp"

#include <chrono>
#include <thread>
#include <optional>
#include <future>
#include <csignal>
#include <sstream>
#include <pthread.h>

namespace ScoreUtils::MaxScoreRetriever{
    ScoreValuesMap maxScoreValues;
    Il2CppObject* currentDifficultyBeatmap = nullptr;
    std::map<Il2CppObject*, std::function<void(Il2CppObject*)>> taskMap;

    void addMaxScoreData(Il2CppObject* difficultyBeatmap, int maxScore){
        std::string levelID = *il2cpp_utils::RunMethod<StringW>(THROW_UNLESS(il2cpp_utils::RunMethod(difficultyBeatmap, "get_level")), "get_levelID");
        std::string characteristic = *il2cpp_utils::RunMethod<StringW>(THROW_UNLESS(il2cpp_utils::RunMethod(THROW_UNLESS(il2cpp_utils::RunMethod(difficultyBeatmap, "get_parentDifficultyBeatmapSet")), "get_beatmapCharacteristic")), "get_serializedName");
        int difficulty = *il2cpp_utils::RunMethod<int>(difficultyBeatmap, "get_difficulty");

        auto foundLevel = maxScoreValues.find(levelID);
        if (foundLevel == maxScoreValues.end()) {
            maxScoreValues.insert(std::make_pair(levelID, CharacMap{std::make_pair(characteristic, DiffMap{std::make_pair(difficulty, maxScore)})}));
            return;
        }
        auto foundCharac = foundLevel->second.find(characteristic);
        if (foundCharac == foundLevel->second.end()) {
            foundLevel->second.insert(std::make_pair(characteristic, DiffMap{std::make_pair(difficulty, maxScore)}));
            return;
        }
        auto foundDiff = foundCharac->second.find(difficulty);
        if (foundDiff == foundCharac->second.end()) {
            foundCharac->second.insert(std::make_pair(difficulty, maxScore));
        }
        else foundDiff->second = maxScore;
    }

    int RetrieveMaxScoreDataFromCache(){
        std::string levelID = *il2cpp_utils::RunMethod<StringW>(THROW_UNLESS(il2cpp_utils::RunMethod(currentDifficultyBeatmap, "get_level")), "get_levelID");
        std::string characteristic = *il2cpp_utils::RunMethod<StringW>(THROW_UNLESS(il2cpp_utils::RunMethod(THROW_UNLESS(il2cpp_utils::RunMethod(currentDifficultyBeatmap, "get_parentDifficultyBeatmapSet")), "get_beatmapCharacteristic")), "get_serializedName");
        int difficulty = *il2cpp_utils::RunMethod<int>(currentDifficultyBeatmap, "get_difficulty");

        auto foundLevel = maxScoreValues.find(levelID);
        if (foundLevel == maxScoreValues.end()) return -1;
        auto foundCharac = foundLevel->second.find(characteristic);
        if (foundCharac == foundLevel->second.end()) return -1;
        auto foundDiff = foundCharac->second.find(difficulty);
        if (foundDiff == foundCharac->second.end()) return -1;
        int maxScore = foundDiff->second;
        return maxScore;
    }

    void RetrieveMaxScoreFromMapData(Il2CppObject* playerData, Il2CppObject* difficultyBeatmap, function_ptr_t<void, int> callback){
        currentDifficultyBeatmap = difficultyBeatmap;
        const MethodInfo* envInfoMethod = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe("", "BeatmapEnvironmentHelper", "GetEnvironmentInfo", 1));
        auto* envInfo = THROW_UNLESS(il2cpp_utils::RunStaticMethod(envInfoMethod, difficultyBeatmap));
        auto* settings = THROW_UNLESS(il2cpp_utils::RunMethod(playerData, "get_playerSpecificSettings"));
        const MethodInfo* asyncMethod = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe("", "IDifficultyBeatmap", "GetBeatmapDataAsync", 2));
        auto* task = THROW_UNLESS(il2cpp_utils::RunMethod(difficultyBeatmap, "GetBeatmapDataAsync", envInfo, settings));
        taskMap.insert(std::make_pair(task, [=](Il2CppObject* result){
            int maxScore = *il2cpp_utils::RunStaticMethod<int>(THROW_UNLESS(il2cpp_utils::FindMethodUnsafe("", "ScoreModel", "ComputeMaxMultipliedScoreForBeatmap", 1)), result);
            addMaxScoreData(difficultyBeatmap, maxScore);
            if (currentDifficultyBeatmap != difficultyBeatmap) return getLogger().info("Map no longer selected! Blocking callback!");
            announceScoreAcquired(maxScore, callback);
        }));
    }

    void acquireMaxScore(Il2CppObject* playerData, Il2CppObject* difficultyBeatmap){
        int score = currentDifficultyBeatmap != nullptr ? RetrieveMaxScoreDataFromCache() : -1;
        if (score != -1) return announceScoreAcquired(score);
        RetrieveMaxScoreFromMapData(playerData, difficultyBeatmap, nullptr);
    }

    void RetrieveMaxScoreDataCustomCallback(function_ptr_t<void, int> callback){
        int score = currentDifficultyBeatmap != nullptr ? RetrieveMaxScoreDataFromCache() : -1;
        if (score != -1) return callback(score);
        auto* method = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe("UnityEngine", "Resources", "FindObjectsOfTypeAll", 0));
        auto* generic = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(method, {il2cpp_utils::GetClassFromName("", "PlayerDataModel")}));
        auto* model = THROW_UNLESS(il2cpp_utils::RunStaticMethod<ArrayW<Il2CppObject*>, false>(generic)).get(0);
        auto* playerData = THROW_UNLESS(il2cpp_utils::RunMethod(model, "get_playerData"));
        RetrieveMaxScoreFromMapData(playerData, currentDifficultyBeatmap, callback);
    } 
}