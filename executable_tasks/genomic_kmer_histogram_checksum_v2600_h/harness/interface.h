#pragma once

#include <cstdint>
#include <string>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters);
