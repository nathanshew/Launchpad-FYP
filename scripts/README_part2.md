# Part 2 Script Usage

## Deploy validator

```powershell
npm run deploy:part2 -- --network sepolia
```

## Deploy Sepolia mock cycle fixtures

This deploys 3 `MockV2Pair` contracts and writes `part2_data/candidate_cycle.sepolia.mock.json`.

```powershell
npm run deploy:mock-cycle -- --network sepolia
```

## Validate candidate (call mode)

```powershell
npm run validate:part2 -- --network sepolia
```

By default, `validateCycle.ts` loads `part2_data/candidate_cycle.example.json`.
To use another file, set env var `CYCLE_FILE`:

```powershell
$env:CYCLE_FILE='part2_data/candidate_cycle.sepolia.mock.json'
npm run validate:part2 -- --network sepolia
Remove-Item Env:CYCLE_FILE
```

## Validate candidate (tx mode)

Set `"mode": "tx"` in candidate JSON, then run:

```powershell
npm run validate:part2 -- --network sepolia
```

Tx mode uses `validateCycleTx(...)` and prints tx hash + block number.
