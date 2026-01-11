//
// Created by jusra on 11-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_LEVELSAVER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_LEVELSAVER_HPP
#include "Engine/GameEngine.h"
#include "SaveLoad/SaveLoadSystem.h"

#include "nlohmann/json.hpp"

class LevelMemento : public IMemento {
private:
    int levelNumber;
    float completionTime; // in seconden
    std::string timestamp;

public:
    LevelMemento(int level = 0, float time = 0.0f)
        : levelNumber(level), completionTime(time) {}

    // IMemento implementatie
    std::string getTimestamp() const override {
        return timestamp;
    }

    void setTimestamp(const std::string& ts) override {
        timestamp = ts;
    }

    nlohmann::json toJson() const override {
        nlohmann::json j;
        j["levelNumber"] = levelNumber;
        j["completionTime"] = completionTime;
        j["timestamp"] = timestamp;
        return j;
    }

    void fromJson(const nlohmann::json& j) override {
        if (j.contains("levelNumber") && j.contains("completionTime") && j.contains("timestamp")) {
            levelNumber = j["levelNumber"].get<int>();
            completionTime = j["completionTime"].get<float>();
            timestamp = j["timestamp"].get<std::string>();
        }
    }

    bool isValid() const override {
        return levelNumber > 0 && completionTime >= 0.0f && !timestamp.empty();
    }

    int getLevelNumber() const { return levelNumber; }
    void setLevelNumber(int lvl) { levelNumber = lvl; }

    float getCompletionTime() const { return completionTime; }
    void setCompletionTime(float time) { completionTime = time; }
};

class LevelSaver {
public:
    void save(int level, float time) {
        auto system = std::make_unique<SaveLoadSystem>();
        system->initialize("saves");

        if (!system) {
            std::cout << "Empty\n";
            return;
        }

        std::string slotName = "level_" + std::to_string(level);
        LevelMemento memento;

        if (system->exists(slotName)) {
            system->load(memento, slotName);

            if (time >= memento.getCompletionTime()) {
                return;
            }
        }

        LevelMemento newMemento(level, time);
        system->save(newMemento, slotName);
    }

    float getCompletionTime(int level) {
        auto system = std::make_unique<SaveLoadSystem>();
        system->initialize("saves");

        if (!system) {
            std::cout << "Empty\n";
            return -1.0f;
        }

        LevelMemento memento;
        std::string slotName = "level_" + std::to_string(level);
        if (system->exists(slotName) && system->load(memento, slotName)) {
            return memento.getCompletionTime();
        }

        return -1.0f;
    }
};


#endif //VUURJONGEN_WATERMEISJE_GAME_LEVELSAVER_HPP