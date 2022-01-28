cmake ../mcpCharacterization/ -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j3 -DCMAKE_ECLIPSE_VERSION=4.1 && make && ./mcpCharacterization -o outputs/ -m spe -i "inputs" -v 0 -mcp 2127 -speMean 20e-12 $1 $2 $3 $4 $5 $6

