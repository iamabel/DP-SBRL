#include <fstream>
#include <iterator>
#include <iostream>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

namespace {
  string sjoin(const set<string> &s) {
    stringstream ss;
    int i = 0;
    for(string str : s) {
      if(i != 0) {
        ss << " ";
      }
      ss << str;
      i++;
    }
    return ss.str();
  }

  string vjoin(const vector<string> &v) {
    stringstream ss;
    for(int i = 0; i < v.size(); ++i) {
      if(i != 0) {
        ss << " ";
      }
      ss << v[i];
    }
    return ss.str();
  }

  // Split string on spaces
  vector<string> split(string str) {
    stringstream ss(str);
    istream_iterator<string> begin(ss);
    istream_iterator<string> end;
    vector<string> str_v(begin, end);

    return str_v;
  }

  unordered_set<string> getFrequent(int num_transactions, float minsup,
                                    const unordered_map<string, int> &set_counts) {
    unordered_set<string> frequent_items;
    for (auto item_count: set_counts) {
      if (item_count.second >= minsup*num_transactions) {
        frequent_items.insert(item_count.first);
      }
    }
    return frequent_items;
  }

  unordered_set<string> getLargerItemsets(const unordered_set<string> &frequent_items) {
    // Turn string itemsets into their sets.
    vector<vector<string>> frequent_sets;
    for (string itemset : frequent_items) {
      frequent_sets.push_back(split(itemset));
    }
    int k = frequent_sets[0].size();

    unordered_set<string> true_larger_sets;
    for (int i = 0; i < frequent_sets.size(); ++i) {      // Get all set unions.
      for (int j = i+1; j < frequent_sets.size(); ++j) {
        set<string> itemset(frequent_sets[i].begin(), frequent_sets[i].end());
        for (int z = 0; z < k; ++z) {
          itemset.insert(frequent_sets[j][z]);
        }

        if (itemset.size() == k+1) {
          true_larger_sets.insert(sjoin(itemset));
        }
      }
    }

    return true_larger_sets;
  }

  unordered_set<string> largerFrequentItemsets(const unordered_set<string> &frequent_items,
                                               const vector<unordered_set<string>> &transactions,
                                               float minsup) {
    unordered_map<string, int> counts;
    unordered_set<string> potential_freq = getLargerItemsets(frequent_items);

    // Count occurences of potential frequent itemsets, size k+1.
    vector<vector<string>> potential_freq_v;
    for (string itemset : potential_freq) {
      potential_freq_v.push_back(split(itemset));
    }
    int k = potential_freq_v[0].size();
    bool subset = true;
    // TODO: Can just do intersection of the sets of occurences, which would be faster
    for (vector<string> freq_v : potential_freq_v) {
      for (int j = 0; j < transactions.size(); ++j) {
        for (int z = 0; z < k; ++z) {
          if (transactions[j].find(freq_v[z]) == transactions[j].end()) {
            subset = false;
            break;
          }
        }
        if (subset) counts[vjoin(freq_v)]++;
        subset = true;
      }
    }

    return getFrequent(transactions.size(), minsup, counts);
  }
} //  end namespace

int main(int argc, char** argv) {
  // (confidence pruning)
  char *filename = (char *)"../Testing/dat1.txt";
  float minsup = 0.375f;
  float minconf = 0.5f;
  for (int i = 1; i < argc; ++i) {
      if (string(argv[i]) == "--file") {
          if (i + 1 < argc) {
              filename = argv[++i];
          } else { // Trial flag called but unspecified
                cerr << "--file option requires one argument." << endl;
              return 1;
          }
      } else if (string(argv[i]) == "--minsup") {
        if (i + 1 < argc) {
            minsup = atof(argv[++i]);
        } else { // Graph flag called but unspecified
              cerr << "--minsup option requires one argument." << endl;
            return 1;
        }
      } else if (string(argv[i]) == "--minconf") {
        if (i + 1 < argc) {
            minconf = atof(argv[++i]);
        } else { // Graph flag called but unspecified
              cerr << "--minconf option requires one argument." << endl;
            return 1;
        }
      }
  }

  // Items, counts, 'transactions'.
  unordered_map<string, int> oneset_counts;
  vector<unordered_set<string>> transactions;
  // Need a standardized delimiter that distinguishes 'items'.
  string delim = " ";
  {
    ifstream in(filename);

    if (!in.good()) {
        fprintf(stderr, "Can't open transactions file\n");
        return -1;
    }

    string transaction;
    while (getline(in, transaction)) {
      int start = 0, end = transaction.find(delim);
      unordered_set<string> items_curr;
      string item;

      while (end != string::npos) {
        // Note if element exists, this increments, if it does not it is init
        // to 0 then incremented to 1.
        item = transaction.substr(start, end - start);

        // Ensure uniqueness per transaction
        // Sets maintain uniquesness, however we want to count elements as well, this allows us to
        // limit iterations (we won't have to iterate through the set after).
        if (items_curr.find(item) == items_curr.end()) {
          oneset_counts[item]++;
          items_curr.insert(item);
        }

        start = end + 1;
        end = transaction.find(delim, start);
      }

      item = transaction.substr(start);
      if (items_curr.find(item) == items_curr.end()) {
        oneset_counts[item]++;
        items_curr.insert(item);
      }

      transactions.push_back(items_curr);
    }
    in.close();
  }
  unordered_set<string> frequent_onesets = getFrequent(transactions.size(), minsup, oneset_counts);

  unordered_set<string> next_sets = largerFrequentItemsets(frequent_onesets, transactions, minsup);
  unordered_set<string> frequent_itemsets(frequent_onesets.begin(), frequent_onesets.end());
  while (next_sets.size() > 1) {
    frequent_itemsets.insert(next_sets.begin(), next_sets.end());
    next_sets = largerFrequentItemsets(next_sets, transactions, minsup);
  }
  frequent_itemsets.insert(next_sets.begin(), next_sets.end());

  printf("-------------Frequent Itemsets------------\n");
  for (string itemset : frequent_itemsets) {
    printf("%s\n", itemset.c_str());
  }

  return 0;
}
