import { ethers } from "hardhat";

async function main() {
  const [deployer] = await ethers.getSigners();
  const balance = await ethers.provider.getBalance(deployer.address);

  console.log("Deployer:", deployer.address);
  console.log("Sepolia balance (ETH):", ethers.formatEther(balance));

  // Deploying with zero balance always fails with provider "insufficient funds".
  if (balance === 0n) {
    throw new Error(
      `Deployer ${deployer.address} has 0 ETH on the selected network. Fund this address with Sepolia ETH, then retry.`
    );
  }

  const Validator = await ethers.getContractFactory("ArbitrageValidator");
  const validator = await Validator.deploy();
  await validator.waitForDeployment();

  console.log("ArbitrageValidator deployed:", await validator.getAddress());
}

main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
