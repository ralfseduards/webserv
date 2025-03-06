#!/bin/bash

# Configuration
SERVER="localhost"
PORT="8080"
TIMEOUT=3
PASS=0
FAIL=0
TOTAL=0

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Function to run a test
run_test() {
    local name="$1"
    local request="$2"
    local expected_code="$3"

    echo -e "${YELLOW}Testing: ${name}${NC}"
    echo -e "${YELLOW}Request:${NC} $request"

    # Send request and store response
    response=$(echo -e "$request" | nc -w $TIMEOUT $SERVER $PORT)
    status_line=$(echo "$response" | head -n 1)

    # Check if response contains expected status code
    if [[ "$status_line" == *"$expected_code"* ]]; then
        echo -e "${GREEN}PASS: Response contains expected status code $expected_code${NC}"
        echo -e "${GREEN}Response: $status_line${NC}"
        ((PASS++))
    else
        echo -e "${RED}FAIL: Response does not contain expected status code $expected_code${NC}"
        echo -e "${RED}Response: $status_line${NC}"
        ((FAIL++))
    fi

    echo "---------------------------------------"
    ((TOTAL++))
}

echo "===== Testing 400 Bad Request Error Code ====="
echo "Server: $SERVER:$PORT"
echo "---------------------------------------"

# Test 1: Invalid HTTP method
run_test "Invalid HTTP method" "INVALID_METHOD / HTTP/1.1\r\nHost: $SERVER\r\n\r\n" "400"

# Test 2: Missing colon in header
run_test "Missing colon in header" "GET / HTTP/1.1\r\nHost $SERVER\r\n\r\n" "400"

# Test 3: Invalid HTTP version
run_test "Invalid HTTP version" "GET / HTTP/9.9\r\nHost: $SERVER\r\n\r\n" "400"

# Test 4: Malformed header format
run_test "Malformed header format" "GET / HTTP/1.1\r\nMalformed-Header\r\nHost: $SERVER\r\n\r\n" "400"

# Test 5: Space before colon
run_test "Space before colon" "GET / HTTP/1.1\r\nHost : $SERVER\r\n\r\n" "400"

# Test 6: Invalid characters in header name
run_test "Invalid characters in header name" "GET / HTTP/1.1\r\nX@Invalid#: value\r\nHost: $SERVER\r\n\r\n" "400"

# Test 7: Extremely long header value
long_header=$(printf 'X-Long: %10000s' | tr ' ' 'A')
run_test "Long header value" "GET / HTTP/1.1\r\n$long_header\r\nHost: $SERVER\r\n\r\n" "400"

# Test 8: Duplicate Content-Length headers
run_test "Duplicate Content-Length" "GET / HTTP/1.1\r\nHost: $SERVER\r\nContent-Length: 10\r\nContent-Length: 20\r\n\r\n" "400"

# Display summary
echo "===== Test Summary ====="
echo -e "${GREEN}Passed: $PASS/${TOTAL}${NC}"
echo -e "${RED}Failed: $FAIL/${TOTAL}${NC}"

if [ $FAIL -eq 0 ]; then
    echo -e "${GREEN}All tests passed successfully!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed!${NC}"
    exit 1
fi
