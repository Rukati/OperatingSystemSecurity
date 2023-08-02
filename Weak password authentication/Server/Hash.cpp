#include "Header.h"

#include <iostream>
#include <string>
#include <boost/uuid/detail/sha1.hpp>

std::string sha256(const std::string& input, const std::string& salt) {
	std::string salted_input = input + salt;
	boost::uuids::detail::sha1 sha;
	sha.process_bytes(salted_input.data(), salted_input.size());
	unsigned int digest[5];
	sha.get_digest(digest);
	char output[41];
	snprintf(output, sizeof(output),
		"%08x%08x%08x%08x%08x", digest[0], digest[1], digest[2], digest[3], digest[4]);
	return output;
}

std::string Hash(std::string ClientPass, std::string ClientSalt) {
	std::string input = ClientPass;
	std::string salt = ClientSalt;
	std::string hashed = sha256(input, salt);
	return hashed;
}