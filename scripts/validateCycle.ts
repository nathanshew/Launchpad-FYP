import * as fs from "fs";
import * as path from "path";
import { ethers, network } from "hardhat";

type CandidateCycle = {
  validatorAddress: string;
  tokens: string[];
  pools: string[];
  amountIn: string;
  minOut: string;
  mode?: "call" | "tx";
};

function parseArgs(): { filePath: string } {
  const args = process.argv.slice(2);
  const fileFlagIndex = args.findIndex((x) => x === "--file");
  if (fileFlagIndex === -1 || !args[fileFlagIndex + 1]) {
    throw new Error("Usage: npx hardhat run scripts/validateCycle.ts --network <name> --file part2_data/candidate_cycle.example.json");
  }

  return { filePath: args[fileFlagIndex + 1] };
}

async function main() {
  const { filePath } = parseArgs();
  const absolutePath = path.isAbsolute(filePath) ? filePath : path.join(process.cwd(), filePath);

  const payload = JSON.parse(fs.readFileSync(absolutePath, "utf8")) as CandidateCycle;

  const validator = await ethers.getContractAt("ArbitrageValidator", payload.validatorAddress);
  const amountIn = BigInt(payload.amountIn);
  const minOut = BigInt(payload.minOut);
  const mode = payload.mode ?? "call";

  console.log("Network:", network.name);
  console.log("Validator:", payload.validatorAddress);

  if (mode === "tx") {
    const tx = await validator.validateCycleTx(payload.tokens, payload.pools, amountIn, minOut);
    const receipt = await tx.wait();
    console.log("Validation tx hash:", tx.hash);
    console.log("Validation block number:", receipt?.blockNumber ?? "unknown");
    return;
  }

  // Static call mode: no chain state write, but full revert/success behavior is checked.
  const result = await validator.validateCycle.staticCall(payload.tokens, payload.pools, amountIn, minOut);
  console.log("Validation success (eth_call)");
  console.log("Final amount out:", result.toString());
  console.log("Profit (raw units):", (result - amountIn).toString());
}

main().catch((err) => {
  console.error("Validation failed:", err.message ?? err);
  process.exitCode = 1;
});
