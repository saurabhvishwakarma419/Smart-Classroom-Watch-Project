#!/bin/bash

echo "================================"
echo "Smart Classroom Watch - Test Suite"
echo "================================"

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Track failures
FAILED=0

# Firmware Tests
echo -e "\n${GREEN}Running Firmware Tests...${NC}"
cd tests/firmware
if pio test; then
    echo -e "${GREEN}✓ Firmware tests passed${NC}"
else
    echo -e "${RED}✗ Firmware tests failed${NC}"
    FAILED=$((FAILED + 1))
fi
cd ../..

# Backend Tests
echo -e "\n${GREEN}Running Backend Tests...${NC}"
cd tests/backend
if npm test; then
    echo -e "${GREEN}✓ Backend tests passed${NC}"
else
    echo -e "${RED}✗ Backend tests failed${NC}"
    FAILED=$((FAILED + 1))
fi
cd ../..

# Mobile Tests
echo -e "\n${GREEN}Running Mobile App Tests...${NC}"
cd tests/mobile
if npm test; then
    echo -e "${GREEN}✓ Mobile tests passed${NC}"
else
    echo -e "${RED}✗ Mobile tests failed${NC}"
    FAILED=$((FAILED + 1))
fi
cd ../..

# Integration Tests
echo -e "\n${GREEN}Running Integration Tests...${NC}"
cd tests/integration
if npm test; then
    echo -e "${GREEN}✓ Integration tests passed${NC}"
else
    echo -e "${RED}✗ Integration tests failed${NC}"
    FAILED=$((FAILED + 1))
fi
cd ../..

# Summary
echo -e "\n================================"
if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed! ✓${NC}"
    exit 0
else
    echo -e "${RED}$FAILED test suite(s) failed ✗${NC}"
    exit 1
fi
