// SPDX-License-Identifier: MIT
pragma solidity ^0.8.24;

interface IUniswapV2Pair {
    function token0() external view returns (address);
    function token1() external view returns (address);
    function getReserves() external view returns (uint112 reserve0, uint112 reserve1, uint32 blockTimestampLast);
}

contract ArbitrageValidator {
    error LengthMismatch();
    error EmptyPath();
    error ZeroAmountIn();
    error HopTokenMismatch(uint256 hopIndex, address pool, address tokenIn, address tokenOut);
    error ZeroReserve(uint256 hopIndex, address pool);
    error BelowMinOut(uint256 finalAmount, uint256 minOut);
    error NotProfitable(uint256 finalAmount, uint256 initialAmount);

    event CycleValidated(address indexed sender, uint256 amountIn, uint256 minOut, uint256 amountOutFinal);

    // Validates a multi-hop route against live pair reserves and reverts if unprofitable.
    function validateCycle(
        address[] calldata tokens,
        address[] calldata pools,
        uint256 amountIn,
        uint256 minOut
    ) external view returns (uint256 amountOutFinal) {
        return _validateCycle(tokens, pools, amountIn, minOut);
    }

    // Optional transaction path for testnet evidence (tx hash/block number).
    function validateCycleTx(
        address[] calldata tokens,
        address[] calldata pools,
        uint256 amountIn,
        uint256 minOut
    ) external returns (uint256 amountOutFinal) {
        amountOutFinal = _validateCycle(tokens, pools, amountIn, minOut);
        emit CycleValidated(msg.sender, amountIn, minOut, amountOutFinal);
    }

    function _validateCycle(
        address[] calldata tokens,
        address[] calldata pools,
        uint256 amountIn,
        uint256 minOut
    ) internal view returns (uint256 amountOutFinal) {
        if (tokens.length < 2) revert EmptyPath();
        if (pools.length != tokens.length - 1) revert LengthMismatch();
        if (amountIn == 0) revert ZeroAmountIn();

        uint256 initialAmount = amountIn;

        for (uint256 i = 0; i < pools.length; i++) {
            IUniswapV2Pair pair = IUniswapV2Pair(pools[i]);
            address tokenIn = tokens[i];
            address tokenOut = tokens[i + 1];

            address pairToken0 = pair.token0();
            address pairToken1 = pair.token1();
            bool direct = tokenIn == pairToken0 && tokenOut == pairToken1;
            bool reverse = tokenIn == pairToken1 && tokenOut == pairToken0;
            if (!direct && !reverse) {
                revert HopTokenMismatch(i, pools[i], tokenIn, tokenOut);
            }

            (uint112 reserve0, uint112 reserve1,) = pair.getReserves();
            uint256 reserveIn = direct ? uint256(reserve0) : uint256(reserve1);
            uint256 reserveOut = direct ? uint256(reserve1) : uint256(reserve0);

            if (reserveIn == 0 || reserveOut == 0) {
                revert ZeroReserve(i, pools[i]);
            }

            amountIn = _getAmountOut(amountIn, reserveIn, reserveOut);
        }

        amountOutFinal = amountIn;

        if (amountOutFinal < minOut) {
            revert BelowMinOut(amountOutFinal, minOut);
        }
        if (amountOutFinal <= initialAmount) {
            revert NotProfitable(amountOutFinal, initialAmount);
        }
    }

    function _getAmountOut(uint256 amountIn, uint256 reserveIn, uint256 reserveOut) internal pure returns (uint256) {
        uint256 amountInWithFee = amountIn * 997;
        uint256 numerator = amountInWithFee * reserveOut;
        uint256 denominator = (reserveIn * 1000) + amountInWithFee;
        return numerator / denominator;
    }
}
