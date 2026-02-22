#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "include/lyrics.h"

#include <string>
#include <set>
#include <map>
#include <fstream>
#include <vector>

using namespace std;
using namespace testing;

TEST(CleanToken, PunctuationAtBothEnds) {
  string input1 = "!HELLO?";
  string expected1 = "hello";
  EXPECT_THAT(cleanToken(input1), StrEq(expected1))
  << "Input: " << input1 << " -> Should convert case and clean both ends.";

  string input2 = ":::WORD:::";
  string expected2 = "word";
  EXPECT_THAT(cleanToken(input2), StrEq(expected2));
}


TEST(CleanToken, PunctuationInMiddleCombinations) {
  string input1 = "fast-forward";
  string expected1 = "fast-forward";
  EXPECT_THAT(cleanToken(input1), StrEq(expected1))
  << "Makes sure punctuation is kept.";

  string input2 = "(\"o'clock";
  string expected2 = "o'clock";
  EXPECT_THAT(cleanToken(input2), StrEq(expected2))
  << "Remove leading punctuation, keep middle punctuation.";

  string input3 = "u.s.a.)";
  string expected3 = "u.s.a";
  EXPECT_THAT(cleanToken(input3), StrEq(expected3))
  << "Remove end punctuation, keep middle punctuation.";
}

TEST(CleanToken, NoLetterAndHeavyPunctuation) {
  string input1 = "99.9%!";
  string expected1 = ""; 
  EXPECT_THAT(cleanToken(input1), StrEq(expected1));

  string input2 = "((((z))))";
  string expected2 = "z"; 
  EXPECT_THAT(cleanToken(input2), StrEq(expected2));
}

TEST(GatherTokens, SpacingAndUniqueness) {
  string input1 = "  one  two   three ";
  set<string> expected1 = {"one", "two", "three"};
  EXPECT_THAT(gatherTokens(input1), ContainerEq(expected1))
    << "Should handle all varieties of whitespace.";

  string input2 = "First, second. First? Second!!!";
  set<string> expected2 = {"first", "second"};
  EXPECT_THAT(gatherTokens(input2), ContainerEq(expected2))
  << "Should clean and keep only unique tokens remain.";
}

TEST(GatherTokens, MultipleSpacesBetweenTokens) {
  string input = "word1  word2   word3";
  set<string> expected = {"word1", "word2", "word3"};

  EXPECT_THAT(gatherTokens(input), ContainerEq(expected))
  << "Input: " << input << " correctly handle multiple spaces.";
}


TEST(BuildIndices, TinyFileContents) {
  string filename = "data/tiny.txt";
  map<string, set<string>> inverted_index;
  map<string, string> artist_index;

  int songs_processed = buildIndices(filename, inverted_index, artist_index);

  int expected_songs = 4;
  EXPECT_THAT(songs_processed, Eq(expected_songs));

  map<string, string> expected_artist_index = {
        {"Jingle bells", "James Pierpont"},
        {"Brother John", "Unknown"},
        {"Baa baa Black Sheep", "Unknown"}, 
        {"Carol of the bells", "Mykola Leontovych"}
  };
  EXPECT_THAT(artist_index, ContainerEq(expected_artist_index));

  set<string> bells_songs = {"Jingle bells", "Carol of the bells", "Brother John"};
  EXPECT_THAT(inverted_index["bells"], ContainerEq(bells_songs)); 

  set<string> black_songs = {};
  EXPECT_THAT(inverted_index["black"], ContainerEq(black_songs));
    
  set<string> you_songs = {"Brother John", "Baa baa Black Sheep"};
  EXPECT_THAT(inverted_index["you"], ContainerEq(you_songs));

  set<string> are_songs = {"Brother John"};
  EXPECT_THAT(inverted_index["are"], ContainerEq(are_songs)); 
}

TEST(BuildIndices, FileNotFound) {
  map<string, set<string>> inverted_index;
  map<string, string> artist_index;
    
  string filename = "non_existent_file.txt";


  int songs_processed = buildIndices(filename, inverted_index, artist_index);

  EXPECT_THAT(songs_processed, Eq(0))
  << "Should return 0 when the file is not found.";

  EXPECT_THAT(inverted_index.size(), Eq(0))
  << "Inverted index should be empty when file is not found.";
  EXPECT_THAT(artist_index.size(), Eq(0))
  << "Artist index should be empty when file is not found.";
}

class FindQueryMatchesTest : public ::testing::Test {
protected:
  map<string, set<string>> simple_index = {
        {"happy", {"SongA", "SongB", "SongD"}},
        {"love", {"SongA", "SongC"}},
        {"sad", {"SongB", "SongC", "SongE"}},
        {"today", {"SongA", "SongB", "SongC", "SongD", "SongE"}}, 
        {"tomorrow", {"SongE"}},
        {"yesterday", {}} 
  };
};


TEST_F(FindQueryMatchesTest, FirstTermNotFound) {
  string query = "absent +love";
  set<string> expected = {}; 
    
  EXPECT_THAT(findQueryMatches(simple_index, query), ContainerEq(expected))
  << "Query: " << query << " -> First term not found means the initial set is empty.";
}

TEST_F(FindQueryMatchesTest, LaterTermNotFoundIntersection) {
  string query = "happy +absent"; 
  set<string> expected = {};

  EXPECT_THAT(findQueryMatches(simple_index, query), ContainerEq(expected))
  << "Query: " << query << " -> Intersection with an absent term should result in empty set.";
}


TEST_F(FindQueryMatchesTest, LaterTermNotFoundDifference) {
  string query = "happy -absent"; 
  set<string> expected = {"SongA", "SongB", "SongD"};

  EXPECT_THAT(findQueryMatches(simple_index, query), ContainerEq(expected))
  << "Query: " << query << " -> Difference with an absent term should not change the set.";
}

TEST_F(FindQueryMatchesTest, ComplexQueryExample) {
    
  string query = "love -sad happy +today";
  set<string> expected = {"SongA", "SongB", "SongD"};
    
  EXPECT_THAT(findQueryMatches(simple_index, query), ContainerEq(expected));
}

TEST_F(FindQueryMatchesTest, LaterTermNotFoundUnion) {    
    string query = "happy absent";
    set<string> expected =
    {"SongA", "SongB", "SongD"};
    
  EXPECT_THAT(findQueryMatches(simple_index, query), ContainerEq(expected));
}