#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <functional>

using namespace std;

void read_file(string filename, stringstream &buffer){
  std::ifstream file(filename.c_str());
  if ( file )
  {
    buffer << file.rdbuf();
    file.close();
  }
}

int main()
{
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
      string msg("C++ Boost");
      send(*sock.get(), msg.c_str(), msg.size(), 0);
    }
  }

  stringstream text;
  read_file("./1.json", text);

  boost::property_tree::ptree jobj;
  boost::property_tree::read_json(text, jobj);
  double x = 0, y = 0, z = 0;
  int len = 0;

  BOOST_FOREACH(boost::property_tree::ptree::value_type &coord, jobj.get_child("coordinates"))
  {
    len += 1;
    x += coord.second.get<double>("x");
    y += coord.second.get<double>("y");
    z += coord.second.get<double>("z");
  }

  printf("%.8f\n", x / len);
  printf("%.8f\n", y / len);
  printf("%.8f\n", z / len);
  return 0;
}
