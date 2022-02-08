#include <iostream>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;

int const MORE_PADDING = 5;

bool isExist(char* path) {
    struct stat buf;
    return stat(path, &buf) == 0;
}

void debug(vector<string>& data) {
    for (auto &i: data) {
        cout << i << "\n";
    }
}
// trim from right
inline string& rtrim(string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// Normalize pre-tabs
string normalize(string const& s) {
    auto it = s.begin();
    string spaces;
    while (*it == '\t' && it != s.end()) {
        spaces += "    ";
        it++;
    }
    while (it != s.end()) {
        spaces += *it;
        it++;
    }
    return spaces;
}

void removeComment(string& s) {
    auto pos = s.find_last_of("/");
    if (pos != string::npos) {
        if ((pos == 1 && s[0] == '/') || (pos > 1 && s[pos - 1] == '/' && s[pos - 2] != '"'))
            s.erase(pos - 1);
    }
}

vector<string> parseFromFile(char* path) {
    ifstream ifs(path);
    vector<string> lines;

    string buffer;
    while(getline(ifs, buffer)) {
        if (buffer.length() == 0) 
            lines.push_back("");
        else {
            rtrim(buffer); // Remove trailing characters
            buffer = normalize(buffer); // Normalize 
            removeComment(buffer);
            lines.push_back(buffer);
        }
    }

    ifs.close();
    return lines;
}

void format(vector<string>& data) {
    int maxPadding = 0;
    vector<string> lines(data.size() + 1);

    for(int i = 0; i < data.size(); i++) {
        if (data[i].length() == 0) continue;
        int j;
        maxPadding = max(maxPadding, (int)data[i].length());
        for (j = data[i].length() - 1; j >= 0; j--) {
            if (data[i][j] == ';' || data[i][j] == '{' || data[i][j] == '}') {
                break;
            }
        } // Find the last "special characters"
        while (j >=0 && (data[i][j] == ';' || data[i][j] == '{' || data[i][j] == '}')) {
            if (data[i][j] == ';') lines[i + 1] = lines[i + 1] + ";";
            else if(data[i][j] == '{') lines[i + 1] = "{" + lines[i + 1];
            else lines[i + 1] = "}" + lines[i + 1];
            data[i][j] = ' ';
            j--;
        }
        // finding the first "{"
        j = 0;
        while(j < data[i].length() && data[i][j] == ' ') j++;
        while(j < data[i].length() && data[i][j] == '{') {
            lines[i] += '{';
            data[i][j] = ' ';
            j++;
        }
        rtrim(data[i]); // remove trailing space
    }

    maxPadding += MORE_PADDING; // little space to read easier
    int i = 1;

    if (lines[0].size() > 0) {
        data.insert(data.begin(), lines[0]);
    }

    for (auto &line: data) {
        int length = max(maxPadding, (int)line.length());
        while (line.length() < length) line += " ";
        for (auto &ch : lines[i]) line += ch;
        i++;
    }
}

void writeBack(vector<string>& data, char* path) {
    ofstream ofs(path);

    for (auto &line: data) {
        ofs << line << "\n";
    }

    ofs.close();
}


auto main(int argc, char** argv) -> int {
    if (argc != 2) {
        cerr << "Wrong format!" << "\n";
        cerr << "[ Usage ]: ./program <path_to_file>" << "\n";
        return 1;
    }

    if (!isExist(argv[1])) {
        cerr << "Wrong path..." << "\n";
        return 1;
    }

    auto parsedData = parseFromFile(argv[1]);
    format(parsedData);
    writeBack(parsedData, argv[1]);
    return 0;
}
