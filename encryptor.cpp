#include "encryptor.h"

Encryptor::Encryptor(const QString &passwd)
{
    std::string str = passwd.toStdString();
    CryptoPP::SHA256 hash;
    uint8_t digest[CryptoPP::SHA256::DIGESTSIZE];
    hash.Update((const CryptoPP::byte *)str.c_str(), passwd.size());
    hash.Final(digest);
    memcpy(iv_, digest, CryptoPP::AES::DEFAULT_BLOCKSIZE);
    memcpy(key_, digest + 16, CryptoPP::AES::DEFAULT_KEYLENGTH);
}

std::string Encryptor::encrypt(const std::string &input) const
{
    CryptoPP::SecByteBlock key(key_, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::SecByteBlock iv(iv_, CryptoPP::AES::DEFAULT_BLOCKSIZE);
    std::unique_ptr<CryptoPP::byte[]> cipher = std::make_unique<CryptoPP::byte[]>(input.size());

    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);
    cfbEncryption.ProcessData(cipher.get(), (const CryptoPP::byte *)input.c_str(), input.size());
    return std::string((char *)cipher.get(), input.size());
}

std::string Encryptor::decrypt(const std::string &input) const
{
    CryptoPP::SecByteBlock key(key_, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::SecByteBlock iv(iv_, CryptoPP::AES::DEFAULT_BLOCKSIZE);
    std::unique_ptr<CryptoPP::byte[]> plain = std::make_unique<CryptoPP::byte[]>(input.size());

    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfbDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);
    cfbDecryption.ProcessData(plain.get(), (const CryptoPP::byte *)input.c_str(), input.size());
    return std::string((char *)plain.get(), input.size());
}
