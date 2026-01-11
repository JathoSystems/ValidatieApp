#ifndef VUURJONGEN_WATERMEISJE_GAME_LEVELSAVER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_LEVELSAVER_HPP
#include "SaveLoad/SaveLoadSystem.h"

#include "nlohmann/json.hpp"

class LevelMemento : public IMemento {
private:
    int levelNumber;
    float completionTime;
    int redGems;
    int blueGems;
    std::string timestamp;

public:
    LevelMemento(int level = 0, float time = 0.0f, int red = 0, int blue = 0)
        : levelNumber(level), completionTime(time), redGems(red), blueGems(blue) {
    }

    std::string getTimestamp() const override {
        return timestamp;
    }

    void setTimestamp(const std::string &ts) override {
        timestamp = ts;
    }

    nlohmann::json toJson() const override {
        nlohmann::json j;
        j["levelNumber"] = levelNumber;
        j["completionTime"] = completionTime;
        j["red_gems"] = redGems;
        j["blue_gems"] = blueGems;
        j["timestamp"] = timestamp;
        return j;
    }

    void fromJson(const nlohmann::json &j) override {
        if (j.contains("levelNumber") && j.contains("completionTime") && j.contains("timestamp")) {
            levelNumber = j["levelNumber"].get<int>();
            completionTime = j["completionTime"].get<float>();
            timestamp = j["timestamp"].get<std::string>();
            if (j.contains("red_gems")) {
                redGems = j["red_gems"].get<int>();
            } else {
                redGems = 0;
            }
            if (j.contains("blue_gems")) {
                blueGems = j["blue_gems"].get<int>();
            } else {
                blueGems = 0;
            }
        }
    }

    bool isValid() const override {
        return levelNumber > 0 && completionTime >= 0.0f && !timestamp.empty();
    }

    int getLevelNumber() const { return levelNumber; }
    void setLevelNumber(int lvl) { levelNumber = lvl; }

    float getCompletionTime() const { return completionTime; }
    void setCompletionTime(float time) { completionTime = time; }

    int getRedGems() const { return redGems; }
    void setRedGems(int gems) { redGems = gems; }

    int getBlueGems() const { return blueGems; }
    void setBlueGems(int gems) { blueGems = gems; }
};

class LevelSaver {
public:
    void save(int level, float time, int redGems = 0, int blueGems = 0) {
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

        LevelMemento newMemento(level, time, redGems, blueGems);
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

    int getRedGems(int level) {
        auto system = std::make_unique<SaveLoadSystem>();
        system->initialize("saves");

        if (!system) {
            return -1;
        }

        LevelMemento memento;
        std::string slotName = "level_" + std::to_string(level);
        if (system->exists(slotName) && system->load(memento, slotName)) {
            return memento.getRedGems();
        }

        return -1;
    }

    int getBlueGems(int level) {
        auto system = std::make_unique<SaveLoadSystem>();
        system->initialize("saves");

        if (!system) {
            return -1;
        }

        LevelMemento memento;
        std::string slotName = "level_" + std::to_string(level);
        if (system->exists(slotName) && system->load(memento, slotName)) {
            return memento.getBlueGems();
        }

        return -1;
    }
};


#endif //VUURJONGEN_WATERMEISJE_GAME_LEVELSAVER_HPP
