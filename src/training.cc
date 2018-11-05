/**
 * Training file.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>

using namespace std;

struct WORD
{
	string word;
	long int count;
};

void parse(queue<string> sms, const char* file);

int main(int argc, char const *argv[])
{

	ifstream fin(argv[2]);

	queue<string> ham_sms;
	queue<string> spam_sms;

	long int spamCount = 0;
	long int hamCount = 0;

	string type;
	string newSMS;

	getline (fin, newSMS);
	newSMS.clear();

	//Get SMS messages from training file and
	//store in a queue deoending on the message
	//classification (spam|ham).
	//
	while(!fin.eof())
	{
		getline ( fin, type, ',' );
		getline ( fin, newSMS);

		if(type == "ham")
		{

			ham_sms.push(newSMS);
			type.clear();
			newSMS.clear();
			hamCount++;
		}
		else if(type == "spam")
		{
			spam_sms.push(newSMS);
			type.clear();
			newSMS.clear();
			spamCount++;
		}
	}

	fin.close();

	//Parse ham meesages and store word count, words, and
	//word occurences in csv probability file.
	//
	parse(ham_sms, argv[4]);

	//Parse spam meesages and store word count, words, and
	//word occurences in csv probability file.
	//
	parse(spam_sms, argv[6]);	





	return 0;
}

void parse(queue<string> sms, const char* file)
{
	vector<string> words;
	queue<WORD> wordsandcounts;
	long int totalWordCount = 0;

	//Parse sms messages and store every word.
	//
	while(!sms.empty())
	{
		//Convert sms string to array of chars.
		//
		string temp = sms.front();
		char* cpy = new char[temp.length()+1];
		strcpy(cpy, temp.c_str());

		//Parse char array and add a word to words queue
		//when a non word character is encountered.
		//
		string newWord;

		//Loop unitl length - 3 because each sms ends with
		// ",,," which we don't need.
		// 
		for (int i = 0; i < temp.length(); ++i)
		{
			if(cpy[i] == ' ' || cpy[i] == ',' || cpy[i] == '.' || cpy[i] == '\"' || 
				cpy[i] == ':' || cpy[i] == '?' || cpy[i] == '(' || cpy[i] == ')' || 
				cpy[i] == '{' || cpy[i] == '}' || cpy[i] == '[' || cpy[i] == ']' ||
				cpy[i] == '!' || cpy[i] == '\n'|| cpy[i] == '\r')
			{
				if(newWord.empty())
				{
					//do nothing
				}
				else
				{
					//Put new word in queue.
					//
					words.push_back(newWord);

					//Clear word.
					//
					newWord.clear();
				}
			}
			else
			{
				//Add letter to word.
				//
				newWord += cpy[i];
			}
		}

		//Pop sms from sms queue.
		//
		sms.pop();
	}

	//Find occurences of each word by iterating through all words.
	//O(n^2) amount of time, could probably improve this.
	//
	long int index = 0;

	while(index < words.size())
	{

		if(words[index] == "\0")
		{
			index++;
		}
		else
		{
			WORD word_;
			word_.word = words[index];
			word_.count = 0;

			for (int i = 0; i < words.size(); ++i)
			{
				if(word_.word == words[i])
				{
					//Increment occurence of word
					//
					word_.count++;

					//Remove word occurence from vector
					//
					words[i] = "\0";
				}
			}

			wordsandcounts.push(word_);
			index++;

		}
		
	}

	
	//Put words and counts in probability file.
	//
	ofstream fout(file);

	totalWordCount = wordsandcounts.size();

	fout << totalWordCount << endl;

	WORD word_ = wordsandcounts.front();

	while(!wordsandcounts.empty())
	{
		WORD word_ = wordsandcounts.front();
		fout << word_.word << "," << word_.count << endl;
		wordsandcounts.pop();
	}
	
	fout.close();

}



