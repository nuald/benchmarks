#include <iostream>
#include "simdjson/jsonparser.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <functional>

using namespace simdjson;

int main(int argc, char *argv[]) {
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
      string msg("C++ simdjson");
      send(*sock.get(), msg.c_str(), msg.size(), 0);
    }
  }

  padded_string p = get_corpus("1.json"); 
  ParsedJson pj;
  int res = simdjson::SUCCESS;
  if (pj.allocate_capacity(p.size())) { // allocate memory for parsing up to p.size() bytes
    res = json_parse(p, pj); // do the parsing, return 0 on success
  }
  if (res != simdjson::SUCCESS) {
    std::cout << pj.get_error_message() << std::endl;
  }

  ParsedJson::Iterator pjh(pj);
  if (!pjh.is_ok()) {
    std::cerr << " Could not iterate parsed result. " << std::endl;
    return EXIT_FAILURE;
  }

  double x = 0, y = 0, z = 0;
  int len = 0;

  if (pjh.is_object()) {
    if (pjh.move_to_key("coordinates")) {
      if (pjh.is_array()) {
        if (pjh.down()) {
          do { // moving through array
            
            if (pjh.is_object()) {
              len++;
              if (pjh.down()) {
                do { // moving through hash {x:, y:, z:}
                  if (pjh.get_string_length() == 1) {
                      char c = pjh.get_string()[0];
                      pjh.next();

                      switch(c) {
                        case 'x':
                          x += pjh.get_double();
                          break;

                        case 'y':
                          y += pjh.get_double();
                          break;

                        case 'z':
                          z += pjh.get_double();
                          break;
                      }
                  } else {
                    pjh.next();
                  }

                } while(pjh.next());  // moving through hash {x:, y:, z:}
                pjh.up();
              }
            }
          } while(pjh.next()); // moving through array
        }
      }
    }
  }

  std::cout << x / len << std::endl;
  std::cout << y / len << std::endl;
  std::cout << z / len << std::endl;

  return EXIT_SUCCESS;
}
