/*
 * ============================================================================
 *  ConvertColorSRC.cpp
 * ============================================================================
 *
 *  Description : A real-time, interactive terminal-based color converter.
 *                Converts color values between three formats:
 *                  - HEX  (e.g. #FF5733 or FF5733)
 *                  - RGB  (e.g. rgb(255, 87, 51))
 *                  - HSV  (e.g. hsv(11, 80, 100))
 *
 *  How it works: The user types a color value character-by-character.
 *                After every keystroke the screen refreshes and, if the
 *                current input forms a valid color in any of the three
 *                formats, the converted equivalents are displayed live.
 *
 *  Controls    : [ENTER]     - Exit the program
 *                [BACKSPACE] - Delete the last character
 *                [Any key]   - Append the character to the input
 *
 * ============================================================================
 */

// ── Standard Library Headers ────────────────────────────────────────────────
#include <iostream>   // For std::cout (console output)
#include <string>     // For std::string
#include <sstream>    // For std::stringstream (parsing numeric values from strings)
#include <cctype>     // For std::isxdigit(), std::isprint()
#include <cstdlib>    // For std::system() (clearing the console)
#include <conio.h>    // For _getch() (reading a single keystroke without echo)

using std::string;

// ════════════════════════════════════════════════════════════════════════════
//  CONVERSION FUNCTIONS
//  Each function takes a validated input string in one color format and
//  prints the equivalent values in the other two formats.
// ════════════════════════════════════════════════════════════════════════════

/*
 * ──────────────────────────────────────────────────────────────────────────
 *  convertHSV()
 * ──────────────────────────────────────────────────────────────────────────
 *  Input  : A valid HSV string, e.g. "hsv(120, 50, 75)"
 *  Output : Prints the equivalent RGB and HEX values to the console.
 *
 *  Algorithm (HSV → RGB):
 *      1. Normalize S and V from [0..100] to [0..1].
 *      2. If S == 0 the color is achromatic (grey); R = G = B = V.
 *      3. Otherwise, determine the hue sector (0–5) by dividing H by 60.
 *      4. Compute intermediate values (p, q, t) and assign R, G, B
 *         according to the sector.
 *      5. Scale the [0..1] results to [0..255] with rounding.
 * ──────────────────────────────────────────────────────────────────────────
 */
void convertHSV(string input) {
    int H, S, V;                                          // Hue (0-360), Saturation (0-100), Value (0-100)
    int R, G, B;                                          // Red, Green, Blue components (0-255)
    char hex_r1, hex_r2, hex_g1, hex_g2, hex_b1, hex_b2; // Individual hex digit chars for each channel
    char sep1, sep2;                                      // Separator characters (',' or ';')
    int length = input.size();

    // Extract the content between "hsv(" and ")" and parse H, S, V
    std::stringstream ss(input.substr(4, length - 5));
    if (ss >> H >> sep1 >> S >> sep2 >> V) {
                float r_f, g_f, b_f;       // Floating-point RGB results in [0..1]
        float s_f = S / 100.0;             // Normalize saturation to [0..1]
        float v_f = V / 100.0;             // Normalize value to [0..1]

        // ── Achromatic case (no saturation → pure grey) ──
        if (S == 0) {
            r_f = g_f = b_f = v_f;
        } else {
            // ── Chromatic case ──
            float h_f = H / 60.0;          // Divide hue into one of 6 sectors (0-5)
            int i = (int)h_f;              // Sector index
            float f = h_f - i;             // Fractional part of the sector
            float p = v_f * (1.0 - s_f);                   // Intermediate value p
            float q = v_f * (1.0 - (s_f * f));             // Intermediate value q
            float t = v_f * (1.0 - (s_f * (1.0 - f)));    // Intermediate value t

            // Assign R, G, B based on which 60° sector the hue falls in
            switch (i % 6) {
                case 0: r_f = v_f; g_f = t; b_f = p; break;   //   0° - 59°  (Red → Yellow)
                case 1: r_f = q; g_f = v_f; b_f = p; break;   //  60° - 119° (Yellow → Green)
                case 2: r_f = p; g_f = v_f; b_f = t; break;   // 120° - 179° (Green → Cyan)
                case 3: r_f = p; g_f = q; b_f = v_f; break;   // 180° - 239° (Cyan → Blue)
                case 4: r_f = t; g_f = p; b_f = v_f; break;   // 240° - 299° (Blue → Magenta)
                case 5: r_f = v_f; g_f = p; b_f = q; break;   // 300° - 359° (Magenta → Red)
            }
        }

        // Scale from [0..1] to [0..255] with rounding (+0.5)
        R = (int)(r_f * 255 + 0.5);
        G = (int)(g_f * 255 + 0.5);
        B = (int)(b_f * 255 + 0.5);

        // Print the RGB equivalent
        std::cout << "rgb(" << R << ", " << G << ", " << B << ")";

        // ── Convert RGB to HEX ──
        char hexChars[] = "0123456789ABCDEF";   // Lookup table for hex digits

        hex_r1 = hexChars[R / 16];     // High nibble of Red
        hex_r2 = hexChars[R % 16];     // Low  nibble of Red

        hex_g1 = hexChars[G / 16];     // High nibble of Green
        hex_g2 = hexChars[G % 16];     // Low  nibble of Green

        hex_b1 = hexChars[B / 16];     // High nibble of Blue
        hex_b2 = hexChars[B % 16];     // Low  nibble of Blue

        // Print the HEX equivalent (e.g. #FF5733)
        std::cout << "\n" << "#" << hex_r1 << hex_r2 << hex_g1 << hex_g2 << hex_b1 << hex_b2;
    }
}

/*
 * ──────────────────────────────────────────────────────────────────────────
 *  convertRGB()
 * ──────────────────────────────────────────────────────────────────────────
 *  Input  : A valid RGB string, e.g. "rgb(255, 87, 51)"
 *  Output : Prints the equivalent HEX and HSV values to the console.
 *
 *  Algorithm (RGB → HEX):
 *      Each channel (0-255) is split into two hex digits using
 *      integer division (/16) and modulus (%16).
 *
 *  Algorithm (RGB → HSV):
 *      1. Find the min and max of R, G, B.
 *      2. V = max / 255  (scaled to 0-100%).
 *      3. S = (max - min) / max  (scaled to 0-100%).
 *      4. H is computed based on which channel equals max,
 *         using the standard 60° sector formula.
 * ──────────────────────────────────────────────────────────────────────────
 */
void convertRGB(string input) {
    char hex_r1, hex_r2, hex_g1, hex_g2, hex_b1, hex_b2; // Individual hex digit chars
    char sep1, sep2;                                      // Separators between R, G, B values
    int R, G, B;                                          // Red, Green, Blue (0-255)
    int H, S, V;                                          // Hue (0-360), Saturation (0-100), Value (0-100)
    int min, max;                                         // Min and max of R, G, B channels
    int length = input.size();
    
    // Extract the content between "rgb(" and ")" and parse R, G, B
    std::stringstream ss(input.substr(4, length - 5));
    if (ss >> R >> sep1 >> G >> sep2 >> B) {

        // ── RGB → HEX Conversion ──
        char hexChars[] = "0123456789ABCDEF";   // Lookup table for hex digits
        
        hex_r1 = hexChars[R / 16];     // High nibble of Red
        hex_r2 = hexChars[R % 16];     // Low  nibble of Red
        
        hex_g1 = hexChars[G / 16];     // High nibble of Green
        hex_g2 = hexChars[G % 16];     // Low  nibble of Green

        hex_b1 = hexChars[B / 16];     // High nibble of Blue
        hex_b2 = hexChars[B % 16];     // Low  nibble of Blue

        // Print the HEX equivalent
        std::cout << "#" << hex_r1 << hex_r2 << hex_g1 << hex_g2 << hex_b1 << hex_b2;

        // ── RGB → HSV Conversion ──

        // Step 1: Find the maximum and minimum channel values
        max = R;
        if (G > max) max = G;
        if (B > max) max = B;
        min = R;
        if (G < min) min = G;
        if (B < min) min = B;

        // Step 2: Calculate Value (brightness) as a percentage
        V = (max / 255.0) * 100;

        // Step 3: Calculate Saturation as a percentage
        if (max == 0) {
            S = 0;              // Black has no saturation
        } else {
            S = ((float)(max - min) / max) * 100;
        }

        // Step 4: Calculate Hue in degrees (0-360)
        if (max == min) {
            H = 0;              // Achromatic (grey) — hue is undefined, default to 0
        } 
        else {
            float delta = max - min;
            if (max == R) {
                H = 60 * (G - B) / delta;              // Red is dominant
            } 
            else if (max == G) {
                H = 60 * ((B - R) / delta + 2);        // Green is dominant
            } 
            else if (max == B) {
                H = 60 * ((R - G) / delta + 4);        // Blue is dominant
            }
            if (H < 0) H += 360;   // Ensure hue is positive (wrap around)
        }

        // Print the HSV equivalent
        std::cout << "\n" << "hsv(" << (int)H << ", " << (int)S << ", " << (int)V << ")";
    }
}

/*
 * ──────────────────────────────────────────────────────────────────────────
 *  convertHex()
 * ──────────────────────────────────────────────────────────────────────────
 *  Input  : A valid HEX string, e.g. "#FF5733" or "FF5733"
 *  Output : Prints the equivalent RGB and HSV values to the console.
 *
 *  Algorithm:
 *      1. Determine whether the '#' prefix is present and adjust
 *         the starting index accordingly.
 *      2. Parse each 2-character hex pair into an integer (0-255)
 *         using std::stoi() with base 16.
 *      3. Convert to HSV using the same min/max algorithm as in
 *         convertRGB().
 * ──────────────────────────────────────────────────────────────────────────
 */
void convertHex(string input) {
    int R, G, B;        // Red, Green, Blue (0-255)
    int H, S, V;        // Hue (0-360), Saturation (0-100), Value (0-100)
    int min, max;        // Min and max of R, G, B channels

    int start;           // Index where hex digits begin (1 if '#' present, 0 otherwise)

    // Parse hex digits — handle both "#RRGGBB" (7 chars) and "RRGGBB" (6 chars)
    if ((input.size() == 7) && (input[0] == '#')) {
        R = std::stoi(input.substr(1,2), nullptr, 16);    // Characters [1..2]
        G = std::stoi(input.substr(3,2), nullptr, 16);    // Characters [3..4]
        B = std::stoi(input.substr(5,2), nullptr, 16);    // Characters [5..6]
    }
    else {
        R = std::stoi(input.substr(0,2), nullptr, 16);    // Characters [0..1]
        G = std::stoi(input.substr(2,2), nullptr, 16);    // Characters [2..3]
        B = std::stoi(input.substr(4,2), nullptr, 16);    // Characters [4..5]
    }

    // Print the RGB equivalent
    std::cout << "rgb(" << R << ", " << G << ", " << B << ")";

    // ── HEX → HSV Conversion (via RGB) ──

    // Step 1: Find the maximum and minimum channel values
    max = R;
    if (G > max) max = G;
    if (B > max) max = B;
    min = R;
    if (G < min) min = G;
    if (B < min) min = B;

    // Step 2: Calculate Value (brightness) as a percentage
    V = (max / 255.0) * 100;

    // Step 3: Calculate Saturation as a percentage
    if (max == 0) {
        S = 0;              // Black has no saturation
    } else {
        S = ((float)(max - min) / max) * 100;
    }

    // Step 4: Calculate Hue in degrees (0-360)
    if (max == min) {
        H = 0;              // Achromatic (grey) — hue is undefined, default to 0
    } 
    else {
        float delta = max - min;
        if (max == R) {
            H = 60 * (G - B) / delta;              // Red is dominant
        } 
        else if (max == G) {
            H = 60 * ((B - R) / delta + 2);        // Green is dominant
        } 
        else if (max == B) {
            H = 60 * ((R - G) / delta + 4);        // Blue is dominant
        }
        if (H < 0) H += 360;   // Ensure hue is positive (wrap around)
    }

    // Print the HSV equivalent
    std::cout << "\n" << "hsv(" << (int)H << ", " << (int)S << ", " << (int)V << ")";
}

// ════════════════════════════════════════════════════════════════════════════
//  VALIDATION FUNCTIONS
//  Each function checks whether the input string is a valid representation
//  of a color in the respective format. Returns true if valid, false if not.
// ════════════════════════════════════════════════════════════════════════════

/*
 * ──────────────────────────────────────────────────────────────────────────
 *  isHSV()
 * ──────────────────────────────────────────────────────────────────────────
 *  Validates that the input matches the format: hsv(H, S, V)
 *
 *  Checks:
 *    - Minimum length of 10 characters (e.g. "hsv(0,0,0)")
 *    - Starts with "hsv(" and ends with ")"
 *    - Contains three integers separated by matching delimiters (',' or ';')
 *    - H is in [0, 360], S is in [0, 100], V is in [0, 100]
 * ──────────────────────────────────────────────────────────────────────────
 */
bool isHSV(string input) {
    int H, S, V;
    char sep1, sep2;
    int length = input.size();

    string prefix = "hsv(";
    string suffix = ")";

    // Minimum valid input: "hsv(0,0,0)" = 10 characters
    if (length < 10) return false;

    // Verify the prefix "hsv(" and suffix ")"
    if (input.substr(0, 4) != prefix || input.substr(length - 1) != suffix) {
        return false;
    }

    // Extract content between parentheses and attempt to parse H, sep, S, sep, V
    std::stringstream ss(input.substr(4, length - 5));

    if (ss >> H >> sep1 >> S >> sep2 >> V) {
        
        // Validate: separators must match (both ',' or both ';')
        // and all values must be within their valid ranges
        if ((sep1 == ',' || sep1 == ';') && (sep1 == sep2) &&
            (H >= 0 && H <= 360) &&
            (S >= 0 && S <= 100) &&
            (V >= 0 && V <= 100)) {
            return true;
        }
    }

    return false;   // Input did not pass all validation checks
}

/*
 * ──────────────────────────────────────────────────────────────────────────
 *  isRGB()
 * ──────────────────────────────────────────────────────────────────────────
 *  Validates that the input matches the format: rgb(R, G, B)
 *
 *  Checks:
 *    - Minimum length of 10 characters (e.g. "rgb(0,0,0)")
 *    - Starts with "rgb(" and ends with ")"
 *    - Contains three integers separated by matching delimiters (',' or ';')
 *    - R, G, B are each in [0, 255]
 * ──────────────────────────────────────────────────────────────────────────
 */
bool isRGB(string input) {
    int R, G, B;
    char sep1, sep2;
    int length = input.size();

    string prefix = "rgb(";
    string suffix = ")";

    // Minimum valid input: "rgb(0,0,0)" = 10 characters
    if (length < 10) return false;

    // Verify the prefix "rgb(" and suffix ")"
    if (input.substr(0, 4) != prefix || input.substr(length - 1) != suffix) {
        return false;
    }

    // Extract content between parentheses and attempt to parse R, sep, G, sep, B
    std::stringstream ss(input.substr(4, length - 5));
    
    if (ss >> R >> sep1 >> G >> sep2 >> B) {

        // Validate: separators must match (both ',' or both ';')
        // and all values must be within the 0-255 range
        if ((sep1 == ',' || sep1 == ';') && (sep1 == sep2) &&
            (R >= 0 && R <= 255) &&
            (G >= 0 && G <= 255) &&
            (B >= 0 && B <= 255)) {
            return true;
        }
    }
    
    return false;   // Input did not pass all validation checks
}

/*
 * ──────────────────────────────────────────────────────────────────────────
 *  isHex()
 * ──────────────────────────────────────────────────────────────────────────
 *  Validates that the input is a valid 6-digit hex color code.
 *  Accepts both "#RRGGBB" (7 chars) and "RRGGBB" (6 chars).
 *
 *  Checks:
 *    - Length is either 7 (with '#') or 6 (without '#')
 *    - All characters after the optional '#' are valid hex digits (0-9, A-F, a-f)
 * ──────────────────────────────────────────────────────────────────────────
 */
bool isHex(string input) {
    int start;      // Index from which to start checking hex digits

    // Determine format: 7 characters with leading '#', or 6 characters without
    if ((input.size() == 7) && (input[0] == '#')) {
        start = 1;      // Skip the '#' prefix
    }
    else if (input.size() == 6) {
        start = 0;      // No prefix to skip
    }
    else {
        return false;   // Invalid length — not a hex color
    }
    
    // Verify every remaining character is a valid hexadecimal digit
    for (int i = start; i < input.size(); i++) {
        if (!(std::isxdigit(input[i]))) {
            return false;   // Found a non-hex character
        }
    }

    return true;    // All characters are valid hex digits
}

// ════════════════════════════════════════════════════════════════════════════
//  FORMAT DISPATCHER
// ════════════════════════════════════════════════════════════════════════════

/*
 * ──────────────────────────────────────────────────────────────────────────
 *  isFormat()
 * ──────────────────────────────────────────────────────────────────────────
 *  Determines the format of the input color string and dispatches
 *  the appropriate conversion function.
 *
 *  Priority order: HEX → RGB → HSV
 *  (If the input doesn't match any format, nothing is printed.)
 * ──────────────────────────────────────────────────────────────────────────
 */
void isFormat(string input) {    
    if (isHex(input)) {
        std::cout << "\n";
        convertHex(input);      // Input is HEX → convert to RGB & HSV
    }
    else if (isRGB(input)) {
        std::cout << "\n";
        convertRGB(input);      // Input is RGB → convert to HEX & HSV
    }
    else if (isHSV(input)) {
        std::cout << "\n";
        convertHSV(input);      // Input is HSV → convert to RGB & HEX
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  UTILITY FUNCTIONS
// ════════════════════════════════════════════════════════════════════════════

/*
 * ──────────────────────────────────────────────────────────────────────────
 *  clearConsole()
 * ──────────────────────────────────────────────────────────────────────────
 *  Clears the terminal screen.
 *  Uses "cls" on Windows and "clear" on Unix/macOS via preprocessor macros.
 * ──────────────────────────────────────────────────────────────────────────
 */
void clearConsole() {
    #if defined(_WIN32) || defined(_WIN64)
        std::system("cls");     // Windows clear-screen command
    #else
        std::system("clear");   // Unix / macOS clear-screen command
    #endif
}

// ════════════════════════════════════════════════════════════════════════════
//  MAIN — Program Entry Point
// ════════════════════════════════════════════════════════════════════════════

/*
 * ──────────────────────────────────────────────────────────────────────────
 *  main()
 * ──────────────────────────────────────────────────────────────────────────
 *  Implements a real-time, character-by-character input loop:
 *
 *    1. Clear the screen and display usage instructions.
 *    2. Show the current input and any live conversion results.
 *    3. Wait for a single keystroke via _getch() (no echo, no buffering).
 *    4. Handle the keystroke:
 *         - ENTER     (ASCII 13) → Exit the program.
 *         - BACKSPACE (ASCII  8) → Remove the last character.
 *         - Printable character   → Append to the input string.
 *    5. Loop back to step 1.
 *
 *  This creates a smooth, real-time conversion experience where results
 *  update instantly as the user types.
 * ──────────────────────────────────────────────────────────────────────────
 */
int main() {
    string input;   // Accumulates the user's typed color value
    char ch;        // Holds the latest keystroke

    // ── Main input loop (runs until the user presses ENTER) ──
    while (true) {
        clearConsole();     // Refresh the screen on every iteration
        
        // Display the program header and usage instructions
        std::cout << "This terminal program lets you convert your color values to Hex, RGB and HSV simultaneously.";

        std::cout << "\n\nTo use this program, just type your values in your desired formats below-";

        std::cout << "\nFor RGB conversion: rgb(<value>, <value>, <value>)";
        std::cout << "\nFor HSV conversion: hsv(<value>, <value>, <value>)";
        std::cout << "\nFor Hex conversion: simply type your 6 digit value with # or without # at the very first";

        std::cout << "\n\nTo close this program hit [ENTER] or to edit values, use [BACKSPACE]\n\n\n";

        // Show the current input and perform live conversion if valid
        std::cout << "Enter the color: " << input;
        
        if (!input.empty()) {
            isFormat(input);    // Validate and convert the current input
        } 
        
        // Wait for the next keystroke (blocking, no echo)
        ch = _getch();

        if (ch == 13) {          // 13 is the ASCII code for Enter
            std::cout << "\nProgram instance terminated!";
            break;               // Exit the main loop
        } 
        else if (ch == 8) {      //  8 is the ASCII code for Backspace
            if (!input.empty()) {
                input.pop_back();   // Remove the last typed character
            }
        } 
        else if (std::isprint(ch)) {
            input += ch;         // Append the printable character to input
        }
    }
    return 0;
}
