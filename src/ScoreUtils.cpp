#include "main.hpp"
#include "ScoreUtils.hpp"

ScoreUtils::MaxScoreRetriever::RetrievedMaxScoreEvent onGotMaxScore;

ScoreUtils::MaxScoreRetriever::RetrievedMaxScoreEvent& ScoreUtils::MaxScoreRetriever::GetRetrievedMaxScoreCallback(){
    return onGotMaxScore;
}

void announceScoreAcquired(int x, function_ptr_t<void, int> callback){
    callback != nullptr ? callback(x) : onGotMaxScore.invoke(x);
}