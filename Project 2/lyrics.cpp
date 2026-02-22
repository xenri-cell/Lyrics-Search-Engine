#include "include/lyrics.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

string cleanToken(const string& token) {
  if(token.empty())  {
    return "";
  }
  // TODO student

  string s = token;

  for (char &c : s)  {
    c = tolower(c);
  }

  size_t start = 0;
  while (start < s.length() && ispunct(s[start]))  {
    start++;
  }

  size_t end = s.length();
  while (end > start && ispunct(s[end - 1]))  {
    end--;
  }

if (start >= end)  {
  return "";
}

  s = s.substr(start, end - start);

  bool has_letter = false;
  for (char c: s)  {
    if (isalpha(c))  {
      has_letter = true;
      break;
    }
  }

  if (!has_letter)  {
    return "";
  }

  return s;
}

set<string> gatherTokens(const string& text) {

  set<string> unique_token;

  stringstream ss(text);
  string token;

  while (ss >> token)  {
    string cleaned = cleanToken(token);
    if (!cleaned.empty())  {
      unique_token.insert(cleaned);
    }
  }
  return unique_token;
  // TODO student
  //return {};
}

int buildIndices(const string& filename,
                 map<string, set<string>>& inverted_index,
                 map<string, string>& artist_index) {

  // TODO student

  ifstream file(filename);
  if (!file.is_open())  {
    return 0;
  }

  string title, artist, lyrics;
  int song_count = 0;

  while (getline(file, title) && getline(file, artist) && getline(file, lyrics))  {
    artist_index[title] = artist;

    set<string> tokens = gatherTokens(lyrics);

    for (const string& token : tokens)  {
      inverted_index[token].insert(title);
    }
    song_count++;
  }
  return song_count;
}

set<string> apply_set_op(const set<string>& A, const set<string>& B, char op) {
  set<string> result_temp;

  if (op == ' ')  {
    set_union(A.begin(), A.end(),
    B.begin(), B.end(), 
    inserter(result_temp, result_temp.begin()));
  }
  else if (op == '+')  {
    set_intersection(A.begin(),A.end(), B.begin(), B.end(),
    inserter(result_temp, result_temp.begin()));
  }
  else if (op == '-')  {
    set_difference(A.begin(), A.end(), B.begin(), B.end(),
  inserter(result_temp, result_temp.begin()));
  }
  return result_temp;


}


set<string> findQueryMatches(const map<string, set<string>>& index,
                             const string& sentence) {
  // TODO student

  set<string> result_matches;
  stringstream ss(sentence);
  string term_with_modifier;
  bool first_term = true;

  while (ss >> term_with_modifier)  {
    char modifier = ' ';
    string raw_term;

    if (term_with_modifier.length() > 1 &&
    (term_with_modifier[0] == '+' || term_with_modifier[0] == '-'))  {
      modifier = term_with_modifier[0];
      raw_term = term_with_modifier.substr(1);
    } 
    else {
      raw_term = term_with_modifier;
    }

    string cleaned_term = cleanToken(raw_term);

    set<string> current_matches;
    if(!cleaned_term.empty())  {
      auto it = index.find(cleaned_term);
      if (it != index.end())  {
        current_matches = it->second;
      }
    }
    if (first_term)  {
      result_matches = current_matches;
      first_term = false;
    }
    else  {
      result_matches = apply_set_op(result_matches, current_matches, modifier);
    }
  }

  return result_matches;
}

//Helper function for Search Engines
int CountUniqueArtists(const map<string, string>& artist_index)  {
  set<string> unique_artists;
  for (const auto& pair : artist_index)  {
    unique_artists.insert(pair.second);
  }
  return unique_artists.size();
}


void searchEngine(const string& filename) {
  // TODO student

  map<string, set<string>> inverted_index;
  map<string, string> artist_index;

  int songs_processed = buildIndices(filename, inverted_index, artist_index);
  int distinct_words = inverted_index.size();
  int distinct_artists = CountUniqueArtists(artist_index);

  if(songs_processed == 0)  {
    //cout << "Error: Could not open file or no songs were processed." << endl;
    return;
  }

  cout << "Processed " << songs_processed << " songs." << endl;
  cout << "Found " << distinct_words << " distinct words." << endl;
  cout << "Found " << distinct_artists << " distinct artists." << endl;
  cout << endl;
  
  string query_sentence;
  cout <<  "Enter query sentence (press ENTER to exit):" << endl;

  while (getline(cin, query_sentence))  {
    if (query_sentence.empty())  {
      break;
    }
    set<string> matching_titles = findQueryMatches(inverted_index, query_sentence);

    cout << "Found " << matching_titles.size() << " matching songs." << endl;
    for (const string& title : matching_titles)  {
      string artist = artist_index.at(title);
      cout << "  " << title << " (by " << artist << ")" << endl;
    }
    cout << endl;
    cout << "Enter query sentence (press ENTER to exit):" << endl;
  }
  cout << "Thank you for using the search engine!" << endl;
}
