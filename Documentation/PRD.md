# Product Requirements Document (PRD)
## DEX Arbitrage Cycle Detection System

**Date:** March 10, 2026  
**Project:** Launchpad FYP - DEX Arbitrage Challenge  
**Language:** C++  
**Target:** Part 1 - Off-Chain Cycle Detection

---

## 1. Project Overview

### 1.1 Objective
Build a C++ application that analyzes Uniswap V2 pool data to detect and rank profitable arbitrage cycles. The system must demonstrate clear structure, readable implementation, and modular design for future extensibility.

### 1.2 Key Requirements
- Load and parse JSON pool data from `v2pools.json`
- Construct a directed token swap graph
- Detect arbitrage cycles using depth-first search (DFS)
- Simulate swap outputs using the AMM constant product formula
- Optimize trade sizes to maximize profit accounting for slippage
- Rank opportunities by absolute USD profit
- Generate user-friendly HTML report with top 10 cycles

### 1.3 Evaluation Criteria (Priority Order)
1. **Code Quality**: Clear structure and readable implementation
2. **Correctness**: Accurate cycle detection and profit calculation
3. **Strategy**: Reasonable detection and ranking logic
4. **Documentation**: Well-commented code and clear explanations

---

## 2. System Architecture

### 2.1 Design Philosophy
- **Object-Oriented Programming**: Encapsulate components with clear responsibilities
- **Modularity**: Each component can be updated independently
- **Readability**: Self-documenting code with meaningful names and comments
- **Testability**: Components can be unit tested in isolation

### 2.2 Component Structure

```
src/
├── Token.h / Token.cpp
│   └── Represents a token (address, decimals, symbol)
│
├── Pool.h / Pool.cpp
│   ├── Represents a Uniswap V2 pool
│   ├── Stores reserves and token references
│   └── Calculates swap output using AMM formula
│
├── Graph.h / Graph.cpp
│   ├── Adjacency list representation
│   ├── Builds graph from pool data
│   └── Provides neighbor lookup for traversal
│
├── CycleDetector.h / CycleDetector.cpp
│   ├── DFS-based cycle detection
│   ├── Tracks visited nodes and path
│   └── Returns all detected cycles
│
├── ArbitrageAnalyzer.h / ArbitrageAnalyzer.cpp
│   ├── Simulates full cycle execution
│   ├── Tests multiple trade sizes (1%, 10%, 20%)
│   ├── Calculates profit in USD
│   └── Ranks cycles by profitability
│
├── HTMLExporter.h / HTMLExporter.cpp
│   └── Generates user-friendly HTML report
│
├── JSONParser.h / JSONParser.cpp
│   └── Parses v2pools.json into C++ objects
│
└── main.cpp
  └── Entry point and workflow orchestration

tests/
├── test_Token.cpp
│   └── Token class unit tests
│
├── test_Pool.cpp
│   └── Pool swap calculation tests
│
├── test_Graph.cpp
│   └── Graph construction tests
│
├── test_CycleDetector.cpp
│   └── Cycle detection algorithm tests
│
└── test_main.cpp
  └── Google Test main entry point
```

---

## 3. Detailed Component Specifications

### 3.1 Token Class
**Responsibility**: Store token metadata

**Data Members**:
- `std::string address` - Token contract address
- `int decimals` - Token decimal places
- `std::string symbol` - Token symbol (optional, for readability)

**Methods**:
- Constructor
- Getters for all fields

### 3.2 Pool Class
**Responsibility**: Represent a liquidity pool and calculate swap outputs

**Data Members**:
- `std::string poolId` - Pool contract address
- `Token* token0` - Reference to first token
- `Token* token1` - Reference to second token
- `double reserve0` - Reserve amount of token0
- `double reserve1` - Reserve amount of token1
- `double reserveUSD` - Total pool value in USD

**Methods**:
- `double calculateSwapOutput(double amountIn, bool isToken0Input)`
  - Implements: `amountOut = (reserveOut * amountIn * 997) / (reserveIn * 1000 + amountIn * 997)`
  - Accounts for 0.3% fee (997/1000)
  - Updates reserves for simulation purposes
- `double getReserve(Token* token)` - Get reserve for specific token
- Getters for metadata

**Formula Notes**:
- Uses Uniswap V2 constant product formula: `x * y = k`
- Fee is taken from input amount before swap calculation
- Price impact increases with trade size (slippage)

### 3.3 Graph Class
**Responsibility**: Build and maintain token swap graph

**Data Structure**:
- `std::map<std::string, std::vector<Pool*>> adjacencyList`
  - Key: Token address
  - Value: Vector of pools where this token can be swapped

**Methods**:
- `void addPool(Pool* pool)` - Add pool to graph (creates bidirectional edges)
- `std::vector<Pool*> getPoolsForToken(std::string tokenAddress)` - Get all pools containing a token
- `std::vector<std::string> getAllTokens()` - Get all token addresses in graph

### 3.4 CycleDetector Class
**Responsibility**: Find all arbitrage cycles in the graph

**Algorithm**: Depth-First Search (DFS) with backtracking

**Methods**:
- `std::vector<Cycle> findCycles(Graph& graph, int maxDepth = 5)`
  - Starts from each token as potential cycle start
  - Uses DFS to explore paths
  - Detects when path returns to starting token
  - Limits depth to avoid excessive computation
  
**Cycle Structure**:
```cpp
struct Cycle {
    std::vector<std::string> tokens;  // Token addresses in order
    std::vector<Pool*> pools;          // Pools used for each swap
};
```

**DFS Logic**:
- Track current path and visited pools
- When current token == start token and depth > 2: cycle found
- Backtrack to explore other paths
- Avoid revisiting same pool in one path

### 3.5 ArbitrageAnalyzer Class
**Responsibility**: Evaluate profitability of cycles

**Methods**:
- `std::vector<ArbitrageOpportunity> analyzeCycles(std::vector<Cycle> cycles)`
  - For each cycle:
    - Test trade sizes: 1%, 10%, 20% of first pool's starting token reserve
    - Simulate full cycle for each size
    - Calculate profit in USD
    - Select optimal trade size
  - Return ranked opportunities

**ArbitrageOpportunity Structure**:
```cpp
struct ArbitrageOpportunity {
    Cycle cycle;
    double optimalTradeSize;      // In starting token units
    double profitUSD;             // Absolute profit in USD
    double percentageReturn;      // ROI percentage
    std::vector<double> amounts;  // Amount at each step
};
```

**Profit Calculation**:
1. Start with trade amount in starting token
2. For each hop in cycle:
   - Calculate output using Pool's AMM formula
   - Update pool reserves (for simulation only)
3. Compare final amount to initial amount
4. Convert profit to USD using reserveUSD data
5. Sort by `profitUSD` descending

**Trade Size Strategy (Option B)**:
- First pool in cycle determines starting token
- Calculate percentages of that token's reserve in first pool
- Example: If cycle starts with ETH and first pool has 100 ETH reserve:
  - Test 1 ETH (1%), 10 ETH (10%), 20 ETH (20%)

### 3.6 HTMLExporter Class
**Responsibility**: Generate human-readable output

**Methods**:
- `void exportReport(std::vector<ArbitrageOpportunity> opportunities, std::string filename)`

**Output Format**:
- HTML table with top 10 opportunities
- Columns:
  - Rank (#1-10)
  - Cycle Path (Token symbols with arrows: ETH → USDC → DAI → ETH)
  - Token Addresses (full addresses)
  - Pool IDs (for each hop)
  - Optimal Trade Size (in starting token)
  - Profit (USD)
  - ROI (%)
- Styled for readability (CSS)
- Include summary statistics (total cycles found, profitable cycles, etc.)

### 3.7 JSONParser Class
**Responsibility**: Parse v2pools.json into C++ objects

**Methods**:
- `std::vector<Pool*> parsePoolsFromFile(std::string filename)`

**Implementation Notes**:
- Use a C++ JSON library (e.g., nlohmann/json, RapidJSON)
- Handle missing or malformed data gracefully
- Convert string reserves to doubles
- Create Token objects and deduplicate (same token used in multiple pools)

**JSON Structure to Parse**:
```json
{
  "id": "0x...",
  "reserve0": "100.5",
  "reserve1": "200000.0",
  "reserveUSD": "400000.0",
  "token0": {
    "id": "0x...",
    "decimals": "18"
  },
  "token1": {
    "id": "0x...",
    "decimals": "18"
  }
}
```

---

## 4. Algorithm Details

### 4.1 Uniswap V2 Constant Product Formula

**Core Principle**: `x * y = k` (constant)

**Swap Calculation** (with 0.3% fee):
```
Given:
- reserveIn = current reserve of input token
- reserveOut = current reserve of output token
- amountIn = amount being swapped
- fee = 0.3% = 0.003

Formula:
amountInWithFee = amountIn * 997
numerator = amountInWithFee * reserveOut
denominator = reserveIn * 1000 + amountInWithFee
amountOut = numerator / denominator
```

**Key Properties**:
- Larger trades get worse prices (price impact/slippage)
- Cannot drain pool completely (asymptotic)
- Each swap changes the price for subsequent swaps

### 4.2 Cycle Profitability

A cycle is profitable if:
```
finalAmount > initialAmount
```

**Calculation Steps**:
1. Initialize: `currentAmount = startingTradeSize`
2. For each pool in cycle:
   - Determine which token is input/output
   - Calculate `currentAmount = pool.calculateSwapOutput(currentAmount, direction)`
3. Compare `currentAmount` (final) to `startingTradeSize` (initial)
4. `profit = (currentAmount - startingTradeSize) * priceUSD`

### 4.3 Reserve Constraints

**Important**: While reserves define theoretical maximum, practical limits are much lower due to slippage.

- Swapping >20% of pool reserves typically causes >15-20% slippage
- Most profitable arbitrage uses 1-10% of pool reserves
- Our 20% upper limit tests the boundary but often won't be optimal

**No need for explicit constraints** - the profit calculation naturally penalizes oversized trades.

---

## 5. Workflow

### 5.1 Main Execution Flow
```
1. Parse v2pools.json → Load pools and tokens
2. Build graph → Create adjacency list
3. Detect cycles → Run DFS from each token
4. Analyze cycles → Test trade sizes, calculate profits
5. Rank by USD profit → Sort descending
6. Export top 10 → Generate HTML report
```

### 5.2 Expected Output

**Console**:
```
Loaded 50,000 pools
Built graph with 10,000 tokens
Found 1,234 cycles
Analyzing profitability...
Top 10 profitable cycles written to report.html
```

**HTML Report**: Table with top 10 cycles, all relevant details

---

## 6. Implementation Guidelines

### 6.1 Code Quality Standards
- **Naming**: Use clear, descriptive names (e.g., `calculateSwapOutput` not `calc`)
- **Comments**: Explain WHY, not what (code shows what)
- **Error Handling**: Graceful handling of edge cases (empty cycles, invalid data)
- **Memory Management**: Use smart pointers or proper cleanup
- **Const Correctness**: Use `const` where appropriate

### 6.2 Modularity Checklist
- Each class has single responsibility
- Minimal coupling between components
- Easy to swap implementations (e.g., different cycle detectors)
- Each component can be tested independently

### 6.3 Future Extensibility
Design should readily support:
- Different cycle detection algorithms (Johnson's, Bellman-Ford)
- Multiple DEX protocols (Uniswap V3, Sushiswap)
- Additional ranking metrics (risk-adjusted, ROI)
- Different output formats (JSON, CSV)

---

## 7. Third-Party Dependencies

**Required**:
- JSON parsing library (e.g., `nlohmann/json`)
- Testing framework (Google Test / gtest)
- Build system (CMake)

---

## 8. Testing Strategy

### 8.1 Testing Framework
**Google Test (gtest)** - Industry-standard C++ testing framework

**Setup**:
- Install via package manager or include as CMake dependency
- Link test executables against gtest libraries
- Run tests with `ctest` or direct executable

**Test Organization**:
- Each component has its own test file in `tests/` directory
- Test files include corresponding headers from `src/`
- Tests are isolated and can run independently

### 8.2 Unit Tests

#### 8.2.1 Token Tests (`test_Token.cpp`)
```cpp
TEST(TokenTest, ConstructorAndGetters) {
  // Verify token creation and getter methods
}
```

#### 8.2.2 Pool Tests (`test_Pool.cpp`) - **CRITICAL**
```cpp
TEST(PoolTest, SwapCalculationSmallAmount) {
  // Test: 1 ETH swap in 100 ETH / 200K USDC pool
  // Expected: ~1,980 USDC (with 0.3% fee)
}

TEST(PoolTest, SwapCalculationLargeAmount) {
  // Test: 20 ETH swap (20% of pool)
  // Verify significant slippage
}

TEST(PoolTest, FeeCalculation) {
  // Verify 0.3% fee is correctly applied
}

TEST(PoolTest, ReserveUpdate) {
  // Verify reserves update correctly after swap simulation
}
```

#### 8.2.3 Graph Tests (`test_Graph.cpp`)
```cpp
TEST(GraphTest, AddPoolCreatesEdges) {
  // Verify pool creates bidirectional edges
}

TEST(GraphTest, GetPoolsForToken) {
  // Verify correct pools returned for token
}
```

#### 8.2.4 CycleDetector Tests (`test_CycleDetector.cpp`)
```cpp
TEST(CycleDetectorTest, SimpleTriangleCycle) {
  // Create simple A→B→C→A cycle
  // Verify it's detected
}

TEST(CycleDetectorTest, NoFalseCycles) {
  // Graph with no cycles shouldn't return any
}

TEST(CycleDetectorTest, MaxDepthRespected) {
  // Verify cycles beyond maxDepth aren't returned
}
```

### 8.3 Integration Tests
- Full pipeline on small sample dataset (10-20 pools)
- Verify top cycle matches manual calculation
- Test complete workflow from JSON parsing to HTML output

### 8.4 Edge Cases
- Pools with very low liquidity
- Cycles with >5 hops
- Identical forward/backward paths
- Malformed JSON data
- Pools with zero reserves

### 8.5 Running Tests
```bash
# Build tests
mkdir build && cd build
cmake ..
make

# Run all tests
ctest --verbose

# Or run test executable directly
./tests/arbitrage_tests
```

---

## 9. Deliverables

### 9.1 Code
- All source files as specified in section 2.2
  - `src/` directory: 8 classes (16 files) + main.cpp
  - `tests/` directory: 5 test files
- CMakeLists.txt with test configuration
- README.md with build/run instructions

### 9.2 Output
- `report.html` with top 10 arbitrage opportunities
- Test results showing all tests pass

### 9.3 Documentation
- Inline comments in code
- This PRD
- AI_use.md tracking AI assistance

---

## 10. Success Metrics

### 10.1 Correctness
- ✅ Cycles correctly form loops (start == end token)
- ✅ Profit calculations use accurate AMM formula
- ✅ Top cycles are actually profitable (final > initial)

### 10.2 Code Quality
- ✅ Clear class separation and responsibilities
- ✅ Readable code with minimal cognitive complexity
- ✅ Appropriate comments and documentation
- ✅ All unit tests pass

### 10.3 Functionality
- ✅ Processes full v2pools.json dataset
- ✅ Finds at least 10 profitable cycles
- ✅ HTML report is readable and informative
- ✅ Optimal trade sizes are reasonable (within pool constraints)

---

## Appendix A: Technical Considerations

### A.1 Performance
- Graph construction: O(P) where P = number of pools
- Cycle detection: Exponential in depth, limited to 5 hops
- Expected runtime: <5 minutes on modern hardware

### A.2 Precision
- Use `double` for amounts (sufficient for demo)
- Production systems would use arbitrary precision libraries

### A.3 Pool Data Assumptions
- All pools use Uniswap V2 formula
- 0.3% fee applies to all pools
- Reserve data is accurate at snapshot time
- No token tax or special transfer logic

---

**Last Updated:** March 10, 2026
