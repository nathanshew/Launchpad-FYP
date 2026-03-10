# Part 2 Script Usage

## Deploy validator

```powershell
npm run deploy:part2 -- --network sepolia
```

## Validate candidate (call mode)

```powershell
npm run validate:part2 -- --network sepolia --file part2_data/candidate_cycle.example.json
```

## Validate candidate (tx mode)

Set `"mode": "tx"` in candidate JSON, then run:

```powershell
npm run validate:part2 -- --network sepolia --file part2_data/candidate_cycle.example.json
```

Tx mode uses `validateCycleTx(...)` and prints tx hash + block number.
