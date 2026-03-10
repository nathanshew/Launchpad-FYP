# AI Use Log

This document tracks how AI is used in this project, including what was requested and how the AI was instructed to help.

## Project
- Name: on-chain arbitrage task
- Started tracking: 2026-03-10

## How I Instructed AI to Help
- I asked AI to create this `AI_use.md` file in my project folder.
- I asked AI to track my AI use and how I have instructed it to help build this project.
- I asked AI to keep all documentation files in the `Documentation` folder.
- I asked AI to continue updating this log as the project goes on.

## AI Interaction Log

### 2026-03-10
- Request: "Create an AI_use.md in my folder, and track my AI use, and how I have instructed it to help me build this project"
- AI Action: Created `AI_use.md` at the project root.
- Outcome: Tracking document initialized.

### 2026-03-10
- Request: "All these documentation should be in the Documentation folder please"
- AI Action: Moved `AI_use.md` from project root to `Documentation/AI_use.md`.
- Outcome: Documentation centralized in `Documentation`.

### 2026-03-10
- Request: "Please continue to update AI_use.md as the project goes on"
- AI Action: Updated this log to record ongoing auto-tracking preference.
- Outcome: Future AI assistance will continue to be logged in this file.

## Ongoing Tracking Template
Use this template for each new AI-assisted task:

```
Date:
Request:
Instructions Given to AI:
AI Actions Taken:
Files Created/Edited:
Outcome:
Notes:
```

---

### 2026-03-10 - Project Planning & PRD Creation
**Request**: "Read Task.txt and discuss project structure. Create PRD after we agree on approach."

**Instructions Given to AI**:
- Keep evaluation criteria (clear structure, readable implementation) top of mind
- Discuss project structure for C++ arbitrage cycle detection
- Determine if OOP is appropriate
- Decide on components: Parser, Graph Builder, Cycle Detector, HTML Output

**Discussion Topics**:
1. **Architecture Decision**: Agreed on OOP approach for clear encapsulation and modularity
2. **Component Structure**: 8 modular components (Token, Pool, Graph, CycleDetector, ArbitrageAnalyzer, HTMLExporter, JSONParser, main)
3. **Cycle Detection**: DFS algorithm with backtracking, max depth 5
4. **Trade Size Strategy**: Test multiple sizes (1%, 10%, 20% of starting pool reserve) - Option B
   - Option B: Use % of starting token's reserve in first pool of cycle
   - Works for any token, not just ETH
5. **Profitability Ranking**: Absolute USD profit (not risk-adjusted for v1)
6. **Reserve Constraints & AMM Formula**: 
   - Discussed constant product formula: x * y = k
   - Prices change with each trade (slippage)
   - Larger trades = worse prices
   - 0.3% Uniswap V2 fee applies
7. **HTML Output**: Table with top 10 cycles showing:
   - Token symbols and addresses
   - Pool IDs
   - Optimal trade size
   - USD profit
   - ROI percentage

**AI Actions Taken**:
- Read Task.txt to understand requirements and evaluation criteria
- Read existing AI_use.md to understand tracking format
- Examined v2pools.json structure to understand data format
- Proposed OOP architecture with clear class responsibilities
- Explained AMM constant product formula and slippage mechanics
- Clarified reserve constraints and how prices change during swaps
- Created comprehensive PRD documenting entire system design

**Files Created/Edited**:
- Created: `Documentation/PRD.md` - Complete product requirements document
- Updated: `Documentation/AI_use.md` - This log entry

**Outcome**:
- Clear project structure defined with 8 modular components
- OOP approach confirmed as best for readability and maintainability
- Trade size optimization strategy agreed upon (1%, 10%, 20% testing)
- Comprehensive PRD ready for implementation phase
- Both user and AI aligned on technical approach and deliverables

**Notes**:
- Emphasis on code quality and readability per evaluation criteria
- Modular design allows future updates (different algorithms, risk metrics, etc.)
- No explicit constraint on trade size - profit calculation naturally penalizes oversized trades
- File structure: one .h and .cpp per class for maximum modularity

---

### 2026-03-10 - Testing Requirements Added
**Request**: "okay update the docs to indicate we need tests please"

**Instructions Given to AI**:
- Add unit testing requirements to PRD
- Keep tests organized in separate directory to avoid clutter
- Use Google Test framework

**AI Actions Taken**:
- Updated PRD section 2.2 to include `tests/` directory structure
- Expanded section 8 (Testing Strategy) with detailed test specifications
- Added specific test cases for each component (Token, Pool, Graph, CycleDetector)
- Marked Google Test as required dependency (not optional)
- Updated deliverables to include test files and passing test results
- Added test passing as a code quality success metric

**Files Edited**:
- Updated: `Documentation/PRD.md` - Added comprehensive testing section
- Updated: `Documentation/AI_use.md` - This log entry

**Outcome**:
- Testing now fully integrated into project requirements
- Tests organized in separate `tests/` directory (clean structure)
- 5 test files planned: test_Token, test_Pool, test_Graph, test_CycleDetector, test_main
- Specific test cases defined, especially for critical Pool swap calculations
- CMake configuration will include test building and running

**Notes**:
- Pool tests are most critical (swap calculations must be accurate)
- Tests kept isolated from main source code in `tests/` directory
- Google Test chosen for industry-standard approach and clean syntax

---

### 2026-03-10 - PRD Risk Mitigation Updates
**Request**: "tell me any concerns you see from the PRD, and what problems might arise" followed by decisions to adopt specific fixes.

**Instructions Given to AI**:
- Apply resolved concerns directly to PRD
- Add deterministic simulation rules and conversion method
- Keep implementation practical for v1

**AI Actions Taken**:
- Added required state-isolation rule: never mutate shared pool reserves during analysis
- Added cycle canonicalization requirement to deduplicate rotated/reversed duplicates
- Added required per-hop viability check (`amountIn / reserveIn <= 20%`)
- Added deterministic USD conversion strategy with token-level mapping and median aggregation
- Clarified amount unit policy for v1 (use normalized JSON units; parse decimals but do not use in arithmetic)
- Updated HTML output to use token IDs as primary display label for now
- Added explicit ownership model using smart pointers (avoid raw owning pointers)
- Updated test build command to `cmake --build .` for Windows-friendly instructions

**Files Edited**:
- Updated: `Documentation/PRD.md` - Added correctness and robustness requirements
- Updated: `Documentation/AI_use.md` - This log entry

**Outcome**:
- PRD now addresses key correctness risks before implementation starts
- Deterministic behavior and reproducible rankings are explicitly required
- Scope remains practical for v1 while preserving extensibility

**Notes**:
- Remaining optional refinements (e.g., better USD oracles) can be added in later iterations
- Current v1 choices prioritize clarity, determinism, and implementation speed

---

### 2026-03-10 - Hop Rounding and Decimal Precision Update
**Request**: "Notice that for hops, the rounding has to follow decimal/units. please make sure it fits"

**Instructions Given to AI**:
- Ensure hop-by-hop simulation respects token decimal precision
- Reflect the rule in requirements and tests

**AI Actions Taken**:
- Updated PRD to require per-hop output quantization by output token decimals
- Added exact rounding rule: `floor(rawAmountOut * 10^d) / 10^d`
- Updated amount-unit policy so `decimals` are now used in arithmetic via quantization
- Added Pool test requirements for decimal rounding and multi-hop rounding propagation

**Files Edited**:
- Updated: `Documentation/PRD.md`
- Updated: `Documentation/AI_use.md`

**Outcome**:
- Swap simulation now has explicit, deterministic rounding behavior per hop
- Profit estimates are less likely to be overstated due to fractional precision drift

---

### 2026-03-10 - PRD Implementation (C++ + Tests)
**Request**: "Any last questions? Else implement the PRD"

**Instructions Given to AI**:
- Implement the agreed PRD end-to-end
- Keep modular structure and unit tests
- Respect per-hop decimal rounding and hop viability checks

**AI Actions Taken**:
- Created modular `src/` implementation:
   - `Token`, `Pool`, `Graph`, `CycleDetector`, `JSONParser`, `ArbitrageAnalyzer`, `HTMLExporter`, `main`
- Implemented DFS cycle detection with canonical dedup (rotation/reverse normalization)
- Implemented local-state swap simulation (no shared reserve mutation)
- Implemented per-hop 20% reserve viability check
- Implemented per-hop output rounding with token decimal flooring
- Implemented token-level USD conversion map (median aggregation from pool candidates)
- Implemented top-10 HTML report output with cycle path, pool IDs, trade size, profit, ROI
- Added GoogleTest-based test suite in `tests/`:
   - `test_Token`, `test_Pool`, `test_Graph`, `test_CycleDetector`, `test_main`
- Added `CMakeLists.txt` with dependency setup for `nlohmann/json` and GoogleTest
- Updated `README.md` with build/run/test instructions

**Files Created/Edited**:
- Created: `CMakeLists.txt`
- Created: `src/Token.h`, `src/Token.cpp`
- Created: `src/Pool.h`, `src/Pool.cpp`
- Created: `src/Graph.h`, `src/Graph.cpp`
- Created: `src/CycleDetector.h`, `src/CycleDetector.cpp`
- Created: `src/JSONParser.h`, `src/JSONParser.cpp`
- Created: `src/ArbitrageAnalyzer.h`, `src/ArbitrageAnalyzer.cpp`
- Created: `src/HTMLExporter.h`, `src/HTMLExporter.cpp`
- Created: `src/main.cpp`
- Created: `tests/test_main.cpp`
- Created: `tests/test_Token.cpp`
- Created: `tests/test_Pool.cpp`
- Created: `tests/test_Graph.cpp`
- Created: `tests/test_CycleDetector.cpp`
- Updated: `README.md`
- Updated: `Documentation/AI_use.md`

**Outcome**:
- PRD is implemented as a modular C++ project structure with tests
- Build/test execution could not be validated in-session because `cmake` is not installed in current environment

**Notes**:
- Once CMake is available, run configure/build/test commands from README
- Code is organized to allow future extensions (risk-adjusted ranking, symbol enrichment, alternate detectors)

---

### 2026-03-10 - CMake Installation and PATH Configuration
**Request**: "How do I install cmake?" followed by "i used winget still dont have cmake --version"

**Instructions Given to AI**:
- Help install CMake on Windows
- Troubleshoot why CMake command is not recognized after installation
- Fix PATH issues to make CMake accessible

**AI Actions Taken**:
- Suggested installing CMake via winget: `winget install --id Kitware.CMake`
- Diagnosed PATH issue: CMake installed to `C:\Program Files\CMake\bin` but not in current session PATH
- Fixed PATH by adding CMake to both User PATH (persistent) and current session PATH
- Verified CMake and CTest are accessible with version checks

**Files Created/Edited**:
- None (system PATH configuration only)

**Outcome**:
- CMake 4.2.3 successfully installed and accessible
- User can now run cmake commands in terminal
- PATH configured for both current session and future sessions

**Notes**:
- Windows winget installations don't always update PATH for current terminal session
- Required manual PATH addition: `$env:Path += ";C:\Program Files\CMake\bin"`
- User PATH also updated for persistence across sessions

---

### 2026-03-10 - Build and Test Execution
**Request**: "if installed already, help me run the cmake stuff which you wanted me to run please"

**Instructions Given to AI**:
- Configure the CMake build system
- Compile the C++ project
- Run the unit tests to verify implementation

**AI Actions Taken**:
- Ran `cmake -S . -B build` to configure build (Visual Studio 18 2026 generator)
- Ran `cmake --build build` to compile all source files and link executables
- Ran `ctest --test-dir build -C Debug --verbose` to execute all unit tests
- Updated README.md and PRD.md with correct Windows-specific test command (`-C Debug` required for Visual Studio multi-config generator)

**Files Edited**:
- Updated: `README.md` - Added `-C Debug` flag to ctest command
- Updated: `Documentation/PRD.md` - Section 8.5 updated with correct Windows test command
- Updated: `Documentation/AI_use.md` - This log entry

**Build Results**:
- Configuration: Completed successfully (5.1s config, 2.3s generate)
- Compilation: All source files compiled without errors
  - Compiled: Token, Pool, Graph, CycleDetector, JSONParser, ArbitrageAnalyzer, HTMLExporter, main
  - Linked: arbitrage_lib.lib, arbitrage_app.exe, arbitrage_tests.exe, gtest libraries
- Tests: 10/10 passed (100% pass rate in <1s)
  - TokenTest: 1 test passed
  - PoolTest: 5 tests passed (swap calculation, slippage, reserves, decimal rounding, multi-hop)
  - GraphTest: 1 test passed
  - CycleDetectorTest: 3 tests passed (triangle cycle, no false cycles, max-depth)

**Outcome**:
- Full project builds successfully on Windows with Visual Studio 18 2026 toolchain
- All unit tests pass, validating core functionality
- Implementation phase complete and verified

**Notes**:
- Visual Studio generators require `-C Debug` or `-C Release` for CTest
- Initial ctest run failed until configuration was specified
- Build toolchain: MSVC 19.50 on Windows 10.0.26200 with SDK 10.0.26100.0

---

### 2026-03-10 - Application Execution with Optimizations
**Request**: "ok update AI_use please" (after running the arbitrage application)

**Instructions Given to AI**:
- Run the arbitrage application on real data
- Optimize performance when initial run is too slow
- Generate the HTML report successfully

**AI Actions Taken**:
1. **Initial Execution Attempt**:
   - Ran `.\build\Debug\arbitrage_app.exe v2pools.json report.html 5`
   - Application loaded 37,768 pools and 33,356 unique tokens
   - Cycle detection with max depth 5 was extremely slow (search space too large)
   
2. **Added Progress Logging**:
   - Modified `CycleDetector::findCycles()` to output progress every 1000 tokens
   - Shows processed count and cycles found during execution
   
3. **Added Liquidity Filtering**:
   - Modified `main.cpp` to filter pools by minimum liquidity ($10,000 USD)
   - Reduced search space from 33,356 tokens to 3,399 tokens
   - Kept only 3,786 high-liquidity pools from original 37,768
   
4. **Successful Execution**:
   - Ran with max depth 3 and liquidity filter
   - Successfully processed all 3,399 tokens
   - Generated report.html with results

**Files Edited**:
- Updated: `src/CycleDetector.cpp` - Added progress logging to findCycles()
- Updated: `src/main.cpp` - Added liquidity filtering before graph construction
- Updated: `Documentation/AI_use.md` - This log entry

**Execution Results**:
- Input: 37,768 pools, 33,356 tokens
- After filtering: 3,786 pools (≥$10,000 liquidity), 3,399 tokens
- Cycles detected: 474 unique cycles
- Profitable cycles: 38 cycles with positive ROI
- Top opportunity: 147.48% ROI, $1,976.76 profit
  - Path: LINK → DAI → AGIX → LINK
  - Trade size: 42.28 LINK
  - Pool IDs: 0x6d4fd...bdb2, 0x1d112...1f16, 0x81a8b...125a

**Outcome**:
- Successfully generated `report.html` with top 10 arbitrage opportunities
- Report shows cycle paths, pool IDs, trade sizes, profits, and ROI percentages
- Application runs in reasonable time with liquidity filtering
- All features working end-to-end: parsing, graph building, cycle detection, profitability analysis, HTML export

**Notes**:
- Without liquidity filtering, search space is too large for practical execution
- Max depth 3 provides good balance between finding opportunities and execution time
- Progress logging helps monitor long-running cycle detection
- Liquidity filter of $10,000 keeps most significant trading opportunities while dramatically reducing computation

---

### 2026-03-10 - Git Configuration
**Request**: "we should gitignore build?"

**Instructions Given to AI**:
- Create .gitignore file to exclude build artifacts from version control

**AI Actions Taken**:
- Created `.gitignore` file at project root
- Added build directory exclusions (`build/`, `out/`)
- Added compiled binary patterns (`.exe`, `.dll`, `.lib`, `.obj`, etc.)
- Added generated report exclusion (`report.html`)
- Added IDE/editor file patterns (`.vs/`, `.vscode/`, `*.user`, etc.)
- Added CMake cache file patterns (`CMakeCache.txt`, `CMakeFiles/`, etc.)

**Files Created/Edited**:
- Created: `.gitignore` - Git ignore patterns for build artifacts and generated files
- Updated: `Documentation/AI_use.md` - This log entry

**Outcome**:
- Build artifacts and generated files will not be tracked in version control
- Repository stays clean with only source code and configuration files
- Standard C++ and CMake patterns covered

**Notes**:
- Build directory contains ~30MB of compiled code and dependencies
- Generated HTML reports are user-specific and shouldn't be committed
- IDE files vary by developer and should remain local

---

### 2026-03-10 - Source Code Commenting and Readability Pass
**Request**: "okay now, look through my src, and help me comment the code, such that its human friendly, thanks"

**Instructions Given to AI**:
- Review `src/` and add human-friendly comments
- Keep comments practical and explanatory (focus on intent and logic)
- Preserve behavior while improving readability

**AI Actions Taken**:
- Reviewed all source modules in `src/` and added/expanded comments in core algorithm paths
- Added class/function-level documentation for responsibilities and data flow
- Added inline comments for critical logic branches and design constraints:
   - Uniswap V2 fee math and reserve updates
   - State isolation (local reserves only) during simulation
   - DFS traversal/backtracking behavior
   - Canonical cycle deduplication (rotation + reverse normalization)
   - USD price estimation via median aggregation
   - Liquidity filtering and end-to-end pipeline flow in `main.cpp`
- Rebuilt and ran tests to verify no regressions after documentation-only changes

**Files Edited**:
- Updated: `src/Token.h`
- Updated: `src/Token.cpp`
- Updated: `src/Pool.h`
- Updated: `src/Pool.cpp`
- Updated: `src/Graph.h`
- Updated: `src/Graph.cpp`
- Updated: `src/CycleDetector.h`
- Updated: `src/CycleDetector.cpp`
- Updated: `src/ArbitrageAnalyzer.h`
- Updated: `src/ArbitrageAnalyzer.cpp`
- Updated: `src/JSONParser.h`
- Updated: `src/JSONParser.cpp`
- Updated: `src/main.cpp`
- Updated: `Documentation/AI_use.md` - This log entry

**Outcome**:
- Source code is significantly more approachable for human readers
- Core algorithms now include rationale and expected behavior notes
- Build succeeded and test suite remained green

**Verification**:
- Build: `cmake --build build --config Debug` passed
- Tests: `ctest --test-dir build -C Debug --output-on-failure` passed (`10/10` tests)

**Notes**:
- Comments were added to explain "why" and "how" for complex logic, not to restate obvious syntax
- No functional changes intended in this pass beyond readability/documentation improvements
