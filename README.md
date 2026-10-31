# epistemic-game-engine

A Java project combining several loosely related simulation and game systems, more ambitious in scope than a typical course assignment.

- `src/methodology/` — an epistemic reasoning engine (`EpistemicGame`, `EpistemicGameEngine`, `Hypothesis`, `Scenario`) for weighing competing hypotheses given evidence
- `src/protocols/` — potential-field-based agent navigation (`PotentialField`, `EnvironmentSimulation`, `EnvironmentPotentialField`, `Sensor`, `Goal`, `Actor`, `ActionsDesigner`, `EvaluationProcess`)
- `src/text_adventure/` and `src/simulation/ASCIIGame.java` — a text-adventure layer (`Room`, `Item`, `ItemUseAction`, `TextAdventure`)
- `src/Location.java`, `Player.java`, `Game.java` — a small standalone location/player game loop
- `src/rooms/Main.java` — an empty stub
