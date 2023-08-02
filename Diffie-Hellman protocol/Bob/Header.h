#pragma once

#include <iostream>
#include <random>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>

class KEY {
private:
	BIGNUM* secret_key = BN_new();
	BIGNUM* public_key = BN_new();

public:

	BIGNUM* Secret(int bits) {
		BN_generate_prime_ex(secret_key, bits, true, NULL, NULL, NULL);
		std::cout << "\nGenerated a secret key >>>> " << BN_bn2dec(secret_key) << std::endl;
		return secret_key;
	}

	BIGNUM* Public(int bits) {
		BN_generate_prime_ex(public_key, bits, true, NULL, NULL, NULL);
		std::cout << "\nGenerated a public key >>>> " << BN_bn2dec(public_key) << std::endl;
		return public_key;
	}

	BIGNUM* Partical(BIGNUM* public_key2) {
		// Вычисление частичного ключа
		BIGNUM* partical_key = BN_new(); 

		BN_mod_exp(partical_key, public_key2, secret_key, public_key, BN_CTX_new());
		std::cout << "\nPartical key >>>> " << BN_bn2dec(partical_key) << std::endl;
		return partical_key;
	}

	BIGNUM* Full(BIGNUM* partical2) {
		// Вычисление полного ключа
		BIGNUM* full_key = BN_new();
		
		BN_mod_exp(full_key, partical2, secret_key, public_key, BN_CTX_new());
		std::cout << "--------------------------------\n";
		std::cout << "part: " << BN_bn2dec(partical2) << " \nsecret: " << BN_bn2dec(secret_key) << "\nPubl: " << BN_bn2dec(public_key);
		std::cout << "\n\nFull key >>>> " << BN_bn2dec(full_key) << std::endl;
		std::cout << "--------------------------------";
		return full_key;
	}
};

class BOB
{
public:

	std::string encrypt_message(const std::string& message, int key) {
		std::string encrypted_message = "";
		for (char c : message) {
			encrypted_message += static_cast<char>(c + key);
		}
		std::cout << "\tEncrypted >>>> " << encrypted_message;
		return encrypted_message;
	}

	std::string decrypt_message(const std::string& encrypted_message, int key) {
		std::string decrypted_message = "";
		for (char c : encrypted_message) {
			decrypted_message += static_cast<char>(c - key);
		}
		std::cout << "\tDecrypted >>>> " << decrypted_message;
		return decrypted_message;
	}
};