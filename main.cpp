#include <iomanip>
#include <iostream>
#include <string>
#include "crypto_helpers.h"

using namespace std;

int main() {

    string input, private_key;

    cout << "Input your private key (128 bit hex):" << endl;

    getline(cin, private_key);

    if (private_key.size() != 32) {
        cout << "Your private key must be 128-bit hex." << endl;
        return 1;
    }

    while (true) {
        cout << "Would you like to encrypt or decrypt? (E or D)" << endl;
        getline(cin, input);

        char mode = toupper(input[0]);

        switch (mode) {
            case 'E':
                cout << "Mode set to encryption, please input your message:" << endl;
                getline(cin, input);
                Encrypt(private_key, input);
                return 0;
            case 'D':
                cout << "Mode set to decryption, please input your cipher:" << endl;
                getline(cin, input);
                Decrypt(private_key, input);
                return 0;
            default:
                cout << "Please enter a valid mode." << endl;
                break;
        }
    }
}