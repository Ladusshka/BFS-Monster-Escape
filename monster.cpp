#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>
#include <compare>

enum struct Direction {
    UP, DOWN, LEFT, RIGHT
};

struct Position {
    size_t row = -1, col = -1;

    constexpr Position() = default;
    constexpr Position(size_t r, size_t c) : row(r), col(c) {}

    Position move(Direction d) const {
        using enum Direction;
        switch (d) {
            case UP:    return { row - 1, col };
            case DOWN:  return { row + 1, col };
            case LEFT:  return { row, col - 1 };
            case RIGHT: return { row, col + 1 };
        }
        assert(0);
    }

    friend constexpr auto operator <=> (Position, Position) = default;
};

enum struct Tile : uint8_t {
    WALL, EMPTY, TRAP
};

struct Map {
    static inline constexpr Position INVALID_POS = { -size_t(1), -size_t(1) };

    std::vector<std::vector<Tile>> tiles;
    Position hero = INVALID_POS, beast = INVALID_POS;
    Position exit = INVALID_POS;


    Map() = default;
    Map(std::string_view s) {
        tiles.emplace_back();

        auto set_pos = [&](Position& p, const char *err_msg) {
            if (p != INVALID_POS) throw std::invalid_argument(err_msg);
            p = { tiles.size() - 1, tiles.back().size() };
            tiles.back().push_back(Tile::EMPTY);
        };

        for (char c : s) switch (c) {
                case 'H': set_pos(hero, "Multiple heroes");
                    break;
                case 'B': set_pos(beast, "Multiple beasts");
                    break;
                case 'E': set_pos(exit, "Multiple exits");
                    break;
                case ' ': tiles.back().push_back(Tile::EMPTY);
                    break;
                case 'W': tiles.back().push_back(Tile::WALL);
                    break;
                case 'T': tiles.back().push_back(Tile::TRAP);
                    break;
                case '\n': tiles.emplace_back();
                    break;
                default:
                    throw std::invalid_argument("Unknown tile");
            }

        if (hero == INVALID_POS) throw std::invalid_argument("No hero");
        if (beast == INVALID_POS) throw std::invalid_argument("No beast");
        if (exit == INVALID_POS) throw std::invalid_argument("No exit");

        for (const auto& row : tiles)
            if (row.size() != tiles[0].size())
                throw std::invalid_argument("Non-rectangular maze");
    }

    // Get tile at given position, WALL if the position is out of map
    Tile operator [] (Position p) const {
        if (p.row >= tiles.size()) return Tile::WALL;
        const auto& row = tiles[p.row];
        return p.col < row.size() ? row[p.col] : Tile::WALL;
    }
};

using Path = std::vector<Position>;

#endif

// Beast is always a struct with a method `move`
// with the same signature as in SampleBeast
struct State{
    Position Hero_position;
    Position Beast_position;

    bool operator < (const State & other) const{
        if(this->Hero_position != other.Hero_position){
            return this->Hero_position < other.Hero_position;
        }
        return this->Beast_position < other.Beast_position;
    }
    bool operator == (const State & other)const{
        return (this->Hero_position == other.Hero_position && this->Beast_position==other.Beast_position);
    }
};

template < typename Beast >
Path find_escape_route(const Map& map, const Beast& beast) {

        State start = {map.hero, map.beast}; // defining start positions
        Path result; // result will be written there
        if(start.Hero_position == start.Beast_position){ // if from the start beast and hero are in one position -> no reason to continue
            return result;
        }


        std::queue<State> queue; // queue for state of positions of a monster and hero
        std::set<State> visited; // which positions have already been visited
        std::map<State, State> previous_positions; // to get the full path back

        queue.emplace(start); // to start running bfs
        visited.insert(start); // start position to be already visited

       State last = start;

        while(!queue.empty()){

            auto CurrentState = queue.front();
            queue.pop();

            auto UP_hero = CurrentState.Hero_position.move(Direction::UP); // change hero position UP
            if(map[UP_hero] == Tile::EMPTY && UP_hero != CurrentState.Beast_position ){  // hero can go to empty tile and not to the current beast position
                auto UP_beast = beast.move(map, UP_hero, CurrentState.Beast_position); // find new position of beast
                if(UP_hero != UP_beast && !visited.contains({UP_hero,UP_beast})){ // if state of their new positions have not been visited yet + they are not the same
                    State NewState = {UP_hero, UP_beast}; // new State of two positions
                    queue.push(NewState);
                    visited.insert(NewState);
                    previous_positions[NewState] = CurrentState;

                    if(UP_hero == map.exit){
                        last = NewState;
                        break;
                    }

                }
            }


            auto DOWN_hero = CurrentState.Hero_position.move(Direction::DOWN); // change hero position DOWN
            if(map[DOWN_hero] == Tile::EMPTY && DOWN_hero != CurrentState.Beast_position ){  // hero can go to empty tile and not to the current beast position
                auto DOWN_beast = beast.move(map, DOWN_hero, CurrentState.Beast_position); // find new position of beast
                if(DOWN_hero != DOWN_beast && !visited.contains({DOWN_hero,DOWN_beast})){ // if state of their new positions have not been visited yet + they are not the same
                    State NewState = {DOWN_hero, DOWN_beast}; // new State of two positions
                    queue.push(NewState);
                    visited.insert(NewState);
                    previous_positions[NewState] = CurrentState;
                    if(DOWN_hero == map.exit){
                        last = NewState;
                        break;
                    }
                }
            }


            auto LEFT_hero = CurrentState.Hero_position.move(Direction::LEFT); // change hero position LEFT
            if(map[LEFT_hero] == Tile::EMPTY && LEFT_hero != CurrentState.Beast_position ){  // hero can go to empty tile and not to the current beast position
                auto LEFT_beast = beast.move(map, LEFT_hero, CurrentState.Beast_position); // find new position of beast
                if(LEFT_hero != LEFT_beast && !visited.contains({LEFT_hero,LEFT_beast})){ // if state of their new positions have not been visited yet + they are not the same
                    State NewState = {LEFT_hero, LEFT_beast}; // new State of two positions
                    queue.push(NewState);
                    visited.insert(NewState);
                    previous_positions[NewState] = CurrentState;
                    if(LEFT_hero == map.exit){
                        last = NewState;
                        break;
                    }
                }
            }

            auto RIGHT_hero = CurrentState.Hero_position.move(Direction::RIGHT); // change hero position RIGHT
            if(map[RIGHT_hero] == Tile::EMPTY && RIGHT_hero != CurrentState.Beast_position ){  // hero can go to empty tile and not to the current beast position
                auto RIGHT_beast = beast.move(map, RIGHT_hero, CurrentState.Beast_position); // find new position of beast
                if(RIGHT_hero != RIGHT_beast && !visited.contains({RIGHT_hero,RIGHT_beast})){ // if state of their new positions have not been visited yet + they are not the same
                    State NewState = {RIGHT_hero, RIGHT_beast}; // new State of two positions
                    queue.push(NewState);
                    visited.insert(NewState);
                    previous_positions[NewState] = CurrentState;
                    if(RIGHT_hero == map.exit){
                        last = NewState;
                        break;
                    }
                }
            }

        }

        if(last == start){
            return result;
        }

        auto tmp = last;
        while(tmp!=start){
            result.push_back(tmp.Hero_position);
            tmp = previous_positions[tmp];
        }
        result.push_back(map.hero);

        std::reverse(result.begin(), result.end());
        return result;


}

#ifndef __PROGTEST__


// Sample beast which does two moves per one hero move and it might be
// allowed to step on traps
struct SampleBeast {
    SampleBeast(bool can_step_on_trap) : can_step_on_trap(can_step_on_trap) {}

    Position move(const Map& map, Position hero, Position beast) const {
        return one_move(map, hero, one_move(map, hero, beast));
    }

private:
    Position one_move(const Map& map, Position hero, Position beast) const {
        using enum Direction;

        if (beast.row != hero.row) {
            Position target = beast.move(beast.row > hero.row ? UP : DOWN);
            if (can_move_to(map, target)) return target;
        }

        if (beast.col != hero.col) {
            Position target = beast.move(beast.col > hero.col ? LEFT : RIGHT);
            if (can_move_to(map, target)) return target;
        }

        return beast;
    }

    bool can_move_to(const Map& map, Position p) const {
        switch (map[p]) {
            case Tile::EMPTY: return true;
            case Tile::TRAP: return can_step_on_trap;
            default: return false;
        }
    }

    bool can_step_on_trap;
};

// solutions for SampleBeast<true> and SampleBeast<false>
const std::tuple<size_t, size_t, Map> TESTS[] = {
        {  7,  7, Map{"E     H              B"}},
        {  0,  0, Map{"E            H       B"}},
        { 14, 14, Map{"E            H   W   B"}},
        {  0,  0, Map{"E  W  H              B"}},
        {  0,  0, Map{"            H       BE"}},
        {  0,  0, Map{"E  T  H              B"}},
        {  7,  7, Map{"E     H        T     B"}},

        { 10, 10, Map{"E        H  W   W   WB\n"
                      "              W   W   "}},

        { 10, 10, Map{"E   W  H    W   W   WB\n"
                      "              W   W   "}},


        {  0, 26, Map{"W   W   T   T   T   WB\n"
                      "E W   W   W   W  H    "}},

        { 16, 16, Map{"W   W   W H W   W   WB\n"
                      "E W   W   W   W   W   "}},

        {  0, 27, Map{"E                     \n"
                      "   WWWWWWWWWTWWWWWW   \n"
                      "   W  B         H W   \n"
                      "                      "}},

        { 36, 36, Map{"E                     \n"
                      "  WWWWWWWWWWWWWWW W   \n"
                      "            W   WHW B \n"
                      "            W W W W   \n"
                      "            W W W W   \n"
                      "            W W W W   \n"
                      "              W   W   "}},

        {  0, 33, Map{"E                                 B\n"
                      "                                   \n"
                      "                                   \n"
                      "                                   \n"
                      "                         T         \n"
                      "                           H       \n"
                      "                                   "}},

        { 35, 35, Map{"E                                 B\n"
                      "                                   \n"
                      "                                   \n"
                      "                                   \n"
                      "                         W         \n"
                      "                         W H       \n"
                      "                                   "}},
};

template < typename Beast >
bool test_map(size_t ref, const Map& map, const Beast& beast) {
    auto sol = find_escape_route(map, beast);

    // only basic check, feel free to improve
    return sol.size() == ref;
}

int main() {
    int ok = 0, fail = 0;

    for (auto&& [ ref_t, ref_f, map ] : TESTS) {
        (test_map(ref_t, map, SampleBeast{true}) ? ok : fail)++;
        (test_map(ref_f, map, SampleBeast{false}) ? ok : fail)++;
    }

    if (fail == 0) std::cout << "Passed all " << ok << " tests!" << std::endl;
    else std::cout << fail << " of " << ok + fail << " tests failed" << std::endl;
    return 0;
}

#endif


