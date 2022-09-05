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
    Il2CppObject* currentDifficultyBeatmap;

    void addMaxScoreData(Il2CppObject* difficultyBeatmap, int maxScore){
        std::string levelID = *il2cpp_utils::RunMethod<StringW>(THROW_UNLESS(il2cpp_utils::RunMethod(difficultyBeatmap, "get_level")), "get_levelID");
        std::string characteristic = *il2cpp_utils::RunMethod<StringW>(THROW_UNLESS(il2cpp_utils::RunMethod(THROW_UNLESS(il2cpp_utils::RunMethod(difficultyBeatmap, "get_parentDifficultyBeatmapSet")), "get_beatmapCharacteristic")), "get_serializedName");
        auto difficulty = *il2cpp_utils::RunMethod<int>(difficultyBeatmap, "get_difficulty");

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
        auto levelID = *il2cpp_utils::RunMethod<StringW>(THROW_UNLESS(il2cpp_utils::RunMethod(currentDifficultyBeatmap, "get_level")), "get_levelID");
        auto characteristic = *il2cpp_utils::RunMethod<StringW>(THROW_UNLESS(il2cpp_utils::RunMethod(THROW_UNLESS(il2cpp_utils::RunMethod(currentDifficultyBeatmap, "get_parentDifficultyBeatmapSet")), "get_beatmapCharacteristic")), "get_serializedName");
        auto difficulty = *il2cpp_utils::RunMethod<int>(currentDifficultyBeatmap, "get_difficulty");

        auto foundLevel = maxScoreValues.find(levelID);
        if (foundLevel == maxScoreValues.end()) return -1;
        auto foundCharac = foundLevel->second.find(characteristic);
        if (foundCharac == foundLevel->second.end()) return -1;
        auto foundDiff = foundCharac->second.find(difficulty);
        if (foundDiff == foundCharac->second.end()) return -1;
        int maxScore = foundDiff->second;
        return maxScore;
    }

    #define MapTaskFinish2(Class, Func) il2cpp_utils::MakeDelegate(Class, static_cast<std::function<void(Il2CppObject*)>>(Func)) \

    void RetrieveMaxScoreFromMapData(Il2CppObject* playerData, Il2CppObject* difficultyBeatmap, function_ptr_t<void, int> callback){
        currentDifficultyBeatmap = difficultyBeatmap;
        const MethodInfo* envInfoMethod = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe("", "BeatmapEnvironmentHelper", "GetEnvironmentInfo", 1));
        auto* envInfo = THROW_UNLESS(il2cpp_utils::RunStaticMethod(envInfoMethod, difficultyBeatmap));
        auto* settings = THROW_UNLESS(il2cpp_utils::RunMethod(playerData, "get_playerSpecificSettings"));
        const MethodInfo* asyncMethod = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe("", "IDifficultyBeatmap", "GetBeatmapDataAsync", 2));
        auto* task = THROW_UNLESS(il2cpp_utils::RunMethod(difficultyBeatmap, "GetBeatmapDataAsync", envInfo, settings));
        const MethodInfo* continueMethod = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe("System.Threading.Tasks", "Task", "ContinueWith", 1));
        Il2CppClass* taskAction = il2cpp_utils::MakeGeneric(il2cpp_utils::GetClassFromName("System", "Action`1"), {il2cpp_utils::GetClassFromName("System.Threading.Tasks", "Task")}); 
        il2cpp_utils::RunMethod(task, continueMethod, MapTaskFinish2(taskAction, [=](Il2CppObject* result){
            auto* beatmapData = THROW_UNLESS(il2cpp_utils::RunMethod(result, "get_ResultOnSuccess"));
            int maxScore = *il2cpp_utils::RunStaticMethod<int>(THROW_UNLESS(il2cpp_utils::FindMethodUnsafe("", "ScoreModel", "ComputeMaxMultipliedScoreForBeatmap", 1)), beatmapData);
            addMaxScoreData(difficultyBeatmap, maxScore);
            if (currentDifficultyBeatmap != difficultyBeatmap) return;
            announceScoreAcquired(maxScore, callback);
        }));
    }

    void acquireMaxScore(Il2CppObject* playerData, Il2CppObject* difficultyBeatmap){
        currentDifficultyBeatmap = difficultyBeatmap;
        int score = RetrieveMaxScoreDataFromCache();
        if (score != -1) return announceScoreAcquired(score);
        RetrieveMaxScoreFromMapData(playerData, difficultyBeatmap, nullptr);
    }

    void RetrieveMaxScoreDataCustomCallback(function_ptr_t<void, int> callback){
        int score = RetrieveMaxScoreDataFromCache();
        if (score != -1) return callback(score);
        auto* internal = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe("UnityEngine", "Resources", "FindObjectsOfTypeAll", 0));
        auto* generic = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(internal, {il2cpp_utils::GetClassFromName("", "PlayerDataModel")}));
        auto model = THROW_UNLESS(il2cpp_utils::RunMethodRethrow<ArrayW<Il2CppObject*>, false>((Il2CppObject*)nullptr, generic)).get(0);
        auto* playerData = THROW_UNLESS(il2cpp_utils::RunMethod(model, "get_playerData"));
        RetrieveMaxScoreFromMapData(playerData, currentDifficultyBeatmap, callback);
    } 
}