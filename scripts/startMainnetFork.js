require("dotenv").config();

const { spawn } = require("child_process");

const forkUrl = process.env.MAINNET_RPC_URL || process.env.ALCHEMY_MAINNET_RPC_URL;

if (!forkUrl) {
  console.error(
    "Missing MAINNET_RPC_URL (or ALCHEMY_MAINNET_RPC_URL). Add it to .env before running npm run fork:mainnet."
  );
  process.exit(1);
}

const child = spawn("npx", ["hardhat", "node", "--fork", forkUrl], {
  stdio: "inherit",
  shell: true,
  env: process.env,
});

child.on("exit", (code, signal) => {
  if (signal) {
    process.kill(process.pid, signal);
    return;
  }
  process.exit(code ?? 0);
});
