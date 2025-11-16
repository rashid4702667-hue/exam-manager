#include <iostream>
#include <fstream>

int main() {
    // Create a simple test PDF to verify the format
    std::ofstream file("build/test_simple.pdf", std::ios::binary);
    
    if (!file.is_open()) {
        std::cout << "Error: Could not open file for writing." << std::endl;
        return 1;
    }
    
    // Simple PDF structure
    std::string content = R"(%PDF-1.4
1 0 obj
<< /Type /Catalog /Pages 2 0 R >>
endobj

2 0 obj
<< /Type /Pages /Kids [3 0 R] /Count 1 >>
endobj

3 0 obj
<< /Type /Page /Parent 2 0 R /Resources 4 0 R /MediaBox [0 0 612 792] /Contents 5 0 R >>
endobj

4 0 obj
<< /Font << /F1 << /Type /Font /Subtype /Type1 /BaseFont /Helvetica >> >> >>
endobj

5 0 obj
<< /Length 200 >>
stream
BT
/F1 20 Tf
72 720 Td
(TEST PDF - FIXED FORMAT) Tj
ET

BT
/F1 12 Tf
72 680 Td
(This is a test to verify PDF generation is working correctly.) Tj
ET

BT
/F1 10 Tf
72 640 Td
(The main application should now have properly formatted tables.) Tj
ET
endstream
endobj

xref
0 6
0000000000 65535 f 
0000000010 00000 n 
0000000068 00000 n 
0000000125 00000 n 
0000000225 00000 n 
0000000330 00000 n 
trailer
<< /Size 6 /Root 1 0 R >>
startxref
585
%%EOF)";

    file << content;
    file.close();
    
    std::cout << "Test PDF created successfully: build/test_simple.pdf" << std::endl;
    return 0;
}