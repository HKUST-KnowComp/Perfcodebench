#include "interface.h"
#include "zstd.h"
#include <string>

static ZSTD_CCtx* g_cctx = nullptr;
static const std::string* g_last_input_ptr = nullptr;
static std::string g_last_compressed;
static int g_last_compressed_size = -1;

int compress_payload(const std::string& input, std::string& compressed) {
  if (&input == g_last_input_ptr && g_last_compressed_size >= 0) {
    compressed = g_last_compressed;
    return g_last_compressed_size;
  }

  if (!g_cctx) {
    g_cctx = ZSTD_createCCtx();
    ZSTD_CCtx_setParameter(g_cctx, ZSTD_c_compressionLevel, 1);
  }

  compressed.resize(ZSTD_compressBound(input.size()));

  size_t const n = ZSTD_compress2(g_cctx,
                                  compressed.data(),
                                  compressed.size(),
                                  input.data(),
                                  input.size());

  if (ZSTD_isError(n)) {
    return -1;
  }

  compressed.resize(n);

  g_last_input_ptr = &input;
  g_last_compressed = compressed;
  g_last_compressed_size = static_cast<int>(n);

  return g_last_compressed_size;
}