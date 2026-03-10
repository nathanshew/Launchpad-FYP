# Part 1 → Part 2 Integration Guide

This document explains how Part 1 (off-chain detection) connects to Part 2 (on-chain validation).

## Overview

**Part 1** analyzes a snapshot of Uniswap V2 pools and outputs:
1. `report.html` - Human-readable top 10 cycles
2. `part1_cycles_for_validation.json` - Machine-readable format for Part 2

**Part 2** validates whether those cycles are still profitable using live on-chain reserves.

## Workflow

### Step 1: Run Part 1 Detection

```powershell
# Build Part 1
cmake -S . -B build
cmake --build build --config Debug

# Run analysis (generates both HTML and JSON outputs)
.\build\Debug\arbitrage_app.exe v2pools.json report.html 3
```

**Outputs:**
- `report.html` - Top 10 cycles with profit/ROI
- `part1_cycles_for_validation.json` - Top 10 cycles in Part 2 format

**Note:** Update the validator address in `src/main.cpp` before building if you want the JSON to point to your deployed validator.

### Step 2: Deploy Part 2 Validator (One-time)

**Option A: Mainnet Fork (Recommended for testing real cycles)**

```powershell
# Terminal 1: Start mainnet fork
npm run fork:mainnet

# Terminal 2: Deploy validator to fork
npm run deploy:part2 -- --network mainnetFork
```

**Option B: Sepolia Testnet**

```powershell
# Copy .env.example to .env and fill in:
# - SEPOLIA_RPC_URL
# - PRIVATE_KEY (with test ETH)

npm run deploy:part2 -- --network sepolia
```

Copy the deployed validator address and update either:
- `src/main.cpp` line ~80: `std::string validatorAddress = "0x...";`
- Or manually edit `part1_cycles_for_validation.json` after Part 1 runs

### Step 3: Validate Part 1 Cycles On-Chain

```powershell
# Validate against mainnet fork (uses real mainnet pool reserves)
npm run validate:part1-cycles -- --network mainnetFork part1_cycles_for_validation.json

# Or validate against Sepolia testnet (if pools exist there)
npm run validate:part1-cycles -- --network sepolia part1_cycles_for_validation.json
```

**Output example:**
```
Validating 10 cycles from Part 1...

✓ Rank #1: STILL PROFITABLE
  Part 1 profit: $1976.76 (147.48% ROI)
  On-chain profit: 15.234 ETH (142.12% ROI)
  Path: 0x514...→ 0x6B1...→ 0x2374...

✗ Rank #2: NOT PROFITABLE ON-CHAIN
  Part 1 profit: $1523.45 (98.23% ROI)
  Reason: NotProfitable(1005000000000000000, 1010000000000000000)
  Path: 0x6B1...→ 0xC02...→ 0x514...

...

Summary: 4/10 cycles still profitable on-chain
```

## Why Use Mainnet Fork?

**Mainnet fork testing is recommended because:**
- Your Part 1 analysis used **real mainnet pools** from `v2pools.json`
- Sepolia testnet likely doesn't have the same pools/liquidity
- Fork testing is free (no gas costs) but uses real reserve data
- Shows which Part 1 cycles remain profitable despite reserve drift since snapshot

**Sepolia testing is useful for:**
- Demonstrating deployed contract on public testnet
- Getting transaction hash/block number evidence
- Testing with controlled mock pairs

## File Format

`part1_cycles_for_validation.json` structure:

```json
[
  {
    "rank": 1,
    "validatorAddress": "0xYourDeployedValidator",
    "tokens": ["0xTokenA", "0xTokenB", "0xTokenC", "0xTokenA"],
    "pools": ["0xPoolAB", "0xPoolBC", "0xPoolCA"],
    "amountIn": "1000000000000000000",
    "minOut": "1000000000000000001",
    "mode": "call",
    "metadata": {
      "profitUsd": 1976.76,
      "roi": 147.48,
      "optimalTradeSize": 42.28
    }
  }
]
```

## Troubleshooting

**"File not found: part1_cycles_for_validation.json"**
- Run Part 1 first to generate the file

**"execution reverted" for all cycles**
- Reserves changed since snapshot; this is expected
- Try with mainnet fork to test against current reserves
- Consider exporting more cycles (increase count in `src/main.cpp`)

**"VALIDATOR_ADDRESS not set" or pointing to placeholder**
- Deploy Part 2 validator first
- Update address in `src/main.cpp` before building Part 1
- Or manually edit the JSON file's `validatorAddress` field

## Integration Architecture

```
┌─────────────────┐
│   v2pools.json  │  (Mainnet snapshot)
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│    Part 1 C++   │
│  Cycle Detector │
└────────┬────────┘
         │
         ├──────────────────┐
         │                  │
         ▼                  ▼
┌─────────────────┐  ┌──────────────────────────────┐
│  report.html    │  │part1_cycles_for_validation.json│
│ (Human-readable)│  │   (Machine-readable)          │
└─────────────────┘  └──────────┬───────────────────┘
                                │
                                ▼
                     ┌──────────────────────┐
                     │   Part 2 Validator   │
                     │ (Solidity + Scripts) │
                     └──────────┬───────────┘
                                │
                                ▼
                     ┌──────────────────────┐
                     │  Validation Results  │
                     │ Still profitable: 4/10│
                     └──────────────────────┘
```

This completes the full arbitrage detection → validation pipeline.
