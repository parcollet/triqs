#pragma once
#include <string>
#include "./group.hpp"
#include "./scheme.hpp"

namespace h5 {

  template <> struct io<std::string> {
    static std::string scheme() { return "string"; }
    static void write(group g, std::string const &name, std::string const &value);
    static void read(group g, std::string const &name, std::string &value);
    static void write_attribute(hid_t id, std::string const &name, std::string const &value);
    static void read_attribute(hid_t id, std::string const &name, std::string &s);
  };

  template <> struct io<const char *> {
    using s_t = io<std::string>;
    static std::string scheme() { return "string"; }
    static void write(group g, std::string const &name, const char *s) { s_t::write(g, name, std::string{s}); }
    static void read(group g, std::string const &name, char *s) = delete;
    static void write_attribute(hid_t id, std::string const &name, const char *s) { s_t::write_attribute(id, name, std::string{s}); }
    static void read_attribute(hid_t id, std::string const &name, char *s) = delete;
  };

} // namespace h5
