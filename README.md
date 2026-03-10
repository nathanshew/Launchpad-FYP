# Launchpad-FYP

**DEX Arbitrage Mini Challenge - Complete Implementation**

This project implements off-chain arbitrage cycle detection (Part 1) and on-chain validation (Part 2) for Uniswap V2 pools.

📄 **[Read the comprehensive final report](Documentation/REPORT.md)**

Quick links:
- [Integration Guide](Documentation/INTEGRATION.md)
- [AI Usage Log](Documentation/AI_USE.md)
- [Product Requirements (Part 1)](Documentation/PRD.md)
- [Product Requirements (Part 2)](Documentation/PRD_PART2.md)

## Requirements
- C++17 compiler
- CMake 3.20+
- Git (optional, for version control)

Windows note:
- If `cmake` is not recognized in a fresh terminal, run:
```powershell
$env:Path += ";C:\Program Files\CMake\bin"
```

## Features
- Parse `v2pools.json`
- Build token swap graph
- Detect cycles via DFS with canonical dedup
- Simulate swaps with Uniswap V2 math (0.3% fee)
- Enforce per-hop 20% input-to-reserve viability check
- Round each hop output to output-token decimals
- Rank profitable cycles by absolute USD profit
- Export top-10 opportunities to HTML report
- Filter low-liquidity pools before cycle search (current threshold: `reserveUSD >= 10000`)

## Project Structure
- `src/` application source files
- `tests/` GoogleTest unit tests
- `Documentation/` PRD, AI use log, and task text

## Build (from project root)
```powershell
cmake -S . -B build
cmake --build build --config Debug
```

## Run
```powershell
# Usage: arbitrage_app.exe [input_json] [output_html] [max_depth]
.\build\Debug\arbitrage_app.exe v2pools.json report.html 3
```

Run notes:
- Recommended `max_depth`: `3` (practical runtime on current dataset)
- Depth `5` can be very slow on large graphs
- Output report is written to `report.html`

## Test
```powershell
ctest --test-dir build -C Debug --verbose
```

Expected current result (with `v2pools.json`, depth `3`):
- Pools loaded: `37768`
- Tokens loaded: `33356`
- After liquidity filter: `3786` pools, `3399` tokens in graph
- Cycles detected: `474`
- Profitable cycles: `38`

## Part 2 (On-Chain Validation)

Part 2 is implemented with Solidity + Hardhat in parallel to the C++ Part 1 flow.

### Part 2 Prerequisites
- Node.js 20+
- npm

### Install Part 2 Dependencies
```powershell
npm install
```

### Compile Part 2 Contracts
```powershell
npm run build:part2
```

### Run Part 2 Tests
```powershell
npm run test:part2
```

### Deploy Validator (Optional, testnet)
1. Copy `.env.example` to `.env` and fill RPC/private key values.
2. Run:
```powershell
npm run deploy:part2 -- --network sepolia
```

### Validate a Candidate Cycle
```powershell
npm run validate:part2 -- --network sepolia --file part2_data/candidate_cycle.example.json
```

Notes:
- `mode: "call"` performs `eth_call` simulation.
- `mode: "tx"` sends a transaction via `validateCycleTx(...)` and prints tx hash + block number.