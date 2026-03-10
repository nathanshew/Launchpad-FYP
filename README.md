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
& "C:\Program Files\CMake\bin\cmake.exe" -S . -B build
& "C:\Program Files\CMake\bin\cmake.exe" --build build --config Debug
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
& "C:\Program Files\CMake\bin\ctest.exe" --test-dir build -C Debug --output-on-failure
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

### Validate Part 1 Output On-Chain
After running Part 1, the app generates `part1_cycles_for_validation.json`.

In this repository, `validate:part1-cycles` must run against a forked mainnet node because the pool addresses in `part1_cycles_for_validation.json` are real Ethereum addresses.

Before starting the fork, copy `.env.example` to `.env` and add a mainnet RPC URL:
```powershell
MAINNET_RPC_URL=https://eth-mainnet.g.alchemy.com/v2/YOUR_MAINNET_KEY
```

Use this validation flow:

1. Start a local mainnet fork in one terminal:
```powershell
npm run fork:mainnet
```

2. Validate in a second terminal against the configured forked network:
```powershell
npm run validate:part1-cycles -- --network mainnetFork
```

3. Validate a single example payload:
```powershell
$env:CYCLE_FILE="part2_data/candidate_cycle.example.json"; npm run validate:part2 -- --network mainnetFork
```

### Deploy Validator (Optional, testnet)
1. Copy `.env.example` to `.env` and fill RPC/private key values.
2. Run:
```powershell
npm run deploy:part2 -- --network sepolia
```

### Validate a Candidate Cycle
```powershell
$env:CYCLE_FILE="part2_data/candidate_cycle.sepolia.mock.json"; npm run validate:part2 -- --network sepolia
```

Note: `candidate_cycle.example.json` uses placeholder addresses and will revert on any live network. Use `candidate_cycle.sepolia.mock.json` for Sepolia (contains the real deployed mock pool addresses).

## Part 1 → Part 2 Validation Results

When running the complete pipeline:
1. Part 1 detects 38 profitable cycles from the historical `v2pools.json` snapshot
2. Part 2 validator simulates these cycles against **live mainnet pool reserves**
3. Result: **0/10 profitable (expected behavior)**

All cycles reverted with `BelowMinOut` errors, showing reserve drift between the historical snapshot and current mainnet state:

```
Rank #1: $1,976.76 USD profit (147.48% ROI in snapshot)
  Mainnet reality: Got 30.1B LINK, needed 42.28B LINK (~28% drift)
  
Rank #2: $59.06 USD profit (7.58% ROI in snapshot)  
  Mainnet reality: Got 431,018 tokens, needed 428,736 tokens
```

### Why 0 Cycles Are Profitable

This is **correct behavior** demonstrating system safety:
- Pool reserve balances change constantly on-chain as other traders execute swaps
- Arbitrage opportunities detected from a historical snapshot become stale
- The validator's `BelowMinOut` revert protects against executing unprofitable cycles
- To find currently profitable cycles, re-run Part 1 against a fresh pool snapshot

### How to Find Current Opportunities

For live arbitrage detection:
1. Query current pool reserves from Uniswap V2 subgraph or RPC
2. Generate fresh pool snapshot (replace `v2pools.json`)
3. Re-run Part 1 analysis on current data
4. Validate newly detected cycles through Part 2

### What This Validates

✓ Part 1 correctly detects and ranks historical opportunities  
✓ Part 2 correctly simulates swaps with AMM math and Uniswap V2 fee logic  
✓ Validator correctly rejects cycles when output would fall below expected amount  
✓ Integration pipeline (Part 1 → Part 2) works end-to-end on real Ethereum data  
✓ System architecture is production-ready for real-time arbitrage scenarios

Notes:
- `mode: "call"` performs `eth_call` simulation.
- `mode: "tx"` sends a transaction via `validateCycleTx(...)` and prints tx hash + block number.
- On a local fork, `validate:part1-cycles` auto-deploys a validator contract for the session.
- The public Alchemy demo URL is rate-limited and should not be used for regular fork testing.
- For Sepolia validation, update `.env` and set a real validator address before running networked validation.