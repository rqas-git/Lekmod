# Playing and checking rules

Plan from the current position: victory goal, difficulty, speed, happiness,
income, nearby opponents and available resources. Inspect yields and production
costs before prescribing a build order. Expansion, mod balance and civilization
bonuses can change the best choice.

Settlers need a legal city site; compare food, production, fresh water, luxuries,
strategic resources and defensibility. Founding consumes the settler. Check the
new city's owner, name and production. Workers need the required technology and
a legal build on that tile; verify completion and changed yields. Roads have
maintenance costs and need a connected route to provide their intended benefit.

Choose research for a concrete unlock or resource need. Policies require culture
and eligibility; branch unlock and policy adoption are separate actions. Religion
and trade mechanics depend on installed expansions and mods. Use Civilopedia,
active SQLite tables and source when exact prerequisites or formulas matter.

War declarations, movement, ranged attacks and melee attacks are separate orders.
Ranged units cannot normally capture a city; a melee unit must enter it after
sufficient damage. Check movement, range, line of sight, territory access and the
war state before issuing an attack. Inspect actual damage and ownership afterward.
AI contact, trade, peace and city-state interactions deserve explicit observation;
an AI slot alone does not establish that those systems were exercised.

The [official vanilla manual](https://downloads.2kgames.com/civ5/site13/community/feature_manual/Civ_V_Manual_English_v1.0.pdf)
is a starting point, not an exact reference for Brave New World or Lekmod. For a
mod regression, separate ordinary gameplay coverage from changed mechanics and
record actions actually observed, not merely enabled in the scenario.
