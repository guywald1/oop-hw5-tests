#include <algorithm>
#include <iostream>  // TODO: remove
#include <map>
#include <set>
#include <string>
#include <vector>
#include "../../src/part2/Stream.h"
#include "../lib/catch.hpp"

// TODO: remove
using std::cout;
using std::endl;

using std::map;
using std::set;
using std::string;
using std::vector;

// ** helpers ** //

template <typename T>
vector<T*> mapVecToPtrs(vector<T> const& vec) {
  vector<T*> result;
  for (auto& t : vec) {
    result.push_back(new T(t));
  }
  return result;
}

template <typename T>
vector<T> derefVec(vector<T*> const& vec) {
  vector<T> result;
  for (auto& t : vec) {
    result.push_back(*t);
  }
  return result;
}

// ** tests ** //

TEST_CASE("Collect") {
  // normal
  auto vec = mapVecToPtrs(vector<int>{1, 2, 5, 3, 8, 1});
  REQUIRE(Stream<int>::of(vec).collect<vector<int*>>().size() == 6);
  REQUIRE(Stream<int>::of(vec).collect<set<int*>>().size() == 6);

  // empty
  auto vecEmpty = mapVecToPtrs(vector<int>{});
  REQUIRE(Stream<int>::of(vecEmpty).collect<vector<int*>>().size() == 0);

  // map
  map<string, int*> m;
  m.insert(std::pair<string, int*>{"foo", new int(1)});
  m.insert(std::pair<string, int*>{"bar", new int(3)});
  m.insert(std::pair<string, int*>{"baz", new int(4)});
  m.insert(std::pair<string, int*>{"boz", new int(2)});
  REQUIRE(Stream<int>::of(m).collect<vector<int*>>().size() == 4);
  // collect into set, sort and assert equality
  vector<int> res;
  for (int* val : Stream<int>::of(m).collect<set<int*>>()) {
    res.push_back(*val);
  }
  std::sort(res.begin(), res.end());
  REQUIRE(res == vector<int>{1, 2, 3, 4});
}

// note: requires `Stream#count`
TEST_CASE("filter") {
  auto vec = mapVecToPtrs(vector<string>{"air", "cat", "cool", "block", "care"});
  REQUIRE(Stream<string>::of(vec).filter([](string const* str) { return str->size() > 42; }).count() ==
          0);  // no matches
  REQUIRE(Stream<string>::of(vec).filter([](string const* str) { return str->size() < 42; }).count() ==
          5);  // all match
  REQUIRE(Stream<string>::of(vec).filter([](string const* str) { return str->at(0) == 'c'; }).count() == 3);
  REQUIRE(Stream<string>::of(vec).filter([](string const* str) { return str->at(0) == 'a'; }).count() == 1);
}

TEST_CASE("map") {
  auto vec = mapVecToPtrs(vector<string>{"a", "aa", "aaaa", "aaa"});
  auto sizesPtrs =
      Stream<string>::of(vec).map<int>([](string const* str) { return new int(str->size()); }).collect<vector<int*>>();
  auto sizes = derefVec(sizesPtrs);
  std::sort(sizes.begin(), sizes.end());
  REQUIRE(sizes == vector<int>{1, 2, 3, 4});
}

TEST_CASE("distinct") {
  vector<int*> vec = mapVecToPtrs(vector<int>{1, 2, 5, 3, 8, 1});
  REQUIRE(Stream<int>::of(vec).distinct().count() == 5);
  REQUIRE(Stream<int>::of(vec).distinct([](int const* n, int const* m) { return *n % 2 == *m % 2; }).count() == 2);
}

TEST_CASE("forEach") {
  vector<int*> vec = mapVecToPtrs(vector<int>{1, 2, 5, 3, 8, 1});
  vector<int> res;
  Stream<int>::of(vec).forEach([&res](int* num) { res.push_back(*num); });
  std::sort(res.begin(), res.end());
  REQUIRE(res == vector<int>{1, 1, 2, 3, 5, 8});
}

TEST_CASE("reduce") {
  auto vec = mapVecToPtrs(vector<int>{1, 2, 5, 3, 8, 1});
  int initial = 0;
  int* sum =
      Stream<int>::of(vec).reduce(&initial, [](int const* acc, int const* curr) { return new int(*acc + *curr); });
  REQUIRE(*sum == 20);
}

TEST_CASE("max/min") {
  auto vec = mapVecToPtrs(vector<int>{1, 2, 5, 3, 8, 1});
  REQUIRE(*Stream<int>::of(vec).max() == 8);
  auto vec2 = mapVecToPtrs(vector<int>{42});
  REQUIRE(*Stream<int>::of(vec2).min() == 42);
  REQUIRE(*Stream<int>::of(vec2).max() == 42);
}

TEST_CASE("allMatch & anyMatch") {
  auto vec = mapVecToPtrs(vector<string>{"cat", "dog"});
  REQUIRE(Stream<string>::of(vec).allMatch([](string const* str) { return str->size() == 3; }) == true);
  REQUIRE(Stream<string>::of(vec).allMatch([](string const* str) { return str->size() == 2; }) == false);
  REQUIRE(Stream<string>::of(vec).allMatch([](string const* str) { return str->at(0) == 'c'; }) == false);
  REQUIRE(Stream<string>::of(vec).anyMatch([](string const* str) { return str->at(0) == 'c'; }) == true);
  REQUIRE(Stream<string>::of(vec).anyMatch([](string const* str) { return str->at(0) == 'a'; }) == false);
}

TEST_CASE("findFirst") {
  auto vec = mapVecToPtrs(vector<int>{1, 2, 3, 7, 6, 5, 4});
  REQUIRE(*Stream<int>::of(vec).findFirst([](int const* n) { return *n < 2; }) == 1);
  REQUIRE(*Stream<int>::of(vec).findFirst([](int const* n) { return *n > 1; }) == 2);
  REQUIRE(*Stream<int>::of(vec).findFirst([](int const* n) { return *n > 4; }) == 7);
  REQUIRE(*Stream<int>::of(vec).findFirst([](int const* n) { return *n % 2 == 0; }) == 2);
}

TEST_CASE("integration") {
  auto vec = mapVecToPtrs(vector<int>{1, 2, 3, 7, 6, 5, 4, 2, 1});
  REQUIRE(Stream<int>::of(vec)
              .filter([](int const* n) { return *n <= 4; })  // 1, 2, 3, 4, 2, 1
              .distinct()                                    // 1, 2, 3, 4
              .count() == 4);
  auto vec2 = mapVecToPtrs(vector<string>{"Michael Scott", "Dwight Schrute", "Pam Beasley"});
  REQUIRE(derefVec(
              Stream<string>::of(vec2)
                  .map<int>([](string const* str) { return new int(str->size()); })  // 13, 14, 11
                  .map<int>([](int const* num) { return new int(*num % 2); })        // 1, 0, 1
                  .sorted()                                                          // 0, 1, 1
                  .collect<vector<int*>>()) == vector<int>{0, 1, 1});
  auto vec3 = mapVecToPtrs(vector<string>{"Bears", "Beets", "Battlestar Galactica"});
  REQUIRE(derefVec(
              Stream<string>::of(vec3)
                  .sorted([](string const* str1, string const* str2) {
    return str1->size() > str2->size(); })  // "Battlestar Galactica", "Bears", "Beats"
                  .map<char>([](string const* str) { return new char(str->at(str->size() - 1)); }) // 'a', 's', 's'
                  .distinct() // 'a', 's'
                  .collect<vector<char*>>()
            ) == vector<char>{'a', 's'});
}
