#include "../include/npc.h"
#include "../include/dragon.h"
#include "../include/bull.h"
#include "../include/frog.h"
#include "../include/visitor.h"
#include <fstream>
#include <ctime>

class TextObserver : public IFightObserver {
    TextObserver() {}
public:
    static std::shared_ptr<IFightObserver> get() {
        static TextObserver inst;
        return std::shared_ptr<IFightObserver>(&inst, [](auto*){});
    }
    void on_fight(const std::shared_ptr<NPC> a, const std::shared_ptr<NPC> d, bool win) override {
        if (win) {
            std::cout << "\nMurder --------\n";
            a->print();
            d->print();
        }
    }
};

class FileObserver : public IFightObserver {
    std::ofstream log;
public:
    FileObserver() : log("log.txt") {}
    void on_fight(const std::shared_ptr<NPC> a, const std::shared_ptr<NPC> d, bool win) override {
        if (win) {
            log << "Murder: " << a->name << " killed " << d->name << "\n";
        }
    }
};

std::shared_ptr<NPC> factory(std::istream& is) {
    int type; is >> type;
    std::shared_ptr<NPC> res;
    switch (type) {
        case DragonType: res = std::make_shared<Dragon>(is); break;
        case BullType:   res = std::make_shared<Bull>(is); break;
        case FrogType:   res = std::make_shared<Frog>(is); break;
        default: return nullptr;
    }
    if (res) {
        res->subscribe(TextObserver::get());
        static auto file_obs = std::make_shared<FileObserver>();
        res->subscribe(file_obs);
    }
    return res;
}

std::shared_ptr<NPC> factory(NpcType type, const std::string& name, int x, int y) {
    std::shared_ptr<NPC> res;
    switch (type) {
        case DragonType: res = std::make_shared<Dragon>(name, x, y); break;
        case BullType:   res = std::make_shared<Bull>(name, x, y); break;
        case FrogType:   res = std::make_shared<Frog>(name, x, y); break;
        default: return nullptr;
    }
    if (res) {
        res->subscribe(TextObserver::get());
        static auto file_obs = std::make_shared<FileObserver>();
        res->subscribe(file_obs);
    }
    return res;
}

using set_t = std::set<std::shared_ptr<NPC>>;

void save(const set_t& array, const std::string& filename) {
    std::ofstream fs(filename);
    fs << array.size() << "\n";
    for (auto& n : array) n->save(fs);
}

set_t load(const std::string& filename) {
    set_t result;
    std::ifstream is(filename);
    if (is.good() && is.is_open()) {
        int count; is >> count;
        for (int i = 0; i < count; ++i)
            result.insert(factory(is));
        is.close();
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const set_t& array) {
    for (auto& n : array) n->print();
    return os;
}

set_t fight(const set_t& array, size_t distance) {
    set_t dead;
    for (const auto& attacker : array) {
        for (const auto& defender : array) {
            if (attacker != defender && attacker->is_close(defender, distance)) {
                FightContext ctx{attacker, defender};
                auto visitor = std::make_shared<FightVisitorImpl>(ctx);
                defender->accept(visitor);
                if (ctx.kill_defender) {
                    attacker->fight_notify(defender, true);
                    dead.insert(defender);
                }
            }
        }
    }
    return dead;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    set_t array;

    std::cout << "Generating ...\n";
    for (size_t i = 0; i < 100; ++i) {
        NpcType type = NpcType(std::rand() % 3 + 1);
        std::string name;
        switch (type) {
            case DragonType: name = "Dragon_" + std::to_string(i); break;
            case BullType:   name = "Bull_" + std::to_string(i); break;
            case FrogType:   name = "Frog_" + std::to_string(i); break;
        }
        array.insert(factory(type, name, std::rand() % 500, std::rand() % 500));
    }

    save(array, "npc.txt");
    array = load("npc.txt");
    std::cout << "Initial NPCs:\n" << array;

    for (size_t d = 50; d <= 500 && !array.empty(); d += 50) {
        auto dead = fight(array, d);
        for (auto& x : dead) array.erase(x);
        std::cout << "Fight stats ----------\ndistance: " << d << "\nkilled: " << dead.size() << "\n\n";
    }

    std::cout << "Survivors:\n" << array;
    return 0;
}