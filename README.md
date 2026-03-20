# ConvertColorSRC — Real-Time Terminal Color Converter
> [!WARNING]
> **AI Usage Disclosure**
> This `README.md` is generated with the help of AI LLMs. That does not imply that the code is written by AI too.
> 
> This project is meant to start my journey with GitHub and to begin learning C++. I wouldn't like throwing AI slop in this OR my upcoming projects, so I prefer to be safe by warning y'all about the AI-assisted parts:
> - Claude was used to generate this README.
> - Claude was used to ONLY add descriptive/explanatory comments in the source code (separate file).
> - For the source code file without comments, I wrote it manually. I used Google ONLY to understand unique functions (their syntax, parameters and examples as well), header files, and the conversion formulas.


A **real-time, interactive terminal program** written in C++ that converts color values between **HEX**, **RGB**, and **HSV** formats simultaneously as you type.

---

## Table of Contents

1. [How It Works](#how-it-works)
2. [Supported Formats](#supported-formats)
3. [Controls](#controls)
4. [Color Conversion Formulas](#color-conversion-formulas)
   - [HSV → RGB](#1-hsv--rgb)
   - [RGB → HEX](#2-rgb--hex)
   - [HEX → RGB](#3-hex--rgb)
   - [RGB → HSV](#4-rgb--hsv)
5. [C++ Functions & Methods Reference](#c-functions--methods-reference)
   - [Standard Library Functions](#standard-library-functions)
   - [String Methods](#string-methods)
   - [Stream Operations](#stream-operations)
   - [Console / System Functions](#console--system-functions)
6. [Program Architecture](#program-architecture)
   - [Validation Functions](#validation-functions)
   - [Conversion Functions](#conversion-functions)
   - [Utility Functions](#utility-functions)
7. [How to Compile & Run](#how-to-compile--run)

---

## How It Works

The program runs an **infinite loop** that:

1. **Clears** the console screen.
2. **Displays** usage instructions and the current input.
3. **Validates** the input in real-time against all three color formats.
4. **Converts** and prints the equivalent values in the other two formats (if valid).
5. **Waits** for a single keystroke — no need to press Enter after every character.
6. **Repeats** from step 1.

This creates a **live preview** — results update instantly as you type each character.

---

## Supported Formats

| Format | Syntax                | Example            | Value Ranges                                |
|--------|-----------------------|--------------------|---------------------------------------------|
| **RGB**| `rgb(R, G, B)`        | `rgb(255, 87, 51)` | R, G, B: **0 – 255**                        |
| **HSV**| `hsv(H, S, V)`        | `hsv(11, 80, 100)` | H: **0 – 360**, S: **0 – 100**, V: **0 – 100** |
| **HEX**| `#RRGGBB` or `RRGGBB` | `#FF5733`          | Each pair: **00 – FF**                       |

> **Note:** RGB and HSV values can be separated by either commas `,` or semicolons `;`, but **both separators must match** (e.g. `rgb(255, 87, 51)` but `rgb(255, 87; 51)`).

---

## Controls

| Key           | Action                                    |
|---------------|-------------------------------------------|
| Any character | Appends to the current input              |
| `BACKSPACE`   | Removes the last character from input     |
| `ENTER`       | Exits the program                         |

---

## Color Conversion Formulas

### 1. HSV → RGB

HSV (Hue, Saturation, Value) is converted to RGB using the **sector-based algorithm**:

**Step 1 — Normalize S and V:**
```
s = S / 100.0      →  range becomes [0, 1]
v = V / 100.0      →  range becomes [0, 1]
```

**Step 2 — Achromatic check:**
If `S == 0`, the color is a shade of grey:
```
R = G = B = V × 255
```

**Step 3 — Chromatic conversion:**
The hue wheel (0°–360°) is divided into **6 sectors** of 60° each:

```
h = H / 60.0          // Sector as a float (e.g. 2.5)
i = floor(h)           // Sector index: 0, 1, 2, 3, 4, or 5
f = h - i              // Fractional part within the sector
```

**Intermediate values:**
```
p = v × (1 - s)
q = v × (1 - s × f)
t = v × (1 - s × (1 - f))
```

**Assign R, G, B based on sector `i`:**

| Sector (`i`) | Hue Range     | R   | G   | B   |
|:------------:|:-------------:|:---:|:---:|:---:|
| 0            | 0° – 59°     | v   | t   | p   |
| 1            | 60° – 119°   | q   | v   | p   |
| 2            | 120° – 179°  | p   | v   | t   |
| 3            | 180° – 239°  | p   | q   | v   |
| 4            | 240° – 299°  | t   | p   | v   |
| 5            | 300° – 359°  | v   | p   | q   |

**Step 4 — Scale to 0–255:**
```
R = round(r × 255)
G = round(g × 255)
B = round(b × 255)
```
> In the code, rounding is achieved by `(int)(value * 255 + 0.5)`.

---

### 2. RGB → HEX

Each RGB channel (0–255) is converted into a **two-digit hexadecimal** number.

**Algorithm — Nibble decomposition:**
```
high_nibble = value / 16     →  gives the first hex digit  (0–F)
low_nibble  = value % 16     →  gives the second hex digit (0–F)
```

**Lookup table:** `"0123456789ABCDEF"` maps index → hex character.

**Example:**
```
R = 255  →  255 / 16 = 15 → 'F',   255 % 16 = 15 → 'F'   →  "FF"
G =  87  →   87 / 16 =  5 → '5',    87 % 16 =  7 → '7'   →  "57"
B =  51  →   51 / 16 =  3 → '3',    51 % 16 =  3 → '3'   →  "33"

Result: #FF5733
```

---

### 3. HEX → RGB

The reverse of RGB → HEX. Each pair of hex characters is parsed as a **base-16 integer**.

```
R = hex_to_int("FF")  →  255
G = hex_to_int("57")  →   87
B = hex_to_int("33")  →   51
```

> In the code, `std::stoi(substring, nullptr, 16)` performs this conversion, where `16` specifies hexadecimal (base 16).

---

### 4. RGB → HSV

**Step 1 — Find min and max of (R, G, B):**
```
max = maximum of (R, G, B)
min = minimum of (R, G, B)
delta = max - min
```

**Step 2 — Value (V):**
```
V = (max / 255.0) × 100       →  percentage [0, 100]
```

**Step 3 — Saturation (S):**
```
if max == 0:
    S = 0                       →  pure black, saturation undefined
else:
    S = (delta / max) × 100    →  percentage [0, 100]
```

**Step 4 — Hue (H) in degrees:**
```
if max == min:
    H = 0                       →  achromatic (grey), hue undefined

else if max == R:
    H = 60 × (G - B) / delta                →  result in [-60, 60]

else if max == G:
    H = 60 × ((B - R) / delta + 2)          →  result in [60, 180]

else if max == B:
    H = 60 × ((R - G) / delta + 4)          →  result in [180, 300]

if H < 0:
    H = H + 360                 →  wrap negative values into [0, 360)
```

---

## C++ Functions & Methods Reference

Below is every standard library function and method used in the program, explained in detail.

### Standard Library Functions

#### `std::isxdigit(char c)`
**Header:** `<cctype>`
**Purpose:** Checks whether a character is a valid **hexadecimal digit**.
**Valid characters:** `0–9`, `A–F`, `a–f`
**Returns:** Non-zero (true) if valid hex digit, `0` (false) otherwise.
**Used in:** `isHex()` — to verify every character in the input is a valid hex digit.
```cpp
std::isxdigit('A');   // → true  (valid hex)
std::isxdigit('G');   // → false (not hex)
std::isxdigit('3');   // → true  (valid hex)
```

---

#### `std::isprint(char c)`
**Header:** `<cctype>`
**Purpose:** Checks whether a character is **printable** (i.e., not a control character).
**Printable range:** ASCII 32 (`' '`) through 126 (`'~'`).
**Returns:** Non-zero (true) if printable, `0` (false) otherwise.
**Used in:** `main()` — to ensure only visible characters are appended to the input string.
```cpp
std::isprint('A');    // → true  (printable)
std::isprint('\n');   // → false (control character)
std::isprint('\t');   // → false (control character)
```

---

#### `std::stoi(string str, size_t* pos, int base)`
**Header:** `<string>`
**Purpose:** Converts a **string to an integer** in the specified number base.
**Parameters:**
| Parameter | Description |
|-----------|-------------|
| `str`     | The string to convert |
| `pos`     | Pointer to store the index of the first unconverted character (`nullptr` if unused) |
| `base`    | The number base for conversion (e.g., `10` for decimal, `16` for hexadecimal) |

**Used in:** `convertHex()` — to parse hex color pairs like `"FF"` into integers like `255`.
```cpp
std::stoi("FF", nullptr, 16);   // → 255
std::stoi("0A", nullptr, 16);   // → 10
std::stoi("42", nullptr, 10);   // → 42
```

---

#### `std::system(const char* command)`
**Header:** `<cstdlib>`
**Purpose:** Executes a **shell command** in the operating system's command processor.
**Used in:** `clearConsole()` — to clear the terminal screen.
```cpp
std::system("cls");     // Clears screen on Windows
std::system("clear");   // Clears screen on Unix/macOS
```

---

#### `_getch()`
**Header:** `<conio.h>` *(Windows-specific)*
**Purpose:** Reads a **single character** from the keyboard **without echoing** it to the screen and **without waiting for Enter**.
**Returns:** The ASCII code of the pressed key as an `int`.
**Used in:** `main()` — to capture each keystroke in the real-time input loop.
```cpp
char ch = _getch();
// User presses 'A' → ch = 65 (ASCII for 'A')
// User presses Enter → ch = 13
// User presses Backspace → ch = 8
```

> **Why not `std::cin`?** — `std::cin` requires the user to press Enter before the input is processed. `_getch()` processes each key **immediately**, enabling the real-time behavior.

---

### String Methods

#### `.size()`
**Purpose:** Returns the **number of characters** in a string.
**Returns:** `size_t` (unsigned integer).
**Used in:** Validation functions to check input length, and loops to iterate through characters.
```cpp
string s = "hello";
s.size();   // → 5

string hex = "#FF5733";
hex.size();   // → 7
```

---

#### `.substr(size_t pos, size_t len)`
**Purpose:** Extracts a **substring** starting at position `pos` with length `len`.
**Parameters:**
| Parameter | Description |
|-----------|-------------|
| `pos`     | Starting index (0-based) |
| `len`     | Number of characters to extract (optional — if omitted, extracts to the end) |

**Returns:** A new `string` containing the extracted characters.
**Used in:** Multiple places — parsing color values, checking prefixes/suffixes, extracting hex pairs.
```cpp
string input = "rgb(255, 87, 51)";

input.substr(0, 4);     // → "rgb("       (first 4 characters)
input.substr(4, 11);    // → "255, 87, 51" (content between parentheses)
input.substr(15);       // → ")"          (from index 15 to end)

string hex = "#FF5733";
hex.substr(1, 2);       // → "FF"  (Red channel)
hex.substr(3, 2);       // → "57"  (Green channel)
hex.substr(5, 2);       // → "33"  (Blue channel)
```

---

#### `.empty()`
**Purpose:** Checks whether the string has **zero characters**.
**Returns:** `true` if the string is empty, `false` otherwise.
**Used in:** `main()` — to skip conversion when no input has been typed yet, and to prevent popping from an empty string.
```cpp
string s = "";
s.empty();    // → true

string t = "a";
t.empty();    // → false
```

---

#### `.pop_back()`
**Purpose:** Removes the **last character** from the string.
**Used in:** `main()` — to implement **backspace** functionality, deleting the most recent character.
```cpp
string s = "hello";
s.pop_back();
// s is now "hell"
```

---

#### `operator+=`
**Purpose:** **Appends** a character (or string) to the end of the string.
**Used in:** `main()` — to build the input string character-by-character as the user types.
```cpp
string s = "rgb";
s += '(';
// s is now "rgb("
```

---

### Stream Operations

#### `std::stringstream`
**Header:** `<sstream>`
**Purpose:** Creates an **in-memory stream** from a string, allowing you to **parse** it using the `>>` extraction operator just like reading from `std::cin`.
**Used in:** Validation and conversion functions — to extract numeric values and separators from the color string.
```cpp
std::stringstream ss("255, 87, 51");

int R, G, B;
char sep1, sep2;

ss >> R >> sep1 >> G >> sep2 >> B;
// R = 255, sep1 = ',', G = 87, sep2 = ',', B = 51
```

**How `>>` works here:**
- `>> R` reads an integer, skipping leading whitespace.
- `>> sep1` reads a single character (the comma `,`).
- `>> G` reads the next integer, again skipping whitespace.
- The stream returns a **falsy value** if parsing fails at any point.

---

### Console / System Functions

#### Preprocessor Macros: `_WIN32` and `_WIN64`
**Purpose:** These are **compiler-defined macros** that indicate the **target operating system**.
| Macro    | Defined when compiling for |
|----------|----------------------------|
| `_WIN32` | 32-bit or 64-bit Windows   |
| `_WIN64` | 64-bit Windows only        |

**Used in:** `clearConsole()` — to select the correct clear-screen command:
- Windows → `"cls"`
- Unix/macOS → `"clear"`

```cpp
#if defined(_WIN32) || defined(_WIN64)
    std::system("cls");     // Windows
#else
    std::system("clear");   // Unix / macOS
#endif
```

---

## Program Architecture

### Validation Functions

These functions verify whether the input string is a valid color value. Each returns `bool`.

| Function   | Validates          | Key Checks                                             |
|:----------:|:------------------:|:-------------------------------------------------------|
| `isHex()`  | `#RRGGBB`/`RRGGBB`| Length is 6 or 7; all characters are hex digits         |
| `isRGB()`  | `rgb(R, G, B)`     | Prefix `rgb(`, suffix `)`, R/G/B in [0, 255]           |
| `isHSV()`  | `hsv(H, S, V)`     | Prefix `hsv(`, suffix `)`, H [0, 360], S/V [0, 100]   |

**`isFormat()`** acts as a **dispatcher** — it calls each validator in order (HEX → RGB → HSV) and triggers the matching conversion function.

---

### Conversion Functions

Each function takes a **validated** input string and prints the other two formats.

| Function       | Input Format | Outputs          |
|:--------------:|:------------:|:----------------:|
| `convertHex()` | HEX          | RGB + HSV        |
| `convertRGB()` | RGB          | HEX + HSV        |
| `convertHSV()` | HSV          | RGB + HEX        |

---

### Utility Functions

| Function         | Purpose                                      |
|:----------------:|:---------------------------------------------|
| `clearConsole()` | Clears the terminal (cross-platform)         |
| `main()`         | Entry point — runs the real-time input loop  |

---

## How to Compile & Run

### Using g++ (MinGW on Windows)

```bash
g++ -o ConvertColor ConvertColorSRC.cpp
ConvertColor.exe
```

### Using Visual Studio Developer Command Prompt

```bash
cl ConvertColorSRC.cpp /Fe:ConvertColor.exe
ConvertColor.exe
```

> **Note:** This program uses `<conio.h>` and `_getch()`, which are **Windows-specific**. It will not compile on Linux/macOS without modifications.

---

*Built with <3 in C++*
