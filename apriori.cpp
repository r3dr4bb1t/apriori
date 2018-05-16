
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <tuple>
#include <fstream>
#include <cmath>
#include <iomanip>

using namespace std;

long double roundup(long double value, int pos) {
	double temp = value;

	temp = value * pow(10, pos);
	temp = floor(temp + 0.5);
	temp *= pow(10, -pos);
	return temp;
}



class Repos {
private:
	int step;
	long double minSup;

	vector<vector<int> > C, L;
	vector<vector<vector<int> > > frequentSet;
	vector<vector<int> > transactions;
	vector<tuple<vector<int>, vector<int>, long double, long double> > associationRules;
public:
	Repos(vector<vector<int> > t, long double m) {
		step = 0;
		minSup = m;
		for (auto&row : t) {
			sort(row.begin(), row.end());
			transactions.push_back(row);
		}
		frequentSet.push_back({ {} });
	}

	vector<tuple<vector<int>, vector<int>, long double, long double> > GETRULES() {
		return associationRules;
	}

	void mainstep() {
		while (true) {
			C = Candidate();
			if (C.size() == 0) break;
			step++;

			L = Lplusone();
			frequentSet.push_back(L);
		}

		for (auto&i : frequentSet) {
			for (auto&items : i) {
				getassociation(items, {}, {}, 0);
			}
		}
	}




	void getassociation(vector<int> items, vector<int> X, vector<int> Y, int index) {
		if (index == items.size()) {
			if (X.size() == 0 || Y.size() == 0) return;
			long double XY_sup = getsup(items);
			long double X_sup = getsup(X);

			if (X_sup == 0) return;

			long double support = (double)XY_sup;
			long double confidence = (long double)XY_sup / X_sup * 100.0;
			associationRules.push_back({ X, Y, support, confidence });
			return;
		}

		X.push_back(items[index]);
		getassociation(items, X, Y, index + 1);
		X.pop_back();
		Y.push_back(items[index]);
		getassociation(items, X, Y, index + 1);
	}


	vector<int> getElement(vector<vector<int> > itemset) {
		vector<int> element;
		set<int> s;
		for (auto&row : itemset) for (auto&col : row) s.insert(col);
		for (auto iter = s.begin(); iter != s.end(); iter++) element.push_back(*iter);
		return element;
	}


	vector<vector<int> > Candidate() {
		if (step == 0) {
			vector<vector<int> > cand;
			vector<int> element = getElement(transactions);
			for (auto&i : element) cand.push_back(vector<int>(1, i));

			return cand;
		}
		else {
			return prune(join());
		}
	}

	vector<vector<int> > join() {
		vector<vector<int> > ret;
		for (int i = 0; i<L.size(); i++) {
			for (int j = i + 1; j<L.size(); j++) {
				int k;
				for (k = 0; k<step - 1; k++) {
					if (L[i][k] != L[j][k]) break;
				}
				if (k == step - 1) {
					vector<int> tmp;
					for (int k = 0; k<step - 1; k++) {
						tmp.push_back(L[i][k]);
					}
					int a = L[i][step - 1];
					int b = L[j][step - 1];
					if (a>b) swap(a, b);
					tmp.push_back(a), tmp.push_back(b);
					ret.push_back(tmp);
				}
			}
		}
		return ret;
	}

	vector<vector<int> > prune(vector<vector<int> > j) {
		vector<vector<int> > t;

		set<vector<int> > s;
		for (auto&row : L) s.insert(row);

		for (auto&row : j) {
			int i;
			for (i = 0; i<row.size(); i++) {
				vector<int> tmp = row;
				tmp.erase(tmp.begin() + i);
				if (s.find(tmp) == s.end()) {
					break;
				}
			}
			if (i == row.size()) {
				t.push_back(row);
			}
		}
		return t;
	}
	long double getsup(vector<int> item) {
		int r = 0;
		for (auto&row : transactions) {
			int i, j;
			if (row.size() < item.size()) continue;
			for (i = 0, j = 0; i < row.size(); i++) {
				if (j == item.size()) break;
				if (row[i] == item[j]) j++;
			}
			if (j == item.size()) {
				r++;
			}
		}
		return (long double)r / transactions.size()*100.0;
	}

	vector<vector<int> > Lplusone() {
		vector<vector<int> > cand;
		for (auto&row : C) {
			long double support = getsup(row);
			if (roundup(support, 2) < minSup) continue;
			cand.push_back(row);
		}
		return cand;
	}
};



class Read {
private:
	ifstream fin;
	vector<vector<int> > transactions;
public:
	Read(string filename) {
		fin.open(filename);
		if (!fin) {
			cout << "no input file \n";
			exit(0);
		}
		parse();
	}
	void parse() {
		string str;
		while (!getline(fin, str).eof()) {
			vector<int> arr;
			int pre = 0;
			for (int i = 0; i<str.size(); i++) {
				if (str[i] == '\t') {
					int num = atoi(str.substr(pre, i).c_str());
					arr.push_back(num);
					pre = i + 1;
				}
			}
			int num = atoi(str.substr(pre, str.size()).c_str());
			arr.push_back(num);

			transactions.push_back(arr);
		}
	}
	vector<vector<int> > getTransactions() {
		return transactions;
	}
};


class write {
private:
	ofstream fout;
	vector<tuple<vector<int>, vector<int>, long double, long double> > associationRules;
public:
	write(string filename, vector<tuple<vector<int>, vector<int>, long double, long double> > tmprule) {
		fout.open(filename);
		if (!fout) {
			cout << "no Output d\n";
			exit(0);
		}
		associationRules = tmprule;
		genoutput();
	}

void genoutput() {
	for (auto&i : associationRules) {
		fout << ToString(get<0>(i)) << '\t';
		fout << ToString(get<1>(i)) << '\t';

		fout << fixed;
		fout.precision(2);
		fout << get<2>(i)-0.01 << '\t';

		fout << fixed;
		fout.precision(2);
		fout << get<3>(i);

		fout << endl;
	}
}

string ToString(vector<int> arr) {
	string str = "{";
	for (int i = 0; i<arr.size(); i++) {
		str += to_string(arr[i]);
		if (i != arr.size() - 1) {
			str += ",";
		}
	}
	str += "}";
	return str;
}
};

int main(int argc, char ** argv) {
	if (argc != 4)
	{
		cout << "input error";
		exit(0);;
	}
	string minsup(argv[1]);
	string input(argv[2]);
	string output(argv[3]);

	Read Reading(input);

	vector<vector<int> > transactions = Reading.getTransactions();

	Repos a(transactions, stold(minsup));
	a.mainstep();
	write writing(output, a.GETRULES());



	return 0;
}

