#!/usr/bin/env bash

export ProjectRoot=$(pwd)
export BuildDir="${ProjectRoot}/build"
export BinaryName="cttp_parser"
export TestName="parser_tests"

# Check for prerequisites
if ! command -v git &> /dev/null
then
    echo "Git could not be found. Please install it first."
    exit
fi

if ! command -v cmake &> /dev/null
then
    echo "CMake could not be found. Please install it first."
    exit
fi

# Show some information
echo "Project: cTTP Parser"
echo "GCC: $(gcc --version | head -n 1)"
echo "Git: $(git --version)"
echo "CMake: $(cmake --version)"

# Clean up previous build files
echo "Cleaning up previous build files..."
rm -rf "${BuildDir}"

# Set up the project
echo "Setting up the project..."
mkdir -p "${BuildDir}"
git submodule init && git submodule update

# Build the project
cd "${BuildDir}"
cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_POSITION_INDEPENDENT_CODE=ON
make

# Run the tests
echo "Running the tests..."
./test/${TestName}

# Run the parser
echo "Running the parser..."
./src/${BinaryName} --template "${ProjectRoot}/conf/test.conf" -i "Hello, my name is Alice and I am 25 years old"
echo "Done!"
