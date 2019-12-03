#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <functional>

using namespace std;

class Tape {
  int pos;
  vector<int> tape;

public:
  Tape() {
    pos = 0;
    tape.push_back(0);
  }

  int get() { return tape[pos]; }
  void inc() { tape[pos]++; }
  void dec() { tape[pos]--; }
  void advance() { pos++; if (tape.size() <= pos) tape.push_back(0); }
  void devance() { if (pos > 0) pos--; }
};

class Program {
  string code;
  map<int, int> bracket_map;

public:
  Program(string text) {
    vector<int> leftstack;
    string symbols = "[]<>+-,.";

    int pc = 0;
    for (int i = 0; i < text.size(); i++) {
      char c = text[i];

      if (symbols.find(c) == string::npos) continue;

      if (c == '[') leftstack.push_back(pc);
      else
        if (c == ']' && leftstack.size() != 0) {
          int left = leftstack[leftstack.size() - 1];
          leftstack.pop_back();
          int right = pc;
          bracket_map[left] = right;
          bracket_map[right] = left;
        }

      pc++;
      code.push_back(c);
    }
  }

  void run() {
    Tape tape;
    for (int pc = 0; pc < code.length(); pc++) {
      switch (code[pc]) {
        case '+':
          tape.inc();
          break;
        case '-':
          tape.dec();
          break;
        case '>':
          tape.advance();
          break;
        case '<':
          tape.devance();
          break;
        case '[':
          if (tape.get() == 0) pc = bracket_map[pc];
          break;
        case ']':
          if (tape.get() != 0) pc = bracket_map[pc];
          break;
        case '.':
          printf("%c", tape.get());
          fflush(stdout);
          break;
      }
    }
  }
};

string read_file(string filename){
  ifstream textstream(filename.c_str());
  textstream.seekg(0, ios_base::end);
  const int lenght = textstream.tellg();
  textstream.seekg(0);
  string text(lenght, ' ');
  textstream.read(&text[0], lenght);
  textstream.close();
  return text;
}

int main(int argc, char** argv) {
  {
    unique_ptr<int, function<void(int*)>> sock(
      new int(socket(AF_INET, SOCK_STREAM, 0)),
      [](int *s){ close(*s); });
    struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(9001)
    };
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    if (!connect(*sock.get(), (struct sockaddr *)&serv_addr, sizeof(serv_addr))) {
      string msg("C++");
      send(*sock.get(), msg.c_str(), msg.size(), 0);
    }
  }

  string text = read_file(string(argv[1]));
  Program p(text);
  p.run();
  return 0;
}

