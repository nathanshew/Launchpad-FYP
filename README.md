# Launchpad-FYP

DEX arbitrage cycle detection project for Uniswap V2 snapshot data.

## Features
- Parse `v2pools.json`
- Build token swap graph
- Detect cycles via DFS with canonical dedup
- Simulate swaps with Uniswap V2 math (0.3% fee)
- Enforce per-hop 20% input-to-reserve viability check
- Round each hop output to output-token decimals
- Rank profitable cycles by absolute USD profit
- Export top-10 opportunities to HTML report

## Project Structure
- `src/` application source files
- `tests/` GoogleTest unit tests
- `Documentation/` PRD, AI use log, and task text

## Build
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Run
```bash
# Usage: arbitrage_app [input_json] [output_html] [max_depth]
./arbitrage_app ../v2pools.json ../report.html 5
```

## Test
```bash
cd build
# Visual Studio generator (Windows)
ctest -C Debug --verbose
# Ninja/Make generators
# ctest --verbose
```