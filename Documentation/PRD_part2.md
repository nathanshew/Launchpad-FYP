# Product Requirements Document (PRD) - Part 2
## On-Chain Profitability Validation

Date: March 10, 2026  
Project: Launchpad FYP - DEX Arbitrage Challenge  
Scope: Part 2 (Optional) from `Documentation/Task.txt`

---

## 1. Decision: Separate or Merge

Recommendation: keep Part 2 as a separate PRD.

Rationale:
- Part 1 and Part 2 use different stacks (C++/CMake vs Solidity + script runtime).
- Separate PRDs reduce ambiguity in requirements, testing, and deliverables.
- Reviewers can quickly verify Part 2 compliance without scanning Part 1 internals.

Cross-reference policy:
- `Documentation/PRD.md` remains the source for Part 1.
- This file is the source for Part 2.
- Final report can summarize both and link each PRD.

---

## 2. Objective

Implement on-chain validation for candidate arbitrage cycles produced off-chain.

The validator must:
1. Fetch live reserves from specified pools.
2. Recompute swap outputs hop-by-hop using Uniswap V2 math.
3. Revert if route is invalid or no longer profitable.

---

## 3. Deliverables

Required deliverables for Part 2:
- Solidity function that accepts:
  - `tokens[]`
  - `pools[]`
  - `amountIn`
  - `minOut`
- Off-chain script that submits candidate cycles for validation.
- Documentation describing:
  - data encoding format,
  - validation logic,
  - revert conditions.

---

## 4. Technical Architecture

### 4.1 Components

- `contracts/ArbitrageValidator.sol`
  - Stateless view validation logic.
  - Reads pair reserves and recomputes cycle output.

- `scripts/validateCycle.ts` (or Python equivalent)
  - Loads candidate cycles from Part 1 output.
  - Encodes call parameters.
  - Calls validator and records pass/fail.

- `part2_test/ArbitrageValidator.t.sol` or `part2_test/ArbitrageValidator.ts`
  - Unit/integration tests on local fork or mocks.

### 4.2 Suggested Repository Layout

```text
contracts/
  ArbitrageValidator.sol
scripts/
  validateCycle.ts
part2_test/
  ArbitrageValidator.test.ts
Documentation/
  PRD.md
  PRD_part2.md
  part2_report.md
```

---

## 5. Contract Interface

### 5.1 Function Signature

```solidity
function validateCycle(
    address[] calldata tokens,
    address[] calldata pools,
    uint256 amountIn,
    uint256 minOut
) external view returns (uint256 amountOutFinal);
```

### 5.2 External Dependency

Use Uniswap V2 pair interface:

```solidity
interface IUniswapV2Pair {
    function token0() external view returns (address);
    function token1() external view returns (address);
    function getReserves() external view returns (uint112, uint112, uint32);
}
```

---

## 6. Data Encoding Format

Input payload rules:
- `tokens`: ordered route tokens, length `N + 1`.
- `pools`: ordered pair addresses per hop, length `N`.
- `amountIn`: raw integer units of `tokens[0]`.
- `minOut`: minimum acceptable final amount in raw units of `tokens[N]`.

Encoding transport:
- Use standard Solidity ABI calldata encoding for `validateCycle(tokens, pools, amountIn, minOut)`.
- Off-chain script must serialize arrays and integers exactly as ABI-encoded cycle data.

Closed-loop arbitrage convention:
- `tokens[0] == tokens[N]` for cycle validation.

Per-hop mapping:
- Hop `i` uses:
  - pool = `pools[i]`
  - input token = `tokens[i]`
  - output token = `tokens[i+1]`

---

## 7. Validation Logic

For each hop:
1. Read `token0`, `token1`, and reserves from the pool.
2. Verify the hop route matches pool composition.
3. Select `(reserveIn, reserveOut)` based on input token side.
4. Compute output with V2 fee formula:
   - `amountInWithFee = amountIn * 997`
   - `amountOut = (amountInWithFee * reserveOut) / (reserveIn * 1000 + amountInWithFee)`
5. Set `amountIn = amountOut` and continue to next hop.

After final hop:
- `amountOutFinal` is final output.
- Require `amountOutFinal >= minOut`.
- Require `amountOutFinal > initialAmountIn`.

---

## 8. Revert Conditions

The function must revert when any of the following is true:
- Invalid array lengths (`tokens` and `pools` mismatch).
- `amountIn == 0`.
- Pool does not contain expected token pair for a hop.
- Any required reserve is zero.
- Final output is below `minOut`.
- Final output is not strictly profitable (`<= initialAmountIn`).

Recommended error style:
- Use custom errors for lower gas and clearer debugging.

---

## 9. Script Requirements

The submission script must:
- Parse Part 1 candidate cycles (top opportunities).
- Convert human-readable size to raw units via token decimals.
- Construct and send `validateCycle(...)` call.
- Record result for each cycle:
  - success/failure,
  - final output,
  - realized profit,
  - revert reason when available.

Execution modes:
- `eth_call` simulation mode (default).
- Transaction mode on Ethereum testnet for on-chain proof/logging.

---

## 10. Testing Strategy

Minimum tests:
- Valid profitable 3-hop cycle returns final output.
- Non-profitable cycle reverts.
- `minOut` too high reverts.
- Route/pool mismatch reverts.
- Length mismatch reverts.
- Zero reserve edge case reverts.

Recommended environment:
- Local fork (Anvil/Hardhat) for realistic reserve reads.
- Deterministic fixtures for unit-level formula tests.

---

## 11. Non-Goals (Part 2)

Out of scope for this phase:
- Atomic execution of swaps.
- Flash loan orchestration.
- MEV protection and private ordering.
- Gas-optimized production bot architecture.

---

## 12. Acceptance Criteria

Part 2 is complete when:
- Contract function compiles and matches required signature.
- Validator correctly recomputes multi-hop outputs from live reserves.
- Revert conditions are implemented and tested.
- Script can submit at least one candidate and capture result.
- Documentation explains encoding, logic, and reverts.
- At least one validation is demonstrated on Ethereum testnet (success or expected revert) with recorded tx hash and block number.

---

## 13. Risks and Mitigations

- Live reserve drift vs Part 1 snapshot:
  - Mitigation: treat failures as expected; log timestamps and block numbers.

- Decimal conversion mistakes in script:
  - Mitigation: enforce raw-unit conversion utility and tests.

- Invalid candidate data ordering:
  - Mitigation: strict route/pool checks and explicit error handling.

---

## 14. Implementation Notes

Suggested starting sequence:
1. Implement and test Solidity validator with mocks.
2. Add fork-based test using real pair addresses.
3. Implement submission script.
4. Produce `Documentation/part2_report.md` with outcomes.
