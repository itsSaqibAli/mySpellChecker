#include <bits/stdc++.h>

using namespace std;

class TrieNode {
public:
    unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
    string word;

    TrieNode() {
        isEndOfWord = false;
        word = "";
    }
};

class Trie {
public:
    TrieNode* root;

    Trie() {
        root = new TrieNode();
    }

    void insert(const string& word) {
        TrieNode* node = root;
        for (char c : word) {
            if (node->children.find(c) == node->children.end()) {
                node->children[c] = new TrieNode();
            }
            node = node->children[c];
        }
        node->isEndOfWord = true;
        node->word = word; // Store the word in the end node
    }

    string getWord(TrieNode* node) {
        return node->word; // Retrieve the word stored in the end node
    }
};


// Implementing the recursive edit distance word search function

vector<pair<string, int> > search(Trie& trie, TrieNode* node, const string& word, int index, vector<int>& prevRow, int maxDist) {
    vector<pair<string, int> > results;

    // Check if we're at the end of a word in the trie
    if (node->isEndOfWord && prevRow.back() <= maxDist) {
        results.emplace_back(trie.getWord(node), prevRow.back());
    }

    // Iterate over each child in the trie node
    for (auto& [ch, childNode] : node->children) {  // Changed `char` to `ch`
        vector<int> curRow(prevRow.size());
        curRow[0] = prevRow[0] + 1;

        for (int i = 1; i <= word.size(); ++i) {
            int insertCost = curRow[i - 1] + 1;
            int deleteCost = prevRow[i] + 1;
            int replaceCost = prevRow[i - 1] + (word[i - 1] == ch ? 0 : 1);
            curRow[i] = min({insertCost, deleteCost, replaceCost});
        }

        // If the minimum value in the current row is within the maxDist, continue searching
        if (*min_element(curRow.begin(), curRow.end()) <= maxDist) {
            auto childResults = search(trie, childNode, word, index + 1, curRow, maxDist);
            results.insert(results.end(), childResults.begin(), childResults.end());
        }
    }

    return results;
}


// Wrapper function to find closest Words

vector<pair<string, int>> findClosestWords(Trie& trie, const string& word, int maxDist) {
    // Initialize the previous row (edit distance matrix)
    vector<int> prevRow(word.size() + 1);
    for (int i = 0; i <= word.size(); ++i) {
        prevRow[i] = i;
    }

    // Perform search from the root of the trie
    return search(trie, trie.root, word, 0, prevRow, maxDist);
}

// Load dictionary and insert into Trie

Trie loadDictionary(const string& filename) {
    Trie trie;
    ifstream infile(filename);
    string word;
    while (infile >> word) {
        trie.insert(word);
    }
    return trie;
}


// Main function

int main() {
    Trie trie = loadDictionary("dictionary.txt");

    string misspelledWord;

    while(true) {

        cout << "Enter the word or press 2 to exit: ";
        cin >> misspelledWord;

        if(misspelledWord == "2") break;

        // Check if the word is correctly spelled
        if (trie.root->children.find(misspelledWord[0]) != trie.root->children.end()) {
            TrieNode* node = trie.root;
            for (char c : misspelledWord) {
                if (node->children.find(c) == node->children.end()) {
                    break;
                }
                node = node->children[c];
            }
        }

        int maxDist = 3;
        vector<pair<string, int>> suggestions = findClosestWords(trie, misspelledWord, maxDist);

        // Sort suggestions by edit distance and then lexicographically
        sort(suggestions.begin(), suggestions.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            if (a.second == b.second)
                return a.first < b.first;
            return a.second < b.second;
        });

        cout << "Suggestions for '" << misspelledWord << "':" << endl;
        int suggestionCount = 0;
        for (const auto& [suggestion, dist] : suggestions) {
            if (suggestionCount++ == 10) break;
            cout << " - " << suggestion << " - " <<endl;
        }
    }

    return 0;
}
