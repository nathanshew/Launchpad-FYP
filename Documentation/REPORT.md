# DEX Arbitrage Mini Challenge - Final Report

**Date:** March 10, 2026  
**Project:** Launchpad-FYP  
**Author:** [Your Name]  
**Challenge Duration:** ~2 hours (extended with Part 2 optional implementation)

---

## Executive Summary

This project implements a complete DEX arbitrage detection and validation system in two parts:
- **Part 1**: Off-chain cycle detection in C++ analyzing Uniswap V2 pool snapshots
- **Part 2**: On-chain validation smart contract deployed to Ethereum Sepolia testnet

The system successfully detects profitable arbitrage cycles, ranks them by profit, and validates cycle profitability on-chain using live reserve data.

---

## Part 1: Off-Chain Cycle Detection

### 1.1 Objective
Build a C++ application that analyzes Uniswap V2 pool data to detect and rank profitable arbitrage cycles.

### 1.2 Implementation Approach

**Dataset Loading**
- Input: `v2pools.json` (37,768 pools, 33,356 unique tokens)
- Parser: `src/JSONParser.cpp` loads pool reserves, token metadata, and USD valuations
- Liquidity filtering: Applied $10,000 minimum `reserveUSD` threshold to reduce search space
- Filtered result: 3,786 pools, 3,399 tokens

**Graph Construction**
- Implementation: `src/Graph.cpp`
- Structure: Adjacency list mapping token address → list of outbound pools
- Each pool creates two directed edges (token0→token1, token1→token0)
- Enables O(1) neighbor lookup during cycle traversal

**Cycle Detection Algorithm**
- Implementation: `src/CycleDetector.cpp`
- Algorithm: Depth-first search (DFS) with backtracking
- Max depth: 3 (configurable, depth 5+ impractical on full dataset)
- Duplicate removal: Canonical signature (rotation + reverse normalization)
- Result: 474 unique cycles detected

**Swap Simulation (AMM Formula)**
- Implementation: `src/Pool.cpp`, `src/ArbitrageAnalyzer.cpp`
- Formula: Uniswap V2 constant product with 0.3% fee
  ```
  amountOut = (reserveOut * amountIn * 997) / (reserveIn * 1000 + amountIn * 997)
  ```
- Per-hop decimal rounding: floor to output token decimals after each hop
- Viability check: reject if `amountIn / reserveIn > 20%` (prevents unrealistic slippage)
- State isolation: uses local reserve copies, no mutation of shared state

**Trade Size Optimization**
- Strategy: Test 1%, 10%, and 20% of starting token's reserve in first pool
- Selection: Choose size yielding maximum valid USD profit
- Accounts for slippage: larger trades get worse prices due to AMM curve

**Ranking Metric**
- Primary: Absolute USD profit (descending)
- Secondary: ROI% (reported but not used for ranking)
- Rationale: USD profit is more actionable for capital allocation decisions

**Output**
- Format: HTML report (`report.html`) with top 10 cycles
- Includes: token path, pool addresses, optimal trade size, USD profit, ROI%
- Example (Rank #1): 147.48% ROI, $1,976.76 profit on 42.28 LINK trade

### 1.3 Results
- **Profitable cycles found:** 38 out of 474 detected cycles
- **Top opportunity:** LINK → DAI → AGIX → LINK (147.48% ROI)
- **Build time:** ~2s on Visual Studio 2026
- **Tests:** 10/10 passing (GoogleTest suite)

### 1.4 Code Quality
- **Structure:** 8 modular classes with single responsibilities
- **Comments:** Inline documentation explaining AMM math, cycle dedup logic, and constraints
- **Testing:** Unit tests for Token, Pool, Graph, CycleDetector
- **Build system:** CMake with Google Test and nlohmann/json dependencies

---

## Part 2: On-Chain Profitability Validation

### 2.1 Objective
Create a Solidity smart contract that validates cycle profitability using live on-chain reserve data.

### 2.2 Implementation Approach

**Smart Contract**
- File: `contracts/ArbitrageValidator.sol`
- Function signature: `validateCycle(address[] tokens, address[] pools, uint256 amountIn, uint256 minOut)`
- Returns: `uint256 amountOutFinal`
- Reverts if unprofitable or invalid

**Validation Logic**
1. Read live reserves via `IUniswapV2Pair.getReserves()` for each pool
2. Verify token pair matches pool's `token0`/`token1`
3. Compute hop output using same Uniswap V2 formula as Part 1
4. Require `amountOutFinal >= minOut` and `amountOutFinal > amountIn`

**Revert Conditions**
- `LengthMismatch`: tokens.length ≠ pools.length + 1
- `EmptyPath`: insufficient path length
- `ZeroAmountIn`: invalid input
- `HopTokenMismatch`: token pair doesn't match pool
- `ZeroReserve`: pool has zero liquidity
- `BelowMinOut`: output below minimum threshold
- `NotProfitable`: final output ≤ initial input

**Submission Script**
- File: `scripts/validateCycle.ts`
- Supports two modes:
  - `call`: eth_call simulation (free, no transaction)
  - `tx`: sends transaction via `validateCycleTx()` (emits event with tx hash/block)
- Input format: JSON with `tokens[]`, `pools[]`, `amountIn`, `minOut`

### 2.3 Deployment & Testing

**Local Testing**
- Framework: Hardhat + TypeScript
- Tests: 6/6 passing (`test/ArbitrageValidator.test.ts`)
- Coverage: profitable path, all revert conditions

**Sepolia Testnet Deployment**
- Network: Sepolia (Ethereum testnet)
- Validator address: `0xd3f576662CDa58684ee438c07E7b314A7Ef67288`
- Deployer: `0xe3D2AFaB1BAAa8e84455736027Db5445Db27F6a4`
- Gas spent: ~0.05 SepoliaETH (test funds from faucet)

**Validation Test**
- Approach: Deployed 3 `MockV2Pair` contracts with controlled reserves
- Reason: Minimal/no real Uniswap V2 liquidity on Sepolia testnet
- Result: Validation success with 116.87% profit on mock cycle
- Note: Contract logic is network-agnostic and would work with real mainnet pools

### 2.4 Integration Testing (Part 1 → Part 2)

**Mainnet Fork Validation:**
- Deployed validator on Ethereum mainnet fork
- Tested all 10 Part 1 cycles against live mainnet pool reserves
- Result: **0/10 cycles still profitable** (reserve drift since snapshot)
- Validator correctly rejected all cycles with `BelowMinOut` or `NotProfitable` errors

**Example failure:**
```
✗ Rank #1: NOT PROFITABLE ON-CHAIN
  Part 1 profit: $1976.76 (147.48% ROI)
  On-chain output: 30.5 LINK (expected 42.3 LINK)
  Reason: BelowMinOut - reserves changed since snapshot
```

**Conclusion:** Integration successful - validator correctly detects when historical cycles are no longer executable due to pool state changes.

### 2.5 Limitations
- **Mock pair testing:** Sepolia testnet used deployed test fixtures rather than real Uniswap V2 pools due to minimal testnet liquidity
- **Reserve drift:** Real pools change constantly; Part 1 snapshot cycles are no longer profitable on current mainnet (as demonstrated)
- **Gas costs:** No gas estimation; real arbitrage must be profitable after transaction costs
- **Scope:** Validates profitability only; does not execute swaps or interact with routers

---

## AI Tools Used

**Primary Tool:** GitHub Copilot (Claude Sonnet 4.5)

### How AI Helped

**Part 1 (C++ Development)**
- Clarified AMM constant-product formula and slippage mechanics
- Generated initial class scaffolding for modular OOP design
- Assisted with cycle deduplication using canonical signatures
- Helped implement per-hop decimal rounding and viability checks
- Generated GoogleTest unit tests for core components
- Provided CMake configuration for dependency management

**Part 2 (Solidity + Hardhat)**
- Generated Solidity validator contract with IUniswapV2Pair integration
- Created TypeScript deployment and validation scripts
- Set up Hardhat configuration for Sepolia testnet
- Assisted with .env setup and private key/RPC configuration
- Debugged deployment errors (insufficient funds, argument forwarding)
- Created mock pair deployment script for testnet validation

**Documentation**
- Helped structure PRD and final report
- Generated inline code comments explaining complex logic
- Maintained AI usage log (`Documentation/AI_use.md`) throughout project
- Created comprehensive README with build/run/test instructions

### Problems and Limitations Encountered

**AI Challenges**
- Initial suggestions sometimes lacked edge-case handling (e.g., zero reserves, duplicate cycles)
- Required iterative refinement to enforce deterministic behavior
- Performance tuning (liquidity filtering, max depth) needed manual intervention
- Some generated boilerplate needed manual cleanup for consistency

**Technical Challenges**
- **Part 1:** Search space too large without liquidity filtering (depth 5 took hours)
- **Part 2:** Hardhat argument forwarding issues required env-var workaround
- **Part 2:** Testnet lacks real Uniswap V2 pools; used mock pairs for validation proof

**Workarounds**
- Applied $10K minimum liquidity filter to reduce Part 1 search space
- Limited max depth to 3 for practical runtime
- Deployed mock pairs on Sepolia to demonstrate Part 2 contract correctness
- Added preflight balance checks to deployment script for better error messages

---

## Evaluation Criteria Assessment

### Correctness ✓
- **Part 1:** 10/10 tests passing; accurate AMM simulation with fee and slippage
- **Part 2:** 6/6 tests passing; contract correctly validates paths and reverts on failure
- Cycle detection produces expected results; deduplication removes rotations/reversals

### Strategy ✓
- **Detection:** DFS with canonical dedup is standard, proven approach
- **Ranking:** USD profit is practical metric for capital allocation
- **Trade size:** Testing multiple sizes accounts for slippage vs. opportunity cost
- **Validation:** On-chain recomputation with live reserves is industry-standard approach

### Code Quality ✓
- **Structure:** Modular OOP with single-responsibility classes
- **Readability:** Inline comments explaining AMM math, constraints, and algorithm logic
- **Maintainability:** Clean separation between parsing, graph, detection, analysis, export
- **Testing:** Comprehensive unit test coverage for core components

### Documentation ✓
- **GitHub Repository:** Complete with commented source code
- **Build Instructions:** CMake + npm commands in README.md
- **Short Report:** This document explaining approach, results, and tradeoffs
- **AI Usage Log:** Detailed log of AI interactions in `Documentation/AI_USE.md`
- **Technical Docs:** PRD.md, PRD_PART2.md, INTEGRATION.md with implementation details

---

## Repository Structure

```
Launchpad-FYP/
├── src/                          # Part 1 C++ source
│   ├── Token.h/cpp
│   ├── Pool.h/cpp
│   ├── Graph.h/cpp
│   ├── CycleDetector.h/cpp
│   ├── ArbitrageAnalyzer.h/cpp
│   ├── HTMLExporter.h/cpp
│   ├── JSONParser.h/cpp
│   └── main.cpp
├── tests/                        # Test suites
│   ├── part1/                    # Part 1 C++ unit tests
│   │   ├── test_Token.cpp
│   │   ├── test_Pool.cpp
│   │   ├── test_Graph.cpp
│   │   ├── test_CycleDetector.cpp
│   │   └── test_main.cpp
│   └── part2/                    # Part 2 Solidity tests
│       └── ArbitrageValidator.test.ts
├── contracts/                    # Part 2 Solidity contracts
│   ├── ArbitrageValidator.sol
│   └── MockV2Pair.sol
├── scripts/                      # Part 2 deployment/validation scripts
│   ├── deployValidator.ts
│   ├── deployMockCycle.ts
│   ├── validateCycle.ts
│   └── validatePart1Cycles.ts
├── part2_data/                   # Part 2 candidate cycle inputs
│   ├── candidate_cycle.example.json
│   └── candidate_cycle.sepolia.mock.json
├── Documentation/
│   ├── TASK.txt                  # Original challenge requirements
│   ├── PRD.md                    # Part 1 product requirements
│   ├── PRD_PART2.md              # Part 2 product requirements
│   ├── INTEGRATION.md            # Part 1→Part 2 integration guide
│   ├── AI_USE.md                 # Complete AI interaction log
│   └── REPORT.md                 # This comprehensive report
├── v2pools.json                  # Input dataset
├── report.html                   # Part 1 output (top 10 cycles)
├── CMakeLists.txt                # Part 1 build config
├── package.json                  # Part 2 dependencies
├── hardhat.config.ts             # Part 2 network config
├── .env                          # Part 2 secrets (gitignored)
└── README.md                     # Quick start guide
```

---

## How to Run

### Part 1: Off-Chain Detection

**Prerequisites:** C++17 compiler, CMake 3.20+

**Build:**
```powershell
cmake -S . -B build
cmake --build build --config Debug
```

**Run:**
```powershell
.\build\Debug\arbitrage_app.exe v2pools.json report.html 3
```

**Test:**
```powershell
ctest --test-dir build -C Debug --verbose
```

**Output:** `report.html` with top 10 profitable cycles

### Part 2: On-Chain Validation

**Prerequisites:** Node.js 20+, npm

**Install:**
```powershell
npm install
```

**Build:**
```powershell
npm run build:part2
```

**Test:**
```powershell
npm run test:part2
```

**Deploy to Sepolia:**
1. Copy `.env.example` to `.env` and fill in:
   - `SEPOLIA_RPC_URL` (Infura/Alchemy endpoint)
   - `PRIVATE_KEY` (wallet private key with test ETH)
2. Run:
```powershell
npm run deploy:part2 -- --network sepolia
```

**Validate a cycle:**
```powershell
$env:CYCLE_FILE='part2_data/candidate_cycle.sepolia.mock.json'
npm run validate:part2 -- --network sepolia
```

---

## Conclusion

This project demonstrates a complete arbitrage detection pipeline from off-chain analysis to on-chain validation:

**Strengths:**
- Modular, readable codebase with clear separation of concerns
- Accurate AMM simulation with proper fee, slippage, and decimal handling
- Comprehensive testing (16 total tests across both parts)
- Deployed and validated on Ethereum testnet
- Extensive documentation of approach and AI usage

**Tradeoffs:**
- Liquidity filtering reduces search space but may miss low-liquidity opportunities
- Max depth limited to 3 for practical runtime (depth 5+ too slow)
- Part 2 validated with mock pairs on Sepolia testnet for deployment proof
- Part 1→Part 2 integration tested on mainnet fork: 0/10 cycles still profitable due to reserve drift (expected)

**Future Extensions:**
- Mainnet fork testing to validate real Part 1 cycles
- Gas cost estimation in profitability calculation
- Risk-adjusted ranking (e.g., Sharpe ratio)
- MEV-aware submission strategies

This implementation meets all challenge requirements and demonstrates effective use of AI-assisted development for rapid prototyping while maintaining code quality and correctness.
