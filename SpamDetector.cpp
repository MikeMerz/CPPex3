//
// Created by mikemerzl on 20/01/2020.
//
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <iostream>
#include "HashMap.hpp"
//--------------DEFINES-----------------
#define INVALID_INPUT "Invalid input"
#define USAGE_ERROR "Usage: SpamDetector <database path> <message path> <threshold>"
#define SPAM "SPAM"
#define NOT_SPAM "NOT_SPAM"
#define EXPECTED_ARG_AMOUNT 4
typedef boost::tokenizer<boost::char_separator<char>> Tok;
//----------------functions-------------------
/**
 * Find all the appreances of the current text.
 * @param map HashMap of all the database.
 * @param allText The string of all the file we check.
 * @return Final sum of the "points" of the file.
 */
static int findAll (const HashMap<std::string , int> & map , const std::string & allText);

/**
 * Check validity for second argument.
 * @param check Line we check 
 * @return true if line is invalid false otherwise
 */
static bool checkValid (const std::string & check);

/**
 * Lowers to line given to smallcase letters.
 * @param text String to lower.
 */
static void lowerAll (std::string & text);

/**
 * Check validity of the given line
 * @param tok Tokenizer
 * @return true if line is invalid false otherwise
 */
bool checkValidLine (const Tok & tok);

/**
 * Prints the final output
 * @param finalScore Final score for the current file.
 * @param minimumScore Threshhold
 */
void finalOutput (int finalScore , int minimumScore);

/**
 * Check validity of the database file given.
 * @param line Current line to check
 * @param tok Tokenizer
 * @return true if invalid false otherwise
 */
bool dbValidCheck (const std::string & line , const Tok & tok);

/**
 * Checks for the validity and amount of arguments given.
 * @param argc Number of arguments given
 * @param argv Arguments given
 * @return true if invalid false otherwise
 */
bool checkArgs (int argc , char *const *argv);

/**
 * Checks if files exists.
 * @param minimumScore Threshhold
 * @param p Current stream
 * @param text File we check
 * @return true if invalid false otherwise.
 */
bool checkExistValid (int minimumScore , const boost::filesystem::path & p ,
                      const boost::filesystem::path & text);

int main (int argc , char *argv[])
{
    if (checkArgs (argc , argv))
    {
        return (EXIT_FAILURE);
    }
    int minimumScore = (int) strtol (argv[3] , nullptr , 10);
    boost::filesystem::path p (argv[1]);
    boost::filesystem::path text (argv[2]);
    boost::filesystem::ifstream in (p);
    boost::filesystem::ifstream inT (text);
    if (checkExistValid (minimumScore , p , text))
    {
        return EXIT_FAILURE;
    }
    boost::char_separator<char> sep {","};
    std::string line;
    HashMap<std::string , int> table;
    while (getline (in , line))
    {
        Tok tok {line , sep};
        if (dbValidCheck (line , tok))
        {
            std::cerr << INVALID_INPUT << std::endl;
            in.close ();
            inT.close ();
            return (EXIT_FAILURE);
        }
        std::string name = *tok.begin ();
        int score = std::stoi ((*(++ tok.begin ())));
        lowerAll (name);
        table.insert (name , score);
    }
    std::string allText;
    std::string temp;
    while (getline (inT , temp))
    {
        lowerAll (temp);
        allText += temp;
        allText += "\n";
    }
    int finalScore = findAll (table , allText);
    finalOutput (finalScore , minimumScore);
    in.close ();
    inT.close ();
    return 0;

}

bool checkExistValid (int minimumScore , const boost::filesystem::path & p ,
                      const boost::filesystem::path & text)
{
    if (! boost::filesystem::exists (p) || ! boost::filesystem::exists (text) || minimumScore <= 0)
    {
        std::cerr << INVALID_INPUT << std::endl;
        return true;
    }
    return false;
}

bool checkArgs (int argc , char *const *argv)
{
    if (argc != EXPECTED_ARG_AMOUNT)
    {
        std::cerr << USAGE_ERROR << std::endl;
        return true;
    }
    if (checkValid (argv[3]))
    {
        std::cerr << INVALID_INPUT << std::endl;
        return true;
    }
    return false;
}

bool dbValidCheck (const std::string & line , const Tok & tok)
{
    if (line.find_first_of (',') != line.find_last_of (',') ||
        line.find_first_of (',') == std::string::npos || line.size () == 1)
    {
        return true;
    }
    return checkValidLine (tok);
}

void finalOutput (int finalScore , int minimumScore)
{
    if (finalScore >= minimumScore)
    {
        std::cout << SPAM << std::endl;
    }
    else
    {
        std::cout << NOT_SPAM << std::endl;
    }
}

bool checkValidLine (const Tok & tok)
{
    auto it = tok.begin ();
    int counter = 0;
    while (it != tok.end ())
    {
        if (counter == 1)
        {
            if (checkValid (*it))
            {
                return true;
            }
        }
        counter ++;
        ++ it;
    }
    return counter != 2;
}

static int findAll (const HashMap<std::string , int> & map , const std::string & allText)
{
    int score = 0;
    for (const auto & i : map)
    {
        size_t next = allText.find (i.first);
        while (next != std::string::npos)
        {
            score += i.second;
            next = allText.find (i.first , next + 1);
        }
    }
    return score;
}

static bool checkValid (const std::string & check)
{
    std::string::const_iterator start = check.begin ();
    while (start != check.end ())
    {
        if (! isdigit (*start))
        {
            return true;
        }
        ++ start;
    }
    return false;
}

static void lowerAll (std::string & text)
{
    for (size_t i = 0 ; i < text.size () ; ++ i)
    {
        text[i] = std::tolower (text[i]);
    }
}
