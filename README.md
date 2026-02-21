# Antsolver
This is a heads-up no-limit poker AI built from advanced game theory research. The goal for this project is to achieve elite human level performance in heads-up poker, while training entirely on the computing resources found on a laptop. 

The solver is assembled in 3 main stages:

### 1. Generate abstraction
- Card sets are abstracted into 169-1000-1000-1000 buckets on each street
- On the flop and turn, distribution-aware clustering is used
- On the river, percentile hand strength is used 
- Action abstraction is done by restricting bets to simple percentages of the pot.

### 2. Train blueprint strategy
Within this abstracted game, external-sampling MCCFR is run to converge to a Nash equilibrium strategy.

### 3. Real-time search
When playing with or against the AI, depth-limited solving is run on every street except the preflop to resolve the current subgame.

## Installation
The g++ compiler and at least 32GB of RAM are needed. 

First, compile all programs using the top Makefile:

```bash
make
```

Run these programs for the abstraction (takes ~12 hours):

```bash
cd build
# Precomputes table for hand evaluation
./table
# Card abstraction
./sets
./strengths
./distributions
./preflop_river_clusters
./flop_turn_clusters
# Bet abstraction
./tree
```

Run these programs to train the blueprint (takes a few days to converge):

```bash
cd build
# Practically, these should both be run multiple times
./generator
./trainer
```

Finally, play with or against the AI:

```bash
cd build
./play_with_blueprint
./play_with_search
./play_against_search
```

## Performance
...

## Planned Improvements
Improvements for the next iteration of the solver:

- Merge game generation with training by branching chance actions in MCCFR
- Implement regret-based pruning to speed up blueprint and search
- Allow for off-tree actions using action translation or nested search
- Write a full benchmark against Slumbot