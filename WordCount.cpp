// File name: WordCount.cpp
// Author: Jungwon Jang
// Userid: jangj1
// Email: jungwon.jang@vanderbilt.edu
// Class: CS 2201
// Assignment Number: 8
// Honor Statement: I have not given or received unauthorized aid on this assignment.
// Description: counts the number of occurrences of words in a file, while ignoring common words.
// Last Changed: Nov.13.2019


#include <cstdlib>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <fstream>


const double MINIMUM_APPEARANCE = 0.001;
const int STEM_LENGTH = 6;

//DO NOT CHANGE THESE
typedef std::set<std::string> WordSet;
typedef std::map<std::string, int> WordCountMap;


// function prototypes
// you are not allowed to change these function headers,
// but you can add helper functions as needed.
WordSet readCommonWords(const std::string &fname);

WordCountMap processFile(const std::string &fname, const WordSet &common);

double compareTexts(WordCountMap &first,
                    WordCountMap &second,
                    const std::string &fname);


/* main */
int main() {
    std::string commonFile = "texts/common.txt";
    std::string fin1, fin2, fout, str;

    do {
        std::cout << "Enter name of the first input file: ";
        std::cin >> fin1;
        fin1 = "texts/" + fin1;

        std::cout << "Enter name of the second input file: ";
        std::cin >> fin2;
        fin2 = "texts/" + fin2;

        std::cout << "Enter name of the output file: ";
        std::cin >> fout;
        fout = "texts/" + fout;

        WordSet common = readCommonWords(commonFile);

        WordCountMap first = processFile(fin1, common);
        WordCountMap second = processFile(fin2, common);

        double dist = compareTexts(first, second, fout);

        std::cout << "Vector Distance: " << dist << std::endl;

        std::cout << "Would you like to run the program again (Y|N): ";
        std::cin >> str;
    } while (tolower(str[0]) == 'y');


    return 0;
}

/* readCommonWords
 * This method reads words from a given file and places them
 * into a WordSet, which it returns.
 *
 * pre:  a valid file name with 1 word per line, words all in lower case
 * post: all words int the file are placed in the WordSet
 */
WordSet readCommonWords(const std::string &fname) {
    WordSet ret;

    std::ifstream file(fname.c_str());
    // Do error checking here
    if (!file) {
        std::cout << "Error opening output data file" << std::endl;
        std::cout << "Exiting";
        exit(1);
    }

    std::string word;
    while(file>>word){
        ret.insert(word);
    }

    file.close();
    return ret;
}


/* processFile
 * This function reads in all words from the given file
 * after reading a word it converts it to lower case,
 * removes non alphabetic characters and stems it to STEM_LENGTH.
 * If the resulting word is considered common it is ignored.
 * Otherwise, the count in the map that matches the word
 * is incremented.
 *
 * pre:  the name of a text file and a set of words to be ignored.
 * post: The file has been read; a map of cleansed words is created and returned
 *
 */
WordCountMap processFile(const std::string &fname, const WordSet &common) {
    WordCountMap ret;

    std::ifstream file(fname.c_str());
    // Do error checking here
    if (!file) {
        std::cout << "Error opening output data file" << std::endl;
        std::cout << "Exiting";
        exit(1);
    }

    std::string word;
    std::string fword;
    while(file>>word){
        int l = word.length();
        if(l>STEM_LENGTH){
            word.resize(STEM_LENGTH-1);
            l = STEM_LENGTH;
        }
        for(int i=0; i <= l; i++){
            if(std::isupper(word[i])) {
                std::tolower(word[i]);
            }
            fword += word[i];
        }
        if(common.count(fword)==0){
            ret[fword] += 1;
        }
    }
    return ret;
}


/* compareTexts
 * Compares the count maps of 2 texts.
 * The result returned is the euclidean distance
 * where each word with a percentile appearance greater than
 * MINIMUM_APPEARANCE counts as a dimension of the vectors.
 * (they still count toward the full sum used to calculate
 *  percentile appearance)
 *
 * This method also prints out the results to a file using the following
 * format (start and end tags added for clarity):
 * / -- start example output --/
 * word1:	distance = x.xxxxxe-xxx	(score1 = y.yyyyyyyy, score2 = z.zzzzzzz)
 * word2:	distance = x.xxxxxe-xxx	(score1 = y.yyyyyyyy, score2 = z.zzzzzzz)
 * ...
 * wordN:	distance = x.xxxxxe-xxx	(score1 = y.yyyyyyyy, score2 = z.zzzzzzz)
 *
 * Counted words in document 1 = xxxx
 * Counted words in document 2 = xxxx
 *
 * Vector Distance: dist
 * /-- end example output --/
 *
 * word1-wordN are all words whose comparison is include in the final sum.
 * distance values are the square of the difference of the percentile
 *                         scores from the two maps.
 * score1 & score2 values are the scores from the two maps.
 * dist is the final distance that is also returned,
 *    it is the square root of the sum of all the individual distances.
 * To help line things up, there is a tab character after the ":" and another
 *    before the "(" on the line for each word.
 *
 * pre:  two maps of texts to be compared and the name of an output file
 * post: returns the euclidean distance of the two files
 *       if the output file cannot be opened, the method prints
 *       an error message and exits the program.
 */
double compareTexts(WordCountMap &first,
                    WordCountMap &second,
                    const std::string &fname) {

    std::ofstream outfile;
    outfile.open(fname.c_str());
    if (outfile.fail()) {
        std::cout << "Error opening output data file" << std::endl;
        std::cout << "Exiting";
        exit(1);
    }

    double sum1 = 0.0;
    double sum2 = 0.0;
    WordCountMap::iterator p;
    for(p=second.begin(); p != second.end(); p++){
        sum2 += p->second;
    }
    WordCountMap::iterator b;
    for(b=first.begin(); b != first.end(); b++){
        sum1 += b->second;
    }


    double score1 = 0.0;
    double score2 = 0.0;
    double distance = 0.0;
    double fdistance = 0.0;
    WordCountMap::iterator i;
    for(i=second.begin(); i != second.end(); i++){
        if(first.count(i->first) != 0){
            score1 = (double)(first[i->first])/sum1;
            score2 = (double)(i->second)/sum2;
            if(score1 >= MINIMUM_APPEARANCE && score2 >= MINIMUM_APPEARANCE) {
                distance = pow((score1 - score2), 2);
                fdistance += distance;
                std::string word = i->first;
                outfile << word << "\tdistance = " << distance << "\t(score1 = " << score1 << ", score2 = " << score2
                     << ")\n";
            }
        }
    }
    outfile << "\nCounted words in document 1 = " << sum1 << "\n";
    outfile << "Counted words in document 2 = " << sum2 << "\n";
    fdistance = sqrt(fdistance);
    outfile << "\nVector Distance: " << fdistance << "\n";

    outfile.close();

    return fdistance;
}