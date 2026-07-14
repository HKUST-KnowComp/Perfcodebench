#pragma once

#include <cstdint>
#include <string>
#include <vector>

uint64_t regex_log_match_checksum(const std::vector<std::string>& records, int iters);
