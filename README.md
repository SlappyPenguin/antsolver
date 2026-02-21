# Antsolver
This is a heads-up no-limit poker AI built from advanced game theory research. The goal for this project is to achieve elite human level performance in heads-up poker, while training entirely on the computing resources found on a laptop. The solver is assembled in 3 distinct parts:

#### 1. Generate abstraction
Card sets are abstracted into 169-1000-1000-1000 buckets on each street. On the flop and turn, distribution-aware clustering is used, and on the river, percentile hand strength is used. Action abstraction is done by restricting bets to simple percentages of the pot.

#### 2. Train blueprint strategy
Within this abstracted game, external-sampling MCCFR is run to converge to a Nash equilibrium strategy.

#### 3. Real-time search
When playing with or against the AI, depth-limited solving is run on every street except the preflop to resolve the current subgame.

## Installation
The g++ compiler and at least 32GB of RAM are needed. 

First, compile all programs using the top Makefile:

```make```

Then, run these programs for the abstraction (takes ~12 hours):








## Authors

Everything unless specified was written by me. The 7-card hand evaluator in src/evaluation was taken from the ingenious XPokerEval library (https://github.com/tangentforks/XPokerEval).