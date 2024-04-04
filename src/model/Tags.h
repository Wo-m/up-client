#pragma once

#include <string>
enum Tag {
    EXPECTED,
    ANNA,
    OPAL,
    FLIGHTS,
    NONE,
};

inline std::string to_string(Tag tag) {
    switch (tag) {
        case EXPECTED:
            return "EXPECTED";
        case ANNA:
            return "ANNA";
        case OPAL:
            return "OPAL";
        case FLIGHTS:
            return "FLIGHTS";
        case NONE:
            return "NONE";
    }
}

inline Tag tag_from_string(std::string name) {
    if (name == "EXPECTED") return EXPECTED;
    if (name == "ANNA") return ANNA;
    if (name == "OPAL") return OPAL;
    if (name == "FLIGHTS") return FLIGHTS;

    return NONE;
}
