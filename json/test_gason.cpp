#include "gason.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <functional>

using namespace std;

void read_file(string filename, stringstream &buffer){
  ifstream f(filename.c_str());
  if (f)
  {
    buffer << f.rdbuf();
    f.close();
  }
}

int main() {
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
        string msg("C++ gason");
        send(*sock.get(), msg.c_str(), msg.size(), 0);
      }
    }

    stringstream ss;
    read_file("./1.json", ss);

    string text = ss.str();
    char *endptr;
    JsonValue jobj;
    JsonAllocator allocator;
    int status = jsonParse((char *)text.c_str(), &endptr, &jobj, allocator);
    if (status != JSON_OK) return 1;

    JsonValue coordinates;
    for (auto data : jobj) {
      if (strcmp(data->key, "coordinates") == 0) { coordinates = data->value; }
    }
    double x = 0, y = 0, z = 0;
    int len = 0;

    for (auto coord : coordinates) {
      len++;
      for (auto c : coord->value) {
        char *key = c->key;
        if (strcmp(key, "x") == 0) { x += c->value.toNumber(); } else
        if (strcmp(key, "y") == 0) { y += c->value.toNumber(); } else
        if (strcmp(key, "z") == 0) { z += c->value.toNumber(); }
      }
    }

    cout << x / len << endl;
    cout << y / len << endl;
    cout << z / len << endl;

    return 0;
}
