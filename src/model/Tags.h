#include <exception>
#include <string>
enum Tag {
    EXPECTED,
    ANNA,
    OPAL,
};

inline Tag tag_from_string(std::string name) {
    if (name == "EXPECTED") return EXPECTED;
    if (name == "ANNA") return ANNA;
    if (name == "OPAL") return OPAL;

    throw std::exception();
}
