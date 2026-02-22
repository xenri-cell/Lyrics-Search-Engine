# Lyrics-Search-Engine
A search engine that indexes song lyrics and supports Boolean-style queries. Built using C++ maps, sets, file I/O.

## Features
- Cleans and normalizes tokens (punctuate removal, lowercase, alphabet filtering)
- Builds an inverted index mapping words-> song titles
- Tracks artists for each song

## Input Format
The input file should contain repeating groups of: 
1.) song title
2.) Artist name
3.) Lyrics (single line)

## How to Build
g++ main.cpp -o search

## How to Run
./search

