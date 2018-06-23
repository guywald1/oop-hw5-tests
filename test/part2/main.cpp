#define CATCH_CONFIG_MAIN
#include "../lib/catch.hpp"

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "../../src/Stream.h"

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
  vector<int> vecRaw = {1, 2, 5, 3, 8, 1};
  auto vec = mapVecToPtrs(vecRaw);
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
  vector<int> expected = {1, 2, 3, 4};
  REQUIRE(res == expected);
}

// note: requires `Stream#count`
TEST_CASE("filter") {
  vector<string> vecRaw = {"air", "cat", "cool", "block", "care"};
  auto vec = mapVecToPtrs(vecRaw);
  REQUIRE(Stream<string>::of(vec).filter([](string const* str) { return str->size() > 42; }).count() ==
          0);  // no matches
  REQUIRE(Stream<string>::of(vec).filter([](string const* str) { return str->size() < 42; }).count() ==
          5);  // all match
  REQUIRE(Stream<string>::of(vec).filter([](string const* str) { return str->at(0) == 'c'; }).count() == 3);
  REQUIRE(Stream<string>::of(vec).filter([](string const* str) { return str->at(0) == 'a'; }).count() == 1);
}

TEST_CASE("map") {
  vector<string> vecRaw = {"a", "aa", "aaaa", "aaa"};
  auto vec = mapVecToPtrs(vecRaw);
  auto sizesPtrs =
      Stream<string>::of(vec).map<int>([](string const* str) { return new int(str->size()); }).collect<vector<int*>>();
  auto sizes = derefVec(sizesPtrs);
  std::sort(sizes.begin(), sizes.end());
  vector<int> expected = {1, 2, 3, 4};
  REQUIRE(sizes == expected);
}

TEST_CASE("distinct") {
  vector<int> vecRaw = {1, 2, 5, 3, 8, 1};
  vector<int*> vec = mapVecToPtrs(vecRaw);
  REQUIRE(Stream<int>::of(vec).distinct().count() == 5);
  REQUIRE(Stream<int>::of(vec).distinct([](int const* n, int const* m) { return *n % 2 == *m % 2; }).count() == 2);
}

TEST_CASE("forEach") {
  vector<int> vecRaw = {1, 2, 5, 3, 8, 1};
  vector<int*> vec = mapVecToPtrs(vecRaw);
  vector<int> res;
  Stream<int>::of(vec).forEach([&res](int* num) { res.push_back(*num); });
  std::sort(res.begin(), res.end());
  vector<int> expected = {1, 1, 2, 3, 5, 8};
  REQUIRE(res == expected);
}

TEST_CASE("reduce") {
  vector<int> vecRaw = {1, 2, 5, 3, 8, 1};
  vector<int*> vec = mapVecToPtrs(vecRaw);
  int initial = 0;
  int* sum =
      Stream<int>::of(vec).reduce(&initial, [](int const* acc, int const* curr) { return new int(*acc + *curr); });
  REQUIRE(*sum == 20);
}

TEST_CASE("max/min") {
  vector<int> vecRaw = {1, 2, 5, 3, 8, 1};
  vector<int*> vec = mapVecToPtrs(vecRaw);
  REQUIRE(*Stream<int>::of(vec).max() == 8);
  vector<int*> vec2 = {new int(42)};
  REQUIRE(*Stream<int>::of(vec2).min() == 42);
  REQUIRE(*Stream<int>::of(vec2).max() == 42);
}

TEST_CASE("allMatch & anyMatch") {
  vector<string> vecRaw = {"cat", "dog"};
  vector<string*> vec = mapVecToPtrs(vecRaw);
  REQUIRE(Stream<string>::of(vec).allMatch([](string const* str) { return str->size() == 3; }) == true);
  REQUIRE(Stream<string>::of(vec).allMatch([](string const* str) { return str->size() == 2; }) == false);
  REQUIRE(Stream<string>::of(vec).allMatch([](string const* str) { return str->at(0) == 'c'; }) == false);
  REQUIRE(Stream<string>::of(vec).anyMatch([](string const* str) { return str->at(0) == 'c'; }) == true);
  REQUIRE(Stream<string>::of(vec).anyMatch([](string const* str) { return str->at(0) == 'a'; }) == false);
}

TEST_CASE("findFirst") {
  vector<int> vecRaw = {1, 2, 3, 7, 6, 5, 4};
  vector<int*> vec = mapVecToPtrs(vecRaw);
  REQUIRE(Stream<int>::of(vec).findFirst([](int const* n) { return *n > 42; }) == nullptr);
  REQUIRE(*Stream<int>::of(vec).findFirst([](int const* n) { return *n < 2; }) == 1);
  REQUIRE(*Stream<int>::of(vec).findFirst([](int const* n) { return *n > 1; }) == 2);
  REQUIRE(*Stream<int>::of(vec).findFirst([](int const* n) { return *n > 4; }) == 7);
  REQUIRE(*Stream<int>::of(vec).findFirst([](int const* n) { return *n % 2 == 0; }) == 2);
}

TEST_CASE("integration") {
  vector<int> vecRaw = {1, 2, 3, 7, 6, 5, 4, 2, 1};
  vector<int*> vec = mapVecToPtrs(vecRaw);
  REQUIRE(Stream<int>::of(vec)
              .filter([](int const* n) { return *n <= 4; })  // 1, 2, 3, 4, 2, 1
              .distinct()                                    // 1, 2, 3, 4
              .count() == 4);

  vector<string> vecRaw2 = {"Michael Scott", "Dwight Schrute", "Pam Beasley"};
  vector<string*> vec2 = mapVecToPtrs(vecRaw2);
  vector<int> expected = {0, 1, 1};
  REQUIRE(derefVec(
              Stream<string>::of(vec2)
                  .map<int>([](string const* str) { return new int(str->size()); })  // 13, 14, 11
                  .map<int>([](int const* num) { return new int(*num % 2); })        // 1, 0, 1
                  .sorted()                                                          // 0, 1, 1
                  .collect<vector<int*>>()) == expected);

  vector<string> vecRaw3 = {"Bears", "Beets", "Battlestar Galactica"};
  vector<string*> vec3 = mapVecToPtrs(vecRaw3);
  vector<char> expected2 = {'a', 's'};
  REQUIRE(derefVec(
              Stream<string>::of(vec3)
                  .sorted([](string const* str1, string const* str2) { return str1->size() > str2->size(); })  // "Battlestar Galactica", "Bears", "Beats"
                  .map<char>([](string const* str) { return new char(str->at(str->size() - 1)); })             // 'a', 's', 's'
                  .distinct()                                                                                  // 'a', 's'
                  .collect<vector<char*>>()) == expected2);

  map<char, unsigned*> histo;
  auto vec4 = mapVecToPtrs(vector<char>{'h', 'e', 'l', 'l', 'o'});
  vector<unsigned> expected3 = {1, 1, 1, 2};
  Stream<char>::of(vec4).forEach([&histo](char const* c) {
    auto it = histo.find(*c);
    if (it != histo.end()) {
      *it->second = *it->second + 1;
    } else {
      histo.insert(std::pair<char, unsigned*>(*c, new unsigned(1)));
    }
  });
  auto values = derefVec(Stream<unsigned>::of(histo).sorted().collect<vector<unsigned*>>());
  REQUIRE(values == expected3);
}
