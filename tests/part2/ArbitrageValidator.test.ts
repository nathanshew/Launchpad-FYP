import { expect } from "chai";
import { ethers } from "hardhat";

describe("ArbitrageValidator", function () {
  async function deployFixture() {
    const [deployer] = await ethers.getSigners();

    const Validator = await ethers.getContractFactory("ArbitrageValidator");
    const validator = await Validator.deploy();
    await validator.waitForDeployment();

    const tokenA = deployer.address;
    const tokenB = "0x00000000000000000000000000000000000000B0";
    const tokenC = "0x00000000000000000000000000000000000000C0";

    const Pair = await ethers.getContractFactory("MockV2Pair");

    // A -> B gives ~2x output direction
    const pairAB = await Pair.deploy(tokenA, tokenB, 1_000_000n, 2_000_000n);
    await pairAB.waitForDeployment();

    // B -> C gives ~2x output direction
    const pairBC = await Pair.deploy(tokenB, tokenC, 1_000_000n, 2_000_000n);
    await pairBC.waitForDeployment();

    // C -> A gives ~2x output direction
    const pairCA = await Pair.deploy(tokenC, tokenA, 1_000_000n, 2_000_000n);
    await pairCA.waitForDeployment();

    return { validator, tokenA, tokenB, tokenC, pairAB, pairBC, pairCA };
  }

  it("returns final amount for profitable cycle", async function () {
    const { validator, tokenA, tokenB, tokenC, pairAB, pairBC, pairCA } = await deployFixture();

    const tokens = [tokenA, tokenB, tokenC, tokenA];
    const pools = [
      await pairAB.getAddress(),
      await pairBC.getAddress(),
      await pairCA.getAddress()
    ];

    const amountIn = 1_000n;
    const minOut = 1_001n;

    const out = await validator.validateCycle(tokens, pools, amountIn, minOut);
    expect(out).to.be.greaterThan(amountIn);
  });

  it("reverts when not profitable", async function () {
    const { validator, tokenA, tokenB, tokenC, pairAB, pairBC, pairCA } = await deployFixture();

    await pairCA.setReserves(4_000_000, 1_000_000);

    const tokens = [tokenA, tokenB, tokenC, tokenA];
    const pools = [
      await pairAB.getAddress(),
      await pairBC.getAddress(),
      await pairCA.getAddress()
    ];

    const amountIn = 1_000n;
    await expect(validator.validateCycle(tokens, pools, amountIn, 0)).to.be.revertedWithCustomError(
      validator,
      "NotProfitable"
    );
  });

  it("reverts when below minOut", async function () {
    const { validator, tokenA, tokenB, tokenC, pairAB, pairBC, pairCA } = await deployFixture();

    const tokens = [tokenA, tokenB, tokenC, tokenA];
    const pools = [
      await pairAB.getAddress(),
      await pairBC.getAddress(),
      await pairCA.getAddress()
    ];

    const amountIn = 1_000n;
    const minOut = 100_000_000n;
    await expect(validator.validateCycle(tokens, pools, amountIn, minOut)).to.be.revertedWithCustomError(
      validator,
      "BelowMinOut"
    );
  });

  it("reverts on route/pool mismatch", async function () {
    const { validator, tokenA, tokenB, tokenC, pairAB, pairBC, pairCA } = await deployFixture();

    const tokens = [tokenA, tokenC, tokenB, tokenA];
    const pools = [
      await pairAB.getAddress(),
      await pairBC.getAddress(),
      await pairCA.getAddress()
    ];

    await expect(validator.validateCycle(tokens, pools, 1_000n, 0)).to.be.revertedWithCustomError(
      validator,
      "HopTokenMismatch"
    );
  });

  it("reverts on length mismatch", async function () {
    const { validator, tokenA, tokenB, tokenC, pairAB, pairBC } = await deployFixture();

    const tokens = [tokenA, tokenB, tokenC, tokenA];
    const pools = [await pairAB.getAddress(), await pairBC.getAddress()];

    await expect(validator.validateCycle(tokens, pools, 1_000n, 0)).to.be.revertedWithCustomError(
      validator,
      "LengthMismatch"
    );
  });

  it("reverts on zero reserve", async function () {
    const { validator, tokenA, tokenB, tokenC, pairAB, pairBC, pairCA } = await deployFixture();

    await pairBC.setReserves(0, 2_000_000);

    const tokens = [tokenA, tokenB, tokenC, tokenA];
    const pools = [
      await pairAB.getAddress(),
      await pairBC.getAddress(),
      await pairCA.getAddress()
    ];

    await expect(validator.validateCycle(tokens, pools, 1_000n, 0)).to.be.revertedWithCustomError(
      validator,
      "ZeroReserve"
    );
  });
});
