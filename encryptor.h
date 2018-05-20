#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <QString>

#include <cryptopp/sha.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>

class Encryptor
{
public:
    Encryptor(const QString &passwd);
    std::string encrypt(const std::string &input) const;
    std::string decrypt(const std::string &input) const;
private:
    uint8_t iv_[CryptoPP::AES::DEFAULT_BLOCKSIZE];
    uint8_t key_[CryptoPP::AES::DEFAULT_KEYLENGTH];
};

#endif // ENCRYPTOR_H
