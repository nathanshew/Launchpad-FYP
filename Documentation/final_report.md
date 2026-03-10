# Final Report - DEX Arbitrage Mini Challenge (Part 1)

Date: March 10, 2026  
Project: Launchpad-FYP  
Language: C++17

## 1. Objective
Implement an off-chain system that:
- loads a Uniswap V2 pool snapshot,
- constructs a directed token swap graph,
- detects arbitrage cycles,
- simulates swap execution with AMM math,
- ranks opportunities by profit,
- outputs the Top-10 opportunities with suggested trade sizes.

## 2. Dataset Loading
Input file: `v2pools.json`

Implementation:
- `src/JSONParser.cpp` parses pool JSON into in-memory `Token` and `Pool` objects.
- Token metadata (address, symbol when available, decimals) is captured.
- Pool metadata includes token pair, reserves, and reserveUSD.

Run-time summary on current snapshot (from latest run):
- Pools loaded: 37768
- Unique tokens loaded: 33356
- After liquidity filter (`reserveUSD >= 10000`): 3786 pools
- Tokens in filtered graph: 3399

## 3. Graph Construction Method
Implementation files: `src/Graph.h`, `src/Graph.cpp`

Method used:
- Build an adjacency list keyed by token address.
- Each pool creates two directed swap possibilities:
  - token0 -> token1
  - token1 -> token0
- This represents all valid one-hop swaps in a directed token graph.

Why this is suitable:
- Fast neighbor lookup for DFS traversal.
- Natural fit for cycle search over tradable paths.

## 4. Swap Output Model (AMM Formula)
Implementation files: `src/Pool.cpp`, `src/ArbitrageAnalyzer.cpp`

Per-hop output uses Uniswap V2 constant-product math with fee:

`amountOut = (reserveOut * amountIn * 997) / (reserveIn * 1000 + amountIn * 997)`

Simulation details:
- Uses local reserve state per trial (shared pool objects are not mutated).
- Applies per-hop token-decimal flooring to avoid precision overestimation.
- Applies per-hop viability check: reject trial if `amountIn / reserveIn > 20%` at any hop.

## 5. Cycle Detection Logic
Implementation files: `src/CycleDetector.h`, `src/CycleDetector.cpp`

Algorithm:
- Depth-first search (DFS) with backtracking.
- Start DFS from each token.
- Track current token path and used pools.
- A cycle is recorded when traversal returns to start token with valid length.

Duplicate handling:
- Canonicalization removes rotated and reversed duplicates.
- A canonical signature is stored in a set before accepting a cycle.

Current run result:
- Cycles detected: 474

## 6. Ranking Metric and Trade Size Strategy
Implementation file: `src/ArbitrageAnalyzer.cpp`

Trade-size strategy:
- For each cycle, test candidate sizes based on the first hop reserve of the starting token.
- Candidate sizes: 1%, 10%, and 20% of that reserve.
- Keep the size with highest valid profit.

Ranking metric:
- Primary rank key: absolute USD profit (`profitUSD`) in descending order.
- ROI (%) is also reported for context but not used as primary rank key.

Current run result:
- Profitable cycles: 38
- Top-10 exported to `report.html`

## 7. Top-10 Output and Suggested Trade Size
Output artifact: `report.html`

The report includes, per opportunity:
- rank,
- cycle path,
- pool IDs,
- start token,
- suggested trade size (`Optimal Trade Size`),
- profit (USD),
- ROI (%).

Example from rank #1 (latest run):
- Suggested trade size: 42.28133663
- Profit: 1976.7643 USD
- ROI: 147.4814%

## 8. Build, Run, and Test
Build:
```powershell
cmake -S . -B build
cmake --build build --config Debug
```

Run:
```powershell
.\build\Debug\arbitrage_app.exe v2pools.json report.html 3
```

Tests:
```powershell
ctest --test-dir build -C Debug --verbose
```

## 9. AI Tools Used
Primary AI assistant: GitHub Copilot (GPT-5.3-Codex)

How AI was used:
- clarified architecture and modular component boundaries,
- helped derive and verify AMM simulation details,
- generated and refined class scaffolding and implementation,
- helped design and iterate cycle detection and dedup rules,
- assisted with test planning and unit-test implementation,
- helped produce and maintain project documentation.

Problems and limitations encountered:
- Suggestions required iterative correction for deterministic behavior and realistic constraints.
- Performance constraints required practical filtering/depth controls.
- Some generated wording and code required manual tightening for consistency and clarity.

## 10. Conclusion
Part 1 objectives were implemented in C++ with modular design, cycle detection, AMM-based simulation, profitability ranking, and Top-10 reporting.

Deliverables produced:
- working Part 1 codebase under `src/`,
- test suite under `tests/`,
- Top-10 opportunity output in `report.html`,
- documentation including this final report.
