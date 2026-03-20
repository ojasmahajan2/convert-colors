#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <conio.h>

using std::string;

void convertHSV(string input) {
    int H, S, V;
    int R, G, B;
    char hex_r1, hex_r2, hex_g1, hex_g2, hex_b1, hex_b2;
    char sep1, sep2;
    int length = input.size();

    std::stringstream ss(input.substr(4, length - 5));
    if (ss >> H >> sep1 >> S >> sep2 >> V) {
                float r_f, g_f, b_f;
        float s_f = S / 100.0;
        float v_f = V / 100.0;

        if (S == 0) {
            r_f = g_f = b_f = v_f;
        } else {
            float h_f = H / 60.0;
            int i = (int)h_f;
            float f = h_f - i;
            float p = v_f * (1.0 - s_f);
            float q = v_f * (1.0 - (s_f * f));
            float t = v_f * (1.0 - (s_f * (1.0 - f)));

            switch (i % 6) {
                case 0: r_f = v_f; g_f = t; b_f = p; break;
                case 1: r_f = q; g_f = v_f; b_f = p; break;
                case 2: r_f = p; g_f = v_f; b_f = t; break;
                case 3: r_f = p; g_f = q; b_f = v_f; break;
                case 4: r_f = t; g_f = p; b_f = v_f; break;
                case 5: r_f = v_f; g_f = p; b_f = q; break;
            }
        }

        R = (int)(r_f * 255 + 0.5);
        G = (int)(g_f * 255 + 0.5);
        B = (int)(b_f * 255 + 0.5);

        std::cout << "rgb(" << R << ", " << G << ", " << B << ")";

        char hexChars[] = "0123456789ABCDEF";

        hex_r1 = hexChars[R / 16];
        hex_r2 = hexChars[R % 16];

        hex_g1 = hexChars[G / 16];
        hex_g2 = hexChars[G % 16];

        hex_b1 = hexChars[B / 16];
        hex_b2 = hexChars[B % 16];

        std::cout << "\n" << "#" << hex_r1 << hex_r2 << hex_g1 << hex_g2 << hex_b1 << hex_b2;
    }
}

void convertRGB(string input) {
    char hex_r1, hex_r2, hex_g1, hex_g2, hex_b1, hex_b2;
    char sep1, sep2;
    int R, G, B;
    int H, S, V;
    int min, max;
    int length = input.size();
    
    std::stringstream ss(input.substr(4, length - 5));
    if (ss >> R >> sep1 >> G >> sep2 >> B) {
        char hexChars[] = "0123456789ABCDEF";
        
        hex_r1 = hexChars[R / 16];
        hex_r2 = hexChars[R % 16];
        
        hex_g1 = hexChars[G / 16];
        hex_g2 = hexChars[G % 16];

        hex_b1 = hexChars[B / 16];
        hex_b2 = hexChars[B % 16];

        std::cout << "#" << hex_r1 << hex_r2 << hex_g1 << hex_g2 << hex_b1 << hex_b2;

        max = R;
        if (G > max) max = G;
        if (B > max) max = B;
        min = R;
        if (G < min) min = G;
        if (B < min) min = B;

        V = (max / 255.0) * 100;

        if (max == 0) {
            S = 0;
        } else {
            S = ((float)(max - min) / max) * 100;
        }

        if (max == min) {
            H = 0; 
        } 
        else {
            float delta = max - min;
            if (max == R) {
                H = 60 * (G - B) / delta;
            } 
            else if (max == G) {
                H = 60 * ((B - R) / delta + 2);
            } 
            else if (max == B) {
                H = 60 * ((R - G) / delta + 4);
            }
            if (H < 0) H += 360;
        }
        std::cout << "\n" << "hsv(" << (int)H << ", " << (int)S << ", " << (int)V << ")";
    }
}

void convertHex(string input) {
    int R, G, B;
    int H, S, V;
    int min, max;

    int start;

    if ((input.size() == 7) && (input[0] == '#')) {
        R = std::stoi(input.substr(1,2), nullptr, 16);
        G = std::stoi(input.substr(3,2), nullptr, 16);
        B = std::stoi(input.substr(5,2), nullptr, 16);
    }
    else {
        R = std::stoi(input.substr(0,2), nullptr, 16);
        G = std::stoi(input.substr(2,2), nullptr, 16);
        B = std::stoi(input.substr(4,2), nullptr, 16);
    }

    std::cout << "rgb(" << R << ", " << G << ", " << B << ")";

    max = R;
    if (G > max) max = G;
    if (B > max) max = B;
    min = R;
    if (G < min) min = G;
    if (B < min) min = B;

    V = (max / 255.0) * 100;

    if (max == 0) {
        S = 0;
    } else {
        S = ((float)(max - min) / max) * 100;
    }

    if (max == min) {
        H = 0; 
    } 
    else {
        float delta = max - min;
        if (max == R) {
            H = 60 * (G - B) / delta;
        } 
        else if (max == G) {
            H = 60 * ((B - R) / delta + 2);
        } 
        else if (max == B) {
            H = 60 * ((R - G) / delta + 4);
        }
        if (H < 0) H += 360;
    }

    std::cout << "\n" << "hsv(" << (int)H << ", " << (int)S << ", " << (int)V << ")";
}

bool isHSV(string input) {
    int H, S, V;
    char sep1, sep2;
    int length = input.size();

    string prefix = "hsv(";
    string suffix = ")";

    if (length < 10) return false;

    if (input.substr(0, 4) != prefix || input.substr(length - 1) != suffix) {
        return false;
    }

    std::stringstream ss(input.substr(4, length - 5));

    if (ss >> H >> sep1 >> S >> sep2 >> V) {
        
        if ((sep1 == ',' || sep1 == ';') && (sep1 == sep2) &&
            (H >= 0 && H <= 360) &&
            (S >= 0 && S <= 100) &&
            (V >= 0 && V <= 100)) {
            return true;
        }
    }

    return false;
}

bool isRGB(string input) {
    int R, G, B;
    char sep1, sep2;
    int length = input.size();

    string prefix = "rgb(";
    string suffix = ")";

    if (length < 10) return false;

    if (input.substr(0, 4) != prefix || input.substr(length - 1) != suffix) {
        return false;
    }

    std::stringstream ss(input.substr(4, length - 5));
    
    if (ss >> R >> sep1 >> G >> sep2 >> B) {

        if ((sep1 == ',' || sep1 == ';') && (sep1 == sep2) &&
            (R >= 0 && R <= 255) &&
            (G >= 0 && G <= 255) &&
            (B >= 0 && B <= 255)) {
            return true;
        }
    }
    
    return false;
}

bool isHex(string input) {
    int start;

    if ((input.size() == 7) && (input[0] == '#')) {
        start = 1;
    }
    else if (input.size() == 6) {
        start = 0;
    }
    else {
        return false;
    }
    
    for (int i = start; i < input.size(); i++) {
        if (!(std::isxdigit(input[i]))) {
            return false;
        }
    }

    return true;
}

void isFormat(string input) {    
    if (isHex(input)) {
        std::cout << "\n";
        convertHex(input);
    }
    else if (isRGB(input)) {
        std::cout << "\n";
        convertRGB(input);
    }
    else if (isHSV(input)) {
        std::cout << "\n";
        convertHSV(input);
    }
}

void clearConsole() {
    #if defined(_WIN32) || defined(_WIN64)
        std::system("cls");
    #else
        std::system("clear");
    #endif
}

int main() {
    string input;
    char ch;

    while (true) {
        clearConsole();
        
        std::cout << "This terminal program lets you convert your color values to Hex, RGB and HSV simultaneously.";

        std::cout << "\n\nTo use this program, just type your values in your desired formats below-";

        std::cout << "\nFor RGB conversion: rgb(<value>, <value>, <value>)";
        std::cout << "\nFor HSV conversion: hsv(<value>, <value>, <value>)";
        std::cout << "\nFor Hex conversion: simply type your 6 digit value with # or without # at the very first";

        std::cout << "\n\nTo close this program hit [ENTER] or to edit values, use [BACKSPACE]\n\n\n";

        std::cout << "Enter the color: " << input;
        
        if (!input.empty()) {
            isFormat(input);
        } 
        
        ch = _getch();

        if (ch == 13) {          // 13 is the ASCII code for Enter
            std::cout << "\nProgram instance terminated!";
            break;
        } 
        else if (ch == 8) {      //  8 is the ASCII code for Backspace
            if (!input.empty()) {
                input.pop_back();
            }
        } 
        else if (std::isprint(ch)) {
            input += ch;
        }
    }
    return 0;
}
