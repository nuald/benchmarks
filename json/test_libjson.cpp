#include <json-c/json.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
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
      string msg("C++ json-c");
      send(*sock.get(), msg.c_str(), msg.size(), 0);
    }
  }

  stringstream ss;
  read_file("./1.json", ss);
  string text = ss.str();
  json_object* jobj = json_tokener_parse(text.c_str());

  json_object* coordinates;
  if (json_object_object_get_ex(jobj, "coordinates", &coordinates)) {
    int len = json_object_array_length(coordinates);
    double x = 0, y = 0, z = 0;

    for (int i = 0; i < len; i++)  {
      json_object* coord = json_object_array_get_idx(coordinates, i);
      json_object *xobj, *yobj, *zobj;
      if (json_object_object_get_ex(coord, "x", &xobj) &&
	  json_object_object_get_ex(coord, "y", &yobj) &&
	  json_object_object_get_ex(coord, "z", &zobj)) {
	x += json_object_get_double(xobj);
	y += json_object_get_double(yobj);
	z += json_object_get_double(zobj);
      }
    }

    printf("%.8f\n", x / len);
    printf("%.8f\n", y / len);
    printf("%.8f\n", z / len);
  }
}
