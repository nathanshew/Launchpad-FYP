import { ethers } from "hardhat";

async function main() {
  const Validator = await ethers.getContractFactory("ArbitrageValidator");
  const validator = await Validator.deploy();
  await validator.waitForDeployment();

  console.log("ArbitrageValidator deployed:", await validator.getAddress());
}

main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
