#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>

// #define the macros below to 1/0 to enable/disable the mode of operation.
// 
// CBC enables AES encryption in CBC-mode of operation.
// CTR enables encryption in counter-mode.
// ECB enables the basic ECB 16-byte block algorithm. All can be enabled simultaneously.
// Enabling everything

#define AES128 1
// #define AES192 1
// #define AES256 1

#define AES_BLOCKLEN 16 // Block length in bytes - AES is 128b block only

#if defined(AES256) && (AES256 == 1)
#define AES_KEYLEN 32
#define AES_keyExpSize 240
#elif defined(AES192) && (AES192 == 1)
#define AES_KEYLEN 24
#define AES_keyExpSize 208
#else
#define AES_KEYLEN 16   // Key length in bytes
#define AES_keyExpSize 176
#endif

struct AES_ctx {
  uint8_t RoundKey[AES_keyExpSize];
  uint8_t Iv[AES_BLOCKLEN];
};

void AES_init_ctx(struct AES_ctx* ctx, const uint8_t* key);
void AES_init_ctx_iv(struct AES_ctx* ctx, const uint8_t* key, const uint8_t* iv);
void AES_ctx_set_iv(struct AES_ctx* ctx, const uint8_t* iv);

// buffer size is exactly AES_BLOCKLEN bytes;
// you need only AES_init_ctx as IV is not used in ECB
// NB: ECB is considered insecure for most uses
void AES_ECB_encrypt(const struct AES_ctx* ctx, uint8_t* buf);
void AES_ECB_decrypt(const struct AES_ctx* ctx, uint8_t* buf);

// buffer size MUST be mutiple of AES_BLOCKLEN;
// Suggest https:// en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme
// NOTES: you need to set IV in ctx via AES_init_ctx_iv() or AES_ctx_set_iv()
// no IV should ever be reused with the same key
void AES_CBC_encrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length);
void AES_CBC_decrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length);


// Same function for encrypting as for decrypting.
// IV is incremented for every block, and used after xion as XOR-compliment for output
// Suggesting https:// en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme
// NOTES: you need to set IV in ctx with AES_init_ctx_iv() or AES_ctx_set_iv()
// no IV should ever be reused with the same key
void AES_CTR_xcrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length);


   
   
   
  



int get_SBox_Value(int num);

int get_SBox_Inverse(int num);

//void Expand_Keys();

void Add_Round_Key(int round, unsigned char state[4][4]);

void Sub_Bytes(unsigned char state[4][4]);

void Inv_Sub_Bytes(unsigned char state[4][4]);

void Shift_Rows(unsigned char state[4][4]);

void Inv_Shift_Rows(unsigned char state[4][4]);

void Mix_Columns(unsigned char state[4][4]);

void Inv_Mix_Columns(unsigned char state[4][4]);

void Encrypt(unsigned char plaintext[16], unsigned char Key[32], unsigned char encrypted[16]);

void Decrypt(unsigned char encrypted[16], unsigned char Key[32], unsigned char plaintext[16]);

#endif /* AES_H_ */
