/**
 * Classifies spam and ham.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <cstdlib>
#include <cstring>

using namespace std;

struct WORD
{
	string word;
	int count;
};

void readProbabilityFile(ifstream &fin, vector<WORD> &word, long int &count);
void readTestFile(ifstream &fin, queue<string> &sms);
void parseSMS(string sms, vector<string> &words);
void naiveBayesClassifier(long int totalHamCount, long int totalSpamCount, queue<string> &sms, vector<WORD> &spam, vector<WORD> &ham, const char* file);

int main(int argc, char const *argv[])
{

	long int totalSpamCount;
	long int totalHamCount;

	queue<string> sms;
	vector<WORD> spam;
	vector<WORD> ham;

	string word;
	string count;
	
	//Get and open spam probability file
	//
	ifstream fin(argv[4]);

	readProbabilityFile(fin, spam, totalSpamCount);

	//Get and open ham probability file
	//
	fin.open(argv[6]);

	readProbabilityFile(fin, ham, totalHamCount);

	//Get and open testing file;
	//
	fin.open(argv[2]);

	readTestFile(fin, sms);

	//Classify sms data and store in output file.
	//
	naiveBayesClassifier(totalHamCount, totalSpamCount, sms, spam, ham, argv[8]);

	

	return 0;
}

void readProbabilityFile(ifstream &fin, vector<WORD> &words, long int &count)
{
	string temp_total_count;
	string temp_count;
	string word;

	getline(fin, temp_total_count);

	count = atoi(temp_total_count.c_str());

	while(!fin.eof())
	{
		getline(fin, word, ',');
		getline(fin, temp_count);

		WORD word_;
		word_.word = word;
		word_.count = atoi(temp_count.c_str());

		words.push_back(word_);
	}

	fin.close();
}

void readTestFile(ifstream &fin, queue<string> &sms)
{
	string temp;
	string newSMS;

	getline (fin, newSMS);
	newSMS.clear();

	//Get SMS messages from training file and
	//store in a queue deoending on the message
	//classification (spam|ham).
	//
	while(!fin.eof())
	{
		getline ( fin, temp, ',' );
		getline ( fin, newSMS);
		sms.push(newSMS);
		newSMS.clear();
	}

	fin.close();
}

void naiveBayesClassifier(long int totalHamCount, long int totalSpamCount, queue<string> &sms, vector<WORD> &spam, vector<WORD> &ham, const char* file)
{
	//Calculate class probabilities
	//
	//	P ( class_1 ) = count( class_1 ) / ( count(class_1) + count(class_2) )
	//
	float spamProbability = (float)(totalSpamCount/(float)(totalSpamCount + totalHamCount));
	float hamProbability = (float)(totalHamCount/(float)(totalSpamCount + totalHamCount));

	vector<string> spam_sms;
	vector<string> ham_sms;

	ofstream fout(file);

	while(!sms.empty())
	{
		//Parse an SMS message and get every word in the message
		//
		vector<string> words;
		vector<float> word_spam_probabilities;
		vector<float> word_ham_probabilities;
		parseSMS(sms.front(), words);
		
		//Calculate conditional probabilities
		//
		//	1. P ( word  ) = count ( word ) / ( total words )
		//
		//	2. P ( spam | word ) = ( P ( word | spam ) * P ( spam ) )/ P ( word )
		//	
		//	3. P ( spam | sms ) = P ( spam | word_1 ) * P ( spam | word_2 ) ...
		//							
		//	4. MAX ( P ( spam | sms ) , P ( ham | sms ) )
		//	
		//	5. Classify sms based on max
		//	

		//First find word and get count then get probability.
		//
		for (int i = 0; i < words.size(); ++i)
		{
			
			WORD newWord;
			float spamWordCount = 0;
			float hamWordCount = 0;

			//Get probability word is spam
			//
			for (int j = 0; j < spam.size(); ++j)
			{
				newWord = spam[j];

				if(words[i] == newWord.word)
				{
					spamWordCount = newWord.count;
					j = spam.size();
				}
			}

			for (int j = 0; j < ham.size(); ++j)
			{
				newWord = ham[j];

				if(words[i] == newWord.word)
				{
					hamWordCount = newWord.count;
					j = ham.size();
				}
			}

			float spam_prob = ( ( (spamWordCount / (float)totalSpamCount) * spamProbability ) / ((spamWordCount + hamWordCount) / (totalSpamCount + totalHamCount))  );
			word_spam_probabilities.push_back(spam_prob);

			float ham_prob = ( ( (hamWordCount / (float)totalHamCount) * hamProbability ) / ((spamWordCount + hamWordCount) / (totalSpamCount + totalHamCount))  );
			word_ham_probabilities.push_back(ham_prob);

			

		}

		//Now get max of sms being spam and sms being ham
		//and claassify.
		//
		float ham_prob  = 0;
		for (int i = 0; i < word_ham_probabilities.size(); ++i)
		{
			ham_prob += word_ham_probabilities[i];
		}

		float spam_prob = 0;
		for (int i = 0; i < word_spam_probabilities.size(); ++i)
		{
			spam_prob += word_spam_probabilities[i];
		}
	
		string temp = sms.front();
	
		for (int i = 0; i < 3; ++i)
		{
			temp[temp.length()-i] = '\0';
		}

		if(spam_prob < ham_prob)
		{
			fout << temp << "spam" << endl;
		}
		else
		{
			fout << temp << "ham" << endl;
		}
		
		sms.pop();
	}

	fout.close();
}

void parseSMS(string sms, vector<string> &words)
{
	//Convert sms string to array of chars.
	//
	if(sms.empty())
	{
		return;
	}
	char* cpy = new char[sms.length()+1];
	strcpy(cpy, sms.c_str());

	//Parse char array and add a word to words queue
	//when a non word character is encountered.
	//
	string newWord;

	//Loop unitl length - 3 because each sms ends with
	// ",,," which we don't need.
	// 
	for (int i = 0; i < sms.length(); ++i)
	{
		if(cpy[i] == ' ' || cpy[i] == ',' || cpy[i] == '.' || cpy[i] == '\"' || 
			cpy[i] == ':' || cpy[i] == '?' || cpy[i] == '(' || cpy[i] == ')' || 
			cpy[i] == '{' || cpy[i] == '}' || cpy[i] == '[' || cpy[i] == ']' ||
			cpy[i] == '!' || cpy[i] == '\n'|| cpy[i] == '\r')
		{
			if(newWord.empty())
			{
				;//do nothing
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
}



