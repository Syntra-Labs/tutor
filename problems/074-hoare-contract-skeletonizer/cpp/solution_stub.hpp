#pragma once

#include <string>
#include <vector>

struct HoareContractSpec {
    std::string fn_name{};
    std::vector<std::string> requires_clauses{};
    std::vector<std::string> ensures_clauses{};
};

inline std::vector<HoareContractSpec> extract_hoare_contracts(const std::string& source_code) {
    // TODO: parse // @requires: ... and // @ensures: ... annotations above function signatures
    (void)source_code;
    return {};
}

inline std::string skeletonize_cpp_source(const std::string& source_code) {
    // TODO: strip function bodies { ... } into { /* TODO */ } while preserving contracts & signatures
    (void)source_code;
    return "";
}
