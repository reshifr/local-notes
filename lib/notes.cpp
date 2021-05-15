#include <regex>
#include <vector>
#include <fstream>
#include <cstring>
#include <iostream>
#include <filesystem>

struct body {
  public:
    std::string filename;
    std::string key;
    std::string content;
};

#define BUF_LEN 0x10000

struct notes {
  private:
    std::string dir_path;
    std::vector<body> bodies;

    bool is_key(const std::string& line) {
      return line.length()!=0 && line[0]=='[' && line[line.length()-1]==']';
    }

    bool is_content(const std::string& line) {
      return line.length()!=0 && !is_key(line);
    }

    std::string getline(char* buf, std::ifstream& stream) {
      stream.getline(buf, BUF_LEN);
      buf[strcspn(buf, "\r\n")] = '\0';
      return std::string(buf);
    }

    void index(void) {
      for(const auto& node: std::filesystem::directory_iterator(dir_path)) {
        if( !node.is_regular_file() )
          continue;
        char buf[BUF_LEN];
        std::ifstream stream(node.path(), std::ios::in);
        while( !stream.eof() ) {
          std::string line = getline(buf, stream);
          if( !is_key(line) )
            continue;
          body cur_body;
          cur_body.filename = node.path().filename();
          cur_body.key = std::string(line);
          for(;;) {
            line = getline(buf, stream);
            if( !is_content(line) )
              break;
            cur_body.content += line+"\n";
          }
          bodies.push_back(cur_body);
        }
      }
    }

    std::vector<std::size_t> index_search(const std::string keyword) {
      std::regex r(keyword, std::regex_constants::icase);
      std::vector<std::size_t> il;
      for(std::size_t i=0; i<bodies.size(); ++i)
        if( std::regex_search(bodies[i].filename, r) ||
            std::regex_search(bodies[i].key, r) ||
            std::regex_search(bodies[i].content, r) )
          il.push_back(i);
      return il;
    }

  public:
    notes(const std::string dir_path) {
      notes::dir_path = dir_path;
      index();
    }

/**
 * X
 * RESET: 0
 * BOLD/BRIGHT: 1
 * UNDERLINE: 4
 * INVERSE: 7
 * BOLD/BRIGHT OFF: 21
 * UNDERLINE OFF: 24
 * INVERSE OFF: 27
 *
 * Y;Z
 * RED: 31;41
 * GREEN: 32;42
 * YELLOW: 33;43
 * BLUE: 34;44
 *
 * \033[X;Y;Zm<Hello>\033[Zm
 */

#define RED_BOLD "\033[1;31m"
#define GREEN_BOLD "\033[1;32m"
#define YELLOW_BOLD "\033[1;33m"
#define BLUE_BOLD "\033[1;34m"
#define RESET "\033[0m"

    void search(const std::string keyword) {
      std::vector<std::size_t> il = index_search(keyword);
      for(const auto& i: il) {
        std::cout<<RED_BOLD<<bodies[i].key<<RESET<<"\n";
        std::cout<<YELLOW_BOLD<<bodies[i].content<<RESET<<
          (i==il.back() ? "" : "\n");
      }
    }
};

#define NOTES_PATH "/mnt/Assign/Lib/notes"
#define MESSAGE "notes KEYWORD"

int main(int argc, char** argv) {
  if( argc==1 || argc>2 ) {
    std::cout<<MESSAGE<<"\n";
    return EXIT_FAILURE;
  }
  notes index(NOTES_PATH);
  index.search(argv[1]);
  return EXIT_SUCCESS;
}
