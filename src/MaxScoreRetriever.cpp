#include "ScoreUtils.hpp"
#include "MaxScoreRetriever.hpp"

using namespace il2cpp_utils;

namespace ScoreUtils::MaxScoreRetriever{
    ScoreValuesMap maxScoreValues;
    Il2CppObject* currentDifficultyBeatmap = nullptr;
    std::map<Il2CppObject*, std::function<void(Il2CppObject*)>> taskMap;

    void addMaxScoreData(Il2CppObject* difficultyBeatmap, int maxScore){
        std::string levelID = *RunMethod<StringW>(*RunMethod(difficultyBeatmap, "get_level"), "get_levelID");
        std::string characteristic = *RunMethod<StringW>(*RunMethod(*RunMethod(difficultyBeatmap, "get_parentDifficultyBeatmapSet"), "get_beatmapCharacteristic"), "get_serializedName");
        int difficulty = *RunMethod<int>(difficultyBeatmap, "get_difficulty");

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
        if (!currentDifficultyBeatmap) return -1;
        std::string levelID = *RunMethod<StringW>(*RunMethod(currentDifficultyBeatmap, "get_level"), "get_levelID");
        std::string charac = *RunMethod<StringW>(*RunMethod(*RunMethod(currentDifficultyBeatmap, "get_parentDifficultyBeatmapSet"), "get_beatmapCharacteristic"), "get_serializedName");
        int difficulty = *RunMethod<int>(currentDifficultyBeatmap, "get_difficulty");

        auto foundLevel = maxScoreValues.find(levelID);
        if (foundLevel == maxScoreValues.end()) return -1;
        auto foundCharac = foundLevel->second.find(charac);
        if (foundCharac == foundLevel->second.end()) return -1;
        auto foundDiff = foundCharac->second.find(difficulty);
        if (foundDiff == foundCharac->second.end()) return -1;
        int maxScore = foundDiff->second;
        return maxScore;
    }

    void RetrieveMaxScoreFromMapData(Il2CppObject* playerData, Il2CppObject* difficultyBeatmap, function_ptr_t<void, int> callback){
        currentDifficultyBeatmap = difficultyBeatmap;
        auto* envInfo = *RunStaticMethod(FindMethodUnsafe("", "BeatmapEnvironmentHelper", "GetEnvironmentInfo", 1), difficultyBeatmap);
        auto* settings = *RunMethod(playerData, "get_playerSpecificSettings");
        auto* task = *RunMethod(difficultyBeatmap, "GetBeatmapDataAsync", envInfo, settings);
        taskMap.insert(std::make_pair(task, [difficultyBeatmap, callback](Il2CppObject* mapData){
            int maxScore = mapData != nullptr ? *RunStaticMethod<int>(FindMethodUnsafe("", "ScoreModel", "ComputeMaxMultipliedScoreForBeatmap", 1), mapData) : -1;
            addMaxScoreData(difficultyBeatmap, maxScore);
            if (currentDifficultyBeatmap != difficultyBeatmap) return getMyLogger().info("Map no longer selected! Blocking callback!");
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
        auto* method = MakeGenericMethod(FindMethodUnsafe("UnityEngine", "Resources", "FindObjectsOfTypeAll", 0), {GetClassFromName("", "PlayerDataModel")});
        auto* model = RunStaticMethod<ArrayW<Il2CppObject*>, false>(method)->get(0);
        auto* playerData = *RunMethod(model, "get_playerData");
        RetrieveMaxScoreFromMapData(playerData, currentDifficultyBeatmap, callback);
    } 
}