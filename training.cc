#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <set>
#include <map>

using namespace std;

vector<string> split(const string& str, const string& del, bool remove_empty=true)
{
    vector<string> arr;
	string token;
	size_t start = 0;
	size_t end = str.find(del);
	
	while (end != string::npos)
	{
		token = str.substr(start, end - start);
		if (token != "" || remove_empty)
			arr.push_back(token);
		start = end + del.size();
		end = str.find(del, start);
	}
	token = str.substr(start, end - start);
	if (token != "" || remove_empty)
		arr.push_back(token);
	
	return arr;
}

struct Document
{
    string type;
    vector<string> words;
    
    Document(string t, const vector<string>& v) : type(t), words(v) {}
};


class NaiveBayesText
{
    map<string, double> Pc;
    map<pair<string, string>, double> Pwc;
    
    vector<Document> D;
    vector<string> C;
    
public:
    NaiveBayesText(const vector<Document> D, vector<string> C)
    {
        this->D = D;
        this->C = C;
    }
    
    void learnBernoulli()
    {
        set<string> V;
        map<pair<string, string>, int> wordtype; //word, type -> phrases
        map<string, int> subsets; //type -> words
        
        for(auto& c : this->C)
            subsets[c] = 0;
        
        for(auto& d : this->D) {
            set<string> words(d.words.begin(), d.words.end()); //we don't want duplicates
            
            for(auto& word : words) {
                auto k = pair<string, string>(word, d.type);
                if(wordtype.find(k) == wordtype.end())
                    wordtype[k] = 1;
                else
                    wordtype[k] += 1;
                V.insert(word);
            }
            
            subsets[d.type] += 1;
        }
        
        for(auto& c : this->C) {
            double t = subsets[c];
            this->Pc[c] = t / this->D.size();
            
            for(auto& w : V) {
                double tt = wordtype[pair<string, string>(w, c)];
                this->Pwc[pair<string, string>(w, c)] = (tt +1) / (t +2);
            }
        }
    }
    
    void learnMultinomial()
    {
        set<string> V;
        map<pair<string, string>, int> wordtype; //word, type -> phrases
        map<string, vector<Document*> > subsets; //type -> words
        map<string, int> wordcount; //type -> ords
        
        for(auto& d : this->D) {
            for(auto& word : d.words) {
                auto k = pair<string, string>(word, d.type);
                if(wordtype.find(k) == wordtype.end())
                    wordtype[k] = 1;
                else
                    wordtype[k] += 1;
                V.insert(word);
            }
            
            subsets[d.type].push_back(&d);
        }
        
        for(auto& c : this->C) {
            double t = subsets[c].size();
            this->Pc[c] = t / this->D.size();
            
            int tf = 0;
            for(auto& p : subsets[c]) {
                tf += p->words.size();
            }
            
            for(auto& w : V) {
                double tt = wordtype[pair<string, string>(w, c)];
                this->Pwc[pair<string, string>(w, c)] = (tt +1) / (tf + V.size());
            }
        }
    }
    
    string savePcCSV(const string& filename)
    {
        ofstream ofs(filename, ofstream::out);
        for(auto& c : this->C) {
            ofs << c << "," << this->Pc[c] << endl;
        }
        ofs.close();
    }
    
    string savePwcCSV(const string& filename)
    {
        ofstream ofs(filename, ofstream::out);
        for(auto& p : this->Pwc) {
            auto w = p.first.first;
            auto c = p.first.second;
            ofs << w << "," << c << "," << p.second << endl;
        }
        ofs.close();
    }
};

int main(int argc, char** argv)
{
    if(argc < 2) {
        cerr << "usage: ./training <bernoulli|multinomial>\n";
        return 1;
    }
    string learning_type = argv[1];
    
    ifstream ifs("training.csv", ifstream::in);
    string line;
    vector<Document> dataset;
    
    while(ifs.good()) {
        getline(ifs, line);
        vector<string> v = split(line, ",");
        if(v.size() == 0)
            continue;
        
        string type = v[0];
        vector<string> words(make_move_iterator(v.begin() +1), make_move_iterator(v.end()));
        dataset.push_back(Document(type, words));
    }
    ifs.close();
    
    vector<string> classes = {"spam", "ham"};
    
    NaiveBayesText nbt(dataset, classes);
    if(learning_type == "bernoulli")
        nbt.learnBernoulli();
    else if(learning_type == "multinomial")
        nbt.learnMultinomial();
    else {
        cerr << "are u dumb?\nusage: ./training <bernoulli|multinomial>\n";
        return 1;
    }
    nbt.savePcCSV(learning_type + "_Pc.csv");
    nbt.savePwcCSV(learning_type + "_Pwc.csv");
    
    return 0;
}

