## History

This folder is separate from the main solver. It's a personal record tracing the progress of state-of-the-art 
algorithms for equilibrium finding, abstraction, and search in poker and large-scale, imperfect-information games more generally. 

The programs provided here only implement a few equilibrium finding algorithms, specifically the earlier variants of counterfactual regret minimisation (CFR). 

### Timeline

| Year | Breakthrough | Paper | Description |
|------|-------------|-------|-------------|
| 1992 | Linear programming | Koller, D., & Megiddo, N. (1992). [The complexity of two-person zero-sum games in extensive form](https://doi.org/10.1016/0022-0000(92)90007-H) | Early attempts at solving extensive games used linear programming |
| 2000 | Regret matching | Hart, S., & Mas‐Colell, A. (2000). [A simple adaptive procedure leading to correlated equilibrium](https://doi.org/10.2307/2999642) | Introduction of regret minimisation algorithms |
| 2007 | CFR | Zinkevich, M., Johanson, M., Bowling, M., & Piccione, C. (2007). [Regret minimization in games with incomplete information](https://papers.nips.cc/paper/2007/hash/4bfc1f0b9fdf5d28d0324f07b3e9b68e-Abstract.html) | Self-play algorithm which converges to a Nash equilibrium |
| 2009 | MCCFR | Lanctot, M., Waugh, K., Zinkevich, M., & Bowling, M. (2009). [Monte Carlo sampling for regret minimization in extensive games](https://papers.nips.cc/paper/2009/hash/06cc2f6c0a5d70dc7f0e4c5b872aa118-Abstract.html) | Optimisation of CFR to sample external actions at each iteration, speeding up per-iteration speed |
| 2009 | Imperfect recall card abstraction | Waugh, K., Zinkevich, M., Johanson, M., Kan, M., Schnizlein, D., & Bowling, M. H. (2009). [A Practical Use of Imperfect Recall](https://www.aaai.org/ocs/index.php/SARA/SARA09/paper/view/1644) | Imperfect recall performs better empirically, despite lack of theoretical guarantees |
| 2013 | Measuring HUNL game size | Johanson, M. (2013). [Measuring the Size of Large No-Limit Poker Games](https://arxiv.org/abs/1302.7008) | Measured ACPC-standard HUNL at ~10^164 game states |
| 2013 | Pseudo-harmonic action translation | Ganzfried, S., & Sandholm, T. (2013). [Action Translation in Extensive-Form Games with Large Action Spaces: Axioms, Paradoxes, and the Pseudo-Harmonic Mapping](https://www.ijcai.org/Proceedings/13/Papers/170.pdf) | Still state-of-the-art algorithm for action translation |
| 2013 | Distribution-aware card abstraction | Johanson, M., Burch, N., Valenzano, R., & Bowling, M. (2013). [Evaluating state-space abstractions in extensive-form games](https://dl.acm.org/doi/10.5555/2484920.2484953) | Expanded abstraction to consider not only hand equities, but hand strength distributions at river |
| 2014 | CFR+ | Tammelin, O. (2014). [Solving large imperfect information games using CFR+](https://arxiv.org/abs/1407.5042) | Optimisation of CFR to keep regrets positive and discount early strategies, speeding up convergence |
| 2014 | Potential-aware card abstraction | Ganzfried, S., & Sandholm, T. (2014). [Potential-aware imperfect-recall abstraction](https://www.aaai.org/ocs/index.php/AAAI/AAAI14/paper/view/8532) | Expanded abstraction to consider potential distributions on all future streets |
| 2015 | Heads-up limit poker solved | Tammelin, O., Burch, N., Johanson, M., & Bowling, M. (2015). [Solving Heads-Up Limit Texas Hold'em](https://www.ijcai.org/Proceedings/15/Papers/091.pdf) | Cepheus effectively solves heads-up limit poker with CFR+ |
| 2015 | Regret-based pruning | Brown, N., & Sandholm, T. (2015). [Regret-based pruning in extensive-form games](https://papers.nips.cc/paper/2015/hash/f7a97a2f1234a7b23bdaabbfa2c2c30f-Abstract.html) | Optimisation of CFR algorithms to not explore negative regret actions, speeding up convergence |
| 2015 | Unsafe subgame solving | Ganzfried, S., & Sandholm, T. (2015). [Endgame Solving in Large Imperfect-Information Games](https://dl.acm.org/doi/10.5555/2772879.2772882) | Search in imperfect-information games, performs empirically well without theoretical garuntees |
| 2017 | Safe and nested subgame solving | Brown, N., & Sandholm, T. (2017). [Safe and nested subgame solving for imperfect-information games](https://papers.nips.cc/paper/2017/hash/b8f9ee1f84984b7e5f63d6b6934a4d4c-Abstract.html) | Improvement of unsafe solving to be theoretically sound and handle off-tree actions |
| 2018 | Superhuman heads-up no-limit poker bot | Brown, N., & Sandholm, T. (2018). [Superhuman AI for heads-up no-limit poker: Libratus beats top professionals](https://www.science.org/doi/10.1126/science.aar6404) | Libratus beats top professionals in no-limit poker, using safe solving until the end of the game |
| 2018 | Depth-limited solving | Brown, N., Sandholm, T., & Amos, B. (2018). [Depth-limited solving for imperfect-information games](https://papers.nips.cc/paper/2018/hash/10851f3f42007e5ed37821e9c9f6ef20-Abstract.html) | Modicum achieves (likely) superhuman performance, using search until the end of the current street |
| 2019 | Superhuman 6-player no-limit poker bot | Brown, N., & Sandholm, T. (2019). [Superhuman AI for multiplayer poker](https://www.science.org/doi/10.1126/science.aay2400) | Pluribus beats top professionals in multiplayer no-limit poker, using depth-limited solving |
| 2020 | Generalised deep learning and search bot | Brown, N., Bakhtin, A., Lerer, A., & Gong, Q. (2020). [Combining deep reinforcement learning and search for imperfect-information games](https://papers.nips.cc/paper/2020/hash/87aaad2f9e2e3e12b9deab490fceca63-Abstract.html) | ReBeL is a generalised bot for all two-player zero-sum games, combining search with neural nets |
