#pragma once

#include <string>

/**
 * Config - Global configuration constants for the arbitrage analyzer.
 * 
 * Centralizes all magic numbers and configurable values for easy modification
 * without recompilation (in future: could be loaded from config file).
 * 
 * Design Principle: Single source of truth for all configuration values.
 */
namespace Config {

// ============================================================================
// ANALYSIS CONSTRAINTS
// ============================================================================

/** Minimum USD liquidity threshold for pool inclusion in analysis */
constexpr double MIN_RESERVE_USD = 10000.0;

/** Maximum number of swaps allowed per arbitrage cycle (DFS depth limit) */
constexpr int MAX_CYCLE_DEPTH = 3;

/** Maximum percentage of pool reserve that can be used in a single swap */
constexpr double MAX_INPUT_RESERVE_RATIO = 0.20;  // 20%

/** Uniswap V2 swap fee (0.3% as fraction) */
constexpr double UNISWAP_FEE_FRACTION = 0.997;    // 1 - 0.003

// ============================================================================
// TRADE SIZE OPTIMIZATION
// ============================================================================

/** Trade size fractions of starting pool reserve to test for optimization */
constexpr double TRADE_SIZE_FRACTION_1 = 0.01;   // 1%
constexpr double TRADE_SIZE_FRACTION_2 = 0.10;   // 10%
constexpr double TRADE_SIZE_FRACTION_3 = 0.20;   // 20%

// ============================================================================
// OUTPUT CONFIGURATION
// ============================================================================

/** Number of top opportunities to include in reports */
constexpr size_t REPORT_TOP_N = 10;

/** Default input file name */
const std::string DEFAULT_INPUT_FILE = "v2pools.json";

/** Default HTML output file name */
const std::string DEFAULT_HTML_OUTPUT = "report.html";

/** Default JSON export file name (for Part 2 validation) */
const std::string DEFAULT_JSON_OUTPUT = "part1_cycles_for_validation.json";

/** Placeholder validator address before Part 2 deployment */
const std::string PLACEHOLDER_VALIDATOR_ADDRESS = "0xYourValidatorAddress";

// ============================================================================
// LOGGING & REPORTING
// ============================================================================

/** Progress update interval (log every N tokens) */
constexpr int PROGRESS_UPDATE_INTERVAL = 1000;

/** Decimal precision for USD amounts in output */
constexpr int PRECISION_USD = 2;

/** Decimal precision for percentage returns in output */
constexpr int PRECISION_PERCENTAGE = 4;

/** Decimal precision for token amounts (e.g., 42.28 LINK) */
constexpr int PRECISION_TOKEN_AMOUNT = 8;

// ============================================================================
// VALIDATION & SAFETY
// ============================================================================

/** Minimum profit (in USD) to consider a cycle worth reporting */
constexpr double MIN_PROFIT_USD = 0.01;

/** Must have valid path length (at least 3 tokens for a triangle) */
constexpr int MIN_CYCLE_LENGTH = 3;

}  // namespace Config
