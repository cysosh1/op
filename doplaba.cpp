#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <iomanip>
#include <cmath>
#include <algorithm>

using namespace std;

string trim(string s) {
    while (!s.empty() && isspace(s.front())) s.erase(s.begin());
    while (!s.empty() && isspace(s.back())) s.pop_back();
    return s;
}

string formatDouble(double x) {
    if (fabs(x) < 1e-9) x = 0.0;

    ostringstream out;
    out << fixed << setprecision(2) << x;
    string s = out.str();

    while (s.size() > 2 && s.back() == '0') s.pop_back();
    if (s.back() == '.') s += '0';

    return s;
}

int main() {
    ifstream fin("input.txt");
    ofstream fout("output.txt");

    int n;
    fin >> n;

    vector<string> names(n);
    map<string, double> paid;
    map<string, double> norm;

    for (int i = 0; i < n; i++) {
        fin >> names[i];
        paid[names[i]] = 0.0;
        norm[names[i]] = 0.0;
    }

    string line;
    getline(fin, line); 

    while (getline(fin, line)) {
        line = trim(line);
        if (line.empty()) continue;

        size_t slashPos = line.find('/');

        string leftPart = line;
        string rightPart = "";

        if (slashPos != string::npos) {
            leftPart = trim(line.substr(0, slashPos));
            rightPart = trim(line.substr(slashPos + 1));
        }

        replace(leftPart.begin(), leftPart.end(), ':', ' ');

        string payer;
        double amount;

        stringstream ss(leftPart);
        ss >> payer >> amount;

        paid[payer] += amount;

        set<string> excluded;

        if (!rightPart.empty()) {
            stringstream rs(rightPart);
            string name;

            while (getline(rs, name, ',')) {
                name = trim(name);
                if (!name.empty()) {
                    excluded.insert(name);
                }
            }
        }

        vector<string> included;
        for (string name : names) {
            if (excluded.find(name) == excluded.end()) {
                included.push_back(name);
            }
        }

        double share = amount / included.size();

        for (string name : included) {
            norm[name] += share;
        }
    }

    for (string name : names) {
        fout << name << " "
             << formatDouble(paid[name]) << " "
             << formatDouble(norm[name]) << endl;
    }

    vector<pair<string, double>> debtors;
    vector<pair<string, double>> creditors;

    for (string name : names) {
        double balance = paid[name] - norm[name];

        if (balance < -1e-7) {
            debtors.push_back({name, -balance});
        } else if (balance > 1e-7) {
            creditors.push_back({name, balance});
        }
    }

    int i = 0, j = 0;

    while (i < debtors.size() && j < creditors.size()) {
        double sum = min(debtors[i].second, creditors[j].second);

        fout << debtors[i].first << " "
             << formatDouble(sum) << " "
             << creditors[j].first << endl;

        debtors[i].second -= sum;
        creditors[j].second -= sum;

        if (debtors[i].second < 1e-7) i++;
        if (creditors[j].second < 1e-7) j++;
    }

    fin.close();
    fout.close();

    return 0;
}