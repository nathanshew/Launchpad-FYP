import * as fs from "fs";
import * as path from "path";
import { ethers, network } from "hardhat";

type CandidateCycle = {
  validatorAddress: string;
  tokens: string[];
  pools: string[];
  amountIn: string;
  minOut: string;
  mode: "call" | "tx";
};

async function main() {
  const validatorAddress = process.env.VALIDATOR_ADDRESS;
  if (!validatorAddress) {
    throw new Error("Missing VALIDATOR_ADDRESS in .env");
  }

  const [deployer] = await ethers.getSigners();
  console.log("Network:", network.name);
  console.log("Deployer:", deployer.address);

  const reserveIn = ethers.parseUnits("1000", 18);
  const reserveOut = ethers.parseUnits("1300", 18);

  // Fixed pseudo token addresses are enough because validator only compares addresses.
  const tokenA = "0x00000000000000000000000000000000000000A1";
  const tokenB = "0x00000000000000000000000000000000000000B2";
  const tokenC = "0x00000000000000000000000000000000000000C3";

  const Pair = await ethers.getContractFactory("MockV2Pair");

  // Each hop is configured to have reserveOut/reserveIn = 1.3 in traversal direction.
  const pairAB = await Pair.deploy(tokenA, tokenB, reserveIn, reserveOut);
  await pairAB.waitForDeployment();

  const pairBC = await Pair.deploy(tokenB, tokenC, reserveIn, reserveOut);
  await pairBC.waitForDeployment();

  const pairCA = await Pair.deploy(tokenC, tokenA, reserveIn, reserveOut);
  await pairCA.waitForDeployment();

  const pools = [
    await pairAB.getAddress(),
    await pairBC.getAddress(),
    await pairCA.getAddress()
  ];

  const amountIn = ethers.parseUnits("1", 18);
  const validator = await ethers.getContractAt("ArbitrageValidator", validatorAddress);
  const simulatedOut = await validator.validateCycle.staticCall(
    [tokenA, tokenB, tokenC, tokenA],
    pools,
    amountIn,
    0n
  );

  const candidate: CandidateCycle = {
    validatorAddress,
    tokens: [tokenA, tokenB, tokenC, tokenA],
    pools,
    amountIn: amountIn.toString(),
    minOut: (amountIn + 1n).toString(),
    mode: "call"
  };

  const outPath = path.join(process.cwd(), "part2_data", "candidate_cycle.sepolia.mock.json");
  fs.writeFileSync(outPath, `${JSON.stringify(candidate, null, 2)}\n`, "utf8");

  console.log("Mock pair AB:", pools[0]);
  console.log("Mock pair BC:", pools[1]);
  console.log("Mock pair CA:", pools[2]);
  console.log("Simulated final amount out:", simulatedOut.toString());
  console.log("Wrote candidate file:", outPath);
}

main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
