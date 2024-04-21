#include <iostream>
#include <string>
#include <sstream>
#include <bitset>
using namespace std;

string stringToHex(string input) {
    stringstream ss;

    // Thiết lập chế độ hexa cho stringstream
    ss << hex;

    for (int i = 0; i<input.length();i++) {
        // Chuyển đổi mỗi ký tự thành một giá trị hexa và thêm vào stringstream
        int hexValue = input[i];
        char hexChars[3];
        sprintf(hexChars, "%02X", hexValue);
        ss << hexChars;
    }

    // Trả về chuỗi hexa từ stringstream
    return ss.str();
}

unsigned int reverse(unsigned int a){
    unsigned int tmp[4] = {a, a, a, a};
    tmp[0] = tmp[0] >> 24;
    tmp[1] = tmp[1] << 8 >> 24 << 8;
    tmp[2] = tmp[2] >> 8 << 24 >> 8;
    tmp[3] = tmp[3] << 24;

    unsigned int res = tmp[0] | tmp[1] | tmp[2] | tmp[3];

    return res;
}

string countBits(string a){
    unsigned long long int length = a.length() * 8;
    string res;
    for (int i = 0; i < sizeof(unsigned long long int); ++i) {
        char c = static_cast<char>((length >> (i * 8)) & 0xFF); // Extract each byte
        res += c;
    }
    
    return res;
}


string addPadding(string a){
    string res = a;
    res += char(128);

    while(res.length() < 56)
        res += char(0);

    res = res;
    string numOfBits = countBits(a);
    res += numOfBits;
    // for (int i = 0; i < 64; i++){
    //     cout << res[i] << endl;
    // }
    // cout << endl;

    return res;
}

void round1(unsigned int& A, unsigned int B, unsigned int C, unsigned int D, unsigned int x, unsigned int s){
    A += (((B & C) | ((~B) & D)) + x);
    A = (A << s) | (A >> (32 - s));
}

void round2(unsigned int& A, unsigned int B, unsigned int C, unsigned int D, unsigned int x, unsigned int s){
    A += (((B & C) | (B & D) | (C & D)) + x + 0x5a827999);
    A = (A << s) | (A >> (32 - s));
}

void round3(unsigned int& A, unsigned int& B, unsigned int& C, unsigned int& D, unsigned int x, unsigned int s){
    A += ((B ^ C ^ D) + x + 0x6ed9eba1);
    A = (A << s) | (A >> (32 - s));
}

void rotateABCD(int* a){
    int tmp = a[3];
    for (int i = 3; i > 0; i--)
        a[i] = a[i - 1];
    a[0] = tmp;
}

unsigned int* MD4(string a){
    a = addPadding(a);
    string tx = "";
    unsigned int* X = new unsigned int[16]{0};
    unsigned int* H = new unsigned int[4]{0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
    unsigned int* ABCD = new unsigned int[4]{H[0], H[1], H[2], H[3]};

    for(int i = 0; i < 64; i += 4){
        string sub = a.substr(i, 4);
        for (int j = 0; j < 4; j++){
            X[i/4] = (X[i/4] << 8) + abs(int(sub[j]));
        }
        X[i/4] = reverse(X[i/4]);
    }

    //Round 1
    int* idx = new int[4]{0, 1, 2, 3}; //Arrange ABCD in order of loop
    int* s = new int[4]{3, 7, 11 , 19};
    for (int i = 0; i < 16; i++){
        round1(ABCD[idx[0]], ABCD[idx[1]], ABCD[idx[2]], ABCD[idx[3]], X[i], s[i % 4]);
        rotateABCD(idx);
    }

    //Round 2
    s[0] = 3; s[1] = 5; s[2] = 9; s[3] = 13;
    for (int i = 0; i < 16; i++){
        round2(ABCD[idx[0]], ABCD[idx[1]], ABCD[idx[2]], ABCD[idx[3]], X[int(i / 4) + ((i % 4) * 4)], s[i % 4]);
        rotateABCD(idx);
    }
    
    //Round 3
    int* idxX = new int[16]{0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
    s[0] = 3; s[1] = 9; s[2] = 11; s[3] = 15;
    for (int i = 0; i < 16; i++){
        round3(ABCD[idx[0]], ABCD[idx[1]], ABCD[idx[2]], ABCD[idx[3]], X[idxX[i]], s[i % 4]);
        rotateABCD(idx);
    }

    //H[i] += ABCD[i]
    for (int i = 0; i < 4; i++){
        H[i] += ABCD[i];
        H[i] = reverse(H[i]);
    }

    delete[] X;
    delete[] ABCD;
    delete[] idx;
    delete[] s;
    delete[] idxX;
    return H;
}

int main(){
    string a = "hello";
    unsigned int* res = MD4(a); 
    for (int i = 0; i < 4; i++){
        cout << hex << res[i];
    }
    return 1;
}