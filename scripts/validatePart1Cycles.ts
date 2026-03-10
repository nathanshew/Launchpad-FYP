import * as fs from "fs";
import * as path from "path";
import { ethers } from "hardhat";

type CandidateCycle = {
  rank?: number;
  validatorAddress: string;
  tokens: string[];
  pools: string[];
  amountIn: string;
  minOut: string;
  mode?: "call" | "tx";
  metadata?: {
    profitUsd: number;
    roi: number;
    optimalTradeSize: number;
  };
};

async function main() {
  const inputFile = process.env.CYCLES_FILE || "part1_cycles_for_validation.json";
  const absolutePath = path.isAbsolute(inputFile) ? inputFile : path.join(process.cwd(), inputFile);

  if (!fs.existsSync(absolutePath)) {
    throw new Error(`File not found: ${absolutePath}`);
  }

  const candidates: CandidateCycle[] = JSON.parse(fs.readFileSync(absolutePath, "utf8"));
  
  // Deploy validator on fork if testing against mainnetFork
  const network = await ethers.provider.getNetwork();
  let validatorAddress = candidates[0].validatorAddress;
  
  if (network.chainId === 31337n) {
    console.log("Detected local fork - deploying validator...");
    const Validator = await ethers.getContractFactory("ArbitrageValidator");
    const validator = await Validator.deploy();
    await validator.waitForDeployment();
    validatorAddress = await validator.getAddress();
    console.log("Validator deployed at:", validatorAddress);
    console.log("");
  }
  
  console.log(`Validating ${candidates.length} cycles from Part 1...`);
  console.log("");

  const results = [];

  for (const candidate of candidates) {
    const validator = await ethers.getContractAt("ArbitrageValidator", validatorAddress);
    const amountIn = BigInt(candidate.amountIn);
    const minOut = BigInt(candidate.minOut);

    try {
      const result = await validator.validateCycle.staticCall(
        candidate.tokens,
        candidate.pools,
        amountIn,
        minOut
      );

      const profit = result - amountIn;
      const profitEth = ethers.formatEther(profit);

      console.log(`✓ Rank #${candidate.rank || "?"}: STILL PROFITABLE`);
      if (candidate.metadata) {
        console.log(`  Part 1 profit: $${candidate.metadata.profitUsd.toFixed(2)} (${candidate.metadata.roi.toFixed(2)}% ROI)`);
      }
      console.log(`  On-chain profit: ${profitEth} ETH (${((Number(profit) * 100) / Number(amountIn)).toFixed(2)}% ROI)`);
      console.log(`  Path: ${candidate.tokens.slice(0, -1).map(t => t.slice(0, 6)).join(" → ")}...`);
      console.log("");

      results.push({
        rank: candidate.rank,
        status: "profitable",
        onChainProfit: profitEth
      });

    } catch (error: any) {
      console.log(`✗ Rank #${candidate.rank || "?"}: NOT PROFITABLE ON-CHAIN`);
      if (candidate.metadata) {
        console.log(`  Part 1 profit: $${candidate.metadata.profitUsd.toFixed(2)} (${candidate.metadata.roi.toFixed(2)}% ROI)`);
      }
      console.log(`  Reason: ${error.message || "execution reverted"}`);
      console.log(`  Path: ${candidate.tokens.slice(0, -1).map(t => t.slice(0, 6)).join(" → ")}...`);
      console.log("");

      results.push({
        rank: candidate.rank,
        status: "unprofitable",
        reason: error.message || "reverted"
      });
    }
  }

  const profitable = results.filter(r => r.status === "profitable").length;
  console.log("=====================================");
  console.log(`Summary: ${profitable}/${candidates.length} cycles still profitable on-chain`);
}

main().catch((error) => {
  console.error("Validation failed:", error.message ?? error);
  process.exitCode = 1;
});
