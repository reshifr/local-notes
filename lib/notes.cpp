#include <regex>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

class body {
  public:
    std::string file_name;
    std::string key;
    std::string content;

    body() : file_name(), key(), content() {}
};

class notes {
  private:
    std::string m_dir_path;
    std::vector<body> m_bodies;

    bool is_key(const std::string& line)
      { return std::regex_match(line, std::regex(R"(^\[.*\]$)")); }
    bool is_content(const std::string& line)
      { return line.length()!=0 && !is_key(line); }

    void indexing(void) {
      for(const auto& node : std::filesystem::directory_iterator(m_dir_path)) {
        if( !node.is_regular_file() )
          continue;
        std::ifstream stream(node.path());
        while( !stream.eof() ) {
          std::string line;
          std::getline(stream, line);
          if( !is_key(line) )
            continue;
          body cur_body;
          cur_body.file_name = node.path().filename().generic_string();
          cur_body.key = line;
          for(;;) {
            line.clear();
            std::getline(stream, line);
            if( !is_content(line) )
              break;
            cur_body.content += line+"\n";
          }
          m_bodies.push_back(cur_body);
        }
      }
    }

    std::vector<std::size_t> find(const std::string& keyword) {
      std::size_t i = 0;
      std::regex regex(keyword);
      std::vector<std::size_t> il;
      for(const auto& body : m_bodies) {
        if( std::regex_search(body.file_name, regex) ||
            std::regex_search(body.key, regex) ||
            std::regex_search(body.content, regex) )
          il.push_back(i);
        ++i;
      }
      return il;
    }

  public:
    notes(const std::string& dir_path) : m_dir_path(dir_path) { indexing(); }

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

    void search(const std::string& keyword) {
      std::vector<std::size_t> il = find(keyword);
      for(const auto& i : il) {
        std::cout<<RED_BOLD<<m_bodies[i].key<<RESET<<"\n";
        std::cout<<YELLOW_BOLD<<m_bodies[i].content<<RESET<<
          (i==il.back() ? "" : "\n");
      }
    }
};

#if defined(_WIN32) || defined(_WIN64)
# define NOTES_PATH R"(D:\Lib\notes)"
#else
# define NOTES_PATH "/mnt/Assign/Lib/notes"
#endif
#define MESSAGE "notes KEYWORD"

int main(int argc, char** argv) {
  if( argc==1 ) {
    std::cout<<MESSAGE<<"\n";
    return EXIT_FAILURE;
  }
  notes index(NOTES_PATH);
  std::string keyword;
  for(std::size_t i=1; i<argc; ++i) {
    keyword += argv[i];
    keyword += i==argc-1 ? "" : " ";
  }
  index.search(keyword);
  return EXIT_SUCCESS;
}
