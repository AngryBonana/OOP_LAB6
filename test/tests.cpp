#include <gtest/gtest.h>
#include "../include/npc.h"
#include "../include/dragon.h"
#include "../include/bull.h"
#include "../include/frog.h"
#include "../include/visitor.h"

// 1. Дракон убивает Быка
TEST(FightTest, DragonKillsBull) {
    auto dragon = std::make_shared<Dragon>("Smaug", 0, 0);
    auto bull = std::make_shared<Bull>("Taurus", 0, 0);

    FightContext ctx{dragon, bull};
    auto visitor = std::make_shared<FightVisitorImpl>(ctx);
    bull->accept(visitor);

    EXPECT_TRUE(ctx.kill_defender);
}

// 2. Бык убивает Жабу
TEST(FightTest, BullKillsFrog) {
    auto bull = std::make_shared<Bull>("Minotaur", 0, 0);
    auto frog = std::make_shared<Frog>("Kermit", 0, 0);

    FightContext ctx{bull, frog};
    auto visitor = std::make_shared<FightVisitorImpl>(ctx);
    frog->accept(visitor);

    EXPECT_TRUE(ctx.kill_defender);
}

// 3. Дракон НЕ убивает Жабу
TEST(FightTest, DragonDoesNotKillFrog) {
    auto dragon = std::make_shared<Dragon>("Drogon", 0, 0);
    auto frog = std::make_shared<Frog>("Peppe", 0, 0);

    FightContext ctx{dragon, frog};
    auto visitor = std::make_shared<FightVisitorImpl>(ctx);
    frog->accept(visitor);

    EXPECT_FALSE(ctx.kill_defender);
}

// 4. Бык НЕ убивает Дракона
TEST(FightTest, BullDoesNotKillDragon) {
    auto bull = std::make_shared<Bull>("Bull1", 0, 0);
    auto dragon = std::make_shared<Dragon>("D1", 0, 0);

    FightContext ctx{bull, dragon};
    auto visitor = std::make_shared<FightVisitorImpl>(ctx);
    dragon->accept(visitor);

    EXPECT_FALSE(ctx.kill_defender);
}

// 5. Жаба НЕ убивает Дракона
TEST(FightTest, FrogDoesNotKillDragon) {
    auto frog = std::make_shared<Frog>("F1", 0, 0);
    auto dragon = std::make_shared<Dragon>("D2", 0, 0);

    FightContext ctx{frog, dragon};
    auto visitor = std::make_shared<FightVisitorImpl>(ctx);
    dragon->accept(visitor);

    EXPECT_FALSE(ctx.kill_defender);
}

// 6. Жаба НЕ убивает Быка
TEST(FightTest, FrogDoesNotKillBull) {
    auto frog = std::make_shared<Frog>("F2", 0, 0);
    auto bull = std::make_shared<Bull>("B2", 0, 0);

    FightContext ctx{frog, bull};
    auto visitor = std::make_shared<FightVisitorImpl>(ctx);
    bull->accept(visitor);

    EXPECT_FALSE(ctx.kill_defender);
}

// 7. Жаба НЕ убивает другую Жабу
TEST(FightTest, FrogDoesNotKillFrog) {
    auto f1 = std::make_shared<Frog>("FrogA", 0, 0);
    auto f2 = std::make_shared<Frog>("FrogB", 0, 0);

    FightContext ctx{f1, f2};
    auto visitor = std::make_shared<FightVisitorImpl>(ctx);
    f2->accept(visitor);

    EXPECT_FALSE(ctx.kill_defender);
}

// 8. Дракон НЕ убивает Дракона
TEST(FightTest, DragonDoesNotKillDragon) {
    auto d1 = std::make_shared<Dragon>("D3", 0, 0);
    auto d2 = std::make_shared<Dragon>("D4", 0, 0);

    FightContext ctx{d1, d2};
    auto visitor = std::make_shared<FightVisitorImpl>(ctx);
    d2->accept(visitor);

    EXPECT_FALSE(ctx.kill_defender);
}

// 9. Бык НЕ убивает Быка
TEST(FightTest, BullDoesNotKillBull) {
    auto b1 = std::make_shared<Bull>("B3", 0, 0);
    auto b2 = std::make_shared<Bull>("B4", 0, 0);

    FightContext ctx{b1, b2};
    auto visitor = std::make_shared<FightVisitorImpl>(ctx);
    b2->accept(visitor);

    EXPECT_FALSE(ctx.kill_defender);
}

// 10. Уникальное имя сохраняется и загружается корректно
TEST(SerializationTest, NameIsSavedAndLoaded) {
    auto original = std::make_shared<Dragon>("Ancalagon", 100, 200);

    std::ostringstream oss;
    original->save(oss); // сохраняет: "1\nAncalagon\n100\n200\n"

    std::istringstream iss(oss.str());
    int type; iss >> type; // читаем тип отдельно
    auto loaded = std::make_shared<Dragon>(iss); // читает имя, x, y

    EXPECT_EQ(loaded->name, "Ancalagon");
    EXPECT_EQ(loaded->x, 100);
    EXPECT_EQ(loaded->y, 200);
}