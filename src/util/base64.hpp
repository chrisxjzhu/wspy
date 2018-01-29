#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

namespace util {

inline char* base64_encode(const char* buf, std::size_t len) noexcept
{
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new(BIO_s_mem());
    BIO_push(b64, mem);

    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, buf, len);
    (void) BIO_flush(b64);

    BUF_MEM* bufmem;
    BIO_get_mem_ptr(b64, &bufmem);
    (void) BIO_set_close(b64, BIO_NOCLOSE);
    BIO_free_all(b64);

    return bufmem->data;
}

inline std::string base64_encode(const std::string& text) noexcept
{
    char* buf = base64_encode(text.c_str(), text.length());
    std::string data(buf);
    std::free(buf);
    return data;
}

}
