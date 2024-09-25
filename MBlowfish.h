// MBlowfish.h -- Blowfish encryption and decryption tool
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdlib>
#include <cstdint>

struct MBlowfishImpl;

class MBlowfish
{
public:
    MBlowfish();
    MBlowfish(const char *passwd);
    MBlowfish(const char *passwd, size_t len);
    virtual ~MBlowfish();

    void set_password(const char *passwd);
    void set_password(const char *passwd, size_t len);

    // NOTE: Parameter bytes must be a multiple of 8.
    bool encrypt(void *ptr, uint32_t bytes);
    bool decrypt(void *ptr, uint32_t bytes);

    // return value needs delete[]
    uint8_t *encrypt_with_length(const void *ptr, uint32_t& length);
    uint8_t *decrypt_with_length(const void *ptr, uint32_t& length);

    void reset();

protected:
    MBlowfishImpl *m_pimpl;
};

////////////////////////////////////////////////////////////////////////////
