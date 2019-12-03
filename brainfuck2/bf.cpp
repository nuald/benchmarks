#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <functional>

using namespace std;

enum op_type { INC, MOVE, LOOP, PRINT };
struct Op;
typedef vector<Op> Ops;

struct Op {
  op_type op;
  int val;
  Ops loop;
  Op(Ops v) : op(LOOP), loop(v) {}
  Op(op_type _op, int v = 0) : op(_op), val(v) {}
};

class Tape {
  int pos;
  vector<int> tape;

public:
  Tape() {
    pos = 0;
    tape.push_back(0);
  }

  int get() { return tape[pos]; }
  void inc(int x) { tape[pos] += x; }
  void move(int x) { pos += x; while (pos >= tape.size()) tape.resize(2 * tape.size()); }
};

class Program {
  Ops ops;

public:

  Program(string code) {
    string::iterator iterator = code.begin();
    ops = parse(&iterator, code.end());
  }

  void run() {
    Tape tape;
    _run(ops, tape);
  }

private:

  Ops parse(string::iterator *iterator, string::iterator end) {
    Ops res;
    while (*iterator != end) {
      char c = **iterator;
      *iterator += 1;
      switch (c) {      
        case '+': res.push_back(Op(INC, 1)); break;
        case '-': res.push_back(Op(INC, -1)); break;
        case '>': res.push_back(Op(MOVE, 1)); break;
        case '<': res.push_back(Op(MOVE, -1)); break;
        case '.': res.push_back(Op(PRINT)); break;
        case '[': res.push_back(Op(parse(iterator, end))); break;
        case ']': return res;
      }
    }
    return res;
  }

  void _run(Ops &program, Tape &tape) {
    for (Ops::iterator it = program.begin(); it != program.end(); it++) {
      Op &op = *it;
      switch (op.op) {
        case INC: tape.inc(op.val); break;
        case MOVE: tape.move(op.val); break;
        case LOOP: while (tape.get() > 0) _run(op.loop, tape); break;
        case PRINT: printf("%c", tape.get()); fflush(stdout); break;
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

int main(int argc, char** argv){
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
