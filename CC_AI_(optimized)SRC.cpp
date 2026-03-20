/*
 * ============================================================================
 *  ConvertColor — Optimized Real-Time Terminal Color Converter
 * ============================================================================
 *
 *  Converts color values between HEX, RGB, and HSV in real time as you type.
 *
 *  Controls:  [Any key]   → Append character
 *             [BACKSPACE] → Delete last character
 *             [ENTER]     → Exit
 *
 *  Optimizations over the original:
 *    • Zero code duplication — pure conversion functions shared by all paths
 *    • Structs (RGB, HSV) for type-safe, self-documenting data
 *    • const& string parameters — no unnecessary heap copies
 *    • std::round() instead of manual +0.5 trick
 *    • ANSI escape clear instead of std::system("cls") — no child process
 *    • sscanf instead of std::stringstream — no allocations, no locale
 *    • constexpr lookup table, static UI string
 *    • Allocation-free prefix/suffix validation (no substr)
 * ============================================================================
 */

// ── Headers ─────────────────────────────────────────────────────────────────
#include <iostream>
#include <string>
#include <cstdio>      // std::sscanf, std::snprintf
#include <cmath>        // std::round, std::fmod
#include <cctype>       // std::isxdigit, std::isprint
#include <conio.h>      // _getch (Windows-specific unbuffered input)
#include <algorithm>    // std::min, std::max

// ── Structured Color Types ──────────────────────────────────────────────────

struct RGB { int r, g, b; };
struct HSV { int h, s, v; };

// ── Compile-Time Constants ──────────────────────────────────────────────────

static constexpr char HEX_DIGITS[] = "0123456789ABCDEF";

static const char* const UI_HEADER =
    "This terminal program lets you convert your color values to Hex, RGB and HSV simultaneously."
    "\n\nTo use this program, just type your values in your desired formats below-"
    "\nFor RGB conversion: rgb(<value>, <value>, <value>)"
    "\nFor HSV conversion: hsv(<value>, <value>, <value>)"
    "\nFor Hex conversion: simply type your 6 digit value with # or without # at the very first"
    "\n\nTo close this program hit [ENTER] or to edit values, use [BACKSPACE]\n\n\n";

// ════════════════════════════════════════════════════════════════════════════
//  PURE CONVERSION FUNCTIONS
//  Operate on numeric values only — no string parsing, no I/O.
// ════════════════════════════════════════════════════════════════════════════

/*
 *  HSV → RGB
 *  H ∈ [0,360], S ∈ [0,100], V ∈ [0,100]  →  R,G,B ∈ [0,255]
 *
 *  Sector-based algorithm: divide hue into 6 × 60° sectors, compute
 *  intermediates p/q/t, and assign channels by sector index.
 */
RGB hsvToRgb(int H, int S, int V) {
    const float s = S / 100.0f;
    const float v = V / 100.0f;

    if (S == 0) {
        const int grey = static_cast<int>(std::round(v * 255.0f));
        return {grey, grey, grey};
    }

    const float h = H / 60.0f;
    const int   i = static_cast<int>(h);
    const float f = h - i;
    const float p = v * (1.0f - s);
    const float q = v * (1.0f - s * f);
    const float t = v * (1.0f - s * (1.0f - f));

    float r_f, g_f, b_f;
    switch (i % 6) {
        case 0: r_f = v; g_f = t; b_f = p; break;
        case 1: r_f = q; g_f = v; b_f = p; break;
        case 2: r_f = p; g_f = v; b_f = t; break;
        case 3: r_f = p; g_f = q; b_f = v; break;
        case 4: r_f = t; g_f = p; b_f = v; break;
        case 5: r_f = v; g_f = p; b_f = q; break;
        default: r_f = g_f = b_f = 0; break;  // unreachable
    }

    return {
        static_cast<int>(std::round(r_f * 255.0f)),
        static_cast<int>(std::round(g_f * 255.0f)),
        static_cast<int>(std::round(b_f * 255.0f))
    };
}

/*
 *  RGB → HSV
 *  R,G,B ∈ [0,255]  →  H ∈ [0,360], S ∈ [0,100], V ∈ [0,100]
 *
 *  Standard min/max algorithm with 60° sector hue computation.
 */
HSV rgbToHsv(int R, int G, int B) {
    const int mx = std::max({R, G, B});
    const int mn = std::min({R, G, B});
    const int delta = mx - mn;

    // Value
    const int V = static_cast<int>(std::round((mx / 255.0f) * 100.0f));

    // Saturation
    int S = 0;
    if (mx != 0) {
        S = static_cast<int>(std::round((static_cast<float>(delta) / mx) * 100.0f));
    }

    // Hue
    int H = 0;
    if (delta != 0) {
        if (mx == R)      H = static_cast<int>(std::round(60.0f * (G - B) / static_cast<float>(delta)));
        else if (mx == G) H = static_cast<int>(std::round(60.0f * ((B - R) / static_cast<float>(delta) + 2.0f)));
        else              H = static_cast<int>(std::round(60.0f * ((R - G) / static_cast<float>(delta) + 4.0f)));

        if (H < 0) H += 360;
    }

    return {H, S, V};
}

/*
 *  RGB → HEX string
 *  Builds "#RRGGBB" via nibble decomposition into the constexpr lookup table.
 */
std::string rgbToHex(int R, int G, int B) {
    char buf[8];  // "#RRGGBB\0"
    buf[0] = '#';
    buf[1] = HEX_DIGITS[R >> 4];   // high nibble  (R / 16)
    buf[2] = HEX_DIGITS[R & 0xF];  // low  nibble  (R % 16)
    buf[3] = HEX_DIGITS[G >> 4];
    buf[4] = HEX_DIGITS[G & 0xF];
    buf[5] = HEX_DIGITS[B >> 4];
    buf[6] = HEX_DIGITS[B & 0xF];
    buf[7] = '\0';
    return buf;
}

/*
 *  HEX string → RGB
 *  Parses "#RRGGBB" or "RRGGBB" via std::stoi with base 16.
 */
RGB hexToRgb(const std::string& hex) {
    const int off = (hex[0] == '#') ? 1 : 0;
    return {
        std::stoi(hex.substr(off,     2), nullptr, 16),
        std::stoi(hex.substr(off + 2, 2), nullptr, 16),
        std::stoi(hex.substr(off + 4, 2), nullptr, 16)
    };
}

// ════════════════════════════════════════════════════════════════════════════
//  VALIDATION FUNCTIONS
//  Each returns true if the input is a valid color in that format.
//  Uses allocation-free checks wherever possible.
// ════════════════════════════════════════════════════════════════════════════

/*
 *  isHex — validates "#RRGGBB" (7 chars) or "RRGGBB" (6 chars).
 *  Checks length, optional '#' prefix, and that all digits are hex.
 */
bool isHex(const std::string& input) {
    const size_t len = input.size();
    int start;

    if (len == 7 && input[0] == '#')
        start = 1;
    else if (len == 6)
        start = 0;
    else
        return false;

    for (size_t i = start; i < len; ++i) {
        if (!std::isxdigit(static_cast<unsigned char>(input[i])))
            return false;
    }
    return true;
}

/*
 *  isRGB — validates "rgb(R, G, B)" with comma or semicolon separators.
 *  Uses sscanf for zero-allocation parsing; checks prefix/suffix without substr.
 */
bool isRGB(const std::string& input) {
    const size_t len = input.size();
    if (len < 10) return false;

    // Prefix check: "rgb(" — character-level, no allocation
    if (input[0] != 'r' || input[1] != 'g' || input[2] != 'b' || input[3] != '(')
        return false;
    // Suffix check: ")"
    if (input[len - 1] != ')')
        return false;

    // Parse the interior "R, G, B" via sscanf — try comma first, then semicolon
    int R, G, B;
    const char* interior = input.c_str() + 4;

    if (std::sscanf(interior, "%d ,%d ,%d", &R, &G, &B) == 3 ||
        std::sscanf(interior, "%d ;%d ;%d", &R, &G, &B) == 3) {
        return (R >= 0 && R <= 255) &&
               (G >= 0 && G <= 255) &&
               (B >= 0 && B <= 255);
    }
    return false;
}

/*
 *  isHSV — validates "hsv(H, S, V)" with comma or semicolon separators.
 *  Same sscanf strategy as isRGB.
 */
bool isHSV(const std::string& input) {
    const size_t len = input.size();
    if (len < 10) return false;

    // Prefix check: "hsv("
    if (input[0] != 'h' || input[1] != 's' || input[2] != 'v' || input[3] != '(')
        return false;
    // Suffix check: ")"
    if (input[len - 1] != ')')
        return false;

    int H, S, V;
    const char* interior = input.c_str() + 4;

    if (std::sscanf(interior, "%d ,%d ,%d", &H, &S, &V) == 3 ||
        std::sscanf(interior, "%d ;%d ;%d", &H, &S, &V) == 3) {
        return (H >= 0 && H <= 360) &&
               (S >= 0 && S <= 100) &&
               (V >= 0 && V <= 100);
    }
    return false;
}

// ════════════════════════════════════════════════════════════════════════════
//  FORMAT DISPATCH — Detect format, convert, and print results
// ════════════════════════════════════════════════════════════════════════════

void detectAndConvert(const std::string& input) {
    if (isHex(input)) {
        const RGB rgb = hexToRgb(input);
        const HSV hsv = rgbToHsv(rgb.r, rgb.g, rgb.b);
        std::cout << "\nrgb(" << rgb.r << ", " << rgb.g << ", " << rgb.b << ")"
                  << "\nhsv(" << hsv.h << ", " << hsv.s << ", " << hsv.v << ")";
    }
    else if (isRGB(input)) {
        int R, G, B;
        const char* interior = input.c_str() + 4;
        if (std::sscanf(interior, "%d ,%d ,%d", &R, &G, &B) != 3)
            std::sscanf(interior, "%d ;%d ;%d", &R, &G, &B);

        const std::string hex = rgbToHex(R, G, B);
        const HSV hsv = rgbToHsv(R, G, B);
        std::cout << "\n" << hex
                  << "\nhsv(" << hsv.h << ", " << hsv.s << ", " << hsv.v << ")";
    }
    else if (isHSV(input)) {
        int H, S, V;
        const char* interior = input.c_str() + 4;
        if (std::sscanf(interior, "%d ,%d ,%d", &H, &S, &V) != 3)
            std::sscanf(interior, "%d ;%d ;%d", &H, &S, &V);

        const RGB rgb = hsvToRgb(H, S, V);
        const std::string hex = rgbToHex(rgb.r, rgb.g, rgb.b);
        std::cout << "\nrgb(" << rgb.r << ", " << rgb.g << ", " << rgb.b << ")"
                  << "\n" << hex;
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  UTILITY
// ════════════════════════════════════════════════════════════════════════════

/*
 *  clearConsole — ANSI escape sequence instead of system("cls").
 *  \033[2J  clears the entire screen.
 *  \033[H   moves the cursor to row 1, column 1.
 *  Works on Windows 10+ (conhost & Windows Terminal) and all Unix terminals.
 */
inline void clearConsole() {
    std::cout << "\033[2J\033[H";
}

// ════════════════════════════════════════════════════════════════════════════
//  MAIN — Real-Time Input Loop
// ════════════════════════════════════════════════════════════════════════════

int main() {
    std::string input;
    input.reserve(32);  // pre-allocate — avoids repeated reallocs for typical input

    while (true) {
        clearConsole();

        std::cout << UI_HEADER
                  << "Enter the color: " << input;

        if (!input.empty()) {
            detectAndConvert(input);
        }

        const char ch = static_cast<char>(_getch());

        if (ch == 13) {                         // ENTER → exit
            std::cout << "\nProgram instance terminated!";
            break;
        }
        else if (ch == 8) {                     // BACKSPACE → delete last
            if (!input.empty()) {
                input.pop_back();
            }
        }
        else if (std::isprint(static_cast<unsigned char>(ch))) {
            input += ch;                        // Append printable character
        }
    }

    return 0;
}
