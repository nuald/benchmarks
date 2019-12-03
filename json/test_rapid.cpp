#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <functional>

using namespace std;
using namespace rapidjson;

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
        string msg("C++ RapidJSON");
        send(*sock.get(), msg.c_str(), msg.size(), 0);
      }
    }

    FILE* fp = std::fopen("./1.json", "r");
    char buffer[65536];
    FileReadStream frs(fp, buffer, sizeof(buffer));
    Document jobj; 
    jobj.ParseStream(frs);

    const Value &coordinates = jobj["coordinates"];
    SizeType len = coordinates.Size();
    double x = 0, y = 0, z = 0;

    for (SizeType i = 0; i < len; i++) {
      const Value &coord = coordinates[i];
      x += coord["x"].GetDouble();
      y += coord["y"].GetDouble();
      z += coord["z"].GetDouble();
    }

    std::cout << x / len << std::endl;
    std::cout << y / len << std::endl;
    std::cout << z / len << std::endl;

    fclose(fp);

    return 0;
}
