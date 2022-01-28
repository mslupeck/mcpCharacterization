#ifndef HELPERS_H_
#define HELPERS_H_

// Used to add colon as delimiter to stream using imbue()
struct comma_is_space : std::ctype<char> {
  comma_is_space() : std::ctype<char>(get_table()) {}
  static mask const* get_table()
  {
    static mask rc[table_size];
    rc[','] = std::ctype_base::space;
    rc['\n'] = std::ctype_base::space;
    return &rc[0];
  }
};

#endif /* HELPERS_H_ */
