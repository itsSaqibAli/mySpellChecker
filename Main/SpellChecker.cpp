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

// Main function

int main() {
    Trie trie;
    vector<string> words = {"example", "sample", "simple", "examine", "exemplary"};

    for (const string& word : words) {
        trie.insert(word);
    }

    string misspelledWord;

    cout << "Enter the misspelled word: ";
    cin >> misspelledWord;

    int maxDist = 2;
    vector<pair<string, int>> suggestions = findClosestWords(trie, misspelledWord, maxDist);

    cout << "Suggestions for '" << misspelledWord << "':" << endl;
    for (const auto& [suggestion, dist] : suggestions) {
        cout << " - " << suggestion << " (distance: " << dist << ")" << endl;
    }

    return 0;
}
