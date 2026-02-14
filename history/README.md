# History

This folder is not a part of the main solver. It is a record tracing the progress of state-of-the-art equilibrium finding, abstraction, and search algorithms for Poker (and imperfect-information games more generally). 

I try to be quite comprehensive with the timeline of major breakthroughs, but the programs provided here in C++ only implement a few equilibrium finding algorithms, specifically the earlier variants of CFR (more coming soon...). 

## Timeline

(1992) Linear programming
Early attempts at solving extensive games 
Koller, D., & Megiddo, N. (1992). The complexity of two-person zero-sum games in extensive form. Games and economic behavior, 4(4), 528-552.

(2000) Regret matching 
Introduction of regret minimisation algorithms
Hart, S., & Mas‐Colell, A. (2000). A simple adaptive procedure leading to correlated equilibrium. Econometrica, 68(5), 1127-1150.

(2007) CFR 
Self-play algorithm which converges to a Nash equilibrium
Zinkevich, M., Johanson, M., Bowling, M., & Piccione, C. (2007). Regret minimization in games with incomplete information. Advances in neural information processing systems, 20.

(2009) MCCFR
Optimisation of CFR to sample external actions at each iteration, speeding up per-iteration speed
Lanctot, M., Waugh, K., Zinkevich, M., & Bowling, M. (2009). Monte Carlo sampling for regret minimization in extensive games. Advances in neural information processing systems, 22.

(2009) Imperfect recall card abstraction
Imperfect recall performs better empirically, despite lack of theoretical garuntees
Waugh, K., Zinkevich, M., Johanson, M., Kan, M., Schnizlein, D., & Bowling, M. H. (2009). A Practical Use of Imperfect Recall. In SARA.

(2013) Pseudo-harmonic action translation
Still state-of-the-art algorithm for action translation
Ganzfried, S., & Sandholm, T. (2013, April). Action Translation in Extensive-Form Games with Large Action Spaces: Axioms, Paradoxes, and the Pseudo-Harmonic Mapping. In IJCAI (pp. 120-128).

(2013) Distribution-aware card abstraction
Expanded abstraction to consider not only hand equities, but future hand strength distributions
Johanson, M., Burch, N., Valenzano, R., & Bowling, M. (2013, May). Evaluating state-space abstractions in extensive-form games. In Proceedings of the 2013 international conference on Autonomous agents and multi-agent systems (pp. 271-278).

(2014) CFR+
Optimisation of CFR to keep regrets positive and discount early strategies, speeding up convergence
Tammelin, O. (2014). Solving large imperfect information games using CFR+. arXiv preprint arXiv:1407.5042.

(2014) Potential-aware card abstraction
Expanded abstraction to consider not only river distributions, but those on all future streets
Ganzfried, S., & Sandholm, T. (2014, June). Potential-aware imperfect-recall abstraction with earth mover's distance in imperfect-information games. In Proceedings of the AAAI Conference on Artificial Intelligence (Vol. 28, No. 1).

(2015) Heads-up limit poker solved 
Cepheus effectively solves limit poker with CFR+
Tammelin, O., Burch, N., Johanson, M., & Bowling, M. (2015, July). Solving Heads-Up Limit Texas Hold'em. In IJCAI (Vol. 15, pp. 645-652).

(2015) Regret-based pruning
Optimisation of CFR algorithms to not explore negative regret actions, speeding up convergence
Brown, N., & Sandholm, T. (2015). Regret-based pruning in extensive-form games. Advances in neural information processing systems, 28.

(2015) Unsafe subgame solving
Introduction of search to imperfect-information games, lacking theoretical garuntees 
Ganzfried, S., & Sandholm, T. (2015, May). Endgame Solving in Large Imperfect-Information Games. In AAMAS (pp. 37-45).

(2017) Safe and nested subgame solving
Improvement of unsafe solving to be theoretically sound and handle off-tree actions
Brown, N., & Sandholm, T. (2017). Safe and nested subgame solving for imperfect-information games. Advances in neural information processing systems, 30.

(2018) Superhuman heads-up no-limit poker bot
Libratus beats top professionals in no-limit poker, using safe solving until the end of the game
Brown, N., & Sandholm, T. (2018). Superhuman AI for heads-up no-limit poker: Libratus beats top professionals. Science, 359(6374), 418-424.

(2018) Depth-limited solving
Modicum achieves (likely) superhuman performance, using search until the end of the current street
Brown, N., Sandholm, T., & Amos, B. (2018). Depth-limited solving for imperfect-information games. Advances in neural information processing systems, 31.

(2019) Superhuman 6-player no-limit poker bot
Pluribus beats to professionals in multiplayer no-limit poker, using depth-limited solving
Brown, N., & Sandholm, T. (2019). Superhuman AI for multiplayer poker. Science, 365(6456), 885-890.

(2020) Generalised deep learning and search bot
ReBeL is a generalised bot for all two-player zero-sum games, combining search with neural nets  
Brown, N., Bakhtin, A., Lerer, A., & Gong, Q. (2020). Combining deep reinforcement learning and search for imperfect-information games. Advances in neural information processing systems, 33, 17057-17069.






