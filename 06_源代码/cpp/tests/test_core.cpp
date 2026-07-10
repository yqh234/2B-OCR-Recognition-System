#include "ocr_core.hpp"

#include <cassert>

int main() {
    assert(ocr2b::normalizeText("  hello\r\nworld  ") == "hello world");
    assert(ocr2b::escapeCsv("plain") == "plain");
    assert(ocr2b::escapeCsv("a,b") == "\"a,b\"");
    assert(ocr2b::escapeCsv("a\"b") == "\"a\"\"b\"");
    return 0;
}
