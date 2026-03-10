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
