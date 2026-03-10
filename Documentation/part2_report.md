# Part 2 Report - On-Chain Profitability Validation

Date: March 10, 2026

## 1. Data Encoding Format

Cycle data is ABI-encoded as:
- `tokens[]`: ordered route token addresses, length `N + 1`
- `pools[]`: ordered Uniswap V2 pair addresses, length `N`
- `amountIn`: raw integer amount of `tokens[0]`
- `minOut`: minimum acceptable final output in raw integer units

Per-hop mapping:
- hop `i` uses `pools[i]`, `tokens[i]` as input, and `tokens[i+1]` as output.

## 2. Validation Logic

Contract: `contracts/ArbitrageValidator.sol`

Validation flow:
1. Verify array lengths and non-zero input amount.
2. For each hop, read pair `token0`, `token1`, and reserves from `getReserves()`.
3. Confirm hop token ordering is valid for the given pool.
4. Compute `amountOut` using Uniswap V2 formula with 0.3% fee.
5. Feed hop output as next hop input.
6. Require final output to be at least `minOut` and strictly greater than initial input.

## 3. Revert Conditions

The validator reverts for:
- `LengthMismatch`
- `EmptyPath`
- `ZeroAmountIn`
- `HopTokenMismatch`
- `ZeroReserve`
- `BelowMinOut`
- `NotProfitable`

## 4. Execution Evidence

### 4.1 Local Test Result
- Status: Completed
- Command: `npm run test:part2`
- Result: `6/6` tests passing
- Notes: Covered profitable path and all core revert conditions (length mismatch, route mismatch, zero reserve, below minOut, not profitable)

### 4.2 Testnet Validation Result
- Network: Sepolia
- Validator address: `0xd3f576662CDa58684ee438c07E7b314A7Ef67288`
- Candidate file: `part2_data/candidate_cycle.sepolia.mock.json`
- Mode: `call` (eth_call validation, no transaction)
- Final amount out: `2168689472068604637` (raw units)
- Profit: `1168689472068604637` (raw units)
- Outcome: Validation success

## 5. Limitations and Notes

- Live reserves can drift significantly from Part 1 snapshot data.
- Many cycles from Part 1 may fail on-chain by the time of validation.
- This module validates profitability only; it does not execute atomic swaps.
